#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <unordered_map>

#include "connections.h"
#include "BoltCountHtml.h"


BoltCountHtml::BoltCountHtml(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_COUNT_HTML;
}

BoltCountHtml::~BoltCountHtml()
{

}

void BoltCountHtml::run()
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

			std::string filename = tuple.getElement(0);

			size_t pos = filename.find(".html");

			if (pos != std::string::npos) // Not found
			{

				htmlMap[filename] = htmlMap[filename] + 1;
				std::stringstream ss;
				ss << filename << std::endl;
				ss << htmlMap[filename] << std::endl;
				Tuple newTuple(ss.str());
				emit(newTuple);
			}
		}
		else
		{
			tupleQueueLock.unlock();
		}
	}
}