#ifndef _Crane_H_
#define _Crane_H_

#include <string>
#include <vector>

#include "Membership.h"
#include "Bolt.h"
#include "Spout.h"
#include "crane_types.h"

class Crane
{

private: 
	uint32_t CraneId;

	uint32_t port;

	std::vector<Bolt> bolts;
	std::vector<Spout*> spouts;

	Membership& membership;

public: 
	Crane(Membership& m, uint32_t port);
	~Crane();

	void addSpout(Spout& spout);
	void addBolt (Bolt& bolt);

	void run();
	void stop();
};

#endif