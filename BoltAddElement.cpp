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
	type = CRANE_TASK_ADD_ELEMENT;
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
				emit(tuple);
			}
			else if (tuple.getElement(1) == "female" ) 
			{
				tuple.addElement("one");
				emit(tuple);
			}
		}
		else
		{
			tupleQueueLock.unlock();
		}
	}
}


void BoltAddElement::emit(Tuple& tuple)
{
	if (subscriptors.size() == 0)
	{
		std::cout << boltId << " - " << taskId <<" - Noone to emit to!" << std::endl;
		return;
	}

	std::vector<std::string> localSubscriptorsAdd;
	std::vector<uint32_t> connFDs;

	for (int i = 0; i < subscriptors.size(); ++i)
	{
		uint32_t taskIdx = rand() % subscriptors.at(i).tasks.size(); 
		connFDs .push_back(subscriptors.at(i).tasks.at(taskIdx).connectionFD);
	}

	assert(connFDs.size() > 0);

	//std::cout << "emiting tuple: " << tuple.getSingleStringComa() << std::endl;

	CRANE_TupleMessage msg;
    memset(&msg,0, sizeof(CRANE_TupleMessage));

    std::string str2Send = tuple.getSingleString();
    str2Send.copy(msg.buffer,str2Send.length(),0);

	for (int i = 0; i < connFDs.size(); ++i)
	{
        write(connFDs.at(i), (char*)&msg, sizeof(CRANE_TupleMessage));
	}

}