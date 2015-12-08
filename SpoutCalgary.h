#ifndef _SpoutCalgary_H_
#define _SpoutCalgary_H_

#include <string>
#include <vector>

#include "Spout.h"

class SpoutCalgary : public Spout
{

protected: 

	std::ifstream file;

	int readFileTimes = 3;	//100 will finish in 3min47s. with 1-2-1 on 3 machine. 400 will be about 15min
	int count = 0;
	//virtual void communicationThread();

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