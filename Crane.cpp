#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "Crane.h"



Crane::Crane(Membership& m):
	membership(m)
{

}

Crane::~Crane()
{

}

void Crane::addSpout(Bolt& spout)
{
	std::vector<Node> nodes = membership.getMembershipList();

/*
	for (int i = 0; i < parallel_level; ++i)
	{
		uint32_t nodeId = rand() % nodes.size();
	}
*/	
}

void Crane::addBolt (Bolt& spout)
{

}

void Crane::run ()
{

}
