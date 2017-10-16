#ifndef ASSISTCOLLECTOR_H
#define ASSISTCOLLECTOR_H

#include <QTcpSocket>
#include <QList>
#include <QTimer>

#include "utilities/shared_buffer.h"

class AssistCollector : public QObject
{
    Q_OBJECT

public:
    AssistCollector();
    virtual ~AssistCollector();

private slots:
    void connectToServer();
    void checkDataStatus();

    void handleConnected();
    void handleReceivedData();
    void handleError(QAbstractSocket::SocketError error);
    void handleDisconnect();

private:
    void greetDataCenter();
    int getAuthorizationCode(char* buf, int size, char* username, char* password);

    SharedBuffer* rcvdDataBuffer;
    QTcpSocket* clientSocket;
    QString ipAddress;
    quint16 port;
    long rcvdDataLength, preRcvdDataLength;
    QTimer* connectTimer;
    QTimer* dataCheckTimer;
};

#endif // ASSISTCOLLECTOR_H
