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
    the specified key.  Calls to setValue() and remove() will send requests to all accessible
    layers.

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
    \property QValueSpaceItem::notify

    This property holds whether change notifications are enabled.

    Explicitly enabling change notifications is not normally necessary as they are automatically
    enabled when a connection is made to the contentsChanged() signals.
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
    QHash<QValueSpaceItem *,int> connections;
};

struct QValueSpaceItemPrivate
{
    enum Type { Data, Write };
    QValueSpaceItemPrivate(Type t) : type(t) {}
    Type type;
};

struct QValueSpaceItemPrivateData : public QValueSpaceItemPrivate
{
    QValueSpaceItemPrivateData(const QByteArray &_path,
                               QAbstractValueSpaceLayer::LayerOptions filter =
                                   QAbstractValueSpaceLayer::UnspecifiedLayer)
        : QValueSpaceItemPrivate(Data), refCount(0), connections(0)
    {
        path = qCanonicalPath(_path);

        QValueSpaceManager *man = QValueSpaceManager::instance();
        if (!man)
            return;

        const QList<QAbstractValueSpaceLayer *> & readerList = man->getLayers();

        for (int ii = 0; ii < readerList.count(); ++ii) {
            QAbstractValueSpaceLayer *read = readerList.at(ii);
            if (filter != QAbstractValueSpaceLayer::UnspecifiedLayer &&
                !(read->layerOptions() & filter)) {
                continue;
            }

            QAbstractValueSpaceLayer::Handle handle =
                read->item(QAbstractValueSpaceLayer::InvalidHandle, path);
            if (QAbstractValueSpaceLayer::InvalidHandle != handle) {
                readers.append(qMakePair(read, handle));

                if (read->supportsRequests())
                    read->notifyInterest(handle, true);
            }
        }
    }

    QValueSpaceItemPrivateData(const QByteArray &_path, const QUuid &uuid)
        : QValueSpaceItemPrivate(Data), refCount(0), connections(0)
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

                if (read->supportsRequests())
                    read->notifyInterest(handle, true);
            }
        }
    }

    QValueSpaceItemPrivateData(const QByteArray &_path,
                               const QList<QAbstractValueSpaceLayer *> &readerList)
        : QValueSpaceItemPrivate(Data), refCount(0), connections(0)
    {
        path = qCanonicalPath(_path);

        for (int ii = 0; ii < readerList.count(); ++ii) {
            QAbstractValueSpaceLayer *read = readerList.at(ii);

            QAbstractValueSpaceLayer::Handle handle =
                read->item(QAbstractValueSpaceLayer::InvalidHandle, path);
            if (QAbstractValueSpaceLayer::InvalidHandle != handle) {
                readers.append(qMakePair(read, handle));

                if (read->supportsRequests())
                    read->notifyInterest(handle, true);
            }
        }
    }

    ~QValueSpaceItemPrivateData()
    {
        for (int ii = 0; ii < readers.count(); ++ii) {
            readers[ii].first->notifyInterest(readers[ii].second, false);
            readers[ii].first->removeHandle(readers[ii].second);
        }

        if (connections)
            delete connections;

    }

    void connect(QValueSpaceItem * space)
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
            QHash<QValueSpaceItem *, int>::Iterator iter =
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

    QList<QAbstractValueSpaceLayer *> layers() const
    {
        QList<QAbstractValueSpaceLayer *> layers;

        for (int i = 0; i < readers.count(); ++i)
            layers.append(readers.at(i).first);

        return layers;
    }

    unsigned int refCount;
    QByteArray path;
    QList<QPair<QAbstractValueSpaceLayer *, QAbstractValueSpaceLayer::Handle> > readers;
    QValueSpaceItemPrivateProxy * connections;
};

struct QValueSpaceItemPrivateWrite : public QValueSpaceItemPrivate
{
    QValueSpaceItemPrivateWrite(const QValueSpaceItemPrivateWrite &other)
        : QValueSpaceItemPrivate(other.type), data(other.data), ops(other.ops)
        {}
    QValueSpaceItemPrivateWrite() : QValueSpaceItemPrivate(Write), data(0) {}
    QValueSpaceItemPrivateData * data;

    struct Op {
        enum Type { Set, Remove };
        Type type;
        QByteArray path;
        QVariant value;
        Op(const QByteArray &p, const QVariant &v)
            : type(Set), path(p), value(v) {}
        Op(const QByteArray &p)
            : type(Remove), path(p) {}
    };
    QList<Op> ops;
};

