#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "connections.h"
#include "BoltFilterFemale.h"


BoltFilterFemale::BoltFilterFemale(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_FILTER_FEMALE;
}

BoltFilterFemale::~BoltFilterFemale()
{

}

void BoltFilterFemale::run()
{
	int counter = 0;
	std::cout << "running female stasl" << std::endl;
	while(!killRunThread)
	{
		tupleQueueLock.lock();
		if ( !tupleQueue.empty() )
		{
			Tuple tuple = tupleQueue.front();
			tupleQueue.pop_front();
			tupleQueueLock.unlock();

			if (tuple.getElement(1) == "female" ) 
			{
				++counter;
				if (counter % 100 == 0)
				{
					std::cout << "Females: " << counter << std::endl;
				}
			}
		}
		else
		{
			tupleQueueLock.unlock();
		}
	}
}
