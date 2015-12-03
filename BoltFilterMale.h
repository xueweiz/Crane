#ifndef _BoltFilterMale_H_
#define _BoltFilterMale_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltFilterMale : public Bolt
{

private: 

public: 
	BoltFilterMale(std::string name, unsigned int parallel_level);
	virtual ~BoltFilterMale();

	virtual void run();

};

#endif