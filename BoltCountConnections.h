#ifndef _BoltCountConnections_H_
#define _BoltCountConnections_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltCountConnections : public Bolt
{

private: 

public: 
	BoltCountConnections(std::string name, unsigned int parallel_level);
	virtual ~BoltCountConnections();

	virtual void run();

};

#endif