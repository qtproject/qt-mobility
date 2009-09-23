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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qvaluespaceobject.h"
#include "qvaluespace.h"
#include "qvaluespace_p.h"
#include "qvaluespacemanager_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qthread.h>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QValueSpaceObject

    \brief The QValueSpaceObject class allows applications to add entries to the Value Space.

    For an overview of the Qt Value Space, please see the QValueSpaceItem documentation.

    The QValueSpaceObject class allows applications to write entries into the
    Value Space that are automatically removed when the QValueSpaceObject is
    destroyed, or the application exits either cleanly or abnormally.  All
    applications in the system will have access to the data set through
    QValueSpaceObject and, if desired, can be notified when the data changes.

    Although, logically, the Value Space is a simple collection of
    hierarchical paths, these paths can conceptually be visualized as a set of
    objects with attributes.  For example, rather than viewing the following list
    as 12 distinct Value Space paths:

    \code
    /Device/Network/Interfaces/eth0/Name
    /Device/Network/Interfaces/eth0/Type
    /Device/Network/Interfaces/eth0/Status
    /Device/Network/Interfaces/eth0/BytesSent
    /Device/Network/Interfaces/eth0/BytesReceived
    /Device/Network/Interfaces/eth0/Time
    /Device/Network/Interfaces/ppp0/Name
    /Device/Network/Interfaces/ppp0/Type
    /Device/Network/Interfaces/ppp0/Status
    /Device/Network/Interfaces/ppp0/BytesSent
    /Device/Network/Interfaces/ppp0/BytesReceived
    /Device/Network/Interfaces/ppp0/Time
    \endcode

    it can be thought of as describing two Value Space objects,
    \c { { /Device/Network/Interfaces/eth0, /Device/Network/Interfaces/ppp0 } },
    each with the six attributes \c { {Name, Type, Status, BytesSent,
    BytesReceived, Time} }.  The QValueSpaceObject class encapsulates this
    abstraction.

    In the case of two or more applications creating an application object with
    overlapping attributes, only the first is visible to observers in the system.
    The other attributes are not discarded, but are buffered until the first
    releases its hold on the attribute, either by manually removing it, destroying
    the QValueSpaceObject or by terminating.  For example:

    \code
    QValueSpaceObject *object1 = new QValueSpaceObject("/Device");
    object1->setAttribute("Buttons", 2);

    // QValueSpaceItem("/Device/Buttons").value() == QVariant(2)

    QValueSpaceObject *object2 = new QValueSpaceObject("/Device");
    object2->setAttribute("Buttons", 3);

    // QValueSpaceItem("/Device/Buttons").value() == QVariant(2)

    object2->removeAttribute("Buttons");
    // QValueSpaceItem("/Device/Buttons").value() == QVariant(3)
    \endcode

    For performance reasons the setting of and removing of attributes is buffered
    internally by the QValueSpaceObject and applied as a batch sometime later.
    Normally this occurs the next time the application enters the Qt event loop,
    but this behavior should not be relied upon.  If an application must
    synchronize application objects with others, the QValueSpaceObject::sync()
    method can be used to force the application of changes.  This call is
    generally unnecessary, and should be used sparingly to prevent unnecessary
    load on the system.

    \i {Note:} The QValueSpaceObject class is not thread safe and may only be used from
    an application's main thread.

    \sa QValueSpaceItem
*/

/*!
    \fn void QValueSpaceObject::itemNotify(const QByteArray &attribute, bool interested)

    Signal that is emitted when interest in \a attribute changes.  If \a interested is true at
    least on QValueSpaceItem is interested in the value of \a attribute.
*/

#define VS_CALL_ASSERT Q_ASSERT(!QCoreApplication::instance() || QCoreApplication::instance()->thread() == QThread::currentThread());

class QValueSpaceObjectPrivate
{
public:
    QValueSpaceObjectPrivate(const QByteArray &objectPath,
                             QValueSpace::LayerOptions filter = QValueSpace::UnspecifiedLayer);
    QValueSpaceObjectPrivate(const QByteArray &objectPath, const QUuid &uuid);

