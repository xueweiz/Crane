#ifndef _BoltSumBytes_H_
#define _BoltSumBytes_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltSumBytes : public Bolt
{

private: 

public: 
	BoltSumBytes(std::string name, unsigned int parallel_level);
	virtual ~BoltSumBytes();

	virtual void run();

};

#endif