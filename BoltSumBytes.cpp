#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <unordered_map>

#include "connections.h"
#include "BoltSumBytes.h"


BoltSumBytes::BoltSumBytes(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_SUM_BYTES;
}

BoltSumBytes::~BoltSumBytes()
{

}

void BoltSumBytes::run()
{
	int counter = 0;
	std::unordered_map<std::string,unsigned int> protocolMap;

	while(!killRunThread)
	{
		tupleQueueLock.lock();
		if ( !tupleQueue.empty() )
		{
			Tuple tuple = tupleQueue.front();
			tupleQueue.pop_front();
			tupleQueueLock.unlock();

			if(tuple.getNumElements() != 3)
			{
				continue;
			}

			std::string protocol = tuple.getElement(0);
			unsigned int sent = atoi(tuple.getElement(1).c_str());
			unsigned int recv = atoi(tuple.getElement(2).c_str());

			unsigned int sum = protocolMap[protocol];
			sum = sum + sent + recv;

			protocolMap[protocol] = sum;
			std::stringstream ss;
			ss << protocol << std::endl;
			ss << protocolMap[protocol] << std::endl;
			Tuple newTuple(ss.str());

			std::cout << newTuple.getSingleStringComa() << std::endl;
			emit(newTuple);

		}
		else
		{
			tupleQueueLock.unlock();
		}
	}
}