    QByteArray path;

    QAbstractValueSpaceLayer *layer;
    QAbstractValueSpaceLayer::Handle handle;

    bool hasSet;
    bool hasWatch;
};

QValueSpaceObjectPrivate::QValueSpaceObjectPrivate(const QByteArray &objectPath,
                                                   QValueSpace::LayerOptions filter)
:   layer(0), handle(QAbstractValueSpaceLayer::InvalidHandle), hasSet(false), hasWatch(false)
{
    path = qCanonicalPath(objectPath);

    if ((filter & QValueSpace::PermanentLayer &&
         filter & QValueSpace::NonPermanentLayer) ||
        (filter & QValueSpace::WriteableLayer &&
         filter & QValueSpace::NonWriteableLayer)) {
        return;
    }

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    for (int ii = 0; ii < layers.count(); ++ii) {
        if (filter == QValueSpace::UnspecifiedLayer ||
            layers.at(ii)->layerOptions() & filter) {
            QAbstractValueSpaceLayer::Handle h =
                    layers.at(ii)->item(QAbstractValueSpaceLayer::InvalidHandle, path);

            if (h != QAbstractValueSpaceLayer::InvalidHandle) {
                layer = layers.at(ii);
                handle = h;
                break;
            }
        }
    }
}

QValueSpaceObjectPrivate::QValueSpaceObjectPrivate(const QByteArray &objectPath, const QUuid &uuid)
:   layer(0), handle(QAbstractValueSpaceLayer::InvalidHandle), hasSet(false), hasWatch(false)
{
    path = qCanonicalPath(objectPath);

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    for (int ii = 0; ii < layers.count(); ++ii) {
        if (layers.at(ii)->id() == uuid) {
            layer = layers.at(ii);
            handle = layer->item(QAbstractValueSpaceLayer::InvalidHandle, path);
            break;
        }
    }
}

/*!
    \overload

    Constructs a QValueSpaceObject with the specified \a parent that publishes values under
    \a path.  This constructor is equivalent to calling
    \c {QValueSpaceItem(path.toUtf8(), parent)}.
*/
QValueSpaceObject::QValueSpaceObject(const QString &path, QObject *parent)
:   QObject(parent), d(new QValueSpaceObjectPrivate(path.toUtf8()))
{
    VS_CALL_ASSERT;
    QValueSpaceManager::instance()->init();
}

/*!
    \overload

    Constructs a QValueSpaceObject with the specified \a parent that publishes values under
    \a path.  This constructor is equivalent to calling
    \c {QValueSpaceItem(QByteArray(path), parent)}.
*/
QValueSpaceObject::QValueSpaceObject(const char *path, QObject *parent)
:   QObject(parent), d(new QValueSpaceObjectPrivate(QByteArray(path)))
{
    VS_CALL_ASSERT;
    QValueSpaceManager::instance()->init();
}

/*!
    \overload

    Constructs a QValueSpaceObject with the specified \a parent that publishes values under
    \a path.  The \a filter parameter is used to limit which layer this QValueSpaceObject will
    access.  This constructor is equivalent to calling
    \c {QValueSpaceObject(path.toUtf8(), filter, parent)}.

    The constructed Value Space object will access the \l {QAbstractValueSpaceLayer}{layer} with
    the highest \l {QAbstractValueSpaceLayer::order()}{order} that matches \a filter and for which
    \a path is a valid path.  If no suitable \l {QAbstractValueSpaceLayer}{layer} is found an
    invalid QValueSpaceObject is constructed.

    \sa isValid()
*/
QValueSpaceObject::QValueSpaceObject(const QString &path,
                                     QValueSpace::LayerOptions filter,
                                     QObject *parent)
:   QObject(parent), d(new QValueSpaceObjectPrivate(path.toUtf8(), filter))
{
    VS_CALL_ASSERT;
    QValueSpaceManager::instance()->init();
}

