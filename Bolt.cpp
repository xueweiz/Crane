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
	boltId = rand() % 100;
}

Bolt::~Bolt()
{
	killListeningThread = true;
	std::cout << "Waiting for thread to finish.. " << std::endl;
	listening.join(); // carefull here;
}

void Bolt::subscribe(Bolt& bolt, uint32_t port)
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

			int ret = connect_to_server(address.c_str(), port, &connectionToServer);

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

}

void Bolt::subscribe2Spout(Bolt& bolt, uint32_t port)
{
	std::vector<struct CRANE_TaskInfo> tasksSubscriber = bolt.tasks;

	for (int i = 0; i < bolt.tasks.size(); ++i)
	{
		subscriptorsAdd .push_back(bolt.tasks.at(i).ip_str);
		subscriptorsPort.push_back(bolt.tasks.at(i).port);
	}

}

void Bolt::emit(Tuple& tuple)
{
	if (subscriptorsAdd.size() == 0)
	{
		std::cout << boltId << " - " << taskId <<" - Noone to emit to!" << std::endl;
		return;
	}

	std::cout << "emiting tuple: " << tuple.getSingleStringComa() << std::endl;

	for (int i = 0; i < subscriptorsAdd.size(); ++i)
	{
		int connectionToServer; //TCP connect

		int ret = connect_to_server(subscriptorsAdd.at(i).c_str(), subscriptorsPort.at(i), &connectionToServer);

		while (ret != 0)
		{
			std::cout <<"Bolt::emit Cannot connect to... "<<(subscriptorsAdd.at(i))<< std::endl; 
			sleep(1);
		}
	    
	    {
            CRANE_TupleMessage msg;
            memset(msg.buffer,0,512);

            msg.more = 0;
            std::string str2Send = tuple.getSingleString();
            str2Send.copy(msg.buffer,str2Send.length(),0);
            std::string sent(msg.buffer);
            //std::cout << "Sent: " << sent << std::endl;

	        write(connectionToServer, &msg, sizeof(CRANE_Message));

	        close(connectionToServer);
	    }   
	}
}

struct CRANE_TaskInfo Bolt::getTaskInfo(uint32_t index)
{
	assert(index < tasks.size());

	return tasks.at(index);
}

void Bolt::addSubscriptor(std::string ip, uint32_t port)
{
	subscriptorsAdd.push_back(ip);
	subscriptorsPort.push_back(port);
}

void Bolt::setPort(uint32_t port)
{
	this->port = port;

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

void Bolt::generateTuples()
{

}

void Bolt::listeningThread()
{
	int listenFd = open_socket(port); 

    //std::cout<<"listeningThread: someone is contacting me... "<< std::endl;

	while(!killListeningThread)
    {
    	size_t ret;
    	int connFd = listen_socket(listenFd);
    	//std::cout << "Conected"<< std::endl;
        
        while (true) // this is dangeorus
        {
        	CRANE_TupleMessage msg;
        	memset(msg.buffer,0,512);
			ret = read(connFd, &msg, sizeof(CRANE_TupleMessage));

	        std::string newTuple (msg.buffer);
	        Tuple tuple(newTuple);
	        std::cout << "Received: " << tuple.getSingleStringComa() << std::endl;
	        //std::cout << "Received: " << std::endl;

	        tupleQueueLock.lock();
	        tupleQueue.push_back(tuple);
	        tupleQueueLock.unlock();

	        if (msg.more == 0)
	        {
	        	break;
	        }
        }
        
        close(connFd);
    }

   
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

void Bolt::setTaskId(uint32_t id)
{
	taskId = id;
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

