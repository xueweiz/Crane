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

void Crane::addSpout(Spout& spout)
{
	spouts.push_back(&spout);	
}

void Crane::addBolt(Bolt& bolt)
{
	static int boltCounter = 0;
	bolt.setBoltId(boltCounter++);

	std::vector<Node> nodes = membership.getMembershipList();

	static uint32_t nodeIdx = 0;
	while (nodes.size() <= 1)
	{
		std::cout << "Crane::addBolt - No other machines present" << std::endl;
		sleep(2);
		nodes = membership.getMembershipList();
	}
	
	for (int i = 0; i < bolt.getParallelLevel(); ++i)
	{
		uint32_t nodeId = (nodeIdx++) % (nodes.size()-1) + 1;
		// uint32_t nodeId = (nodeIdx++) % (nodes.size());

		std::string address = nodes.at(nodeId).ip_str;
		//std::cout<<"Crane::addBolt connecting to "<<address<<std::endl;

		int connectionToServer; //TCP connect
		int ret = connect_to_server(address.c_str(), port, &connectionToServer);
		while (ret != 0)
		{
			std::cout <<"Crane::addBolt Cannot connect to... "<<address<< std::endl; 
			sleep(1);
	        //exit(0);
		}
	    
	    {
            CRANE_Message msg;

            msg.taskType = bolt.getType();
            msg.msgType  = MSG_CREATE_TASK;
            msg.boltId   = bolt.getBoltId();
            msg.taskId   = i;

	        //std::cout<<"Crane::addBolt sending msg"<<address<<std::endl;
	        ret = write(connectionToServer, &msg, sizeof(CRANE_Message));

	        //std::cout<<"Crane::addBolt recving msg"<<address<<std::endl;
	        read(connectionToServer, &msg, sizeof(CRANE_Message));

	        if (msg.msgType != MSG_CREATE_ACK)
	        {
	        	std::cout << "Error creating task in " << address << std::endl;
	        	exit(0);
	        }

	        //std::cout<<"Crane::addBolt msg good. updata local data"<<address<<std::endl;
	        struct CRANE_TaskInfo newtask;
	        newtask.ip_str = address;
	        newtask.port = msg.port;
	        newtask.taskId = msg.taskId;
	        bolt.tasks.push_back(newtask);

	        close(connectionToServer);
	    }   
	}

	std::cout << "Bolt " << bolt.getBoltId() << " added to Crane topology" << std::endl;

}

void Crane::run ()
{
	for (int i = 0; i < spouts.size(); ++i)
	{
		spouts.at(i)->run();
	}
}

void Crane::stop ()
{
	for (int i = 0; i < spouts.size(); ++i)
	{
		spouts.at(i)->stop();
	}

}
