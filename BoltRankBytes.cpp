#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <unordered_map>

#include "connections.h"
#include "BoltRankBytes.h"


BoltRankBytes::BoltRankBytes(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_RANK_BYTES;
}

BoltRankBytes::~BoltRankBytes()
{

}

void BoltRankBytes::run()
{
	int counter = 0;

	std::size_t bytes = 0;
	std::string protocol = "";

	while(!killRunThread)
	{
		tupleQueueLock.lock();
		if ( !tupleQueue.empty() )
		{
			Tuple tuple = tupleQueue.front();
			tupleQueue.pop_front();
			tupleQueueLock.unlock();

			if (tuple.getNumElements() == 0)
			{
				continue;
			}

			if ( atoi(tuple.getElement(1).c_str()) > bytes)
			{
				bytes = atoi(tuple.getElement(1).c_str());
				protocol = tuple.getElement(0);

				std::cout << "More bytes: " << tuple.getSingleStringComa() << std::endl;
			}
		}
		else
		{
			tupleQueueLock.unlock();
		}
	}
}