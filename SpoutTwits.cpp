#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "SpoutTwits.h"



SpoutTwits::SpoutTwits(std::string name, unsigned int parallel_level)
	: Spout(name, parallel_level)
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
	uint32_t counter = 0;
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

		ss << counter++ << "\0";

		Tuple imaginary(ss.str());
		emit(imaginary);
		//sleep(5);
	}


}