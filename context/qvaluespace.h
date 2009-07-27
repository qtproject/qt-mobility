/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact:  Nokia Corporation (qt-info@nokia.com)**
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met:  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.  
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.  
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QVALUESPACE_H
#define QVALUESPACE_H

#include <QVariant>
#include <QByteArray>
#include <QList>
#include <QObject>
#include <QUuid>

#include "qcontextglobal.h"

class QValueSpaceObject;

class Q_CFW_EXPORT QAbstractValueSpaceLayer : public QObject
{
Q_OBJECT
public:
#ifdef QT_ARCH_X86_64
    typedef unsigned long HANDLE;
    static const HANDLE InvalidHandle = 0xFFFFFFFFFFFFFFFF;
#else
    typedef unsigned int HANDLE;
    static const HANDLE InvalidHandle = 0xFFFFFFFF;
#endif

    /* Returns the layer name */
    virtual QString name() = 0;

    enum Type { Server, Client };
    /* Called by the value space to initialize the layer */
    virtual bool startup(Type) = 0;
    /* Called by the value space to re-initialize the layer */
    virtual bool restart() = 0;
    /* Called by the value space server to shutdown the layer */
    virtual void shutdown() = 0;

    virtual QUuid id() = 0;
    virtual unsigned int order() = 0;

    /* Returns the current value of \a handle */
    virtual bool value(HANDLE, QVariant *) = 0;
    /* Returns the current value of a subpath of handle. subPath must start
       with '/' */
    virtual bool value(HANDLE, const QByteArray &, QVariant *) = 0;

    /* Returns the list of immediate children, or an empty set if no children */
    virtual QSet<QByteArray> children(HANDLE) = 0;
    /* Returns an item handle.  Use of an invalid parent is allowed.  Returning
       an InvalidHandle means that you will never, ever expose that key or any
       sub key. Path must start with '/'*/
    virtual HANDLE item(HANDLE parent, const QByteArray &) = 0;

    enum Properties { Publish = 0x00000001 };
    /* Set a property on a handle. */
    virtual void setProperty(HANDLE handle, Properties) = 0;

    /* Removes a previously allocated handle. */
    virtual void remHandle(HANDLE) = 0;

    /* QValueSpaceItem functions */
    virtual bool requestSetValue(HANDLE handle, const QVariant &data) = 0;
    virtual bool requestSetValue(HANDLE handle, const QByteArray &path, const QVariant &data) = 0;
    virtual bool requestRemoveValue(HANDLE handle, const QByteArray &path = QByteArray()) = 0;
    virtual bool syncRequests() = 0;

    /* QValueSpaceObject functions */
    virtual bool setValue(QValueSpaceObject *creator, HANDLE handle, const QVariant &) = 0;
    virtual bool setValue(QValueSpaceObject *creator, HANDLE handle, const QByteArray &, const QVariant &) = 0;
    virtual bool removeValue(QValueSpaceObject *creator, HANDLE handle, const QByteArray &) = 0;
    virtual bool removeSubTree(QValueSpaceObject *creator, HANDLE handle) = 0;
    virtual void addWatch(QValueSpaceObject *creator, HANDLE handle) = 0;
    virtual void removeWatches(QValueSpaceObject *creator, HANDLE parent) = 0;
    virtual void sync() = 0;

signals:
    void handleChanged(unsigned int);
};

// syncqtopia header QValueSpace
namespace QValueSpace {
    Q_CFW_EXPORT void initValuespaceManager();
    void initValuespace();
    Q_CFW_EXPORT void reinitValuespace();

    typedef QAbstractValueSpaceLayer *(*LayerCreateFunc)();
    void installLayer(LayerCreateFunc func);
    void installLayer(QAbstractValueSpaceLayer * layer);

    struct AutoInstall {
        AutoInstall(LayerCreateFunc func) { installLayer(func); }
    };
};

#define QVALUESPACE_AUTO_INSTALL_LAYER(name) \
    QAbstractValueSpaceLayer * _qvaluespaceauto_layercreate_ ## name() \
    { \
        return name::instance(); \
    } \
    static QValueSpace::AutoInstall _qvaluespaceauto_ ## name(_qvaluespaceauto_layercreate_ ## name);

struct QValueSpaceItemPrivate;
class QValueSpaceSubItemIterator;
class Q_CFW_EXPORT QValueSpaceItem : public QObject
{
Q_OBJECT
public:
    QValueSpaceItem(const QValueSpaceItem &base, const QByteArray &path, QObject* parent = 0);
    QValueSpaceItem(const QValueSpaceItem &base, const QString &path, QObject* parent = 0);
    QValueSpaceItem(const QValueSpaceItem &base, const char * path, QObject* parent = 0);
    QValueSpaceItem(const QValueSpaceItem &other, QObject* parent = 0 );
    explicit QValueSpaceItem(const QByteArray &path, QObject* parent = 0);
    explicit QValueSpaceItem(const QString &path, QObject* parent = 0);
    explicit QValueSpaceItem(const char *path, QObject* parent = 0);

    explicit QValueSpaceItem(QObject* parent = 0);
    virtual ~QValueSpaceItem();

    QString itemName() const;

    QList<QString> subPaths() const;

    void remove();
    void remove(const QByteArray &subPath);
    void remove(const char *subPath);
    void remove(const QString &subPath);
    void setValue(const QVariant &value);
    void setValue(const QByteArray & subPath,
                  const QVariant &value);
    void setValue(const char * subPath,
                  const QVariant &value);
    void setValue(const QString & subPath,
                  const QVariant &value);
    bool sync();

    QVariant value(const QByteArray & subPath = QByteArray(),
                   const QVariant &def = QVariant()) const;
    QVariant value(const char * subPath,
                   const QVariant &def = QVariant()) const;
    QVariant value(const QString & subPath,
                   const QVariant &def = QVariant()) const;

    QValueSpaceItem &operator=(const QValueSpaceItem&);
signals:
    void contentsChanged();

protected:
    virtual void connectNotify(const char *);
    virtual void disconnectNotify(const char *);

private:
    QValueSpaceItemPrivate * d;
    friend class QValueSpaceSubItemIterator;
    friend class QValueSpaceSubItemIteratorPrivate;
};

#endif // _QVALUESPACE_H_
