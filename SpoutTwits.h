#ifndef _SpoutTwits_H_
#define _SpoutTwits_H_

#include <string>
#include <vector>

#include "Spout.h"

class SpoutTwits : public Spout
{

private: 

	std::thread generate;

public: 
	SpoutTwits(std::string name, unsigned int parallel_level);
	virtual ~SpoutTwits();

	virtual void run();

	virtual void generateTuples();

};

#endif