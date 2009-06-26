#ifndef QWMPMETADATA_H
#define QWMPMETADATA_H

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

#include <wmp.h>

#include "qwmpglobal.h"

class Q_WMP_EXPORT QMediaMetaData : public QObject
{
    Q_OBJECT
public:
    QMediaMetaData(QObject *parent = 0) : QObject(parent) {}
    ~QMediaMetaData() {}

    virtual QStringList keys() const = 0;

    virtual int valueCount(const QString &key) const = 0;

    virtual QVariant value(const QString &key, int value = 0) const = 0;
    virtual QVariantList values(const QString &key) const = 0;

Q_SIGNALS:
    void changed();
};


class QWmpMetaData : public QMediaMetaData
{
    Q_OBJECT
public:
    QWmpMetaData(QObject *parent = 0);
    ~QWmpMetaData();

    QStringList keys() const;

    int valueCount(const QString &key) const;

    QVariant value(const QString &key, int value = 0) const;
    QVariantList values(const QString &key) const;

    IWMPMedia *media() const;
    void setMedia(IWMPMedia *media);

    static QStringList keys(IWMPMedia *media);

    static int valueCount(IWMPMedia *media, const QString &key);
    
    static QVariant value(IWMPMedia *media, const QString &key, int value);
    static QVariantList values(IWMPMedia *media, const QString &key);

private:
    IWMPMedia *m_media;
};

#endif