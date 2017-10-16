#ifndef SHAREDBUFFER_H
#define SHAREDBUFFER_H

#include <QObject>
#include <QtGlobal>

class SharedBuffer : public QObject
{
    Q_OBJECT

public:
    enum BufferType {
        LOOP_BUFFER = 0,
        COVER_BUFFER = 1
    };

    enum BufferMode {
        ONLY_READ = 0,
        ONLY_WRITE = 1,
        READ_WRITE = 2
    };

    struct SharedBufferHeader {
        quint32 bufferSize;
        quint32 blockSize;
        quint32 writePointer;
        quint32 readWriteLock;
        quint32 stationID;
        quint32 ipAddress;
        quint32 isDirty;
        quint8 remainedBufferHeader[52];
    };

public:
    SharedBuffer(BufferType type, BufferMode mode, void* headerPointer);
    SharedBuffer(BufferType type, BufferMode mode, void* headerPointer, quint32 bufferSize, quint32 blockSize);
    SharedBuffer(BufferType type, BufferMode mode, void* headerPointer, quint32 itemSize);
    ~SharedBuffer();

    quint32 readData(void* dataToRead, quint32 lengthOrCountToRead = 0);
    quint32 writeData(const void* dataFromWrite, quint32 lengthOrCountFromWrite);

    quint32 getBufferSize() const;
    quint32 getBlockSize() const;
    quint32 getWritePointer() const;
    quint32 getReadWriteLock() const;
    bool isDirty() const;
    void setDirty();

    void setStationID(quint32 stationID);
    quint32 getStationID() const;
    void setIPAddress(quint32 ipAddress);
    quint32 getIPAddress() const;
    void* getDataStartPointer() const;

    quint32 getItemCount() const;
    quint32 getItemSize() const;

    bool getDataWriteState() const;

private:
    void timerEvent(QTimerEvent* event);

private:
    BufferType type;
    BufferMode mode;
    SharedBufferHeader* header;
    quint8* dataStartPointer;

    quint32 readPointer;
    quint32 bufferCapacity;

    quint32 preWritePointer;
    bool dataWriteState;

    quint32* itemCount;
    quint32 itemSize;

    void commonInit(void* headerPointer);
};

#endif // SHAREDBUFFER_H
