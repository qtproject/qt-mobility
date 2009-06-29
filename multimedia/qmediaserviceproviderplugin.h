#ifndef QMEDIASERVICEPROVIDERPLUGIN_H
#define QMEDIASERVICEPROVIDERPLUGIN_H

#include <QtCore/qstringlist.h>
#include <QtCore/qplugin.h>
#include <QtCore/qfactoryinterface.h>


class QMediaServiceProvider;

class QAudioDeviceInfo
{
public:
    enum Mode
    {
    };
};

class QAudioFormat {};

class QAbstractAudioInput;
class QAbstractAudioOutput;
class QAbstractAudioDeviceInfo;

struct Q_MULTIMEDIA_EXPORT QMediaServiceProviderFactoryInterface : public QFactoryInterface
{
    virtual QMediaServiceProvider* create(QString const& key) = 0;
};

#define QMediaServiceProviderFactoryInterface_iid \
    "com.nokia.qt.QMediaServiceProviderFactoryInterface"
Q_DECLARE_INTERFACE(QMediaServiceProviderFactoryInterface, QMediaServiceProviderFactoryInterface_iid)

class Q_MULTIMEDIA_EXPORT QMediaServiceProviderPlugin : public QObject, public QMediaServiceProviderFactoryInterface
{
    Q_OBJECT
    Q_INTERFACES(QMediaServiceProviderFactoryInterface:QFactoryInterface)

public:
    QMediaServiceProviderPlugin(QObject *parent = 0);
    ~QMediaServiceProviderPlugin();

    virtual QStringList keys() const = 0;
    virtual QMediaServiceProvider* create(QString const& key) = 0;
};


#endif  // QMEDIASERVICEPROVIDERPLUGIN_H
