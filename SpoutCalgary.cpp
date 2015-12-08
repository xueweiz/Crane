#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "SpoutCalgary.h"
#include "connections.h"

SpoutCalgary::SpoutCalgary(std::string name, unsigned int parallel_level)
	: Spout(name, parallel_level)
{
	//type = CRANE_TASK_GENDER;
}

SpoutCalgary::~SpoutCalgary()
{

}

void SpoutCalgary::run()
{
	this->createEmitQueues();
	this->createEmittingThreads();

	std::cout << "Running SpoutCalgary" << std::endl;
	killGenerateThread = false;
	std::thread th(&SpoutCalgary::generateTuples, this);
    generate.swap(th);

    //killCommunicationThread = false;
	//std::thread th2(&SpoutCalgary::communicationThread, this);
    //communication.swap(th2);
}

void SpoutCalgary::stop()
{
	killGenerateThread = true;
    generate.join();

    killCommunicationThread = true;
    communication.join();

    std::cout << "Thread stoped" << std::endl;
}

void SpoutCalgary::initFile()
{
	std::cout << "Init file... " << std::endl;

	file.open("calgary_access_log");
	if(!file.good())
	{
		std::cout << "Problem loading file calgary_access_log" << std::endl;
		exit(0);
	}
}

std::string SpoutCalgary::getNextAccess()
{
	std::string access;

	std::size_t posGet = std::string::npos;
	std::size_t posEnd = std::string::npos;
	std::size_t posInit= std::string::npos;

	while ( posGet == std::string::npos || posEnd == std::string::npos)
	{
		std::getline(file,access);

		if (file.eof())
		{
			file.close();
			initFile();
			std::cout << "File start over.. " << std::endl;
			return "end";	//only read file once
			
			sleep(3);
			std::getline(file,access);
		}
		
		posGet = access.find("GET");
		posInit= posGet + 4;
		posEnd = access.find(" ", posInit);
	}
	
	access = access.substr(posInit, posEnd-posInit);

	//std::cout << access << std::endl;
	return access;
}


//we should change this method into emitTupleThread();
/*
void SpoutCalgary::communicationThread()	//send tuple to bolt
{
	while(!killCommunicationThread)	{
		if (subscriptors.size() == 0) {
			std::cout << "Nobody subscribed to this spout" << std::endl;
			sleep(1);
			continue;
		}

		std::vector<Tuple> tuples2Send;
		tupleQueueLock.lock();
		if (!tupleQueue.empty()) {
			for (int i = 0; i < 10 && !tupleQueue.empty(); ++i)
			{
				tuples2Send.push_back(tupleQueue.front());
				tupleQueue.pop_front();
			}
		}
		tupleQueueLock.unlock();

		if (tuples2Send.size() == 0) {
			//std::cout << "Spout::communicationThread : No tuples in queue" << std::endl;
			//sleep(1);
			continue;
		}

		std::vector<uint32_t> connFDs;

		for (int i = 0; i < subscriptors.size(); ++i) {
			uint32_t taskId = rand() % subscriptors.at(i).tasks.size(); 
			//for (int j = 0; j < subscriptors.at(i).tasks.size(); ++j) {
				connFDs .push_back(subscriptors.at(i).tasks.at(taskId).connectionFD);
			//}
		}

		for (int i = 0; i < connFDs.size(); ++i) {
	        for (int j = 0; j < tuples2Send.size(); ++j) {
				CRANE_TupleMessage msg;
	            memset(&msg,0, sizeof(CRANE_TupleMessage));
	            //msg.more = tuples2Send.size() - j - 1;
	            //std::cout << "emiting tuple: " << tuples2Send.at(j).getSingleStringComa() << std::endl;
	            
	            std::string str2Send = tuples2Send.at(j).getSingleString();
	            str2Send.copy(msg.buffer,str2Send.length(),0);
	            assert(str2Send.length() > 0);

		        write(connFDs.at(i), (char*)&msg, sizeof(CRANE_TupleMessage));
	    	}
	    	
	    	//CRANE_Message ack;
	    	//ack.ack = 0;
	    	//read(connFDs.at(i), &ack, sizeof(CRANE_Message));

	    	//if(ack.ack != 1)
	    	//{
	    		//std::cout << "Spout::communicationThread : ACK error" << std::endl;
	    	//}
		}
		//std::cout << "Done emiting group" << std::endl;
	}
}
*/


/************************************ CHANGE *****************************************/


void SpoutCalgary::generateTuples()
{
	uint32_t counter = 0;

	initFile();

	while(!killGenerateThread)
	{
		std::stringstream ss;

		std::string str = getNextAccess();
		if(str.compare("end") == 0) {
			return; 
		}

		ss << str << std::endl;
		ss << counter++ << std::endl;

		Tuple imaginary(ss.str());
		emit(imaginary);	//add tuple to all the subscribing bolts, each bolt select one task

		//if (counter % 20 == 0) {
		//	usleep(10000);
		//}
	}
}