#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "SpoutTwits.h"



SpoutTwits::SpoutTwits(std::string name, unsigned int parallel_level)
	: Bolt(name, parallel_level)
{
	//type = CRANE_TASK_GENDER;
}

SpoutTwits::~SpoutTwits()
{

}

void SpoutTwits::run()
{
	std::thread th(&SpoutTwits::generateTuples, this);
    generate.swap(th);
}

void SpoutTwits::generateTuples()
{

	while(true)
	{
		std::stringstream ss;

		if (rand() % 2 == 1)
		{
			ss << "Hello\n";
		}
		else
		{
			ss << "GoodBye\n";
		}
		
		if (rand() % 2 == 1)
		{
			ss << "male\n";
		}
		else
		{
			ss << "female\n";
		}

		ss << rand() % 100 << "\0";

		Tuple imaginary(ss.str());
		emit(imaginary);
		sleep(5);
	}


}