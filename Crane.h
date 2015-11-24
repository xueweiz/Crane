#ifndef _Crane_H_
#define _Crane_H_

#include <string>
#include <vector>

#include "Membership.h"
#include "Bolt.h"
#include "crane_types.h"

class Crane
{

private: 
	uint32_t CraneId;
	uint32_t taskId;
	uint32_t parallel_level;

	std::vector<Bolt> bolts;

	Membership& membership;

public: 
	Crane(Membership& m);
	~Crane();

	void addSpout(Bolt& spout);
	void addBolt (Bolt& spout);

	void run();
	
};

#endif