#ifndef _Tuple_H_
#define _Tuple_H_

#include <string>
#include <vector>
#include <unistd.h>


class Tuple
{
public: 
	std::vector<std::string> elements;
	
	Tuple();
	Tuple(std::vector<std::string> v);
	Tuple(std::string str);
	~Tuple();

	std::string getElement(int element);
	void setElement(int element, std::string str);
	void addElement(std::string str);

	std::string getSingleString();
	std::string getSingleStringComa();

	size_t getNumElements();

};

#endif