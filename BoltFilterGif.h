#ifndef _BoltFilterGif_H_
#define _BoltFilterGif_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltFilterGif : public Bolt
{

private: 

public: 
	BoltFilterGif(std::string name, unsigned int parallel_level);
	virtual ~BoltFilterGif();

	virtual void run();
};

#endif