#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "connections.h"
#include "Bolt.h"

Bolt::Bolt(std::string name, uint32_t parallel_level) :
	name(name), parallel_level(parallel_level)
{
	type = CRANE_TASK_EMPTY;
	boltId = 9999999; // This should be assign by crane, if found this number is an error. 
}

Bolt::~Bolt()
{
	killListeningThread = true;
	std::cout << "Waiting for thread to finish.. " << std::endl;
	listening.join(); // carefull here;
}

void Bolt::subscribe(Bolt& bolt, uint32_t cranePort)
{
	//subscriptors.push_back(bolt);

	assert(bolt.tasks.size() == bolt.getParallelLevel());

	for (int j = 0; j < tasks.size(); ++j)
	{
		std::string destIp = tasks.at(j).ip_str;
		uint32_t	destPort = tasks.at(j).port;

		for (int i = 0; i < bolt.getParallelLevel(); ++i)
		{
			std::string address = bolt.tasks.at(i).ip_str;

			int connectionToServer; //TCP connect

			int ret = connect_to_server(address.c_str(), cranePort, &connectionToServer);

			while (ret != 0)
			{
				std::cout <<"Bolt::subscribe Cannot connect to... "<<address<< std::endl; 
				sleep(1);
		        //exit(0);
			}
		    
		    {
	            CRANE_Message msg;

	            msg.msgType  = MSG_SUBSCRIPTION;
	            destIp.copy(msg.string, address.length(), 0);
	            msg.string[address.length()+1] = '\0';
	            msg.port = destPort;
	            msg.taskId = bolt.tasks.at(i).taskId;
	            msg.boltId = bolt.getBoltId();

		        write(connectionToServer, &msg, sizeof(CRANE_Message));

		        close(connectionToServer);
		    }   
		}
	}
	std::cout << "Bolt " << getBoltId() << " subscribed correctly to "<<  bolt.boltId  << std::endl;

}

void Bolt::emit(Tuple& tuple)
{
	if (subscriptors.size() == 0)
	{
		std::cout << boltId << " - " << taskId <<" - Noone to emit to!" << std::endl;
		return;
	}

	std::vector<uint32_t> connFDs;

	for (int i = 0; i < subscriptors.size(); ++i)
	{
		for (int j = 0; j < subscriptors.at(i).tasks.size(); ++j)
		{
			connFDs .push_back(subscriptors.at(i).tasks.at(j).connectionFD);
		}
	}

	assert(connFDs.size() > 0);

	//std::cout << "emiting tuple: " << tuple.getSingleStringComa() << std::endl;

	CRANE_TupleMessage msg;

	std::string str2Send = tuple.getSingleString();

	memset(&msg,0, sizeof(CRANE_TupleMessage));
    str2Send.copy(msg.buffer,str2Send.length(),0);

    //std::string sent(msg.buffer);
    //std::cout << "Sent: " << sent << std::endl;
	
	for (int i = 0; i < connFDs.size(); ++i)
	{
        write(connFDs.at(i), (char*)&msg, sizeof(CRANE_TupleMessage));
	}

	//std::cout << "emiting tuple: " << tuple.getSingleStringComa() << std::endl;
}

struct CRANE_TaskInfo Bolt::getTaskInfo(uint32_t index)
{
	assert(index < tasks.size());

	return tasks.at(index);
}

void Bolt::addSubscriptor(std::string ip, uint32_t port, uint32_t boltId)
{

	int connectionToServer; //TCP connect

	int ret = connect_to_server(ip.c_str(), port, &connectionToServer);

	while (ret != 0)
	{
		std::cout <<"Connecting to subscriptor to get port... "<< ip << std::endl; 
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
		//std::cout <<"Connecting to subscriptor at ... "<< ip << std::endl; 
		sleep(1);
		ret = connect_to_server(ip.c_str(), port4Connection, &connectionToServer);
	}

	bool flag = false;
	for (int i = 0; i < subscriptors.size(); ++i)
	{
		if (subscriptors.at(i).boltId == boltId)
		{
			struct CRANE_TaskInfo task;
			task.ip_str = ip;
			task.port 	= port;
			task.connectionFD = connectionToServer;

			subscriptors.at(i).tasks.push_back(task);
			flag = true;
		}
	}

	if (!flag)
	{
		struct BoltSubscriptor newBolt;
		newBolt.boltId = boltId;
		struct CRANE_TaskInfo task;
		task.ip_str = ip;
		task.port 	= port;
		task.connectionFD = connectionToServer;


		newBolt.tasks.push_back(task);

		subscriptors.push_back(newBolt);
	}

	subscriptorsAdd.push_back(ip);
	subscriptorsPort.push_back(port);

	//std::cout << "New subscriptor " << ip << " - " << port << std::endl;
}

