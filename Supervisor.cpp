#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "connections.h"
#include "Supervisor.h"
#include "BoltGender.h"

#define logFile std::cout

Supervisor::Supervisor(uint32_t port):
	port(port)
{

}

Supervisor::~Supervisor()
{

}

void Supervisor::run()
{

}

void Supervisor::createListeningThread ()
{
	int listenFd = open_socket(port); 
	portCounter = port +1;

    while(true)
    {
        size_t ret;
        int connFd = listen_socket(listenFd);

        logFile<<"listeningThread: someone is contacting me... "<< std::endl;

        CRANE_Message msg;

        ret = read(connFd, &msg, sizeof(CRANE_Message));

        crane_MessageType msgType = msg.msgType;
		crane_TaskType 	  taskType= msg.taskType;
        
        if (msgType == MSG_CREATE_TASK)
        {
        	if (taskType == CRANE_TASK_GENDER)
        	{
        		BoltFilterByGender* task =  new BoltFilterByGender("task_gender",1);
        		task->setPort(portCounter++);
        		bolts.push_back((Bolt*)task);	

        		msg.msgType = MSG_CREATE_ACK;
        		msg.port = task->getPort();

        		write(connFd, &msg, sizeof(CRANE_Message));
        	}
        }


        close(connFd);
    }

    return;
}
