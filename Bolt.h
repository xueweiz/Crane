#ifndef _Bolt_H_
#define _Bolt_H_

#include <thread>
#include <string>
#include <vector>
#include <list>
#include <mutex>

#include "crane_types.h"
#include "Tuple.h"


class Bolt
{

protected: 

	std::string name; 

	uint32_t port;

	uint32_t boltId;
	uint32_t taskId;
	uint32_t parallel_level;

	//std::vector<Bolt> subscriptors;
	std::vector<std::string> subscriptorsAdd;

	std::mutex tupleQueueLock;
	std::list<Tuple> tupleQueue;

	std::thread listening;
	bool killListeningThread;

	std::thread runThread;
	bool killRunThread;

	void listeningThread();

	CRANE_TaskType type;

	void emit(Tuple& tuple);

public: 

	Bolt(std::string, unsigned int parallel_level);
	virtual ~Bolt();


	std::vector<std::string> tasksAdd; // Vector containig the tasks addresses
	std::vector<uint32_t> tasksPort; // Vector containig the tasks addresses

	virtual void run();

	void subscribe(Bolt& bolt);

	uint32_t getBoltId();
	uint32_t getTaskId();

	uint32_t getParallelLevel();

	std::string getName();

	std::string getIPByTask(uint32_t task);

	std::vector<std::string> getTasksAdd();

	void setPort(uint32_t port);
	uint32_t getPort();

	CRANE_TaskType getType();

};

#endif