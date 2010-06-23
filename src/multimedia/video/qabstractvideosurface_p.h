/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QABSTRACTVIDEOSURFACE_P_H
#define QABSTRACTVIDEOSURFACE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qabstractvideosurface.h>
#include <qvideosurfaceformat.h>

#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qlist.h>
#include <QtCore/qvector.h>
#include <QtCore/qreadwritelock.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

#ifndef QOBJECT_P_H
#if (QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))

class QVariant;
class QThreadData;
class QObjectConnectionListVector;
namespace QtSharedPointer { struct ExternalRefCountData; }

enum { QObjectPrivateVersion = QT_VERSION };

class QAbstractDeclarativeData;

class Q_CORE_EXPORT QObjectPrivate : public QObjectData
{
    Q_DECLARE_PUBLIC(QObject)

public:
    struct ExtraData;
    struct Connection;
    // ConnectionList is a singly-linked list
    struct ConnectionList;
    struct Sender;

    QObjectPrivate(int version = QObjectPrivateVersion);
    virtual ~QObjectPrivate();
    void deleteChildren();

    void setParent_helper(QObject *);
    void moveToThread_helper();
    void setThreadData_helper(QThreadData *currentData, QThreadData *targetData);
    void _q_reregisterTimers(void *pointer);

    bool isSender(const QObject *receiver, const char *signal) const;
    QObjectList receiverList(const char *signal) const;
    QObjectList senderList() const;

    void addConnection(int signal, Connection *c);
    void cleanConnectionLists();

#ifdef QT3_SUPPORT
    void sendPendingChildInsertedEvents();
    void removePendingChildInsertedEvents(QObject *child);
#endif

    static inline Sender *setCurrentSender(QObject *receiver,
                                    Sender *sender);
    static inline void resetCurrentSender(QObject *receiver,
                                   Sender *currentSender,
                                   Sender *previousSender);
    static int *setDeleteWatch(QObjectPrivate *d, int *newWatch);
    static void resetDeleteWatch(QObjectPrivate *d, int *oldWatch, int deleteWatch);
    static void clearGuards(QObject *);

    static QObjectPrivate *get(QObject *o) {
        return o->d_func();
    }

    int signalIndex(const char *signalName) const;
    inline bool isSignalConnected(uint signalIdx) const;

public:
    QString objectName;
    ExtraData *extraData;    // extra data set by the user
    QThreadData *threadData; // id of the thread that owns the object

    QObjectConnectionListVector *connectionLists;

    Connection *senders;     // linked list of connections connected to this object
    Sender *currentSender;   // object currently activating the object
    mutable quint32 connectedSignals[2];

#ifdef QT3_SUPPORT
    QList<QObject *> pendingChildInsertedEvents;
#else
    // preserve binary compatibility with code compiled without Qt 3 support
    // keeping the binary layout stable helps the Qt Creator debugger
    void *unused;
#endif

    QList<QPointer<QObject> > eventFilters;
    union {
        QObject *currentChildBeingDeleted;
        QAbstractDeclarativeData *declarativeData; //extra data used by the declarative module
    };

    // these objects are all used to indicate that a QObject was deleted
    // plus QPointer, which keeps a separate list
    QAtomicPointer<QtSharedPointer::ExternalRefCountData> sharedRefcount;
    int *deleteWatch;
};

#elif (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))

class QVariant;
class QThreadData;
class QObjectConnectionListVector;
namespace QtSharedPointer { struct ExternalRefCountData; }

enum { QObjectPrivateVersion = QT_VERSION };

class QDeclarativeData;

class Q_CORE_EXPORT QObjectPrivate : public QObjectData
{
    Q_DECLARE_PUBLIC(QObject)

public:
    struct ExtraData;

    struct Connection;
    // ConnectionList is a singly-linked list
    struct ConnectionList;
    struct Sender;


    QObjectPrivate(int version = QObjectPrivateVersion);
    virtual ~QObjectPrivate();
    void deleteChildren();

    void setParent_helper(QObject *);
    void moveToThread_helper();
    void setThreadData_helper(QThreadData *currentData, QThreadData *targetData);
    void _q_reregisterTimers(void *pointer);

    bool isSender(const QObject *receiver, const char *signal) const;
    QObjectList receiverList(const char *signal) const;
    QObjectList senderList() const;

    void addConnection(int signal, Connection *c);
    void cleanConnectionLists();

#ifdef QT3_SUPPORT
    void sendPendingChildInsertedEvents();
    void removePendingChildInsertedEvents(QObject *child);
#endif

    static Sender *setCurrentSender(QObject *receiver,
                                    Sender *sender);
    static void resetCurrentSender(QObject *receiver,
                                   Sender *currentSender,
                                   Sender *previousSender);
    static int *setDeleteWatch(QObjectPrivate *d, int *newWatch);
    static void resetDeleteWatch(QObjectPrivate *d, int *oldWatch, int deleteWatch);
    static void clearGuards(QObject *);

    static QObjectPrivate *get(QObject *o) {
        return o->d_func();
    }

    int signalIndex(const char *signalName) const;
    inline bool isSignalConnected(uint signalIdx) const;

public:
    QString objectName;
    ExtraData *extraData;    // extra data set by the user
    QThreadData *threadData; // id of the thread that owns the object

    QObjectConnectionListVector *connectionLists;

    Connection *senders;     // linked list of connections connected to this object
    Sender *currentSender;   // object currently activating the object
    mutable quint32 connectedSignals[2];

#ifdef QT3_SUPPORT
    QList<QObject *> pendingChildInsertedEvents;
#else
    // preserve binary compatibility with code compiled without Qt 3 support
    // ### why?
    QList<QObject *> unused;
#endif

    QList<QPointer<QObject> > eventFilters;
    union {
        QObject *currentChildBeingDeleted;
        QDeclarativeData *declarativeData; //extra data used by the DeclarativeUI project.
    };

    // these objects are all used to indicate that a QObject was deleted
    // plus QPointer, which keeps a separate list
    QAtomicPointer<QtSharedPointer::ExternalRefCountData> sharedRefcount;
    int *deleteWatch;
};

#endif

#endif

class QAbstractVideoSurfacePrivate : public QObjectPrivate
{
public:
    QAbstractVideoSurfacePrivate()
        : error(QAbstractVideoSurface::NoError)
        , active(false)
    {
    }

    mutable QAbstractVideoSurface::Error error;
    QVideoSurfaceFormat format;
    bool active;
};

QT_END_NAMESPACE

#endif
