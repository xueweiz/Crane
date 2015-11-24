#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "connections.h"
#include "Bolt.h"

Bolt::Bolt(std::string name, uint32_t parallel_level) :
	name(name), parallel_level(parallel_level)
{
	type = CRANE_TASK_EMPTY;
}

Bolt::~Bolt()
{

}

void Bolt::subscribe(Bolt& bolt)
{
	//subscriptors.push_back(bolt);

	std::vector<std::string> adds = bolt.getTasksAdd();
	for (int i = 0; i < adds.size(); ++i)
	{
		subscriptorsAdd.push_back(adds.at(i));
	}
}

void Bolt::emit(Tuple& tuple)
{
	for (int i = 0; i < subscriptorsAdd.size(); ++i)
	{
		// sendTCPMessage();
	}
}

void Bolt::setPort(uint32_t port)
{
	this->port = port;

	std::thread th2(&Bolt::listeningThread, this);
    listening.swap(th2);

}

uint32_t Bolt::getPort()
{
	return port;

}

void Bolt::listeningThread()
{

	int listenFd = open_socket(port); 

	while(true)
    {
        size_t ret;
        int connFd = listen_socket(listenFd);

        std::cout<<"listeningThread: someone is contacting me... "<< std::endl;

        CRANE_TupleMessage msg;

        ret = read(connFd, &msg, sizeof(CRANE_TupleMessage));

        std::string newTuple (msg.buffer);

        Tuple tuple(newTuple);

        tupleQueue.push_back(tuple);

        close(connFd);
    }

}

void Bolt::run(Tuple& tuple)
{
	std::cout << "Runing pure bolt run method, this is wrong" << std::endl;
}

uint32_t Bolt::getBoltId()
{
	return boltId;
}

uint32_t Bolt::getTaskId()
{
	return taskId;
}

uint32_t Bolt::getParallelLevel()
{
	return parallel_level;
}

std::string Bolt::getName()
{
	return name;
}

std::string Bolt::getIPByTask(uint32_t task)
{
	assert(tasksAdd.size() == parallel_level);
	if (task > parallel_level)
	{
		std::cout << "Bolt::getIPByTask - task " << task << " does not exist" << std::endl;
		exit(0);
		return "";
	}

	return tasksAdd.at(task);
}

std::vector<std::string> Bolt::getTasksAdd()
{
	return tasksAdd;
}
