#include <cstring>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "connections.h"
#include "Spout.h"

Spout::Spout(std::string name, uint32_t parallel_level) :
	name(name), parallel_level(parallel_level)
{
	spoutId = rand() % 100;
	killEmittingThread = false;
}

Spout::~Spout()
{
	if (generate.joinable()) // carefull here;
	{
		std::cout << "Waiting for thread to finish.. " << std::endl;
		killGenerateThread = true;
		generate.join();
	}
	if (communication.joinable()) // carefull here;
	{
		std::cout << "Waiting for thread to finish.. " << std::endl;
		killCommunicationThread = true;
		communication.join();
	}
}

void Spout::run()
{

}

void Spout::stop()
{
}

uint32_t Spout::getSpoutId()
{
	return spoutId;
}

uint32_t Spout::getParallelLevel()
{
	return parallel_level;
}

std::string Spout::getName()
{
	return name;
}

void Spout::subscribe(Bolt& bolt)
{

/*
	for (int i = 0; i < bolt.tasks.size(); ++i)
	{
		subscriptorsAdd .push_back(bolt.tasks.at(i).ip_str);
		subscriptorsPort.push_back(bolt.tasks.at(i).port);
	}
*/

	for (int i = 0; i < bolt.tasks.size(); ++i)
	{
		std::string ip = bolt.tasks.at(i).ip_str;
		uint32_t port = bolt.tasks.at(i).port;

		int connectionToServer; //TCP connect

		int ret = connect_to_server(ip.c_str(), port, &connectionToServer);

		while (ret != 0)
		{
			std::cout <<"Connecting to subscriptor to get port... "<< ip  << " - " << port << std::endl; 
			sleep(1);
			ret = connect_to_server(ip.c_str(), port, &connectionToServer);
		}

		CRANE_Message msg;
		msg.msgType = MSG_SUBSCRIPTION;
		write(connectionToServer, &msg, sizeof(CRANE_Message));
		read (connectionToServer, &msg, sizeof(CRANE_Message));

		uint32_t port4Connection = msg.port;

		close(connectionToServer);

		ret = connect_to_server(ip.c_str(), port4Connection, &connectionToServer);
		while (ret != 0)
		{
			std::cout <<"Connecting to subscriptor at "<< ip  << " - " << port4Connection << std::endl; 
			sleep(1);
			ret = connect_to_server(ip.c_str(), port4Connection, &connectionToServer);
		}

		bool flag = false;
		for (int j = 0; j < subscriptors.size(); ++j)
		{
			if (subscriptors.at(j).boltId == bolt.getBoltId())
			{
				struct CRANE_TaskInfo task;
				task.ip_str = ip;
				task.port 	= port;
				task.connectionFD = connectionToServer;

				subscriptors.at(j).tasks.push_back(task);
				flag = true;
			}
		}

		if (!flag)
		{
			struct BoltSubscriptor newBolt;
			newBolt.boltId = bolt.getBoltId();
			struct CRANE_TaskInfo task;
			task.ip_str = ip;
			task.port 	= port;
			task.connectionFD = connectionToServer;


			newBolt.tasks.push_back(task);

			subscriptors.push_back(newBolt);
		}
	}
	
}

//we should change this method into emitTupleThread();
void Spout::communicationThread()	
{
	//should not be used. 
	return;

	while( ! killCommunicationThread)
	{
		
		if (subscriptors.size() == 0)
		{
			std::cout << "Nobody subscribed to this spout" << std::endl;
			sleep(1);
			continue;
		}

		std::vector<Tuple> tuples2Send;
		tupleQueueLock.lock();
		if (!tupleQueue.empty())
		{
			for (int i = 0; i < 10 && !tupleQueue.empty(); ++i)
			{
				tuples2Send.push_back(tupleQueue.front());
				tupleQueue.pop_front();
			}
		}
		tupleQueueLock.unlock();

		if (tuples2Send.size() == 0)
		{
			std::cout << "Spout::communicationThread : No tuples in queue" << std::endl;
			sleep(1);
			continue;
		}

		std::vector<uint32_t> connFDs;

		for (int i = 0; i < subscriptors.size(); ++i)
		{
			for (int j = 0; j < subscriptors.at(i).tasks.size(); ++j)
			{
				connFDs .push_back(subscriptors.at(i).tasks.at(j).connectionFD);
			}
		}

		for (int i = 0; i < connFDs.size(); ++i)
	    {
	        for (int j = 0; j < tuples2Send.size(); ++j)
			{
				CRANE_TupleMessage msg;
	            memset(&msg,0, sizeof(CRANE_TupleMessage));
	            //msg.more = tuples2Send.size() - j - 1;
	            //std::cout << "emiting tuple: " << tuples2Send.at(j).getSingleStringComa() << std::endl;
	            
	            std::string str2Send = tuples2Send.at(j).getSingleString();
	            str2Send.copy(msg.buffer,str2Send.length(),0);
	            assert(str2Send.length() > 0);

		        int ret = write(connFDs.at(i), (char*)&msg, sizeof(CRANE_TupleMessage));

		        if (ret <= 0)
		        {
		        	std::cout << "Error writing!!!" << std::endl;
		        	return;
		        }

	    	}
		}
	}
}

void Spout::generateTuples()
{

}

