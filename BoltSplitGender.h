#ifndef _BoltSplitGender_H_
#define _BoltSplitGender_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltSplitGender : public Bolt
{

private: 

public: 
	BoltSplitGender(std::string name, unsigned int parallel_level);
	virtual ~BoltSplitGender();

	virtual void run();

	/* This will emit female for taskIdx 0 and male to taskIdx 1 */
	void emit(Tuple& tuple, uint32_t taskIdx);
};

#endif