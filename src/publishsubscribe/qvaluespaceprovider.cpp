/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qvaluespaceprovider.h"
#include "qvaluespace.h"
#include "qvaluespace_p.h"
#include "qvaluespacemanager_p.h"

#include <QtCore/qcoreapplication.h>

#include <QtCore/qdebug.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QValueSpaceProvider

    \brief The QValueSpaceProvider class allows applications to add entries to the Value Space.

    \ingroup publishsubscribe

    For an overview of the Qt Value Space, please see the QValueSpaceSubscriber documentation.

    The QValueSpaceProvider class allows applications to write entries into the
    Value Space that are automatically removed when the QValueSpaceProvider is
    destroyed, or the application exits either cleanly or abnormally.  All
    applications in the system will have access to the data set through
    QValueSpaceProvider and, if desired, can be notified when the data changes.

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
    BytesReceived, Time} }.  The QValueSpaceProvider class encapsulates this
    abstraction.

    In the case of two or more applications creating an application object with
    overlapping attributes, only the first is visible to observers in the system.
    The other attributes are not discarded, but are buffered until the first
    releases its hold on the attribute, either by manually removing it, destroying
    the QValueSpaceProvider or by terminating.  For example:

    \code
    QValueSpaceProvider *object1 = new QValueSpaceProvider("/Device");
    object1->setAttribute("Buttons", 2);

    // QValueSpaceSubscriber("/Device/Buttons").value() == QVariant(2)

    QValueSpaceProvider *object2 = new QValueSpaceProvider("/Device");
    object2->setAttribute("Buttons", 3);

    // QValueSpaceSubscriber("/Device/Buttons").value() == QVariant(2)

    object2->removeAttribute("Buttons");
    // QValueSpaceSubscriber("/Device/Buttons").value() == QVariant(3)
    \endcode

    For performance reasons the setting of and removing of attributes is buffered
    internally by the QValueSpaceProvider and applied as a batch sometime later.
    Normally this occurs the next time the application enters the Qt event loop,
    but this behavior should not be relied upon.  If an application must
    synchronize application objects with others, the QValueSpaceProvider::sync()
    method can be used to force the application of changes.  This call is
    generally unnecessary, and should be used sparingly to prevent unnecessary
    load on the system.

    \i {Note:} The QValueSpaceProvider class is not thread safe and may only be used from
    an application's main thread.

    \sa QValueSpaceSubscriber
*/

/*!
    \fn void QValueSpaceProvider::attributeInterestChanged(const QString &attribute,
                                                           bool interested)

    Signal that is emitted when interest in \a attribute changes.  If \a interested is true at
    least on QValueSpaceSubscriber is interested in the value of \a attribute.
*/

class QValueSpaceProviderPrivate
{
public:
    QValueSpaceProviderPrivate(const QString &_path,
                               QValueSpace::LayerOptions filter = QValueSpace::UnspecifiedLayer);
    QValueSpaceProviderPrivate(const QString &_path, const QUuid &uuid);

    QString path;

    QAbstractValueSpaceLayer *layer;
    QAbstractValueSpaceLayer::Handle handle;

    bool hasSet;
    bool hasWatch;
};

QValueSpaceProviderPrivate::QValueSpaceProviderPrivate(const QString &_path,
                                                       QValueSpace::LayerOptions filter)
