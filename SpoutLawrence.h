#ifndef _SpoutLawrence_H_
#define _SpoutLawrence_H_

#include <string>
#include <vector>

#include "Spout.h"

class SpoutLawrence : public Spout
{

protected: 

	std::ifstream file;

	virtual void communicationThread();

public: 
	SpoutLawrence(std::string name, unsigned int parallel_level);
	virtual ~SpoutLawrence();

	virtual void run();
	virtual void stop();

	virtual void generateTuples();

	void initFile();
	std::string getNextAccess();

};

#endif