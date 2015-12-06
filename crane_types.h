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
    CRANE_TASK_SPLIT_GENDER,
    CRANE_TASK_FILTER_MALE,
    CRANE_TASK_FILTER_FEMALE,
    CRANE_TASK_ADD_ELEMENT,

    CRANE_TASK_FILTER_GIF,
    CRANE_TASK_COUNT_JPEG,
    CRANE_TASK_COUNT_HTML,
    CRANE_TASK_RANK_HTML,

    CRANE_TASK_COUNT_CONNECTIONS,
    CRANE_TASK_SUM_BYTES,
    CRANE_TASK_RANK_PROTOCOL,
    CRANE_TASK_RANK_BYTES,
    
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
	char buffer [256];
	// How many more messages are missing. 
	//0 means it is only one
	char more; 
	char ack;
};

struct CRANE_TaskInfo
{
	uint32_t taskId;
	std::string ip_str;
	uint32_t port;
	uint32_t connectionFD;
};

struct BoltSubscriptor
{
	uint32_t boltId;
	std::vector<struct CRANE_TaskInfo> tasks;
};

#endif