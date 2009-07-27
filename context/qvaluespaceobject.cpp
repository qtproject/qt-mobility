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

#include "qvaluespaceobject.h"
#include "qvaluespace.h"
#include "qvaluespacemanager_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qthread.h>

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
  \class QValueSpaceObject

  \brief The QValueSpaceObject class allows applications to add entries to the
         Value Space.

  For an overview of the Qt Value Space, please see the QValueSpaceItem
  documentation.

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
  \c { { /Device/Network/Interfaces /eth0, /Device/Network/Interfaces/ppp0 } },
  each with the six attributes \c { {Name, Type, Status, BytesSent,
  BytesReceived, Time} }.  The QValueSpaceObject class encapsulates this
  abstraction.

  In the case of two or more applications creating an application object with
  overlapping attributes, only the first is visible to observers in the system.
  The other attributes are not discarded, but are buffered until the first
  releases its hold on the attribute, either by manually removing it, destroying
  the QValueSpaceObject or by terminating.  For example:

  \code
  QValueSpaceObject * object1 = new QValueSpaceObject("/Device");
  object1->setAttribute("Buttons", 2);

  // QValueSpaceItem("/Device/Buttons") == QVariant(2)

  QValueSpaceObject * object2 = new QValueSpaceObject("/Device");
  object2->setAttribute("Buttons", 3);

  // QValueSpaceItem("/Device/Buttons") == QVariant(2)

  object2->removeAttribute("Buttons");
  // QValueSpaceItem("/Device/Buttons") == QVariant(3)
  \endcode

  For performance reasons the setting of and removing of attributes is buffered
  internally by the QValueSpaceObject and applied as a batch sometime later.
  Normally this occurs the next time the application enters the Qt event loop,
  but this behaviour should not be relied apon.  If an application must
  synchronize application objects with others, the QValueSpaceObject::sync()
  method can be used to force the application of changes.  This call is
  generally unnecessary, and should be used sparingly to prevent unnecessary
  load on the system.

  \i {Note:} The QValueSpaceObject class is not thread safe and may only be used from
  an application's main thread.

  \sa QValueSpaceItem
*/

/*!
    \fn virtual bool requestSetValue(HANDLE handle, const QVariant &data) = 0

    Sends a request to the provider of the value space item identified by \a handle to set it to
    \a data.

    Returns true if the request is successfully sent; otherwise returns false.
*/

/*!
    \fn virtual bool requestSetValue(HANDLE handle, const QByteArray &path, const QVariant &data) = 0

    Sends a request to the provider of the value space item identified by \a handle and \a path
    to set it to \a data.

    Returns true if the request is successfully sent; otherwise returns false.
*/

/*!
    \fn virtual bool requestRemoveValue(HANDLE handle, const QByteArray &path = QByteArray()) = 0

    Sends a request to the provider of the value space item identified by \a handle and \a path
    to remove the item.

    Returns true if the request is successfully sent; otherwise returns false.
*/

#define VS_CALL_ASSERT Q_ASSERT(!QCoreApplication::instance() || QCoreApplication::instance()->thread() == QThread::currentThread());

class QValueSpaceObjectPrivate
{
public:
    QValueSpaceObjectPrivate(const QByteArray &objectPath);

    QByteArray path;

    QAbstractValueSpaceLayer *layer;
    QAbstractValueSpaceLayer::HANDLE handle;

    bool hasSet;
    bool hasWatch;
};

QValueSpaceObjectPrivate::QValueSpaceObjectPrivate(const QByteArray &objectPath)
:   layer(0), handle(QAbstractValueSpaceLayer::InvalidHandle),
    hasSet(false), hasWatch(false)
{
    if (!objectPath.startsWith('/'))
        path.append('/');

    path.append(objectPath);

    while (path.length() > 1 && path.endsWith('/'))
        path.chop(1);

    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    for (int ii = 0; ii < layers.count(); ++ii) {
        QAbstractValueSpaceLayer::HANDLE h =
            layers.at(ii)->item(QAbstractValueSpaceLayer::InvalidHandle, path);

        if (h != QAbstractValueSpaceLayer::InvalidHandle) {
            layer = layers.at(ii);
            handle = h;
            break;
        }
    }
}

/*!
    \overload

    Construct a Value Space object rooted at \a objectPath with the specified \a parent.  This
    constructor is equivalent to \c {QValueSpaceObject(QByteArray(objectPath), parent)}.
*/
QValueSpaceObject::QValueSpaceObject(const char *objectPath, QObject *parent)
:   QObject(parent), d(new QValueSpaceObjectPrivate(objectPath))
{
    VS_CALL_ASSERT;
    QValueSpace::initValuespace();
}

/*!
    \overload

    Construct a Value Space object rooted at \a objectPath with the specified \a parent.  This
    constructor is equivalent to \c {QValueSpaceObject(objectPath.toUtf8(), parent)}.
*/
QValueSpaceObject::QValueSpaceObject(const QString &objectPath, QObject *parent)
:   QObject(parent), d(new QValueSpaceObjectPrivate(objectPath.toUtf8()))
{
    VS_CALL_ASSERT;
    QValueSpace::initValuespace();
}

/*!
    Construct a Value Space object rooted at \a objectPath with the specified \a parent.
*/
QValueSpaceObject::QValueSpaceObject(const QByteArray &objectPath, QObject *parent)
:   QObject(parent), d(new QValueSpaceObjectPrivate(objectPath))
{
    VS_CALL_ASSERT;
    QValueSpace::initValuespace();
}

/*!
    Destroys the Value Space object.  This will remove the object and all its attributes from the
    Value Space.
*/
QValueSpaceObject::~QValueSpaceObject()
{
    VS_CALL_ASSERT;

    if (!d->layer)
        return;

    if (d->hasSet)
        d->layer->removeSubTree(this, d->handle);

    if (d->hasWatch)
        d->layer->removeWatches(this, d->handle);

    delete d;
}

/*!
    Returns the full path to this object as passed to the QValueSpaceObject constructor.
*/
QString QValueSpaceObject::objectPath() const
{
    VS_CALL_ASSERT;
    return QString::fromUtf8(d->path);
}

#if 0
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
        layer->syncChanges();
}
#endif

/*!
    \overload

    This is a convenience overload and is equivalent to \c {setAttribute(QByteArray(attribute), data)}.
*/
void QValueSpaceObject::setAttribute(const char *attribute, const QVariant &data)
{
    VS_CALL_ASSERT;
    setAttribute(QByteArray(attribute), data);
}

/*!
    \overload

    This is a convenience overload and is equivalent to \c {setAttribute(attribute.toUtf8(), data)}.
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

    if (!d->layer || d->handle == QAbstractValueSpaceLayer::InvalidHandle)
        return;

    d->hasSet = true;
    d->layer->setValue(this, d->handle, attribute, data);
}

/*!
    \overload

    This is a convenience overload and is equivalent to \c {removeAttribute(QByteArray(attribute))}.
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
    d->layer->removeValue(this, d->handle, attribute);
}

#if 0
void QValueSpaceObject::connectNotify(const char *member)
{
    QObject::connectNotify(member);
}
#endif
QT_END_NAMESPACE
