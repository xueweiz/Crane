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
	killListeningThread = true;
	std::cout << "Waiting for thread to finish.. " << std::endl;
	listening.join(); // carefull here;
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
	std::cout << "emiting tuple" << std::endl;

	for (int i = 0; i < subscriptorsAdd.size(); ++i)
	{
		// sendTCPMessage();
	}
}

void Bolt::setPort(uint32_t port)
{
	this->port = port;

	killListeningThread = true;
	std::cout << "Waiting for thread to finish.. " << std::endl;
	listening.join(); // carefull here;

	killListeningThread = false;
	std::thread th2(&Bolt::listeningThread, this);
    listening.swap(th2);

    killRunThread = false;
	std::thread th3(&Bolt::run, this);
    runThread.swap(th3);
}

uint32_t Bolt::getPort()
{
	return port;

}

void Bolt::listeningThread()
{

	int listenFd = open_socket(port); 

	size_t ret;
    int connFd = listen_socket(listenFd);

    std::cout<<"listeningThread: someone is contacting me... "<< std::endl;

	while(!killListeningThread)
    {
        CRANE_TupleMessage msg;

        ret = read(connFd, &msg, sizeof(CRANE_TupleMessage));

        std::string newTuple (msg.buffer);

        Tuple tuple(newTuple);

        tupleQueueLock.lock();
        tupleQueue.push_back(tuple);
        tupleQueueLock.unlock();
    }

    close(connFd);
}

void Bolt::run()
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

CRANE_TaskType Bolt::getType()
{
	return type;
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
