#ifndef _Bolt_H_
#define _Bolt_H_

#include <thread>
#include <string>
#include <vector>
#include <list>

#include "crane_types.h"
#include "Tuple.h"


class Bolt
{

private: 

	std::string name; 

	uint32_t port;

	uint32_t boltId;
	uint32_t taskId;
	uint32_t parallel_level;

	//std::vector<Bolt> subscriptors;
	std::vector<std::string> subscriptorsAdd;

	std::vector<std::string> tasksAdd; // Vector containig the tasks addresses

	std::list<Tuple> tupleQueue;

	std::thread listening;

	void listeningThread();

protected:

	CRANE_TaskType type;

	void emit(Tuple& tuple);

public: 
	Bolt(std::string, unsigned int parallel_level);
	virtual ~Bolt();

	virtual void run(Tuple& tuple );

	void subscribe(Bolt& bolt);

	uint32_t getBoltId();
	uint32_t getTaskId();

	uint32_t getParallelLevel();

	std::string getName();

	std::string getIPByTask(uint32_t task);

	std::vector<std::string> getTasksAdd();

	void setPort(uint32_t port);
	uint32_t getPort();

};

#endif