/*!
    \overload

    Constructs a QValueSpaceObject with the specified \a parent that publishes values under
    \a path.  The \a filter parameter is used to limit which layer this QValueSpaceObject will
    access.  This constructor is equivalent to calling
    \c {QValueSpaceObject(QByteArray(path), filter, parent)}.

    The constructed Value Space object will access the \l {QAbstractValueSpaceLayer}{layer} with
    the highest \l {QAbstractValueSpaceLayer::order()}{order} that matches \a filter and for which
    \a path is a valid path.  If no suitable \l {QAbstractValueSpaceLayer}{layer} is found an
    invalid QValueSpaceObject is constructed.

    \sa isValid()
*/
QValueSpaceObject::QValueSpaceObject(const char *path,
                                     QValueSpace::LayerOptions filter,
                                     QObject *parent)
:   QObject(parent), d(new QValueSpaceObjectPrivate(path, filter))
{
    VS_CALL_ASSERT;
    QValueSpaceManager::instance()->init();
}

/*!
    \overload

    Constructs a QValueSpaceObject with the specified \a parent that publishes values under
    \a path.  Only the layer identified by \a uuid will be accessed by this object.  This
    constructor is equivalent to calling \c {QValueSpaceObject(path.toUtf8(), uuid, parent)}.

    Use of this constructor is not platform agnostic.  If possible use one of the constructors that
    take a QAbstractValueSpaceLayer::LayerOptions parameter instead.

    If a layer with a matching \a uuid is not found an invalid QValueSpaceObject will be
    constructed.

    \sa isValid()
*/

QValueSpaceObject::QValueSpaceObject(const QString &path, const QUuid &uuid, QObject *parent)
:   QObject(parent), d(new QValueSpaceObjectPrivate(path.toUtf8(), uuid))
{
    VS_CALL_ASSERT;
    QValueSpaceManager::instance()->init();
}

/*!
    \overload

    Constructs a QValueSpaceObject with the specified \a parent that publishes values under
    \a path.  Only the layer identified by \a uuid will be accessed by this object.  This
    constructor is equivalent to calling \c {QValueSpaceObject(QByteArray(path), uuid, parent)}.

    Use of this constructor is not platform agnostic.  If possible use one of the constructors that
    take a QAbstractValueSpaceLayer::LayerOptions parameter instead.

    If a layer with a matching \a uuid is not found an invalid QValueSpaceObject will be
    constructed.

    \sa isValid()
*/
QValueSpaceObject::QValueSpaceObject(const char *path, const QUuid &uuid, QObject *parent)
:   QObject(parent), d(new QValueSpaceObjectPrivate(path, uuid))
{
    VS_CALL_ASSERT;
    QValueSpaceManager::instance()->init();
}

/*!
    Destroys the QValueSpaceObject.  This will remove the object and all its attributes from the
    Value Space.
*/
QValueSpaceObject::~QValueSpaceObject()
{
    VS_CALL_ASSERT;

    if (!d->layer)
        return;

    if (d->hasSet && !(d->layer->layerOptions() & QValueSpace::PermanentLayer))
        d->layer->removeSubTree(this, d->handle);

    if (d->hasWatch)
        d->layer->removeWatches(this, d->handle);

    delete d;
}

/*!
    Returns the path that this QValueSpaceObject refers to.
*/
QString QValueSpaceObject::path() const
{
    VS_CALL_ASSERT;
    return QString::fromUtf8(d->path);
}

/*!
    Returns true if this object is valid; otherwise returns false.  An object is valid if its
    associated QAbstractValueSpaceLayer is available.
*/
bool QValueSpaceObject::isValid() const
{
    return (d->layer && d->handle != QAbstractValueSpaceLayer::InvalidHandle);
}

/*!
    Returns true if this object will emit signals in response to requests or notifications from
    QValueSpaceItems.
*/
bool QValueSpaceObject::supportsRequests() const
{
    return (d->layer && d->layer->supportsRequests());
}

