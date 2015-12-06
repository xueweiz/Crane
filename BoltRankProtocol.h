#ifndef _BoltRankProtocol_H_
#define _BoltRankProtocol_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltRankProtocol : public Bolt
{

private: 

public: 
	BoltRankProtocol(std::string name, unsigned int parallel_level);
	virtual ~BoltRankProtocol();

	virtual void run();
	void emit(Tuple& tuple, std::string str);

};

#endif