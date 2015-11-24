#ifndef _BoltGender_H_
#define _BoltGender_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltFilterByGender : public Bolt
{

private: 

public: 
	BoltFilterByGender(std::string name, unsigned int parallel_level);
	virtual ~BoltFilterByGender();

	virtual void run();
};

#endif