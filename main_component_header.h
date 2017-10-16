#ifndef MAIN_COMPONENT_HEADER
#define MAIN_COMPONENT_HEADER

#include <QObject>
#include <QtGlobal>
#include <ctime>

struct SoftWorkStatus {
    char messageContent[128];
    time_t messageTime;
    char messageType;
};

typedef void* (*FINDMEMORYINFOFUNC)(int, uint);//内存申请函数指针
typedef void  (*DLLSTATUSREADFUNC)(int*, int);//软件运行状态读函数指针
typedef bool  (*DLLSTATUSWRITEFUNC)(int);//软件运行状态写函数指针
typedef bool  (*SOFTWORKSTATUSWRITEFUNC)(int,SoftWorkStatus);//软件工作状态写函数指针

extern FINDMEMORYINFOFUNC FindMemoryInfoFunc;
extern DLLSTATUSREADFUNC  DllStatusReadFunc;
extern DLLSTATUSWRITEFUNC DllStatusWriteFunc;
extern SOFTWORKSTATUSWRITEFUNC SoftWorkStatusWriteFunc;

typedef bool (*REGISTERSIGNALLISTENER)(QObject* listener, const char* slotName, const char* signalName);//信号监听者注册函数指针
extern REGISTERSIGNALLISTENER registerSignalListener;

typedef bool (*REGISTERSIGNALSENDER)(QObject* sender);//信号发送者注册函数
extern REGISTERSIGNALSENDER registerSignalSender;

#endif // MAIN_COMPONENT_HEADER

