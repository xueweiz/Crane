#ifndef _SpoutCalgary_H_
#define _SpoutCalgary_H_

#include <string>
#include <vector>

#include "Spout.h"

class SpoutCalgary : public Spout
{

protected: 

	std::ifstream file;

	virtual void communicationThread();

public: 
	SpoutCalgary(std::string name, unsigned int parallel_level);
	virtual ~SpoutCalgary();

	virtual void run();
	virtual void stop();

	virtual void generateTuples();

	void initFile();
	std::string getNextAccess();

};

#endif