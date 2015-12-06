#ifndef _BoltAddElement_H_
#define _BoltAddElement_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltAddElement : public Bolt
{

private: 

public: 
	BoltAddElement(std::string name, unsigned int parallel_level);
	virtual ~BoltAddElement();

	virtual void run();

	virtual void emit(Tuple& tuple);

};

#endif