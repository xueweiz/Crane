#ifndef _BoltRankHtml_H_
#define _BoltRankHtml_H_

#include <string>
#include <vector>

#include "Bolt.h"


class BoltRankHtml : public Bolt
{

private: 

public: 
	BoltRankHtml(std::string name, unsigned int parallel_level);
	virtual ~BoltRankHtml();

	virtual void run();
	void emit(Tuple& tuple, std::string str);

};

#endif