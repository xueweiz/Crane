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

	for (int i = 0; i < bolt.getParallelLevel(); ++i)
	{
		uint32_t nodeId = rand() % nodes.size();

		std::string address = nodes.at(nodeId).ip_str;

		int connectionToServer; //TCP connect

	    int ret = connect_to_server(address.c_str(), port, &connectionToServer);
	    if(ret!=0)
	    {
	        std::cout <<"Crane::addBolt Cannot connect to "<<address<< std::endl; 
	        exit(0);
	    }
	    else
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

	        bolt.tasksAdd.push_back(address);
	        bolt.tasksPort.push_back(msg.port);

	        close(connectionToServer);
	    }   
	}

}

void Crane::run ()
{

}
