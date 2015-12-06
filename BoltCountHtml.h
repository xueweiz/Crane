#ifndef _BoltCountHtml_H_
#define _BoltCountHtml_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltCountHtml : public Bolt
{

private: 

public: 
	BoltCountHtml(std::string name, unsigned int parallel_level);
	virtual ~BoltCountHtml();

	virtual void run();

};

#endif