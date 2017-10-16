#include <QList>

#include "common.h"
#include "assist_collector.h"

bool DllMain(int argc, char* argv[])
{
    return true;
}

bool DllInit(int argc, char* argv[])
{
    new AssistCollector();
    return true;
}

bool DllStart()
{
    return true;
}

bool DllStop()
{
    return true;
}

bool DllContraryInit()
{
    return true;
}
