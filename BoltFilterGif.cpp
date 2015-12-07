#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "connections.h"
#include "BoltFilterGif.h"


BoltFilterGif::BoltFilterGif(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_FILTER_GIF;
}

BoltFilterGif::~BoltFilterGif()
{

}

void BoltFilterGif::run()
{
	int counter = 0;
	while(!killRunThread)
	{
		
		Tuple tuple = this->getTuple();

		std::string filename = tuple.getElement(0);

		size_t pos = filename.find(".gif");

		if (pos == std::string::npos) // Not found
		{
			emit(tuple, filename);
		}
		
	}
}

void BoltFilterGif::emit(Tuple& tuple, std::string str)
{
	if (subscriptors.size() == 0)
	{
		std::cout << boltId << " - " << taskId <<" - Noone to emit to!" << std::endl;
		return;
	}

	std::vector<std::string> localSubscriptorsAdd;
	std::vector<uint32_t> connFDs;

	std::hash<std::string> hash_fn;
    std::size_t str_hash = hash_fn(str);

	for (int i = 0; i < subscriptors.size(); ++i)
	{

		size_t taskIdx = str_hash % subscriptors.at(i).tasks.size();
		if(subscriptors.at(i).tasks.size() > taskIdx)
		{
			connFDs .push_back(subscriptors.at(i).tasks.at(taskIdx).connectionFD);
			//localSubscriptorsPort.push_back(subscriptors.at(i).tasks.at(taskIdx).port);
		}
		else
		{
			std::cout << "BoltFilterGif::emit - TaskId IMPOSSIBLE" << std::endl;
			return;
		}
	}

	assert(connFDs.size() > 0);


	CRANE_TupleMessage msg;
    memset(&msg,0, sizeof(CRANE_TupleMessage));

    std::string str2Send = tuple.getSingleString();
    str2Send.copy(msg.buffer,str2Send.length(),0);

	for (int i = 0; i < connFDs.size(); ++i)
	{
        write(connFDs.at(i), (char*)&msg, sizeof(CRANE_TupleMessage));
	}

}