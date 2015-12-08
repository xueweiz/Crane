#ifndef _Supervisor_H_
#define _Supervisor_H_

#include <string>
#include <vector>
#include <thread>

#include "Bolt.h"
#include "crane_types.h"

/* 
This is the container that will run in each machine
This will handle all the tasks of the bolts
*/

class Supervisor
{

private: 

	uint32_t port;
	uint32_t portCounter;
	std::vector<Bolt* > bolts;

	uint32_t taskIdSeed;

	std::thread listening;

public: 
	FileSystem * fs;

	Supervisor(uint32_t port);
	Supervisor(uint32_t port, FileSystem * fs);
	~Supervisor();

	void run();
	void createListeningThread();
	

};

#endif