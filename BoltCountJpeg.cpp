#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <ctime>

#include "connections.h"
#include "BoltCountJpeg.h"


BoltCountJpeg::BoltCountJpeg(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_COUNT_JPEG;
}

BoltCountJpeg::BoltCountJpeg(std::string name, unsigned int parallel_level, FileSystem * inputFs)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_COUNT_JPEG;
	myFs = inputFs;
}

BoltCountJpeg::~BoltCountJpeg()
{

}

void BoltCountJpeg::run()
{
	startTime = clock();

	int counter = 0;
	numOfElemsInTuple = 1;
	while(!killRunThread) {
		
		Tuple tuple = this->getTuple();

		std::string filename = tuple.getElement(0);
		if(filename.compare("CRANE_FINISH") == 0) {
			break; 
		}

		size_t pos = filename.find(".jp");

		// Not found
		if (pos != std::string::npos) {
			counter++;
			if(counter % 1000 == 0){
				std::cout << "Total jpeg: " << counter << std::endl;
			}
		}
	}

	clock_t endTime = clock();
	double secondsPassed = (endTime - startTime) / CLOCKS_PER_SEC;
	
	int seconds = ((int)secondsPassed) % 60;
	int minutes = ((int)secondsPassed) / 60;

	std::cout << "Total jpeg: " << counter << std::endl;
	std::cout << "  done in: " << secondsPassed << " seconds"<< std::endl;
	std::cout << "  which is: " << minutes << " min " <<seconds<<" secs"<< std::endl;



	sleep(60*60);
}