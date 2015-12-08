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

			count++;
			if( count >= readFileTimes ){
				std::cout << "SpoutCalgary::getNextAccess: finished. over "<<count<<"times" << std::endl;
				return "CRANE_FINISH";
			}
			
			initFile();
			std::cout << "SpoutCalgary::getNextAccess: File start over.. "<<count<<" times" << std::endl;
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

/************************************ CHANGE *****************************************/


void SpoutCalgary::generateTuples()
{
	uint32_t counter = 0;

	initFile();

	while(!killGenerateThread) {
		std::stringstream ss;

		std::string str = getNextAccess();
		if(str.compare("CRANE_FINISH") == 0) {
			//std::cout<<"SpoutCalgary::generateTuples: get finish string"<<std::endl;
			ss << str << std::endl;
			ss << counter++ << std::endl;
			
			Tuple finishTuple(ss.str());
			
			//emitAll(finishTuple);
			emit(finishTuple);
			return;
		}

		ss << str << std::endl;
		ss << counter++ << std::endl;

		Tuple imaginary(ss.str());
		emit(imaginary);	//add tuple to all the subscribing bolts, each bolt select one task
	}
}