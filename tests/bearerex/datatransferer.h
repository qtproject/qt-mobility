#ifndef DATATRANSFERER_H
#define DATATRANSFERER_H

#include <QObject>
#include <QString>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QTcpSocket>
#include <QHttp>
#include <QDebug>

// Interface-class for data transferring object

class DataTransferer : public QObject
{
    Q_OBJECT
public:
    explicit DataTransferer(QObject *parent = 0);
    virtual ~DataTransferer() {
        if (m_dataTransferOngoing) {
            qDebug("BearerEx Warning: dataobjects transfer was ongoing when destroyed.");
        }
    }
    virtual bool transferData() = 0;
    bool dataTransferOngoing();

signals:
    void finished(quint32 errorCode, qint64 dataReceived, QString errorType);

public slots:

protected:
    bool m_dataTransferOngoing;
};


// Specializations/concrete classes

class DataTransfererQTcp : public DataTransferer
{
    Q_OBJECT
public:
    DataTransfererQTcp(QObject* parent = 0);
    ~DataTransfererQTcp();

    virtual bool transferData();

public slots:
    void readyRead();
    void error(QAbstractSocket::SocketError socketError);
    void connected();

private:
    QTcpSocket m_qsocket;
};

class DataTransfererQNam : public DataTransferer
{
    Q_OBJECT
public:
    DataTransfererQNam(QObject* parent = 0);
    ~DataTransfererQNam();

    virtual bool transferData();

public slots:
    void replyFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager m_qnam;
};

class DataTransfererQHttp : public DataTransferer
{
    Q_OBJECT
public:
    DataTransfererQHttp(QObject* parent = 0);
    ~DataTransfererQHttp();

    virtual bool transferData();

public slots:
    void done(bool error);

private:
    QHttp m_qhttp;
};

#endif // DATATRANSFERER_H
