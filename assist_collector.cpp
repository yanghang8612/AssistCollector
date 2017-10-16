#include <QTime>
#include <QDebug>

#include "assist_collector.h"

#include "main_component_header.h"

static char encodingTable [64] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

AssistCollector::AssistCollector()
{
    rcvdDataBuffer = new SharedBuffer(
                         SharedBuffer::LOOP_BUFFER,
                         SharedBuffer::ONLY_WRITE,
                         FindMemoryInfoFunc(819, 5993),
                         5993,
                         1);

    clientSocket = new QTcpSocket(this);
    clientSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(clientSocket, SIGNAL(connected()), this, SLOT(handleConnected()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(handleReceivedData()));
    connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnect()));

    connectTimer = new QTimer(this);
    connectTimer->setInterval(3000);
    connect(connectTimer, SIGNAL(timeout()), this, SLOT(connectToServer()));
    connectTimer->start();

    dataCheckTimer = new QTimer(this);
    dataCheckTimer->setInterval(15 * 60 * 1000);
    connect(dataCheckTimer, SIGNAL(timeout()), this, SLOT(checkDataStatus()));
}

AssistCollector::~AssistCollector()
{
    delete this;
}

void AssistCollector::connectToServer()
{
    clientSocket->abort();
    clientSocket->connectToHost("106.37.201.139", 3000);
}

void AssistCollector::checkDataStatus()
{
    qDebug() << preRcvdDataLength << rcvdDataLength;
    if (rcvdDataLength == preRcvdDataLength) {
        dataCheckTimer->stop();
        connectTimer->start();
    }
    else {
        preRcvdDataLength = rcvdDataLength;
    }
}

void AssistCollector::handleConnected()
{
    qDebug() << "AssistCollector: connected";
    if (connectTimer->isActive()) {
        connectTimer->stop();
    }
    if (!dataCheckTimer->isActive()) {
        rcvdDataLength = preRcvdDataLength = 0;
        dataCheckTimer->start();
    }
    greetDataCenter();
}

void AssistCollector::handleReceivedData()
{
    QByteArray rcvdData = clientSocket->readAll();

    rcvdDataLength += rcvdDataBuffer->writeData(rcvdData.data(), rcvdData.length());
    qDebug() << QTime::currentTime() << rcvdDataLength;
}

void AssistCollector::handleError(QAbstractSocket::SocketError)
{
    qDebug() << "AssistCollector: disconnected caused by" << clientSocket->errorString();
    if (dataCheckTimer->isActive()) {
        dataCheckTimer->stop();
    }
    if (!connectTimer->isActive()) {
        connectTimer->start();
    }
}

void AssistCollector::handleDisconnect()
{
    qDebug() << "AssistCollector: disconnected";
    if (dataCheckTimer->isActive()) {
        dataCheckTimer->stop();
    }
    if (!connectTimer->isActive()) {
        connectTimer->start();
    }
}

void AssistCollector::greetDataCenter()
{
    QString greetingMessage = "";
    greetingMessage += "GET /RTA02 HTTP/1.0\r\n";
    greetingMessage += "User-Agent: NTRIP BNC/2.11.2\r\n";
    greetingMessage += "Authorization: Basic ";
    char buffer[1024];
    getAuthorizationCode(buffer, sizeof(buffer),
                         "assist",
                         "assist");
    greetingMessage += (QString(buffer) + "\r\n\r\n");
    clientSocket->write(greetingMessage.toAscii());
}

int AssistCollector::getAuthorizationCode(char* buf, int size, char* username, char* password)
{
    unsigned char inbuf[3];
    char *out = buf;
    int i, sep = 0, fill = 0, bytes = 0;

    while(*username || *password)
    {
        i = 0;
        while(i < 3 && *username) inbuf[i++] = *(username++);
        if(i < 3 && !sep)    {inbuf[i++] = ':'; ++sep; }
        while(i < 3 && *password)  inbuf[i++] = *(password++);
        while(i < 3)         {inbuf[i++] = 0; ++fill; }
        if(out-buf < size-1)
            *(out++) = encodingTable[(inbuf [0] & 0xFC) >> 2];
        if(out-buf < size-1)
            *(out++) = encodingTable[((inbuf [0] & 0x03) << 4)
                | ((inbuf [1] & 0xF0) >> 4)];
        if(out-buf < size-1)
        {
            if(fill == 2)
                *(out++) = '=';
            else
                *(out++) = encodingTable[((inbuf [1] & 0x0F) << 2)
                    | ((inbuf [2] & 0xC0) >> 6)];
        }
        if(out-buf < size-1)
        {
            if(fill >= 1)
                *(out++) = '=';
            else
                *(out++) = encodingTable[inbuf [2] & 0x3F];
        }
        bytes += 4;
    }
    if(out-buf < size)
        *out = 0;
    return bytes;
}

