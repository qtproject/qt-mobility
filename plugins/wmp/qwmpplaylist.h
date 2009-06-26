#ifndef QWMPPLAYLIST_H
#define QWMPPLAYLIST_H

#include "qwmpglobal.h"

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

#include <wmp.h>

class Q_WMP_EXPORT QMediaPlaylist : public QObject
{
    Q_OBJECT
public:
    QMediaPlaylist(QObject *parent = 0) : QObject(parent) {}

    virtual int count() const = 0;

    virtual QStringList keys(int index) const = 0;

    virtual int valueCount(int index, const QString &key) const = 0;

    virtual QVariant value(int index, const QString &key, int value = 0) const = 0;
    virtual QVariantList values(int index, const QString &key) const = 0;

Q_SIGNALS:
    void changed();
};

class QWmpPlaylist : public QMediaPlaylist
{
    Q_OBJECT
public:
    QWmpPlaylist(QObject *parent = 0);
    ~QWmpPlaylist();

    int count() const;

    QStringList keys(int index) const;

    int valueCount(int index, const QString &key) const;

    QVariant value(int index, const QString &key, int value = 0) const;
    QVariantList values(int index, const QString &key) const;

    IWMPPlaylist *playlist() const;
    void setPlaylist(IWMPPlaylist *playlist);

    using QMediaPlaylist::changed;

private:
    IWMPPlaylist *m_playlist;
};

#endif
