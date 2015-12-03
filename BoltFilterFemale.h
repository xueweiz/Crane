#ifndef _BoltFilterFemale_H_
#define _BoltFilterFemale_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltFilterFemale : public Bolt
{

private: 

public: 
	BoltFilterFemale(std::string name, unsigned int parallel_level);
	virtual ~BoltFilterFemale();

	virtual void run();

};

#endif