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

#include "qvaluespaceitem.h"
#include "qvaluespace_p.h"
#include "qvaluespacemanager_p.h"

#include <QSet>
#include <QCoreApplication>
#include <QThread>
#include <QUuid>

QT_BEGIN_NAMESPACE

/*!
    \class QValueSpaceItem

    \brief The QValueSpaceItem class allows access to Value Space items.

    The Value Space is an inter-application hierarchy of readable, writable and
    subscribable data.  The QValueSpaceItem class allows applications to read
    and subscribe to this data.

    Conceptually, the Value Space is a hierarchical tree of which each item can
    optionally contain a QVariant value and sub-items.  A serialized version of a
    simple example might look like this.

    \code
    /Device/Buttons = 3
    /Device/Buttons/1/Name = Context
    /Device/Buttons/1/Usable = true
    /Device/Buttons/2/Name = Select
    /Device/Buttons/2/Usable = false
    /Device/Buttons/3/Name = Back
    /Device/Buttons/3/Usable = true
    \endcode

    Any application in Qt can read values from the Value Space, or be notified
    asynchronously when they change using the QValueSpaceItem class.

    Items in the Value Space can be thought of as representing "objects" adhering
    to a well known schema.  This is a conceptual differentiation, not a physical
    one, as internally the Value Space is treated as one large tree.  In the
    sample above, the \c {/Device/Buttons} schema can be defined as containing a
    value representing the number of mappable buttons on a device, and a sub-item
    for each.  Likewise, the sub-item object schema contains two attributes -
    \c {Name} and \c {Usable}.

    Applications may use the QValueSpaceObject class to create a schema object
    within the Value Space.  Objects remain in the Value Space as long as the
    QValueSpaceObject instance exists - that is, they are not persistent.  If
    the object is destroyed, or the application containing it exits (or crashes)
    the items are removed.

    Change notification is modeled in a similar way.  Applications subscribe to
    notifications at a particular object (i.e. item) in the tree.  If anything in
    that object (i.e. under that item) changes, the application is notified.  This
    allows, for example, subscription to just the \c {/Device/Buttons} item to
    receive notification when anything "button" related changes.

    For example,

    \code
    QValueSpaceItem *buttons = new QValueSpaceItem("/Device/Buttons");
    qWarning() << "There are" << buttons->value().toUInt() << "buttons";
    QObject::connect(buttons, SIGNAL(contentsChanged()),
                   this, SLOT(buttonInfoChanged()));
    \endcode

    will invoke the \c {buttonInfoChanged()} slot whenever any item under
    \c {/Device/Buttons} changes.  This includes the value of \c {/Device/Buttons}
    itself, a change of a sub-object such as \c {/Device/Buttons/2/Name} or the
    creation (or removal) of a new sub-object, such as \c {/Device/Buttons/4}.

    When a QValueSpaceItem is constructed it will access zero, one or many
    \l {QAbstractValueSpaceLayer}{layers}, depending on the constructor and filter parameters used
    to construct it.  Calls to value() will return the value that is stored in the layer with the
    highest \l {QAbstractValueSpaceLayer::order()}{order} of all the accessible layers that contain
    the specified key.

    For example,

    \code
    QValueSpaceItem *buttons = new QValueSpaceItem("/Device/Buttons",
                                                   QAbstractValueSpaceLayer::NonPermanentLayer);
    qWarning() << "There are" << buttons->value().toUInt() << "buttons";
    \endcode

    will print out the number of buttons defined in all non-permanent layers.  If the same button
    is defined in multiple non-permanent layers only the values from the layer with the highest
    \l {QAbstractValueSpaceLayer::order()}{order} will be visible.

    \i {Note:} The QValueSpaceItem class is not thread safe and may only be used from
    an application's main thread.
*/

/*!
    \fn QValueSpaceItem::contentsChanged()

    Emitted whenever the value of this item, or any of its sub-items change.
*/

/*!
    \property QValueSpaceItem::path

    This property holds the current path that the QValueSpaceItem refers to.

    Settings this property causes the QValueSpaceItem to disconnect and reconnect to the Value
    Space with the new path.  As a result all signal/slot connections are disconnected.
*/

/*!
    \property QValueSpaceItem::value

    This property holds the value of this item.
*/

class QValueSpaceItemPrivateProxy : public QObject
{
    Q_OBJECT

signals:
    void changed();

public slots:
    void handleChanged(quintptr handle)
    {
        for (int ii = 0; ii < readers.count(); ++ii)
            if (readers.at(ii).second == handle) {
                emit changed();
                return;
            }
    }