#define QVALUESPACEITEM_D(d) QValueSpaceItemPrivateData *md = \
    (QValueSpaceItemPrivate::Data == d->type) ? \
        static_cast<QValueSpaceItemPrivateData *>(d) : \
        static_cast<QValueSpaceItemPrivateWrite *>(d)->data;

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

    d = new QValueSpaceItemPrivateData("/");
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.

    The constructed Value Space item will access all available
    \l {QAbstractValueSpaceLayer}{layers}.
*/
QValueSpaceItem::QValueSpaceItem(const QByteArray &path, QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivateData(path);
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
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

    d = new QValueSpaceItemPrivateData(path.toUtf8());
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  This
    constructor is equivalent to calling \c {QValueSpaceItem(QByteArray(path), parent)}.

    The constructed Value Space item will access all available
    \l {QAbstractValueSpaceLayer}{layers}.
*/
QValueSpaceItem::QValueSpaceItem(const char *path, QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivateData(QByteArray(path));
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  The
    \a filter parameter is used to limit which layers this QValueSpaceItem will access.

    If a layer matching \a filter is not found an invalid QValueSpaceItem will be constructed.

    \sa isValid()
*/
QValueSpaceItem::QValueSpaceItem(const QByteArray &path,
                                 QAbstractValueSpaceLayer::LayerOptions filter,
                                 QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivateData(path, filter);
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  The
    \a filter parameter is used to limit which layers this QValueSpaceItem will access.  This
    constructor is equivalent to calling \c {QValueSpaceItem(path.toUtf8(), filter, parent)}.

    If a layer matching \a filter is not found an invalid QValueSpaceItem will be constructed.

    \sa isValid()
*/
QValueSpaceItem::QValueSpaceItem(const QString &path,
                                 QAbstractValueSpaceLayer::LayerOptions filter,
                                 QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivateData(path.toUtf8(), filter);
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  The
    \a filter parameter is used to limit which layers this QValueSpaceItem will access.  This
    constructor is equivalent to calling \c {QValueSpaceItem(QByteArray(path), filter, parent)}.

    If a layer matching \a filter is not found an invalid QValueSpaceItem will be constructed.

    \sa isValid()
*/
QValueSpaceItem::QValueSpaceItem(const char *path,
                                 QAbstractValueSpaceLayer::LayerOptions filter,
                                 QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivateData(QByteArray(path), filter);
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  This
    QValueSpaceItem will only use the layer identified by \a uuid.

    Use of this constructor is not platform agnostic.  If possible use one of the constructors that
    take a QAbstractValueSpaceLayer::LayerOptions parameter instead.

    If a layer with a matching \a uuid is not found an invalid QValueSpaceItem will be constructed.

    \sa QAbstractValueSpaceLayer::id(), QValueSpace, isValid()
*/
QValueSpaceItem::QValueSpaceItem(const QByteArray &path, const QUuid &uuid, QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivateData(path, uuid);
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  This
    QValueSpaceItem will only use the layer identified by \a uuid.  This constructor is equivalent
    to calling \c {QValueSpaceItem(path.toUtf8(), uuid, parent)}.

    Use of this constructor is not platform agnostic.  If possible use one of the constructors that
    take a QAbstractValueSpaceLayer::LayerOptions parameter instead.

    If a layer with a matching \a uuid is not found an invalid QValueSpaceItem will be constructed.

    \sa QAbstractValueSpaceLayer::id(), QValueSpace, isValid()
*/
QValueSpaceItem::QValueSpaceItem(const QString &path, const QUuid &uuid, QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivateData(path.toUtf8(), uuid);
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to \a path.  This
    QValueSpaceItem will only use the layer identified by \a uuid.  This constructor is equivalent
    to calling \c {QValueSpaceItem(QByteArray(path), uuid, parent)}.

    Use of this constructor is not platform agnostic.  If possible use one of the constructors that
    take a QAbstractValueSpaceLayer::LayerOptions parameter instead.

    If a layer with a matching \a uuid is not found an invalid QValueSpaceItem will be constructed.

    \sa QAbstractValueSpaceLayer::id(), QValueSpace, isValid()
*/
QValueSpaceItem::QValueSpaceItem(const char *path, const QUuid &uuid, QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    d = new QValueSpaceItemPrivateData(QByteArray(path), uuid);
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    Constructs a QValueSpaceItem with the specified \a parent that refers to the same path as
    \a other.

    The constructed Value Space item will access the same \l {QAbstractValueSpaceLayer}{layers} as
    \a other.
*/
QValueSpaceItem::QValueSpaceItem(const QValueSpaceItem &other, QObject* parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    QVALUESPACEITEM_D(other.d);
    if (other.d->type == QValueSpaceItemPrivate::Data)
        d = md;
    else
        d = new QValueSpaceItemPrivateWrite(*static_cast<QValueSpaceItemPrivateWrite *>(other.d));

    md->AddRef();
}

/*!
    Constructs a QValueSpaceItem with the specified \a parent that refers to the sub-\a path of
    \a base.

    The constructed Value Space item will access the same \l {QAbstractValueSpaceLayer}{layers} as
    \a base.
*/
QValueSpaceItem::QValueSpaceItem(const QValueSpaceItem &base,
                                 const QByteArray &path,
                                 QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    QVALUESPACEITEM_D(base.d);

    if (path.isEmpty() || path == "/")
        d = md;
    else if (md->path == "/")
        d = new QValueSpaceItemPrivateData(path, md->layers());
    else
        d = new QValueSpaceItemPrivateData(md->path + '/' + path, md->layers());

    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to the sub-\a path of
    \a base.  This constructor is equivalent to calling
    \c {QValueSpaceItem(base, path.toUtf8(), parent)}.

    The constructed Value Space item will access the same \l {QAbstractValueSpaceLayer}{layers} as
    \a base.
*/
QValueSpaceItem::QValueSpaceItem(const QValueSpaceItem &base, const QString &path, QObject *parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    QVALUESPACEITEM_D(base.d);

    if (path.isEmpty() || path == QLatin1String("/"))
        d = md;
    else if (md->path == "/")
        d = new QValueSpaceItemPrivateData(path.toUtf8(), md->layers());
    else
        d = new QValueSpaceItemPrivateData(md->path + '/' + path.toUtf8(), md->layers());

    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    \overload

    Constructs a QValueSpaceItem with the specified \a parent that refers to the sub-\a path of
    \a base.  This constructor is equivalent to calling
    \c {QValueSpaceItem(base, QByteArray(path), parent)}.

    The constructed Value Space item will access the same \l {QAbstractValueSpaceLayer}{layers} as
    \a base.
*/
QValueSpaceItem::QValueSpaceItem(const QValueSpaceItem &base, const char *path, QObject* parent)
:   QObject(parent)
{
    VS_CALL_ASSERT;

    QVALUESPACEITEM_D(base.d);

    if (qstrlen(path) == 0 || qstrcmp(path, "/") == 0)
        d = md;
    else if (md->path == "/")
        d = new QValueSpaceItemPrivateData(QByteArray(path), md->layers());
    else
        d = new QValueSpaceItemPrivateData(md->path + '/' + QByteArray(path), md->layers());

    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
    Assign \a other to this.
*/
QValueSpaceItem &QValueSpaceItem::operator=(const QValueSpaceItem& other)
{
    VS_CALL_ASSERT;
    if (other.d == d)
        return *this;

    bool reconnect = false;
    {
        QVALUESPACEITEM_D(d);
        reconnect = md->disconnect(this);
        if (d->type == QValueSpaceItemPrivate::Data) {
            md->Release();
            d = 0;
        } else {
            md->Release();
            delete d;
        }
    }

    {
        QVALUESPACEITEM_D(other.d);
        md->AddRef();

        if (other.d->type == QValueSpaceItemPrivate::Data) {
            d = other.d;
        } else {
            d = new QValueSpaceItemPrivateWrite(
                *static_cast<QValueSpaceItemPrivateWrite *>(other.d));
        }
    }

    if (reconnect) {
        QVALUESPACEITEM_D(d);
        md->connect(this);
    }

    return *this;
}

/*!
    Destroys the QValueSpaceItem
*/
QValueSpaceItem::~QValueSpaceItem()
{
    VS_CALL_ASSERT;
    QVALUESPACEITEM_D(d);
    if(d->type == QValueSpaceItemPrivate::Write)
        delete d;

    md->Release();
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

    QVALUESPACEITEM_D(d);
    if (md->path == path.toUtf8())
        return;

    if (d->type == QValueSpaceItemPrivate::Write)
        delete d;

    md->Release();

    disconnect();

    d = new QValueSpaceItemPrivateData(path.toUtf8());
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

QString QValueSpaceItem::path() const
{
    VS_CALL_ASSERT;
    QVALUESPACEITEM_D(d);

    return QString::fromUtf8(md->path.constData(), md->path.length());
}

/*!
    Returns true if this object is valid; otherwise returns false.  An object is valid if there is
    atleast one layer available.  An invalid object is constructed if the filtering parameters
    passed to the QValueSpaceItem constructor eliminate all available layers.
*/
bool QValueSpaceItem::isValid() const
{
    VS_CALL_ASSERT;
    QVALUESPACEITEM_D(d);

    return !md->readers.isEmpty();
}

/*!
    Request that the item be removed.  The provider of the item determines whether
    the request is honored or ignored.

    \i {Note:} This call asynchronously \i asks the current provider of the object to
    change the value.  To explicitly make a change use QValueSpaceObject.

    Returns false if none of the available value space layers support sending requests; otherwise
    returns true.

    \sa QValueSpaceObject::itemRemove()
*/
bool QValueSpaceItem::remove()
{
    VS_CALL_ASSERT;
    return remove(QByteArray());
}

/*!
    \overload

    Request that the \a subPath of item be removed.  The provider of the sub path
    determines whether the request is honored or ignored.

    \i {Note:} This call asynchronously \i asks the current provider of the object to
    change the value.  To explicitly make a change use QValueSpaceObject.

    Returns false if none of the available value space layers support sending requests; otherwise
    returns true.

    \sa QValueSpaceObject::itemRemove()
*/
bool QValueSpaceItem::remove(const QByteArray &subPath)
{
    VS_CALL_ASSERT;

    if (d->type == QValueSpaceItemPrivate::Data) {
        bool supportsRequests = false;

        QValueSpaceItemPrivateData *md = static_cast<QValueSpaceItemPrivateData *>(d);
        for (int ii = md->readers.count(); ii > 0 && !supportsRequests; --ii)
            supportsRequests |= md->readers[ii - 1].first->supportsRequests();

        if (!supportsRequests)
            return false;
    }

    if(QValueSpaceItemPrivate::Data == d->type) {
        QValueSpaceItemPrivateWrite * write = new QValueSpaceItemPrivateWrite();
        write->data = static_cast<QValueSpaceItemPrivateData *>(d);
        d = write;
    }
    QValueSpaceItemPrivateWrite * write =
        static_cast<QValueSpaceItemPrivateWrite *>(d);

    write->ops.append(QValueSpaceItemPrivateWrite::Op(subPath));

    return true;
}

/*!
    \overload

    Request that the \a subPath of item be removed.  The provider of the sub path
    determines whether the request is honored or ignored.

    \i {Note:} This call asynchronously \i asks the current provider of the object to
    change the value.  To explicitly make a change use QValueSpaceObject.

    Returns false if none of the available value space layers support sending requests; otherwise
    returns true.

    \sa QValueSpaceObject::itemRemove()
*/
bool QValueSpaceItem::remove(const char *subPath)
{
    VS_CALL_ASSERT;
    return remove(QByteArray(subPath));
}

/*!
    \overload

    Request that the \a subPath of item be removed.  The provider of the sub path
    determines whether the request is honored or ignored.

    \i {Note:} This call asynchronously \i asks the current provider of the object to
    change the value.  To explicitly make a change use QValueSpaceObject.

    Returns false if none of the available value space layers support sending requests; otherwise
    returns true.

    \sa QValueSpaceObject::itemRemove()
*/
bool QValueSpaceItem::remove(const QString &subPath)
{
    VS_CALL_ASSERT;
    return remove(subPath.toUtf8());
}

/*!
    Request that the value of this item be changed to \a value.  The provider of
    the item determines whether the request is honored or ignored.

    \i {Note:} This call asynchronously \i asks the current provider of the object to
    change the value.  To explicitly make a change use QValueSpaceObject.

    Returns false if none of the available value space layers support sending requests; otherwise
    returns true.

    \sa QValueSpaceObject::itemSetValue()
*/
bool QValueSpaceItem::setValue(const QVariant &value)
{
    VS_CALL_ASSERT;
    return setValue(QByteArray(), value);
}

/*!
    \overload

    Request that the value of the \a subPath of this item be changed to \a value.
    The provider of the sub path determines whether the request is honored or
    ignored.

    \i {Note:} This call asynchronously \i asks the current provider of the object to
    change the value.  To explicitly make a change use QValueSpaceObject.

    Returns false if none of the available value space layers support sending requests; otherwise
    returns true.

    \sa QValueSpaceObject::itemSetValue()
*/
bool QValueSpaceItem::setValue(const QByteArray &subPath,
                               const QVariant &value)
{
    VS_CALL_ASSERT;

    if (d->type == QValueSpaceItemPrivate::Data) {
        bool supportsRequests = false;

        QValueSpaceItemPrivateData *md = static_cast<QValueSpaceItemPrivateData *>(d);
        for (int ii = md->readers.count(); ii > 0 && !supportsRequests; --ii)
            supportsRequests |= md->readers[ii - 1].first->supportsRequests();

        if (!supportsRequests)
            return false;
    }

    if(QValueSpaceItemPrivate::Data == d->type) {
        QValueSpaceItemPrivateWrite * write = new QValueSpaceItemPrivateWrite();
        write->data = static_cast<QValueSpaceItemPrivateData *>(d);
        d = write;
    }
    QValueSpaceItemPrivateWrite * write =
        static_cast<QValueSpaceItemPrivateWrite *>(d);


    write->ops.append(QValueSpaceItemPrivateWrite::Op(subPath, value));

    return true;
}

/*!
    \overload

    Request that the value of the \a subPath of this item be changed to \a value.
    The provider of the sub path determines whether the request is honored or
    ignored.

    \i {Note:} This call asynchronously \i asks the current provider of the object to
    change the value.  To explicitly make a change use QValueSpaceObject.

    Returns false if none of the available value space layers support sending requests; otherwise
    returns true.

    \sa QValueSpaceObject::itemSetValue()
*/
bool QValueSpaceItem::setValue(const char * subPath,
                               const QVariant &value)
{
    VS_CALL_ASSERT;
    return setValue(QByteArray(subPath), value);
}

/*!
    \overload

    Request that the value of the \a subPath of this item be changed to \a value.
    The provider of the sub path determines whether the request is honored or
    ignored.

    \i {Note:} This call asynchronously \i asks the current provider of the object to
    change the value.  To explicitly make a change use QValueSpaceObject.

    Returns false if none of the available value space layers support sending requests; otherwise
    returns true.

    \sa QValueSpaceObject::itemSetValue()
*/
bool QValueSpaceItem::setValue(const QString & subPath,
                               const QVariant &value)
{
    VS_CALL_ASSERT;
    return setValue(subPath.toUtf8(), value);
}

/*!
    Commit all change requests made by calls to setValue() or remove().

    Returns true if there are no pending change requests to send or the requests were successfully
    sent to all provides; otherwise returns false.
*/
bool QValueSpaceItem::sync()
{
    VS_CALL_ASSERT;
    if (d->type == QValueSpaceItemPrivate::Data)
        return true;

    QValueSpaceItemPrivateWrite * write = static_cast<QValueSpaceItemPrivateWrite *>(d);
    QValueSpaceItemPrivateData * md = write->data;
    QList<QValueSpaceItemPrivateWrite::Op> ops = write->ops;
    write->ops.clear();

    bool rv = true;
    for (int ii = 0; ii < ops.count(); ++ii) {
        const QByteArray & path = ops.at(ii).path;
        const QVariant & value = ops.at(ii).value;

        if (ops.at(ii).type == QValueSpaceItemPrivateWrite::Op::Set) {
            if (path.isEmpty()) {
                for (int ii = md->readers.count(); ii > 0; --ii) {
                    if (!md->readers[ii - 1].first->requestSetValue(md->readers[ii - 1].second,
                                                                    value)) {
                        rv = false;
                    }
                }
            } else {
                const QByteArray vpath(qCanonicalPath(path));
                for (int ii = md->readers.count(); ii > 0; --ii) {
                    if (!md->readers[ii - 1].first->requestSetValue(md->readers[ii - 1].second,
                                                                    vpath, value)) {
                        rv = false;
                    }
                }
            }
        } else {
            if (path.isEmpty()) {
                for (int ii = md->readers.count(); ii > 0; --ii) {
                    if (!md->readers[ii - 1].first->requestRemoveValue(md->readers[ii - 1].second))
                        rv = false;
                }
            } else {
                const QByteArray vpath(qCanonicalPath(path));
                for (int ii = md->readers.count(); ii > 0; --ii) {
                    if (!md->readers[ii - 1].first->requestRemoveValue(md->readers[ii - 1].second,
                                                                       vpath)) {
                        rv = false;
                    }
                }
            }
        }
    }

    for (int ii = md->readers.count(); ii > 0; --ii) {
        if (!md->readers[ii - 1].first->syncRequests())
            rv = false;
    }
    return rv;
}

/*!
    \overload

    This is a convenience overload and is equivalent to
    \c {value(subPath.toUtf8(), def)}.
*/
QVariant QValueSpaceItem::value(const QString & subPath, const QVariant &def) const
{
    VS_CALL_ASSERT;
    return value(subPath.toUtf8(), def);
}

/*!
    \overload

    This is a convenience overload and is equivalent to
    \c {value(QByteArray(subPath), def)}.
*/
QVariant QValueSpaceItem::value(const char * subPath, const QVariant &def) const
{
    VS_CALL_ASSERT;
    return value(QByteArray(subPath), def);
}


/*!
    Returns the value of sub-item \a subPath of this item, or the value of this
    item if \a subPath is empty.  If the item does not exists, \a def is returned.

    The following code shows how the item and \a subPath relate.

    \code
    QValueSpaceItem base("/Settings");
    QValueSpaceItem equiv("/Settings/Nokia/General/Mappings);

    // Is true
    equiv.value() == base.value("Nokia/General/Mapping");
    \endcode
*/
QVariant QValueSpaceItem::value(const QByteArray & subPath, const QVariant &def) const
{
    VS_CALL_ASSERT;
    QVALUESPACEITEM_D(d);
    QVariant value;
    if(subPath.isEmpty()) {
        for(int ii = md->readers.count(); ii > 0; --ii) {
            if(md->readers[ii - 1].first->value(md->readers[ii - 1].second,
                                                &value))
                return value;
        }
    } else {
        const QByteArray vpath(qCanonicalPath(subPath));
        for(int ii = md->readers.count(); ii > 0; --ii) {
            if(md->readers[ii - 1].first->value(md->readers[ii - 1].second,
                                                vpath, &value))
                return value;
        }
    }
    return def;
}

bool QValueSpaceItem::notify()
{
    QVALUESPACEITEM_D(d);

    if (!md->connections)
        return false;

    QHash<QValueSpaceItem *, int>::ConstIterator iter =
        md->connections->connections.constFind(this);

    if (iter == md->connections->connections.end())
        return false;

    return *iter > 0;
}

void QValueSpaceItem::setNotify(bool notify)
{
    if (notify)
        connectNotify(SIGNAL(contentsChanged()));
    else
        disconnectNotify(SIGNAL(contentsChanged()));
}

/*!
    \internal

    Registers for change notifications in response to connection to the contentsChanged() signal.
*/
void QValueSpaceItem::connectNotify(const char *signal)
{
    VS_CALL_ASSERT;
    if(QLatin1String(signal) == SIGNAL(contentsChanged())) {
        QVALUESPACEITEM_D(d);
        md->connect(this);
    } else {
        QObject::connectNotify(signal);
    }
}

/*!
    \internal

    Unregisters for change notifications in response to disconnection from the contentsChanged()
    signal.
*/
void QValueSpaceItem::disconnectNotify(const char *signal)
{
    VS_CALL_ASSERT;
    if(QLatin1String(signal) == SIGNAL(contentsChanged())) {
        QVALUESPACEITEM_D(d);
        md->disconnect(this);
    } else {
        QObject::disconnectNotify(signal);
    }
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
QList<QString> QValueSpaceItem::subPaths() const
{
    VS_CALL_ASSERT;
    QVALUESPACEITEM_D(d);
    QSet<QByteArray> rv;
    for(int ii = 0; ii < md->readers.count(); ++ii)
        rv.unite(md->readers[ii].first->children(md->readers[ii].second));

    QList<QString> rvs;
    for(QSet<QByteArray>::ConstIterator iter = rv.begin();
            iter != rv.end();
            ++iter)
        rvs.append(QString::fromUtf8(iter->constData(), iter->length()));

    return rvs;
}

QT_END_NAMESPACE

#include "qvaluespaceitem.moc"