/*!
    Forcibly sync all Value Space objects.

    For performance reasons attribute changes are batched internally by
    QValueSpaceObject instances.  In cases where the visibility of changes must
    be synchronized with other processes, calling QValueSpaceObject::sync() will
    flush these batches.  By the time sync() returns, all other processes in the
    system will be able to see the attribute changes.

    In the common asynchronous case, calling sync() is unnecessary.
*/
void QValueSpaceObject::sync()
{
    VS_CALL_ASSERT;

    foreach (QAbstractValueSpaceLayer *layer, QValueSpaceManager::instance()->getLayers())
        layer->sync();
}

/*!
    \overload

    This is a convenience overload and is equivalent to
    \c {setAttribute(QByteArray(attribute), data)}.
*/
void QValueSpaceObject::setAttribute(const char *attribute, const QVariant &data)
{
    VS_CALL_ASSERT;
    setAttribute(QByteArray(attribute), data);
}

/*!
    \overload

    This is a convenience overload and is equivalent to
    \c {setAttribute(attribute.toUtf8(), data)}.
*/
void QValueSpaceObject::setAttribute(const QString &attribute, const QVariant &data)
{
    VS_CALL_ASSERT;
    setAttribute(attribute.toUtf8(), data);
}

/*!
    Set an \a attribute on the object to \a data.  If attribute is empty, this call will set the
    object's value.

    For example:

    \code
        QValueSpaceObject object("/Device");
        object.setAttribute("State", "Starting");
        object.sync();

        // QValueSpaceItem("/Device/State").value() == QVariant("Starting")
    \endcode
*/
void QValueSpaceObject::setAttribute(const QByteArray &attribute, const QVariant &data)
{
    VS_CALL_ASSERT;

    if (!isValid()) {
        qWarning("setAttribute called on invalid QValueSpaceObject.");
        return;
    }

    d->hasSet = true;
    d->layer->setValue(this, d->handle, qCanonicalPath(attribute), data);
}

/*!
    \overload

    This is a convenience overload and is equivalent to
    \c {removeAttribute(QByteArray(attribute))}.
*/
void QValueSpaceObject::removeAttribute(const char *attribute)
{
    VS_CALL_ASSERT;
    removeAttribute(QByteArray(attribute));
}

/*!
    \overload

    This is a convenience overload and is equivalent to \c {removeAttribute(attribute.toUtf8())}.
*/
void QValueSpaceObject::removeAttribute(const QString &attribute)
{
    VS_CALL_ASSERT;
    removeAttribute(attribute.toUtf8());
}

/*!
    Removes the object \a attribute and all sub-attributes from the system.

    For example:
    \code
        QValueSpaceObject object("/Device");
        object.setAttribute("State", "Starting");
        object.setAttribute("State/Memory", "1000");
        object.sync();
        // QValueSpaceItem("/Device/State").value() == QVariant("Starting")
        // QValueSpaceItem("/Device/State/Memory").value() == QVariant("1000")

        object.removeAttribute("State");
        object.sync();
        // QValueSpaceItem("/Device/State").value() == QVariant();
        // QValueSpaceItem("/Device/State/Memory").value() == QVariant();
    \endcode
*/
void QValueSpaceObject::removeAttribute(const QByteArray &attribute)
{
    VS_CALL_ASSERT;

    if (!isValid()) {
        qWarning("removeAttribute called on invalid QValueSpaceObject.");
        return;
    }

    d->layer->removeValue(this, d->handle, qCanonicalPath(attribute));
}

/*!
    Registers this QValueSpaceObject for notifications when requests to set or remove value space
    items under objectPath() are received by the underlying QAbstractValueSpaceLayer.

    Generally you do not need to call this function as it is automatically called when
    connections are made to this classes signals.  \a member is the signal that has been connected.

    If you reimplement this virtual function it is important that you call this implementation from
    your implementation.

    \sa itemNotify()
*/
void QValueSpaceObject::connectNotify(const char *member)
{
    VS_CALL_ASSERT;

    if (!d->hasWatch && d->layer && (*member - '0') == QSIGNAL_CODE) {
        d->layer->addWatch(this, d->handle);
        d->hasWatch = true;
    }

    QObject::connectNotify(member);
}

QT_END_NAMESPACE
