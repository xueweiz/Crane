#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "BoltGender.h"



BoltFilterByGender::BoltFilterByGender(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	type = CRANE_TASK_GENDER;
}

BoltFilterByGender::~BoltFilterByGender()
{

}

void BoltFilterByGender::run(Tuple& tuple )
{
	if (tuple.getElement(1) == "male")
	{
		emit(tuple);
	}
}