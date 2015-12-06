#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "SpoutLawrence.h"
#include "connections.h"

SpoutLawrence::SpoutLawrence(std::string name, unsigned int parallel_level)
	: Spout(name, parallel_level)
{
	//type = CRANE_TASK_GENDER;
}

SpoutLawrence::~SpoutLawrence()
{

}

void SpoutLawrence::run()
{
	std::cout << "Running SpoutLawrence" << std::endl;
	killGenerateThread = false;
	std::thread th(&SpoutLawrence::generateTuples, this);
    generate.swap(th);

    killCommunicationThread = false;
	std::thread th2(&SpoutLawrence::communicationThread, this);
    communication.swap(th2);
}

void SpoutLawrence::stop()
{
	killGenerateThread = true;
    generate.join();

    killCommunicationThread = true;
    communication.join();

    std::cout << "Thread stoped" << std::endl;
}

void SpoutLawrence::initFile()
{
	std::cout << "Init file... " << std::endl;

	file.open("lbl-conn-7.red");
	if(!file.good())
	{
		std::cout << "Problem loading file calgary_access_log" << std::endl;
		exit(0);
	}
}

std::string SpoutLawrence::getNextAccess()
{
	std::string access;

	std::stringstream final;
	std::string proto = "?";
	std::string send  = "?";
	std::string recv  = "?";
	std::string aux;
	
	while ( send == "?" || recv == "?")
	{
		std::getline(file,access);

		if (file.eof())
		{
			file.close();
			initFile();
			std::cout << "File start over.. " << std::endl;
			return "end";
		}

		std::stringstream ss(access);

		ss >> aux;
		ss >> aux;
		ss >> proto;
		ss >> send;
		ss >> recv;
	}

	final << proto << std::endl;
	final << send << std::endl;
	final << recv << std::endl;
	
	return final.str();
}

void SpoutLawrence::generateTuples()
{
	uint32_t counter = 0;

	initFile();

	while(!killGenerateThread)
	{
		std::stringstream ss;

		std::string str = getNextAccess();

		if(str.compare("end") == 0)
		{
			return; 
		}

		Tuple imaginary(str);
		emit(imaginary);

		std::cout << imaginary.getSingleStringComa() << std::endl;

		if (counter % 20 == 0)
		{
			usleep(10000);
		}
	}
}

void SpoutLawrence::communicationThread()
{
	while(!killCommunicationThread)
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
			//std::cout << "Spout::communicationThread : No tuples in queue" << std::endl;
			//sleep(1);
			continue;
		}

		std::vector<uint32_t> connFDs;

		for (int i = 0; i < subscriptors.size(); ++i)
		{
			uint32_t taskId = rand() % subscriptors.at(i).tasks.size(); 
			
			//for (int j = 0; j < subscriptors.at(i).tasks.size(); ++j)
			{
				connFDs .push_back(subscriptors.at(i).tasks.at(taskId).connectionFD);
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

		//std::cout << "Done emiting group" << std::endl;
	}
}