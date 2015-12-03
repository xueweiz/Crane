#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "connections.h"
#include "BoltAddElement.h"


BoltAddElement::BoltAddElement(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_FILTER_MALE;
}

BoltAddElement::~BoltAddElement()
{

}

void BoltAddElement::run()
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
				tuple.addElement("cero");
			}
			else if (tuple.getElement(1) == "female" ) 
			{
				tuple.addElement("one");
			}
		}
		else
		{
			tupleQueueLock.unlock();
		}
	}
}
