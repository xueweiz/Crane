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

void Spout::emit(Tuple& tuple)
{
	if (subscriptors.size() == 0)
	{
		std::cout << "Emiting but nobody subscribed..." << std::endl;
	}

	tupleQueueLock.lock();
	tupleQueue.push_back(tuple);
	tupleQueueLock.unlock();
}

void Spout::communicationThread()
{
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
	            std::cout << "emiting tuple: " << tuples2Send.at(j).getSingleStringComa() << std::endl;
	            
	            std::string str2Send = tuples2Send.at(j).getSingleString();
	            str2Send.copy(msg.buffer,str2Send.length(),0);
	            assert(str2Send.length() > 0);

		        write(connFDs.at(i), (char*)&msg, sizeof(CRANE_TupleMessage));

	    	}
	    	/*
	    	CRANE_Message ack;
	    	ack.ack = 0;
	    	read(connFDs.at(i), &ack, sizeof(CRANE_Message));

	    	if(ack.ack != 1)
	    	{
	    		std::cout << "Spout::communicationThread : ACK error" << std::endl;
	    	}
	    	*/
		}

		std::cout << "Done emiting group" << std::endl;
	}
}

void Spout::generateTuples()
{

}


