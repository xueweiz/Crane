#ifndef _CraneTypes_H_
#define _CraneTypes_H_

typedef enum CRANE_MessageType
{
    MSG_CREATE_TASK,
    MSG_DELETE_TASK,
    MSG_CREATE_ACK,
    MSG_SUBSCRIPTION,
    
    MSG_TASK_EMPTY
} crane_MessageType;

typedef enum CRANE_TaskType
{
    CRANE_TASK_GENDER,
    
    CRANE_TASK_EMPTY
} crane_TaskType;

struct CRANE_Message 
{
	CRANE_MessageType msgType;
	CRANE_TaskType 	  taskType;
	uint32_t boltId;
	uint32_t taskId;
	uint32_t port;
	uint32_t number;
	char string[200];
};

struct CRANE_TupleMessage 
{
	char buffer [512];
};

struct CRANE_TaskInfo
{
	uint32_t taskId;
	std::string ip_str;
	uint32_t port;
};

#endif