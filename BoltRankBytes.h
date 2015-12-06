#ifndef _BoltRankBytes_H_
#define _BoltRankBytes_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltRankBytes : public Bolt
{

private: 

public: 
	BoltRankBytes(std::string name, unsigned int parallel_level);
	virtual ~BoltRankBytes();

	virtual void run();
	void emit(Tuple& tuple, std::string str);

};

#endif