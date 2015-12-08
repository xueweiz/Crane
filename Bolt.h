#ifndef _Bolt_H_
#define _Bolt_H_

#include <thread>
#include <string>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>

#include "FileSystem.h"
#include "crane_types.h"
#include "Tuple.h"


class Bolt
{

protected: 

	std::string name; 

	uint32_t port;
	uint32_t portSeed;

	uint32_t boltId;
	uint32_t taskId; // Universal Task ID given by the supervisor
	uint32_t parallel_level;
	uint32_t parallel_id; // Thread, or internal task

	//This structure is for implementing fancier emitions (shuffle or hashing)
	std::vector<struct BoltSubscriptor> subscriptors;

	std::vector<std::string> subscriptorsAdd;
	std::vector<uint32_t> subscriptorsPort;

	std::mutex tupleQueueLock;
	std::condition_variable tupleQueueCV;
	std::list<Tuple> tupleQueue;

	std::thread listening;
	bool killListeningThread;
	void listeningThread();	//administor

	

	std::thread runThread;
	bool killRunThread;

	CRANE_TaskType type;

public: 
	FileSystem* fs = NULL;

	int numOfElemsInTuple = -1;	//need to be set at compile time
	int numOfSubscriptor = 0;
	void robustCreateQueue();

	Bolt(std::string, unsigned int parallel_level);
	Bolt(std::string, unsigned int parallel_level, FileSystem * fs);
	virtual ~Bolt();

	virtual void run();


	/* The default function will emit to all the subscribed bolts */
	virtual void emit(Tuple& tuple);
	virtual void emitAll(Tuple& tuple);

	std::vector<struct CRANE_TaskInfo> tasks;

	void subscribe(Bolt& bolt, uint32_t cranePort);

	void addSubscriptor(std::string ip, uint32_t port, uint32_t boldId);

	uint32_t getBoltId();
	uint32_t getTaskId();
	void setTaskId(uint32_t id);
	void setBoltId(uint32_t id);

	uint32_t getParallelId();
	void setParallelId(uint32_t id);

	uint32_t getParallelLevel();

	std::string getName();

	std::string getIPByTask(uint32_t task);

	void setPort(uint32_t port);
	uint32_t getPort();

	CRANE_TaskType getType();

	struct CRANE_TaskInfo getTaskInfo(uint32_t index);

	Tuple getTuple();

	std::vector< std::vector< std::thread * > > emittingThreads;
	bool killEmittingThread;
	void emitTupleThread(int * boltNum, int * taskNum);
	
	void addToTupleQueue(TupleQueue * tupleQueue, Tuple & tuple);
	std::vector< std::vector< TupleQueue* > > emitQueues;
	void pullFromEmitQueue( std::list<Tuple>& sendQueue, TupleQueue* myQueue );

	int createEmitQueues();
	void createEmittingThreads();

	void point2PointThread(uint32_t port);	//tuple read
	void readTupleQueueFromBuffer( std::list<Tuple> & recvQueue, char * buffer );
};

#endif