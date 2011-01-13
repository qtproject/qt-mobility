#ifndef QDECLARATIVEBLUETOOTHSERVICE_P_H
#define QDECLARATIVEBLUETOOTHSERVICE_P_H

#include <QObject>
#include <qdeclarative.h>
#include <bluetooth/qbluetoothserviceinfo.h>

QTM_USE_NAMESPACE

class QDeclarativeBluetoothServicePrivate;

class QDeclarativeBluetoothService : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ deviceName NOTIFY detailsChanged)
    Q_PROPERTY(QString deviceAddress READ deviceAddress WRITE setDeviceAddress NOTIFY detailsChanged)
    Q_PROPERTY(QString serviceName READ serviceName WRITE setServiceName NOTIFY detailsChanged)
    Q_PROPERTY(QString serviceDescription READ serviceDescription WRITE setServiceDescription NOTIFY detailsChanged)
    Q_PROPERTY(QString serviceUuid READ serviceUuid WRITE setServiceUuid NOTIFY detailsChanged)
    Q_PROPERTY(QString serviceProtocol READ serviceProtocol WRITE setServiceProtocol NOTIFY detailsChanged)
    Q_PROPERTY(qint32 servicePort READ servicePort WRITE setServicePort NOTIFY detailsChanged)
    Q_PROPERTY(bool registered READ isRegistered WRITE setRegistered NOTIFY registeredChanged)

    Q_INTERFACES(QDeclarativeParserStatus)

public:
    explicit QDeclarativeBluetoothService(QObject *parent = 0);
    explicit QDeclarativeBluetoothService(const QBluetoothServiceInfo &service,
                                          QObject *parent = 0);
    ~QDeclarativeBluetoothService();

    QString deviceName() const;
    QString deviceAddress() const;
    QString serviceName() const;
    QString serviceDescription() const;
    QString serviceUuid() const;
    QString serviceProtocol() const;
    qint32 servicePort() const;
    bool isRegistered() const;

    QBluetoothServiceInfo *serviceInfo() const;

    // From QDeclarativeParserStatus
    void classBegin() {}
    void componentComplete();

signals:
    void detailsChanged();
    void registeredChanged();

public slots:
    void setServiceName(QString name);
    void setDeviceAddress(QString address);
    void setServiceDescription(QString description);
    void setServiceUuid(QString uuid);
    void setServiceProtocol(QString protocol);
    void setServicePort(qint32 port);
    void setRegistered(bool registered);

private:
    QDeclarativeBluetoothServicePrivate* d;
    friend class QDeclarativeBluetoothServicePrivate;

};

QML_DECLARE_TYPE(QDeclarativeBluetoothService)

#endif // QDECLARATIVEBLUETOOTHSERVICE_P_H