void Spout::createEmitQueues(){
	emitQueues.clear();
	int count = 0;
	for(int i=0; i < subscriptors.size(); i++){
		std::vector< TupleQueue* > tmp;
		for(int j=0; j < subscriptors[i].tasks.size(); j++){
			TupleQueue* tmpQueue = new TupleQueue();
			tmp.push_back(tmpQueue);
			count++;
		}
		emitQueues.push_back(tmp);
	}

	std::cout<<"Spout::createEmitQueues: total queue "<<count<<std::endl;
}
void Spout::createEmittingThreads(){
	emittingThreads.clear();
	int count = 0;

	for(int i=0; i < subscriptors.size(); i++){
		std::vector< std::thread* > tmp; 

		for(int j=0; j < subscriptors[i].tasks.size(); j++){
			int * boltNum = new int(i);
			int * taskNum = new int(j);
			
			std::thread* th = new std::thread(&Spout::emitTupleThread, this, boltNum, taskNum);
			th->detach();

			tmp.push_back(th);
			count++;
		}

		emittingThreads.push_back(tmp);
	}

	std::cout<<"Spout::createEmittingThreads: subscriped by "<< count <<std::endl;
}


/************************************ CHANGE *****************************************/

//emit tuple to each subscribing bolt, but only one task
void Spout::emit(Tuple& tuple) {
	if (subscriptors.size() == 0) {
		std::cout << "Spout::emit: Emiting but bolt subscribing me" << std::endl;
		sleep(1);
		return;
	}
	//select queue
	for(int i=0; i < subscriptors.size(); i++){
		if( subscriptors[i].tasks.size() == 0 ){
			std::cout << "Spout::emit: Emiting but bolt has no task" << std::endl;
			sleep(1);
			break;
		}
		int j = rand() % subscriptors[i].tasks.size();

		//put to queue, and notify all
		addToTupleQueue( emitQueues[i][j], tuple );
	}
}

//emit tuple to each subscribing bolt, to all the tasks
void Spout::emitAll(Tuple& tuple) {
	if (subscriptors.size() == 0) {
		std::cout << "Spout::emit: Emiting but bolt subscribing me" << std::endl;
		sleep(1);
		return;
	}
	//select queue
	for(int i=0; i < subscriptors.size(); i++){
		//put to queue, and notify all
		for(int j = 0; j < subscriptors[i].tasks.size(); j++)
			addToTupleQueue( emitQueues[i][j], tuple );
	}
}

void Spout::addToTupleQueue(TupleQueue * tupleQueue, Tuple & tuple){
	std::unique_lock<std::mutex> locker( tupleQueue->lock );
    tupleQueue->queue.push_back(tuple);
    tupleQueue->cv.notify_all();
}

void Spout::emitTupleThread(int * boltNum, int * taskNum){
	TupleQueue * myQueue = emitQueues[*boltNum][*taskNum];
	uint32_t connFD = subscriptors[*boltNum].tasks[*taskNum].connectionFD;
	//std::cout<<"Spout::emitTupleThread: emitting thread "<<*boltNum<<" "<<*taskNum<<" ready."<<std::endl;

	std::list<Tuple> sendQueue;


	while( !killEmittingThread ){
		//sleep(1);
		//std::cout<<"Spout::emitTupleThread: one message"<<std::endl;		
		
		//read readySignal from connFD 
		CRANE_Message msg;
		int ret = robustRead (connFD, (char*)&msg, sizeof(CRANE_Message));
		if(ret <= 0){
			std::cout<<"Spout::emitTupleThread: socket is down"<<std::endl;
			//return;
			sleep(1);
			continue;
		}
		
		if(msg.msgType != MSG_READ_READY){
			std::cout<<"Spout::emitTupleThread: read message is not ready msg. Bolt sent wrong data"<<std::endl;
			sleep(1);
			continue;	
		}

		//read Tuples from myQueue
		sendQueue.clear();
		pullFromEmitQueue( sendQueue, myQueue );
		
		//prepare buffer
		int numOfElems = 0; 
		if( !sendQueue.empty() )
			numOfElems = sendQueue.begin()->getNumElements();
		if( numOfElems == 0 ){
			std::cout<<"Spout::emitTupleThread: tuple contains no elems"<<std::endl;
			sleep(1);
			continue;
		}

		std::stringstream ss;
		for(auto it = sendQueue.begin(); it != sendQueue.end(); it++ ){
			for(int i=0; i < numOfElems; i++){
				ss << it->getElement(i) << std::endl;
			}
		}

		const std::string tmp = ss.str();
		const char* cstr = tmp.c_str();

		size_t length = std::strlen(cstr) + 1;	//for the /0 of cstr
		char * buffer = new char[length];
		std::memcpy( buffer, cstr, length );

		//write tuple count to connFD
		ret = robustWrite( connFD, (char*)&length, sizeof(size_t));
		if( ret <= 0 ){
			std::cout<<"Spout::emitTupleThread: socket is down"<<std::endl;
			//return;
			sleep(1);
			continue;
		}
		
		//write buffer
		ret = splitWrite( connFD, buffer, length);
		if( ret <= 0 ){
			std::cout<<"Spout::emitTupleThread: socket is down"<<std::endl;
			//return;
			sleep(1);
			continue;
		}

		delete [] buffer;
	}

	close(connFD);
	std::cout<<"Spout::emitTupleThread: emitting thread "<<*boltNum<<" "<<*taskNum<<" cleanning up."<<std::endl;
}

void Spout::pullFromEmitQueue( std::list<Tuple>& sendQueue, TupleQueue* myQueue ){
	std::unique_lock<std::mutex> locker( myQueue->lock );

	while( myQueue->queue.empty() ){
		//std::cout << "wait" << std::endl;
		myQueue->cv.wait( locker );
	}

	sendQueue.splice( sendQueue.begin(), myQueue->queue, myQueue->queue.begin(), myQueue->queue.end() );
}