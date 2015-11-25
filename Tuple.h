#ifndef _Tuple_H_
#define _Tuple_H_

#include <string>
#include <vector>
#include <unistd.h>


class Tuple
{

private: 
	std::vector<std::string> elements;

public: 
	Tuple();
	Tuple(std::vector<std::string> v);
	Tuple(std::string str);
	~Tuple();

	std::string getElement(int element);
	void setElement(int element, std::string str);

	std::string getSingleString();
	std::string getSingleStringComa();

};

#endif