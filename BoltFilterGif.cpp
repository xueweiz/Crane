#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "connections.h"
#include "BoltFilterGif.h"


BoltFilterGif::BoltFilterGif(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_FILTER_GIF;
}

BoltFilterGif::BoltFilterGif(std::string name, unsigned int parallel_level, FileSystem * inputFs)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_FILTER_GIF;
	myFs = inputFs;
}

BoltFilterGif::~BoltFilterGif()
{

}

void BoltFilterGif::run()
{
	numOfElemsInTuple = 1;
	sleep(6);	//we assume subscribers will be ready in 2s.
	
	this->numOfSubscriptor = 4;
	this->robustCreateQueue();

	this->createEmittingThreads();
	
	while(!killRunThread){
		Tuple tuple = this->getTuple();

		std::string filename = tuple.getElement(0);
		
		if(filename.compare("CRANE_FINISH") == 0) {
			sleep(2);	//to wait for other task of this bolt to finish job
			emitAll(tuple);
			//sleep(60*60);
			break; 
		}

		size_t pos = filename.find(".gif");

		// Not found
		if (pos == std::string::npos) {
			//emit(tuple, filename);
			emit(tuple);
		}
	}
}
