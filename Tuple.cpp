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

	std::string aux;

	std::getline(ss,aux,'\0'); // Here we remove '\0'

	std::stringstream ss2(aux);

	elements.clear();

	while(aux.length() > 0)
	{
		aux = "";
		std::getline(ss,aux);
		elements.push_back(aux);
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

std::string Tuple::getString4Send()
{
	std::stringstream ss;
	for (int i = 0; i < elements.size(); ++i)
	{
		ss << elements.at(i) << std::endl;
	}
	ss << "\0";

	return ss.str();
}
