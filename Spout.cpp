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

	std::thread th(&Spout::communicationThread, this);
    communication.swap(th);
}

Spout::~Spout()
{
	if (listening.joinable()) // carefull here;
	{
		std::cout << "Waiting for thread to finish.. " << std::endl;
		killListeningThread = true;
		listening.join();
	}
}

void Spout::subscribe(Bolt& bolt, uint32_t port)
{
	std::vector<struct CRANE_TaskInfo> tasksSubscriber = bolt.tasks;

	for (int i = 0; i < bolt.tasks.size(); ++i)
	{
		subscriptorsAdd .push_back(bolt.tasks.at(i).ip_str);
		subscriptorsPort.push_back(bolt.tasks.at(i).port);
	}
}

void Spout::emit(Tuple& tuple)
{
	if (subscriptorsAdd.size() == 0)
	{
		std::cout << "Nobody subscribed to this spout" << std::endl;
		return;
	}

	tupleQueueLock.lock();
	tupleQueue.push_back(tuple);
	tupleQueueLock.unlock();
}

void Spout::communicationThread()
{
	while(true)
	{
		if (subscriptorsAdd.size() == 0)
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

		for (int i = 0; i < subscriptorsAdd.size(); ++i)
		{
			int connectionToServer; //TCP connect

			int ret = connect_to_server(subscriptorsAdd.at(i).c_str(), subscriptorsPort.at(i), &connectionToServer);

			while (ret != 0)
			{
				std::cout <<"Spout::emit Cannot connect to... "<<(subscriptorsAdd.at(i))<< std::endl; 
				sleep(1);
				//should put a counter of errors before aborting
			}
		    
	    	for (int i = 0; i < tuples2Send.size(); ++i)
	    	{
	    		CRANE_TupleMessage msg;
	            memset(msg.buffer,0,512);
	            msg.more = tuples2Send.size() - i - 1;
	            
	            std::string str2Send = tuples2Send.at(i).getSingleString();
	            str2Send.copy(msg.buffer,str2Send.length(),0);
	            
	            std::string sent(msg.buffer);
	            //std::cout << "Sent: " << sent << std::endl;

	            std::cout << "emiting tuple: " << tuples2Send.at(i).getSingleStringComa() << std::endl;

		        write(connectionToServer, &msg, sizeof(CRANE_Message));
	    	}
            
	        close(connectionToServer);  
		}
	}
	
}

void Spout::generateTuples()
{

}

void Spout::listeningThread()
{
	// NOT IN USE, BUT COULD BE USE TO HAVE A THREAD RECEIVING FROM SOME SOURCE
	/*
	int listenFd = open_socket(port); 

    //std::cout<<"listeningThread: someone is contacting me... "<< std::endl;

	while(!killListeningThread)
    {
    	size_t ret;
    	int connFd = listen_socket(listenFd);
        
        CRANE_TupleMessage msg;
        memset(msg.buffer,0,512);

        ret = read(connFd, &msg, sizeof(CRANE_TupleMessage));

        std::string newTuple (msg.buffer);
        Tuple tuple(newTuple);
        std::cout << "Received: " << tuple.getSingleStringComa() << std::endl;

        tupleQueueLock.lock();
        tupleQueue.push_back(tuple);
        tupleQueueLock.unlock();

        close(connFd);
    }
    */
}

void Spout::run()
{
	std::cout << "Runing pure Spout run method, this is wrong" << std::endl;
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

