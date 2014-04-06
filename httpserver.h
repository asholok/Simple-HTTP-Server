#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QHash>
#include <QDir>

class HTTPServer : public QObject
{
    Q_OBJECT
private:
    int port;
    bool serverStatus;
    QHash<QString, QString> expretionsDict;
    QString response;
    QTcpServer* server;
    QTcpSocket* clientSocket;
    QStringList createResponse(QStringList request);
    void parse();

public:
    HTTPServer(qint16 port, QObject *parent = 0);
    ~HTTPServer();
    bool isRunning() const;
    int getPort() const;
    QString getContentType(const QString& expretion);
    void setPort(int port);
    void start();
    void stop();

public slots:
    void comunicateWithClient();
    void descardClient();
    void createNewClient();

};

#endif // HTTPSERVER_H
