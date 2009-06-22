#ifndef QMEDIAPLAYLISTIOPLUGIN_H
#define QMEDIAPLAYLISTIOPLUGIN_H

#include <QtCore/qobject.h>
#include <QtCore/qplugin.h>
#include <QtCore/qfactoryinterface.h>

class QString;
class QByteArray;
class QIODevice;
class QStringList;
class QMediaSource;

class QMediaPlaylistReader
{
public:
    virtual ~QMediaPlaylistReader();

    virtual bool atEnd() const = 0;
    virtual QMediaSource readItem() = 0;
    virtual void close() = 0;
};

class QMediaPlaylistWritter
{
public:
    virtual ~QMediaPlaylistWritter();

    virtual bool writeItem(const QMediaSource& item) = 0;
    virtual void close() = 0;
};

struct QMediaPlaylistIOInterface : public QFactoryInterface
{
    virtual bool canRead(QIODevice *device, const QByteArray &format = QByteArray() ) const = 0;
    virtual bool canRead(const QString& location, const QByteArray &format = QByteArray()) const = 0;

    virtual bool canWrite(QIODevice *device, const QByteArray &format) const = 0;

    virtual QMediaPlaylistReader *createReader(QIODevice *device, const QByteArray &format = QByteArray()) = 0;
    virtual QMediaPlaylistReader *createReader(const QString& location, const QByteArray &format = QByteArray()) = 0;

    virtual QMediaPlaylistWritter *createWritter(QIODevice *device, const QByteArray &format) = 0;
};

#define QMediaPlaylistIOInterface_iid "com.nokia.Qt.QMediaPlaylistIOInterface"
Q_DECLARE_INTERFACE(QMediaPlaylistIOInterface, QMediaPlaylistIOInterface_iid);

class QMediaPlaylistIOPlugin : public QObject, public QMediaPlaylistIOInterface
{
Q_OBJECT
Q_INTERFACES(QMediaPlaylistIOInterface:QFactoryInterface)
public:
    explicit QMediaPlaylistIOPlugin(QObject *parent = 0);
    virtual ~QMediaPlaylistIOPlugin();

    virtual bool canRead(QIODevice *device, const QByteArray &format = QByteArray() ) const = 0;
    virtual bool canRead(const QString& location, const QByteArray &format = QByteArray()) const = 0;

    virtual bool canWrite(QIODevice *device, const QByteArray &format) const = 0;

    virtual QStringList keys() const = 0;

    virtual QMediaPlaylistReader *createReader(QIODevice *device, const QByteArray &format = QByteArray()) = 0;
    virtual QMediaPlaylistReader *createReader(const QString& location, const QByteArray &format = QByteArray()) = 0;

    virtual QMediaPlaylistWritter *createWritter(QIODevice *device, const QByteArray &format) = 0;
};

#endif // QMEDIAPLAYLISTIOPLUGIN_H
