#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <unordered_map>

#include "connections.h"
#include "BoltCountConnections.h"


BoltCountConnections::BoltCountConnections(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_COUNT_CONNECTIONS;
}

BoltCountConnections::~BoltCountConnections()
{

}

void BoltCountConnections::run()
{
	int counter = 0;
	std::unordered_map<std::string,unsigned int> htmlMap;

	while(!killRunThread)
	{
		tupleQueueLock.lock();
		if ( !tupleQueue.empty() )
		{
			Tuple tuple = tupleQueue.front();
			tupleQueue.pop_front();
			tupleQueueLock.unlock();

			std::string protocol = tuple.getElement(0);

			htmlMap[protocol] = htmlMap[protocol] + 1;
			std::stringstream ss;
			ss << protocol << std::endl;
			ss << htmlMap[protocol] << std::endl;

			std::cout<<ss.str()<<std::endl;
			
			Tuple newTuple(ss.str());
			emit(newTuple);

		}
		else
		{
			tupleQueueLock.unlock();
		}
	}
}