    void objDestroyed()
    {
        connections.remove((QValueSpaceItem *)sender());
    }

public:
    QList<QPair<QAbstractValueSpaceLayer *, QAbstractValueSpaceLayer::Handle> > readers;
    QHash<const QValueSpaceItem *,int> connections;
};

struct QValueSpaceItemPrivate
{
    QValueSpaceItemPrivate(const QString &_path,
                           QValueSpace::LayerOptions filter = QValueSpace::UnspecifiedLayer)
        : refCount(0), connections(0)
    {
        path = qCanonicalPath(_path);

        QValueSpaceManager *man = QValueSpaceManager::instance();
        if (!man)
            return;

        if ((filter & QValueSpace::PermanentLayer &&
             filter & QValueSpace::NonPermanentLayer) ||
            (filter & QValueSpace::WriteableLayer &&
             filter & QValueSpace::NonWriteableLayer)) {
            return;
        }

        const QList<QAbstractValueSpaceLayer *> & readerList = man->getLayers();

        for (int ii = 0; ii < readerList.count(); ++ii) {
            QAbstractValueSpaceLayer *read = readerList.at(ii);
            if (filter != QValueSpace::UnspecifiedLayer &&
                !(read->layerOptions() & filter)) {
                continue;
            }

            QAbstractValueSpaceLayer::Handle handle =
                read->item(QAbstractValueSpaceLayer::InvalidHandle, path);
            if (QAbstractValueSpaceLayer::InvalidHandle != handle) {
                readers.append(qMakePair(read, handle));

                read->notifyInterest(handle, true);
            }
        }
    }

    QValueSpaceItemPrivate(const QString &_path, const QUuid &uuid)
    :   refCount(0), connections(0)
    {
        path = qCanonicalPath(_path);

        QValueSpaceManager *man = QValueSpaceManager::instance();
        if (!man)
            return;

        const QList<QAbstractValueSpaceLayer *> & readerList = man->getLayers();

        for (int ii = 0; ii < readerList.count(); ++ii) {
            QAbstractValueSpaceLayer *read = readerList.at(ii);
            if (read->id() != uuid)
                continue;

            QAbstractValueSpaceLayer::Handle handle =
                read->item(QAbstractValueSpaceLayer::InvalidHandle, path);
            if (QAbstractValueSpaceLayer::InvalidHandle != handle) {
                readers.append(qMakePair(read, handle));

                read->notifyInterest(handle, true);
            }
        }
    }

    ~QValueSpaceItemPrivate()
    {
        for (int ii = 0; ii < readers.count(); ++ii) {
            readers[ii].first->notifyInterest(readers[ii].second, false);
            readers[ii].first->removeHandle(readers[ii].second);
        }

        if (connections)
            delete connections;

    }

    void connect(const QValueSpaceItem *space)
    {
        if (!connections) {
            qRegisterMetaType<quintptr>("quintptr");
            connections = new QValueSpaceItemPrivateProxy;
            connections->readers = readers;
            connections->connections.insert(space,1);
            QObject::connect(space, SIGNAL(destroyed(QObject*)),
                             connections, SLOT(objDestroyed()));
            QObject::connect(connections, SIGNAL(changed()),
                             space, SIGNAL(contentsChanged()));
            for (int ii = 0; ii < readers.count(); ++ii) {
                readers.at(ii).first->setProperty(readers.at(ii).second,
                                                  QAbstractValueSpaceLayer::Publish);
                QObject::connect(readers.at(ii).first, SIGNAL(handleChanged(quintptr)),
                                 connections, SLOT(handleChanged(quintptr)));
            }
        } else if (!connections->connections.contains(space)) {
            connections->connections[space] = 1;

            QObject::connect(space, SIGNAL(destroyed(QObject*)),
                             connections, SLOT(objDestroyed()));
            QObject::connect(connections, SIGNAL(changed()),
                             space, SIGNAL(contentsChanged()));
        } else {
            ++connections->connections[space];
        }
    }

    bool disconnect(QValueSpaceItem * space)
    {
        if (connections) {
            QHash<const QValueSpaceItem *, int>::Iterator iter =
                connections->connections.find(space);
            if (iter != connections->connections.end()) {
                --(*iter);
                if (!*iter) {
                    QObject::disconnect(space, SIGNAL(destroyed(QObject*)),
                                        connections, SLOT(objDestroyed()));
                    QObject::disconnect(connections, SIGNAL(changed()),
                                        space, SIGNAL(contentsChanged()));
                    connections->connections.erase(iter);
                }
                return true;
            }
        }
        return false;
    }

    void AddRef()
    {
        ++refCount;
    }

