#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "connections.h"
#include "BoltSplitGender.h"


BoltSplitGender::BoltSplitGender(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_SPLIT_GENDER;
}

BoltSplitGender::~BoltSplitGender()
{

}

void BoltSplitGender::run()
{
	while(!killRunThread)
	{
		tupleQueueLock.lock();
		if ( !tupleQueue.empty() )
		{
			Tuple tuple = tupleQueue.front();
			tupleQueue.pop_front();
			tupleQueueLock.unlock();

			if (tuple.getElement(1) == "male" ) //&& atoi(tuple.getElement(2).c_str()) % 2 == 0)
			{
				emit(tuple, 0);
			}
			else if ( (tuple.getElement(1) == "female" && atoi(tuple.getElement(2).c_str()) % 2 == 0) )
			{
				emit(tuple, 1);
			}
		}
		else
		{
			tupleQueueLock.unlock();
		}
	}
}

void BoltSplitGender::emit(Tuple& tuple, uint32_t taskIdx)
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
		if(subscriptors.at(i).tasks.size() > taskIdx)
		{
			connFDs .push_back(subscriptors.at(i).tasks.at(taskIdx).connectionFD);
			//localSubscriptorsPort.push_back(subscriptors.at(i).tasks.at(taskIdx).port);
		}
		else
		{
			std::cout << "BoltSplitGender::emit - TaskId IMPOSSIBLE" << std::endl;
			return;
		}
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

/*
	for (int i = 0; i < localSubscriptorsAdd.size(); ++i)
	{
		int connectionToServer; //TCP connect

		int ret = connect_to_server(localSubscriptorsAdd.at(i).c_str(), localSubscriptorsPort.at(i), &connectionToServer);

		while (ret != 0)
		{
			std::cout <<"Bolt::emit Cannot connect to... "<<(localSubscriptorsAdd.at(i))<< std::endl; 
			sleep(1);
		}
	    
        CRANE_TupleMessage msg;
        memset(msg.buffer,0,512);

        msg.more = 0;
        std::string str2Send = tuple.getSingleString();
        str2Send.copy(msg.buffer,str2Send.length(),0);

        write(connectionToServer, &msg, sizeof(CRANE_Message));

        close(connectionToServer);
	}
*/
}