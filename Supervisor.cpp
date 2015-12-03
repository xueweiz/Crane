#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

#include "connections.h"
#include "Supervisor.h"
#include "BoltGender.h"
#include "BoltSplitGender.h"
#include "BoltFilterMale.h"
#include "BoltFilterFemale.h"
#include "BoltAddElement.h"

#define logFile std::cout

Supervisor::Supervisor(uint32_t port):
	port(port)
{
    taskIdSeed = rand() % 1000;

    std::thread th(&Supervisor::createListeningThread, this);
    listening.swap(th);
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

        //logFile<<"listeningThread: someone is contacting me... "<< std::endl;

        CRANE_Message msg;

        ret = read(connFd, &msg, sizeof(CRANE_Message));

        crane_MessageType msgType = msg.msgType;
		crane_TaskType 	  taskType= msg.taskType;
        
        if (msgType == MSG_CREATE_TASK)
        {
            Bolt* task;
        	if (taskType == CRANE_TASK_GENDER)
        	{
        		task = new BoltFilterByGender("task_gender",1);
        	}
            else if (taskType == CRANE_TASK_SPLIT_GENDER)
            {
                task = new BoltSplitGender("task_split_gender",1);
            }
            else if (taskType == CRANE_TASK_FILTER_MALE)
            {
                task = new BoltFilterMale("task_filter_male",1);
                std::cout << "Creating male task" << std::endl;
            }
            else if (taskType == CRANE_TASK_FILTER_FEMALE)
            {
                task = new BoltFilterFemale("task_filter_female",1);
                std::cout << "Creating female task" << std::endl;
            }
            else if (taskType == CRANE_TASK_ADD_ELEMENT)
            {
                task = new BoltAddElement("task_add_element",1);
                std::cout << "Creating add element task" << std::endl;
            }
            else
            {
                std::cout << "Supervisor::createListeningThread task type not found" << std::endl;
                exit(0);
            }

            task->setPort(portCounter++);
            task->setTaskId(taskIdSeed++);
            task->setParallelId(msg.taskId);
            bolts.push_back((Bolt*)task);   

            msg.msgType = MSG_CREATE_ACK;
            msg.port = task->getPort();
            msg.taskId = task->getTaskId();

            write(connFd, &msg, sizeof(CRANE_Message));

            //std::cout << "Task created: " <<  msg.taskId << " - " << msg.boltId  << " at port " << msg.port << std::endl;
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
                    bolts.at(i)->addSubscriptor(ip,msg.port, msg.boltId);
                }
            }

            if(!found)
            {
                std::cout << "Supervisor::createListeningThread task not found" << std::endl;
                exit(0);
            }

            //std::cout << "Subscription added!: " << msg.taskId << " - " << msg.boltId << " " << ip << " port: " << msg.port << std::endl;
        }

        


        close(connFd);
    }

    return;
}