    void Release()
    {
        Q_ASSERT(refCount);
        --refCount;
        if (!refCount)
            delete this;
    }

    unsigned int refCount;
    QString path;
    QList<QPair<QAbstractValueSpaceLayer *, QAbstractValueSpaceLayer::Handle> > readers;
    QValueSpaceItemPrivateProxy * connections;
};

#define VS_CALL_ASSERT Q_ASSERT(!QCoreApplication::instance() || \
                            QCoreApplication::instance()->thread() == QThread::currentThread());

/*!
    Constructs a QValueSpaceItem with the specified \a parent that refers to the root path.

    The constructed Value Space item will access all available
    \l {QAbstractValueSpaceLayer}{layers}.
*/
QValueSpaceItem::QValueSpaceItem(QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivate(QLatin1String("/"));
    d->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  This
    constructor is equivalent to calling \c {QValueSpaceItem(path.toUtf8(), parent)}.

    The constructed Value Space item will access all available
    \l {QAbstractValueSpaceLayer}{layers}.
*/
QValueSpaceItem::QValueSpaceItem(const QString &path, QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivate(path);
    d->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  This
    constructor is equivalent to calling \c {QValueSpaceItem(QString(path), parent)}.

    The constructed Value Space item will access all available
    \l {QAbstractValueSpaceLayer}{layers}.
*/
QValueSpaceItem::QValueSpaceItem(const char *path, QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivate(QString::fromLatin1(path));
    d->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  The
    \a filter parameter is used to limit which layers this QValueSpaceItem will access.  This
    constructor is equivalent to calling \c {QValueSpaceItem(path.toUtf8(), filter, parent)}.

    If a layer matching \a filter is not found, the constructed QValueSpaceItem will be
    unconnected.

    \sa isConnected()
*/
QValueSpaceItem::QValueSpaceItem(const QString &path,
                                 QValueSpace::LayerOptions filter,
                                 QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivate(path, filter);
    d->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  The
    \a filter parameter is used to limit which layers this QValueSpaceItem will access.  This
    constructor is equivalent to calling \c {QValueSpaceItem(QString(path), filter, parent)}.

    If a layer matching \a filter is not found, the constructed QValueSpaceItem will be
    unconnected.

    \sa isConnected()
*/
QValueSpaceItem::QValueSpaceItem(const char *path,
                                 QValueSpace::LayerOptions filter,
                                 QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivate(QString::fromLatin1(path), filter);
    d->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  This
    QValueSpaceItem will only use the layer identified by \a uuid.  This constructor is equivalent
    to calling \c {QValueSpaceItem(path.toUtf8(), uuid, parent)}.

    Use of this constructor is not platform agnostic.  If possible use one of the constructors that
    take a QAbstractValueSpaceLayer::LayerOptions parameter instead.

    If a layer with a matching \a uuid is not found, the constructed QValueSpaceItem will be
    unconnected.

    \sa QAbstractValueSpaceLayer::id(), QValueSpace, isConnected()
*/
QValueSpaceItem::QValueSpaceItem(const QString &path, const QUuid &uuid, QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivate(path, uuid);
    d->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  This
    QValueSpaceItem will only use the layer identified by \a uuid.  This constructor is equivalent
    to calling \c {QValueSpaceItem(QString(path), uuid, parent)}.

    Use of this constructor is not platform agnostic.  If possible use one of the constructors that
    take a QAbstractValueSpaceLayer::LayerOptions parameter instead.

    If a layer with a matching \a uuid is not found, the constructed QValueSpaceItem will be
    unconnected.

    \sa QAbstractValueSpaceLayer::id(), QValueSpace, isConnected()
*/
QValueSpaceItem::QValueSpaceItem(const char *path, const QUuid &uuid, QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivate(QString::fromLatin1(path), uuid);
    d->AddRef();
}

/*!
    Destroys the QValueSpaceItem
*/
QValueSpaceItem::~QValueSpaceItem()
{
    VS_CALL_ASSERT;

    d->Release();
}

/*!
    Sets the path to \a path.

    Calling this function causes the QValueSpaceItem to disconnect and reconnect to the value
    space with the new \a path.

    Calling this function disconnects all signal/slot connections.
*/
void QValueSpaceItem::setPath(const QString &path)
{
    VS_CALL_ASSERT;

    if (d->path == path)
        return;

    d->Release();

    disconnect();

    d = new QValueSpaceItemPrivate(path);
    d->AddRef();
}

/*!
    Sets the path to the same path as \a item.

    Calling this function causes the QValueSpaceItem to disconnect and reconnect to the value space
    with the a path.

    Calling this function disconnects all signal/slot connections.
*/
void QValueSpaceItem::setPath(QValueSpaceItem *item)
{
    VS_CALL_ASSERT;

    d->Release();

    disconnect();

    d = item->d;

    d->AddRef();
}

QString QValueSpaceItem::path() const
{
    VS_CALL_ASSERT;

    return d->path;
}

/*!
    Changes the path to \a path.
*/
void QValueSpaceItem::cd(const QString &path)
{
    VS_CALL_ASSERT;

    if (path.startsWith(QLatin1Char('/')))
        setPath(path);
    else
        setPath(d->path + QLatin1Char('/') + path);
}

/*!
    Sets the path to parent of the current path.
*/
void QValueSpaceItem::cdUp()
{
    VS_CALL_ASSERT;

    if (d->path == QLatin1String("/"))
        return;

    QString p(d->path);

    int index = p.lastIndexOf(QLatin1Char('/'));

    p.truncate(index);

    setPath(p);
}

/*!
    Returns true if this QValueSpaceItem is connected to atleast one available layer; otherwise
    returns false.  An unconnected QValueSpaceItem is constructed if the filtering parameters
    passed to the constructor eliminate all available layers.
*/
bool QValueSpaceItem::isConnected() const
{
    VS_CALL_ASSERT;

    return !d->readers.isEmpty();
}

/*!
    Returns the value of the sub-item \a subPath of this item, or the value of this item if
    \a subPath is empty.  If the item does not exists, \a def is returned.

    The following code shows how the item and \a subPath relate.

    \code
        QValueSpaceItem base("/Settings");
        QValueSpaceItem equiv("/Settings/Nokia/General/Mappings);

        // Is true
        equiv.value() == base.value("Nokia/General/Mapping");
    \endcode
*/
QVariant QValueSpaceItem::value(const QString & subPath, const QVariant &def) const
{
    VS_CALL_ASSERT;

    QVariant value;
    if (subPath.isEmpty()) {
        for (int ii = d->readers.count(); ii > 0; --ii) {
            if (d->readers[ii - 1].first->value(d->readers[ii - 1].second, &value))
                return value;
        }
    } else {
        const QString vpath(qCanonicalPath(subPath));
        for (int ii = d->readers.count(); ii > 0; --ii) {
            if (d->readers[ii - 1].first->value(d->readers[ii - 1].second, vpath, &value))
                return value;
        }
    }
    return def;
}

/*!
    \overload

    This is a convenience overload and is equivalent to
    \c {value(QString::fromLatin1(subPath), def)}.
*/
QVariant QValueSpaceItem::value(const char *subPath, const QVariant &def) const
{
    VS_CALL_ASSERT;
    return value(QString::fromLatin1(subPath), def);
}

QVariant QValueSpaceItem::valuex(const QVariant &def) const
{
    VS_CALL_ASSERT;

    if (!d->connections || d->connections->connections.value(this) == 0)
        d->connect(this);

    return value(QString(), def);
}

/*!
    \internal

    Registers for change notifications in response to connection to the contentsChanged() signal.
*/
void QValueSpaceItem::connectNotify(const char *signal)
{
    VS_CALL_ASSERT;
    if (QLatin1String(signal) == SIGNAL(contentsChanged()))
        d->connect(this);
    else
        QObject::connectNotify(signal);
}

/*!
    \internal

    Unregisters for change notifications in response to disconnection from the contentsChanged()
    signal.
*/
void QValueSpaceItem::disconnectNotify(const char *signal)
{
    VS_CALL_ASSERT;
    if (QLatin1String(signal) == SIGNAL(contentsChanged()))
        d->disconnect(this);
    else
        QObject::disconnectNotify(signal);
}

/*!
    Returns a list of sub-paths for this item.  For example, given a Value Space
    tree containing:

    \code
    /Settings/Nokia/Device
    /Settings/Nokia/Other
    /Settings/Qt
    /Device/Buttons
    \endcode

    \c { QValueSpaceItem("/Settings").subPaths() } will return a list containing
    \c { { Nokia, Qt } } in no particular order.
*/
QStringList QValueSpaceItem::subPaths() const
{
    VS_CALL_ASSERT;

    QSet<QString> rv;
    for (int ii = 0; ii < d->readers.count(); ++ii)
        rv.unite(d->readers[ii].first->children(d->readers[ii].second));

    QStringList rvs;
    for (QSet<QString>::ConstIterator iter = rv.begin(); iter != rv.end(); ++iter)
        rvs.append(*iter);

    return rvs;
}

QT_END_NAMESPACE

#include "qvaluespaceitem.moc"
