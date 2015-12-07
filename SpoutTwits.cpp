#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "SpoutTwits.h"
#include "connections.h"

SpoutTwits::SpoutTwits(std::string name, unsigned int parallel_level)
	: Spout(name, parallel_level)
{
	//type = CRANE_TASK_GENDER;
}

SpoutTwits::~SpoutTwits()
{

}

void SpoutTwits::run()
{
	std::cout << "Running SpoutTwits" << std::endl;
	killGenerateThread = false;
	std::thread th(&SpoutTwits::generateTuples, this);
    generate.swap(th);

    killCommunicationThread = false;
	std::thread th2(&SpoutTwits::communicationThread, this);
    communication.swap(th2);
}

void SpoutTwits::stop()
{
	killGenerateThread = true;
    generate.join();

    killCommunicationThread = true;
    communication.join();

    std::cout << "Thread stoped" << std::endl;
}

void SpoutTwits::generateTuples()
{
	uint32_t counter = 0;
	int counterMale = 0;
	int counterFemale = 0;
	long long totalTuples = 40*1000*1000;
	while(!killGenerateThread)
	{
		std::stringstream ss;
		int randNum = rand();

		if (randNum % 2 == 1)
		{
			ss << "Hello\n";
		}
		else
		{
			ss << "GoodBye\n";
		}
		
		if (randNum % 3 == 1)
		{
			if (counterMale == totalTuples)
			{
				continue;
			}
			ss << "male\n";
			counterMale++;
		}
		else if (randNum % 3 == 0)
		{
			if (counterFemale == totalTuples)
			{
				continue;
			}
			ss << "female\n";
			counterFemale++;
		}
		else
		{
			ss << "unknown\n";
		}	

		ss << counter++ << "\0";

		Tuple imaginary(ss.str());
		emit(imaginary);
		if (counterMale == totalTuples && counterFemale == totalTuples)
		{
			std::cout << "Done emiting males" << std::endl;
			return;
		}
		//usleep(1);
		/*if (counter % 20 == 0)
		{
			usleep(10000);
		}*/
		//sleep(1);
	}
}

void SpoutTwits::communicationThread()
{
	size_t tupleCounter =0;
	while(!killCommunicationThread)
	{
		//std::cout<<"SpoutTwits::communicationThread once"<<std::endl;

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
			//std::cout << "Spout::communicationThread : No tuples in queue" << std::endl;
			//sleep(1);
			continue;
		}

		tupleCounter += tuples2Send.size();

		if (tupleCounter % 10000 == 0)
		{
			std::cout << "Emitted: " << tupleCounter << std::endl;
		}

		std::vector<uint32_t> connFDs;

		for (int i = 0; i < subscriptors.size(); ++i)
		{
			uint32_t taskId = rand() % subscriptors.at(i).tasks.size(); 
			connFDs .push_back(subscriptors.at(i).tasks.at(taskId).connectionFD);
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