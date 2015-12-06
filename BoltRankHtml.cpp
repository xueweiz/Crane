#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <unordered_map>

#include "connections.h"
#include "BoltRankHtml.h"


BoltRankHtml::BoltRankHtml(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_RANK_HTML;
}

BoltRankHtml::~BoltRankHtml()
{

}

void BoltRankHtml::run()
{
	int counter = 0;

	std::size_t visits = 0;
	std::string page = "";

	while(!killRunThread)
	{
		tupleQueueLock.lock();
		if ( !tupleQueue.empty() )
		{
			Tuple tuple = tupleQueue.front();
			tupleQueue.pop_front();
			tupleQueueLock.unlock();

			if ( atoi(tuple.getElement(1).c_str()) > visits && tuple.getElement(0) != "index.html" )
			{
				visits = atoi(tuple.getElement(1).c_str());
				page = tuple.getElement(0);

				//emit(tuple); // Most ranked page
				std::cout << "Most visited: " << tuple.getSingleStringComa() << std::endl;
			}
		}
		else
		{
			tupleQueueLock.unlock();
		}
	}
}