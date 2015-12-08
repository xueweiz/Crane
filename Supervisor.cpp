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

#include "BoltFilterGif.h"
#include "BoltCountHtml.h"
#include "BoltCountJpeg.h"
#include "BoltRankHtml.h"


#include "BoltRankProtocol.h"
#include "BoltRankBytes.h"
#include "BoltCountConnections.h"
#include "BoltSumBytes.h"

#define logFile std::cout

Supervisor::Supervisor(uint32_t port):
	port(port)
{
    taskIdSeed = rand() % 1000;

    std::thread th(&Supervisor::createListeningThread, this);
    listening.swap(th);
}

Supervisor::Supervisor(uint32_t port, FileSystem * fs):
    port(port), fs(fs)
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
        //std::cout<<"Supervisor::createListeningThread listening"<<std::endl;
        int connFd = listen_socket(listenFd);

        //logFile<<"listeningThread: someone is contacting me... "<< std::endl;

        CRANE_Message msg;

        ret = read(connFd, &msg, sizeof(CRANE_Message));
        //std::cout<<"Supervisor::createListeningThread recv msg"<<std::endl;

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
            else if (taskType == CRANE_TASK_FILTER_GIF)
            {
                task = new BoltFilterGif("task_add_element", 1, fs);
                std::cout << "Creating filter gif" << std::endl;
            }
            else if (taskType == CRANE_TASK_COUNT_JPEG)
            {
                task = new BoltCountJpeg("task_add_element", 1, fs);
                std::cout << "Creating count jpeg" << std::endl;
            }
            else if (taskType == CRANE_TASK_COUNT_HTML)
            {
                task = new BoltCountHtml("task_add_element",1);
                std::cout << "Creating count html" << std::endl;
            }
            else if (taskType == CRANE_TASK_RANK_HTML)
            {
                task = new BoltRankHtml("task_add_element",1);
                std::cout << "Creating rank html" << std::endl;
            }
            else if (taskType == CRANE_TASK_COUNT_CONNECTIONS)
            {
                task = new BoltCountConnections("task_add_element",1);
                std::cout << "Creating BoltCountConnections" << std::endl;
            }
            else if (taskType == CRANE_TASK_SUM_BYTES)
            {
                task = new BoltSumBytes("task_add_element",1);
                std::cout << "Creating BoltSumBytes" << std::endl;
            }
            else if (taskType == CRANE_TASK_RANK_PROTOCOL)
            {
                task = new BoltRankProtocol("task_add_element",1);
                std::cout << "Creating BoltRankProtocol" << std::endl;
            }
            else if (taskType == CRANE_TASK_RANK_BYTES)
            {
                task = new BoltRankBytes("task_add_element",1);
                std::cout << "Creating BoltRankBytes" << std::endl;
            }
            else
            {
                std::cout << "Supervisor::createListeningThread task type not found" << std::endl;
                exit(0);
            }

            task->setPort(portCounter);
            portCounter += 10;
            task->setTaskId(taskIdSeed++);
            task->setParallelId(msg.taskId);
            bolts.push_back((Bolt*)task);   

            msg.msgType = MSG_CREATE_ACK;
            msg.port = task->getPort();
            msg.taskId = task->getTaskId();

            //std::cout<<"Supervisor::createListeningThread send msg"<<std::endl;
            write(connFd, &msg, sizeof(CRANE_Message));

            //std::cout << "Supervisor::createListeningThread: Task created: " <<  msg.taskId << " - " << msg.boltId  << " at port " << msg.port << std::endl;
        }
        else if (msgType == MSG_SUBSCRIPTION)
        {
            /*if(msg.port == 0){
                std::cout << "One Bolt Done! " << std::endl;
                continue;
            }*/

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
