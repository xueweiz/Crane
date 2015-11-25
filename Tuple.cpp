#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "Tuple.h"

Tuple::Tuple()
{

}

Tuple::Tuple(std::vector<std::string> v) :
	elements(v)
{

}

Tuple::Tuple(std::string str) 
{
	std::stringstream ss(str);
	std::string item;

	while (std::getline(ss, item) ) 
	{
        elements.push_back(item);
    }
}

Tuple::~Tuple()
{

}

std::string Tuple::getElement(int element)
{
	if (element > elements.size())
	{
		std::cout << "Element does not exists" << std::endl;
		return "";
	}

	return elements.at(element);
}

std::string Tuple::getSingleString()
{
	std::stringstream ss;
	for (int i = 0; i < elements.size(); ++i)
	{
		ss << elements.at(i) << std::endl;
	}
	ss << "\0";

	return ss.str();
}

std::string Tuple::getSingleStringComa()
{
	std::stringstream ss;
	for (int i = 0; i < elements.size(); ++i)
	{
		ss << elements.at(i) << ",";
	}
	std::string ret = ss.str();
	ret.pop_back();
	//ret.push_back('\n');

	return ret;
}
