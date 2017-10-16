#include <QDebug>

#include "shared_buffer.h"
#include "common.h"

SharedBuffer::SharedBuffer(BufferType type, BufferMode mode, void* headerPointer)
{
    this->type = type;
    this->mode = mode;
    if (type == LOOP_BUFFER) {
        commonInit(headerPointer);
        bufferCapacity = header->bufferSize - sizeof(SharedBufferHeader);
        preWritePointer = 0;
        dataWriteState = false;
        startTimer(SHAREDBUFFER_WRITE_STATE_CHECK_TIMEINTERVAL);
    }
    else {
        qDebug() << "iGMASCollector:" << "Wrong buffertype when construct sharedbuffer instance";
    }
}

SharedBuffer::SharedBuffer(BufferType type, BufferMode mode, void* headerPointer, quint32 bufferSize, quint32 blockSize)
{
    this->type = type;
    this->mode = mode;
    if (type == LOOP_BUFFER) {
        commonInit(headerPointer);
        this->bufferCapacity = bufferSize - sizeof(SharedBufferHeader);
        header->bufferSize = bufferSize;
        header->blockSize = blockSize;
        header->writePointer = 0;
        header->readWriteLock = 0;
        header->stationID = 0;
        header->ipAddress = 0;
        qMemSet(dataStartPointer, 0, bufferCapacity);
    }
    else {
        qDebug() << "iGMASCollector:" << "Wrong buffertype when construct sharedbuffer instance";
    }
}

SharedBuffer::SharedBuffer(BufferType type, BufferMode mode, void* headerPointer, quint32 itemSize)
{
    this->type = type;
    this->mode = mode;
    if (type == COVER_BUFFER) {
        this->header = 0;
        this->dataStartPointer = (quint8*) headerPointer + 4;
        this->itemCount = (quint32*) headerPointer;
        this->itemSize = itemSize;
    }
    else {
        qDebug() << "iGMASCollector:" << "Wrong buffertype when construct sharedbuffer instance";
    }
}

SharedBuffer::~SharedBuffer()
{

}

quint32 SharedBuffer::readData(void* dataToRead, quint32 lengthOrCountToRead)
{
    if (mode == ONLY_WRITE) {
        return -1;
    }
    //qDebug() << "iGMASCollector:" << "Read Pointer is" << readPointer;
    if (type == LOOP_BUFFER) {
        quint32 length = 0;
        if (lengthOrCountToRead == 0 || readPointer == header->writePointer) {
            return 0;
        }
        else if (readPointer < header->writePointer) {
            if (header->writePointer - readPointer >= lengthOrCountToRead) {
                length = lengthOrCountToRead;
            }
            else {
                length = header->writePointer - readPointer;
            }
        }
        else {
            if (bufferCapacity - readPointer > lengthOrCountToRead) {
                length = lengthOrCountToRead;
            }
            else {
                length = bufferCapacity - readPointer;
                qMemCopy(dataToRead, dataStartPointer + readPointer, length);
                readPointer = 0;
                return length + readData((char*) dataToRead + length, lengthOrCountToRead - length);
            }
        }
        qMemCopy(dataToRead, dataStartPointer + readPointer, length);
        readPointer += length;
        return length;
    }
    else if (type == COVER_BUFFER) {
        if (lengthOrCountToRead == 0) {
            qMemCopy(dataToRead, dataStartPointer, itemSize * (*itemCount));
            return *itemCount;
        }
        else {
            qMemCopy(dataToRead, dataStartPointer, itemSize * lengthOrCountToRead);
            return lengthOrCountToRead;
        }
    }
    else {
        return -1;
    }
}

quint32 SharedBuffer::writeData(const void* dataFromWrite, quint32 lengthOrCountFromWrite)
{
    if (mode == ONLY_READ) {
        return -1;
    }
    if (type == LOOP_BUFFER) {
        if (lengthOrCountFromWrite > bufferCapacity) {
            return -1;
        }
        quint32 bufferTailLength = bufferCapacity - header->writePointer;
        if (bufferTailLength > lengthOrCountFromWrite) {
            qMemCopy(dataStartPointer + header->writePointer, dataFromWrite, lengthOrCountFromWrite);
            header->writePointer += lengthOrCountFromWrite;
        }
        else {
            qMemCopy(dataStartPointer + header->writePointer, dataFromWrite, bufferTailLength);
            qMemCopy(dataStartPointer, (char*) dataFromWrite + bufferTailLength, lengthOrCountFromWrite - bufferTailLength);
            header->writePointer = lengthOrCountFromWrite - bufferTailLength;
        }
    }
    else if (type == COVER_BUFFER) {
        qMemCopy(dataStartPointer, dataFromWrite, itemSize * lengthOrCountFromWrite);
        *itemCount = lengthOrCountFromWrite;
    }
    return lengthOrCountFromWrite;
}

quint32 SharedBuffer::getBufferSize() const
{
    return (header != 0) ? header->bufferSize : 0;
}

quint32 SharedBuffer::getBlockSize() const
{
    return (header != 0) ? header->blockSize : 0;
}

quint32 SharedBuffer::getWritePointer() const
{
    return (header != 0) ? header->writePointer : 0;
}

bool SharedBuffer::isDirty() const
{
    return (header != 0 && header->isDirty == 1) ? true : false;
}

void SharedBuffer::setDirty()
{
    if (header != 0) {
        header->isDirty = 1;
    }
}

quint32 SharedBuffer::getReadWriteLock() const
{
    return (header != 0) ? header->readWriteLock : 0;
}

void SharedBuffer::setStationID(quint32 stationID)
{
    if (header != 0) {
        header->stationID = stationID;
    }
}

quint32 SharedBuffer::getStationID() const
{
    return (header != 0) ? header->stationID : 0;
}

void SharedBuffer::setIPAddress(quint32 ipAddress)
{
    if (header != 0) {
        header->ipAddress = ipAddress;
    }
}

quint32 SharedBuffer::getIPAddress() const
{
    return (header != 0) ? header->ipAddress : 0;
}

void* SharedBuffer::getDataStartPointer() const
{
    return dataStartPointer;
}

quint32 SharedBuffer::getItemCount() const
{
    return *itemCount;
}

quint32 SharedBuffer::getItemSize() const
{
    return itemSize;
}

bool SharedBuffer::getDataWriteState() const
{
    return dataWriteState;
}

void SharedBuffer::timerEvent(QTimerEvent*)
{
    if (header->writePointer != preWritePointer) {
        dataWriteState = true;
        preWritePointer = header->writePointer;
    }
    else {
        dataWriteState = false;
    }
}

void SharedBuffer::commonInit(void* headerPointer)
{
    header = (SharedBufferHeader*) headerPointer;
    dataStartPointer = (quint8*) headerPointer + sizeof(SharedBufferHeader);
    readPointer = 0;
}
