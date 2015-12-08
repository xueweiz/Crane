#ifndef _Spout_H_
#define _Spout_H_

#include <thread>
#include <string>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>

#include "crane_types.h"
#include "Tuple.h"
#include "connections.h"
#include "Bolt.h"

class Spout
{

protected: 

	std::string name; 

	uint32_t spoutId;
	uint32_t parallel_level;

	std::vector<struct BoltSubscriptor> subscriptors;

	std::vector<std::string> subscriptorsAdd;
	std::vector<uint32_t> subscriptorsPort;

	std::mutex tupleQueueLock;
	std::list<Tuple> tupleQueue;

	std::thread generate;
	bool killGenerateThread;

	std::thread communication;
	bool killCommunicationThread;

	virtual void communicationThread();
	virtual void generateTuples();

	virtual void emit(Tuple& tuple);
	virtual void emitAll(Tuple& tuple);

public: 

	Spout(std::string, unsigned int parallel_level);
	virtual ~Spout();

	virtual void run();
	virtual void stop();

	void subscribe(Bolt& bolt);

	uint32_t getSpoutId();
	uint32_t getParallelLevel();
	std::string getName();

	std::vector< std::vector< std::thread * > > emittingThreads;
	bool killEmittingThread;
	void emitTupleThread(int * boltNum, int * taskNum);
	
	std::vector< std::vector< TupleQueue* > > emitQueues;

	void createEmitQueues();
	void createEmittingThreads();

	void pullFromEmitQueue( std::list<Tuple>& sendQueue, TupleQueue* myQueue );

	void addToTupleQueue(TupleQueue * queue, Tuple & tuple);
};

#endif