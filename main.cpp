#include <QMap>
#include <QCoreApplication>

#include "main_component_header.h"
#include "assist_collector.h"

FINDMEMORYINFOFUNC FindMemoryInfoFunc;

void* find(int memID, uint memLength)
{
    static QMap<int, void*> map;
    if (!map.contains(memID)) {
        void* pointer = malloc(memLength);
        qMemSet(pointer, 0, memLength);
        map.insert(memID, pointer);
    }
    return map[memID];
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    FindMemoryInfoFunc = find;

    AssistCollector* collector = new AssistCollector();

    return a.exec();
}
