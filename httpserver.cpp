#include "httpserver.h"

HTTPServer::HTTPServer(qint16 port, QObject *parent) :
    QObject(parent), port(port), serverStatus(false){
    server = new QTcpServer();
    response = "HTTP/1.1 %1 OK\r\nConnection: close\r\nContent-Length: %2\r\nContent-Type: %3\r\nDate: %4\r\n\r\n";
    qDebug() << QDir::currentPath() << "    !!!!    ";
    parse();
}

HTTPServer::~HTTPServer(){
    stop();
}

bool HTTPServer::isRunning() const{
    return this->serverStatus;
}

int HTTPServer::getPort() const{
    return this->port;
}

void HTTPServer::parse(){
    QFile sorce("mime.types");

    if ( sorce.open(QIODevice::ReadOnly) ) {
        QByteArray content = sorce.readAll();
        QTextStream in(&content);

        qDebug() << "mime.types";
        while ( !in.atEnd() ) {
            QStringList block = QString(in.readLine()).split(QRegExp("\\s+"));
            QString key = block.takeFirst();
            QString value = block.join(" ");

            if ( key != "" ) {
                expretionsDict.insert(key, value);
            }
        }
    }
}

QString HTTPServer::getContentType(const QString& expretion){
    if ( expretionsDict.contains(expretion) ) {
        return expretionsDict.value(expretion);
    }
    return "application/octet-stream";
}

void HTTPServer::setPort(int port){
    this->port = port;
}

void HTTPServer::start(){
    if ( serverStatus )
        return;

    if ( !server->listen(QHostAddress::LocalHost, this->port) ) {
        qDebug() << "Server could not start!";
        return;
    }
    qDebug() << "Server started!";
    connect(server, SIGNAL(newConnection()),this,SLOT(createNewClient()));
    this->serverStatus = true;
}

void HTTPServer::stop() {
    clientSocket->close();
    server->close();
}

void HTTPServer::createNewClient(){
    clientSocket = server->nextPendingConnection();

    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(comunicateWithClient()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(descardClient()));
}

void HTTPServer::comunicateWithClient(){
    if ( clientSocket->canReadLine() ) {
        QStringList requestContent = QString(clientSocket->readLine()).split(QRegExp("\\s+"));
        QStringList content = createResponse(requestContent);

        clientSocket->write(content[0].toUtf8());
        qDebug() << content[1];
    }
}

void HTTPServer::descardClient(){
    clientSocket->deleteLater();
}

QStringList HTTPServer::createResponse(QStringList request){
    QStringList result;
    QString body, status, contentType, fileName;
    QString date = QDateTime::currentDateTime().toString();
    QString responseContent = response;

    if ( request[0] == "GET" ) {
        QString path = request[1];
        QFile* file;

        if ( path.endsWith("/") ) {
            path.append("index.html");
        }
        file = new QFile(path);
        if ( !file->open(QIODevice::ReadOnly) ) {
            body = "404 Not Found\r\n";
            status = "404";
            contentType = "file not found";
            fileName = "/some-strange-url.notfound";
        } else {
            body = QString(file->readAll());
            fileName = file->fileName();
            status = "200";
            contentType = getContentType(QFileInfo(fileName).completeSuffix());
            fileName = path;
        }
        file->close();
    } else {
        body = "405 Method Not Allowed\r\n";
        status = "405";
        contentType = "none";
        fileName = "/url.forbiden";
    }
    responseContent.arg(status, "%1", contentType, date);
    responseContent.arg(QString::number(response.length()));
    responseContent.append(body);
    result.append(responseContent);
    result.append(status+" "+fileName);

    return result;
}