void Bolt::setPort(uint32_t port)
{
	this->port = port;
	portSeed = port + 111;

	killListeningThread = true;
	//std::cout << "Waiting for thread to finish.. " << std::endl;
	//listening.join(); // carefull here;

	killListeningThread = false;
	std::thread th2(&Bolt::listeningThread, this);
    listening.swap(th2);

    killRunThread = false;
	std::thread th3(&Bolt::run, this);
    runThread.swap(th3);
}

uint32_t Bolt::getPort()
{
	return port;

}

// Admin listening thread
void Bolt::listeningThread()
{
	int listenFd = open_socket(port); 

    //std::cout<<"listeningThread: created at port "<< port << std::endl;

	while(!killListeningThread)
    {
    	size_t ret;
    	int connFd = listen_socket(listenFd);
    	//std::cout << "Connected"<< std::endl;

        CRANE_Message msg;
		ret = read(connFd, &msg, sizeof(CRANE_Message));
    	if (msg.msgType == MSG_SUBSCRIPTION)
		{
			std::thread* th = new std::thread(&Bolt::point2PointThread, this, portSeed++);
			th->detach();

			msg.port = portSeed-1;
			write(connFd, &msg, sizeof(CRANE_Message));
	        //std::cout << "Received: " << std::endl;
		}
		else
		{
			std::cout << "This is a huge error" << std::endl;
		}
        
        close(connFd);
    }

    std::cout<<"listeningThread: Finished. Used to listen at "<< port << std::endl;

   
}

// Reading thread
void Bolt::point2PointThread(uint32_t port4P2P)
{
	int listenFd = open_socket(port4P2P);
	//std::cout<<"Bolt::point2PointThread: created at port "<< port4P2P << std::endl;

	size_t ret;
	int connFd = listen_socket(listenFd);
	size_t errorCounter = 0;

	//std::cout<<"Bolt::point2PointThread: Connected "<< port4P2P << std::endl;

	static uint32_t counter = 0;
    
    while (!killListeningThread) // this is dangeorus
    {
    	CRANE_TupleMessage msg;
    	memset(&msg,0, sizeof(CRANE_TupleMessage));
		ret = read(connFd, (char*)&msg, sizeof(CRANE_TupleMessage));

		if (ret < 0)
		{
			std::cout << "Bolt::point2PointThread - problem reading" << std::endl;
			continue;
		}

		if (ret == 0)
		{
			std::cout << "Bolt::point2PointThread - problem 0" << std::endl;

			errorCounter++;
			if(errorCounter > 10)
				exit(0);
			continue;
		}

		if (ret != sizeof(CRANE_TupleMessage))
		{
			uint32_t aux = ret;
			while (aux < sizeof(CRANE_TupleMessage))
			{
				ret = read(connFd, (char*) (&msg + aux), sizeof(CRANE_TupleMessage) - aux);
				aux += ret;
			}
			//std::cout << "Bolt::point2PointThread - Framing error (" << sizeof(CRANE_TupleMessage) << "): received: " << ret << std::endl;
			/* code */
		}

        std::string newTuple (msg.buffer);
        Tuple tuple(newTuple);

        //if(counter++ % 10 == 0)
        //	std::cout << "Received: " << tuple.getSingleStringComa() << std::endl;
        //std::cout << "Received: " << std::endl;
        
        //std::cout << "receive lock" << std::endl;
        std::unique_lock<std::mutex> locker( tupleQueueLock );
        //std::cout << "receive put data" << std::endl;
        tupleQueue.push_back(tuple);
        //std::cout << "receive wait others" << std::endl;
        tupleQueueCV.notify_all();
    }

    close(connFd);
}

Tuple Bolt::getTuple(){
	//std::cout << "lock" << std::endl;
	std::unique_lock<std::mutex> locker( tupleQueueLock );

	while( tupleQueue.empty() ){
		//std::cout << "wait" << std::endl;
		tupleQueueCV.wait( locker );
	}

	//std::cout << "get data" << std::endl;
	Tuple tuple = tupleQueue.front();
	tupleQueue.pop_front();
	return tuple;
}

void Bolt::run()
{
	std::cout << "Runing pure bolt run method, this is wrong" << std::endl;
}

uint32_t Bolt::getBoltId()
{
	return boltId;
}

uint32_t Bolt::getTaskId()
{
	return taskId;
}

void Bolt::setBoltId(uint32_t id)
{
	boltId = id;
}

void Bolt::setTaskId(uint32_t id)
{
	taskId = id;
}

uint32_t Bolt::getParallelId()
{
	return parallel_id;
}

void Bolt::setParallelId(uint32_t id)
{
	parallel_id = id;
}

CRANE_TaskType Bolt::getType()
{
	return type;
}

uint32_t Bolt::getParallelLevel()
{
	return parallel_level;
}

std::string Bolt::getName()
{
	return name;
}

std::string Bolt::getIPByTask(uint32_t task)
{
	assert(tasks.size() == parallel_level);
	if (task > parallel_level)
	{
		std::cout << "Bolt::getIPByTask - task " << task << " does not exist" << std::endl;
		exit(0);
		return "";
	}

	return tasks.at(task).ip_str;
}

