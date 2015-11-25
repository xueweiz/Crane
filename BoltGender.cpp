#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "BoltGender.h"



BoltFilterByGender::BoltFilterByGender(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_GENDER;
}

BoltFilterByGender::~BoltFilterByGender()
{

}

void BoltFilterByGender::run()
{
	while(!killRunThread)
	{
		tupleQueueLock.lock();
		if ( !tupleQueue.empty() )
		{
			Tuple tuple = tupleQueue.front();
			tupleQueue.pop_front();
			tupleQueueLock.unlock();

			if (tuple.getElement(1) == "male")
			{
				emit(tuple);
			}
		}
		else
		{
			tupleQueueLock.unlock();
		}
	}

}