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
    taskIdSeed = rand();

    std::thread th(&Supervisor::createListeningThread, this);
    listening.swap(th);
}

Supervisor::~Supervisor()
{

}

void Supervisor::run()
{
    while(true)
    {

    }
}

void Supervisor::createListeningThread ()
{
	int listenFd = open_socket(port); 
	portCounter = port +1;

    while(true)
    {
        size_t ret;
        int connFd = listen_socket(listenFd);

        //logFile<<"listeningThread: someone is contacting me... "<< std::endl;

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
                task->setTaskId(taskIdSeed++);
        		bolts.push_back((Bolt*)task);	

        		msg.msgType = MSG_CREATE_ACK;
        		msg.port = task->getPort();
                msg.taskId = task->getTaskId();

        		write(connFd, &msg, sizeof(CRANE_Message));
        	}

            std::cout << "Task created!" << std::endl;
        }
        else if (msgType == MSG_SUBSCRIPTION)
        {
            bool found = false;
            std::string ip(msg.string);
            for (int i = 0; i < bolts.size(); ++i)
            {
                if (bolts.at(i)->getTaskId() == msg.taskId)
                {
                    found = true;
                    bolts.at(i)->addSubscriptor(ip,msg.port);
                }
            }

            if(!found)
            {
                std::cout << "Supervisor::createListeningThread task not found" << std::endl;
                exit(0);
            }

            std::cout << "Subscription added!: " << ip << " port: " << msg.port << std::endl;
        }

        


        close(connFd);
    }

    return;
}
