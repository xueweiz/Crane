#ifndef _CraneTypes_H_
#define _CraneTypes_H_

typedef enum CRANE_MessageType
{
    MSG_CREATE_TASK,
    MSG_DELETE_TASK,
    MSG_CREATE_ACK,
    
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
	uint32_t port;
	uint32_t number_in;
	uint32_t number_out;
	char string_in [200];
	char string_out[200];
};

struct CRANE_TupleMessage 
{
	char buffer [512];
};

#endif