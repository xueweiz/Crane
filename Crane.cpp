#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>


#include "connections.h"
#include "Crane.h"

Crane::Crane(Membership& m, uint32_t port):
	membership(m), port(port)
{

}

Crane::~Crane()
{

}

void Crane::addSpout(Bolt& spout)
{
	std::vector<Node> nodes = membership.getMembershipList();

	for (int i = 0; i < spout.getParallelLevel(); ++i)
	{
		uint32_t nodeId = rand() % nodes.size();
	}

}

void Crane::addBolt(Bolt& bolt)
{
	std::vector<Node> nodes = membership.getMembershipList();

	while (nodes.size() <= 1)
	{
		std::cout << "Crane::addBolt - No other machines present" << std::endl;
		sleep(5);
		nodes = membership.getMembershipList();
	}
	
	for (int i = 0; i < bolt.getParallelLevel(); ++i)
	{
		uint32_t nodeId = rand() % (nodes.size()-1) + 1;

		std::string address = nodes.at(nodeId).ip_str;

		int connectionToServer; //TCP connect

		int ret = connect_to_server(address.c_str(), port, &connectionToServer);

		while (ret != 0)
		{
			std::cout <<"Crane::addBolt Cannot connect to... "<<address<< std::endl; 
			sleep(2);
	        //exit(0);
		}
	    
	    {
            CRANE_Message msg;

            msg.taskType = bolt.getType();
            msg.msgType  = MSG_CREATE_TASK;

	        ret = write(connectionToServer, &msg, sizeof(CRANE_Message));

	        read(connectionToServer, &msg, sizeof(CRANE_Message));

	        if (msg.msgType != MSG_CREATE_ACK)
	        {
	        	std::cout << "Error creating task in " << address << std::endl;
	        	exit(0);
	        }

	        struct CRANE_TaskInfo newtask;
	        newtask.ip_str = address;
	        newtask.port = msg.port;
	        newtask.taskId = msg.taskId;
	        bolt.tasks.push_back(newtask);
	        
	        close(connectionToServer);
	    }   
	}

}

void Crane::run ()
{
	while(true)
	{

	}
}
