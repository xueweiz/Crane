#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "connections.h"
#include "BoltFilterMale.h"


BoltFilterMale::BoltFilterMale(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_FILTER_MALE;
}

BoltFilterMale::~BoltFilterMale()
{

}

void BoltFilterMale::run()
{
	int counter = 0;
	while(!killRunThread)
	{
		tupleQueueLock.lock();
		if ( !tupleQueue.empty() )
		{
			Tuple tuple = tupleQueue.front();
			tupleQueue.pop_front();
			tupleQueueLock.unlock();

			if (tuple.getElement(1) == "male" ) 
			{
				++counter;
				if (counter % 100 == 0)
				{
					std::cout << "Males: " << counter << std::endl;
				}
			}
		}
		else
		{
			tupleQueueLock.unlock();
		}
	}
}
