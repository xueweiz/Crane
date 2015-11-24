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
	std::vector<uint32_t> subscriptorsPort;

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

	virtual void run();

	std::vector<struct CRANE_TaskInfo> tasks;

	void subscribe(Bolt& bolt, uint32_t port);

	void addSubscriptor(std::string ip, uint32_t port);

	uint32_t getBoltId();
	uint32_t getTaskId();

	void setTaskId(uint32_t id);

	uint32_t getParallelLevel();

	std::string getName();

	std::string getIPByTask(uint32_t task);

	void setPort(uint32_t port);
	uint32_t getPort();

	CRANE_TaskType getType();

	struct CRANE_TaskInfo getTaskInfo(uint32_t index);

};

#endif