#ifndef _Tuple_H_
#define _Tuple_H_

#include <string>
#include <vector>


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

	std::string getString4Send();

};

#endif