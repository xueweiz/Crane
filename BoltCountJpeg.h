#ifndef _BoltCountJpeg_H_
#define _BoltCountJpeg_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltCountJpeg : public Bolt
{

private: 

public: 
	BoltCountJpeg(std::string name, unsigned int parallel_level);
	virtual ~BoltCountJpeg();

	virtual void run();

};

#endif