:   layer(0), handle(QAbstractValueSpaceLayer::InvalidHandle), hasSet(false), hasWatch(false)
{
    path = qCanonicalPath(_path);

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

QValueSpaceProviderPrivate::QValueSpaceProviderPrivate(const QString &_path, const QUuid &uuid)
:   layer(0), handle(QAbstractValueSpaceLayer::InvalidHandle), hasSet(false), hasWatch(false)
{
    path = qCanonicalPath(_path);

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

    Constructs a QValueSpaceProvider with the specified \a parent that publishes values under
    \a path.  This constructor is equivalent to calling
    \c {QValueSpaceSubscriber(path.toUtf8(), parent)}.
*/
QValueSpaceProvider::QValueSpaceProvider(const QString &path, QObject *parent)
:   QObject(parent), d(new QValueSpaceProviderPrivate(path))
{
    QValueSpaceManager::instance()->init();
}

/*!
    \overload

    Constructs a QValueSpaceProvider with the specified \a parent that publishes values under
    \a path.  This constructor is equivalent to calling
    \c {QValueSpaceSubscriber(QString(path), parent)}.
*/
QValueSpaceProvider::QValueSpaceProvider(const char *path, QObject *parent)
:   QObject(parent), d(new QValueSpaceProviderPrivate(QString::fromLatin1(path)))
{
    QValueSpaceManager::instance()->init();
}

/*!
    \overload

    Constructs a QValueSpaceProvider with the specified \a parent that publishes values under
    \a path.  The \a filter parameter is used to limit which layer this QValueSpaceProvider will
    access.  This constructor is equivalent to calling
    \c {QValueSpaceProvider(path.toUtf8(), filter, parent)}.

    The constructed Value Space provier will access the \l {QAbstractValueSpaceLayer}{layer} with
    the highest \l {QAbstractValueSpaceLayer::order()}{order} that matches \a filter and for which
    \a path is a valid path.

    If no suitable \l {QAbstractValueSpaceLayer}{layer} is found, the constructed
    QValueSpaceProvider will be unconnected.

    \sa isConnected()
*/
QValueSpaceProvider::QValueSpaceProvider(const QString &path,
                                         QValueSpace::LayerOptions filter,
                                         QObject *parent)
:   QObject(parent), d(new QValueSpaceProviderPrivate(path, filter))
{
    QValueSpaceManager::instance()->init();
}

/*!
    \overload

    Constructs a QValueSpaceProvider with the specified \a parent that publishes values under
    \a path.  The \a filter parameter is used to limit which layer this QValueSpaceProvider will
    access.  This constructor is equivalent to calling
    \c {QValueSpaceProvider(QString::fromLatin1(path), filter, parent)}.

    The constructed Value Space provider will access the \l {QAbstractValueSpaceLayer}{layer} with
    the highest \l {QAbstractValueSpaceLayer::order()}{order} that matches \a filter and for which
    \a path is a valid path.

    If no suitable \l {QAbstractValueSpaceLayer}{layer} is found, the constructed
    QValueSpaceProvider will be unconnected.

    \sa isConnected()
*/
QValueSpaceProvider::QValueSpaceProvider(const char *path,
                                         QValueSpace::LayerOptions filter,
                                         QObject *parent)
:   QObject(parent), d(new QValueSpaceProviderPrivate(QString::fromLatin1(path), filter))
{
    QValueSpaceManager::instance()->init();
}

/*!
    \overload

    Constructs a QValueSpaceProvider with the specified \a parent that publishes values under
    \a path.  Only the layer identified by \a uuid will be accessed by this provider.  This
    constructor is equivalent to calling \c {QValueSpaceProvider(path.toUtf8(), uuid, parent)}.

    Use of this constructor is not platform agnostic.  If possible use one of the constructors that
    take a QAbstractValueSpaceLayer::LayerOptions parameter instead.

    If a layer with a matching \a uuid is not found, the constructed QValueSpaceProvider will be
    unconnected.

    \sa isConnected()
*/

QValueSpaceProvider::QValueSpaceProvider(const QString &path, const QUuid &uuid, QObject *parent)
:   QObject(parent), d(new QValueSpaceProviderPrivate(path, uuid))
{
    QValueSpaceManager::instance()->init();
}

/*!
    \overload

    Constructs a QValueSpaceProvider with the specified \a parent that publishes values under
    \a path.  Only the layer identified by \a uuid will be accessed by this provider.  This
    constructor is equivalent to calling \c {QValueSpaceProvider(QString(path), uuid, parent)}.

    Use of this constructor is not platform agnostic.  If possible use one of the constructors that
    take a QAbstractValueSpaceLayer::LayerOptions parameter instead.

    If a layer with a matching \a uuid is not found, the constructed QValueSpaceProvider will be
    unconnected.

    \sa isConnected()
*/
QValueSpaceProvider::QValueSpaceProvider(const char *path, const QUuid &uuid, QObject *parent)
:   QObject(parent), d(new QValueSpaceProviderPrivate(QString::fromLatin1(path), uuid))
{
    QValueSpaceManager::instance()->init();
}

/*!
    Destroys the QValueSpaceProvider.  This will remove the values set by this provider in all
    \l {QValueSpace::NonPermanentLayer}{non-permanent} layers.
*/
QValueSpaceProvider::~QValueSpaceProvider()
{
    if (!d->layer)
        return;

    if (d->hasSet && !(d->layer->layerOptions() & QValueSpace::PermanentLayer))
        d->layer->removeSubTree(this, d->handle);

    if (d->hasWatch)
        d->layer->removeWatches(this, d->handle);

    delete d;
}

/*!
    Returns the path that this QValueSpaceProvider refers to.
*/
QString QValueSpaceProvider::path() const
{
    return d->path;
}

/*!
    Returns true if this QValueSpaceProvider is connected to an available layer; otherwise returns
    false.
*/
bool QValueSpaceProvider::isConnected() const
{
    return (d->layer && d->handle != QAbstractValueSpaceLayer::InvalidHandle);
}

/*!
    Forcibly sync all Value Space providers using the same layer as this provider.
*/
void QValueSpaceProvider::sync()
{
    if (!d || !d->layer)
        return;

    d->layer->sync();
}

/*!
    Forcibly sync all Value Space providers.

    For performance reasons attribute changes are batched internally by
    QValueSpaceProvider instances.  In cases where the visibility of changes must
    be synchronized with other processes, calling QValueSpaceProvider::sync() will
    flush these batches.  By the time syncAll() returns, all other processes in the
    system will be able to see the attribute changes.

    In the common asynchronous case, calling syncAll() is unnecessary.
*/
void QValueSpaceProvider::syncAll()
{
    foreach (QAbstractValueSpaceLayer *layer, QValueSpaceManager::instance()->getLayers())
        layer->sync();
}

/*!
    Set an \a attribute on the provider to \a data.  If attribute is empty, this call will set the
    value of this providers path.

    For example:

    \code
        QValueSpaceProvider provider("/Device");
        provider.setAttribute("State", "Starting");
        provider.sync();

        // QValueSpaceSubscriber("/Device/State").value() == QVariant("Starting")
    \endcode
*/
void QValueSpaceProvider::setAttribute(const QString &attribute, const QVariant &data)
{
    if (!isConnected()) {
        qWarning("setAttribute called on unconnected QValueSpaceProvider.");
        return;
    }

    d->hasSet = true;
    d->layer->setValue(this, d->handle, qCanonicalPath(attribute), data);
}

/*!
    \overload

    This is a convenience overload and is equivalent to
    \c {setAttribute(QString(attribute), data)}.
*/
void QValueSpaceProvider::setAttribute(const char *attribute, const QVariant &data)
{
    setAttribute(QString::fromLatin1(attribute), data);
}

/*!
    Removes the proivder \a attribute and all sub-attributes from the system.

    For example:
    \code
        QValueSpaceProvider provider("/Device");
        provider.setAttribute("State", "Starting");
        provider.setAttribute("State/Memory", "1000");
        provider.sync();
        // QValueSpaceSubscriber("/Device/State").value() == QVariant("Starting")
        // QValueSpaceSubscriber("/Device/State/Memory").value() == QVariant("1000")

        provider.removeAttribute("State");
        provider.sync();
        // QValueSpaceSubscriber("/Device/State").value() == QVariant();
        // QValueSpaceSubscriber("/Device/State/Memory").value() == QVariant();
    \endcode
*/
void QValueSpaceProvider::removeAttribute(const QString &attribute)
{
    if (!isConnected()) {
        qWarning("removeAttribute called on unconnected QValueSpaceProvider.");
        return;
    }

    d->layer->removeValue(this, d->handle, qCanonicalPath(attribute));
}

/*!
    \overload

    This is a convenience overload and is equivalent to \c {removeAttribute(QString(attribute))}.
*/
void QValueSpaceProvider::removeAttribute(const char *attribute)
{
    removeAttribute(QString::fromLatin1(attribute));
}

/*!
    Registers this QValueSpaceProvider for notifications when QValueSpaceSubscribers are interested
    in values under path().

    Generally you do not need to call this function as it is automatically called when
    connections are made to this classes signals.  \a member is the signal that has been connected.

    If you reimplement this virtual function it is important that you call this implementation from
    your implementation.

    \sa attributeInterestChanged()
*/
void QValueSpaceProvider::connectNotify(const char *member)
{
    if (!d->hasWatch && d->layer && (*member - '0') == QSIGNAL_CODE) {
        d->layer->addWatch(this, d->handle);
        d->hasWatch = true;
    }

    QObject::connectNotify(member);
}

#include "moc_qvaluespaceprovider.cpp"
QTM_END_NAMESPACE
