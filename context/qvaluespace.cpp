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
#include "qvaluespace.h"
#include "qvaluespacemanager_p.h"
#include "qmallocpool.h"

#include <QByteArray>
#include <QObject>
#include <QMap>
#include <QPair>
#include <QCoreApplication>
#include <QThread>
#include <QSet>
#include <QString>
#include <QVarLengthArray>
#include <qpacketprotocol.h>

#include <QtCore/qdebug.h>

#define VS_CALL_ASSERT Q_ASSERT(!QCoreApplication::instance() || QCoreApplication::instance()->thread() == QThread::currentThread());

///////////////////////////////////////////////////////////////////////////////
// Layer plug-in interface documentation
///////////////////////////////////////////////////////////////////////////////

/*!
  \class QAbstractValueSpaceLayer
  \brief The QAbstractValueSpaceLayer class provides support for adding new logical data
  layers to the Qt Value Space.
  */

/*!
    \macro QVALUESPACE_AUTO_INSTALL_LAYER(className)

    \relates QAbstractValueSpaceLayer

    This macro installs new value space layer. \a className is the name of the class implementing
    the new layer.

*/

/*!
  \typedef QAbstractValueSpaceLayer::HANDLE

  The HANDLE type is an opaque, pointer sized contextual handle used to
  represent paths within value space layers.  HANDLES are only ever created by
  QAbstractValueSpaceLayer::item() and are always released by calls to
  QAbstractValueSpaceLayer::remHandle().  The special, \c {InvalidHandle} is reserved to
  represent an invalid handle.
 */

/*!
  \enum QAbstractValueSpaceLayer::Properties

  To allow for efficient layer implementations, expensive handle operations,
  currently only monitoring for changes, are enabled and disabled as needed on
  a per-handle basis.  The Properties enumeration is a bitmask representing
  the different properties that can exist on a handle.

  \value Publish Enable change notification for the handle.  When set, the layer
         should emit QAbstractValueSpaceLayer::handleChanged() signals when appropriate
         for the handle.
 */

/*!
  \enum QAbstractValueSpaceLayer::Type

  Value Space layers are initialized in either a "Server" or a "Client"
  context.  There is only a single server in the value space architecture, and
  its layers are always initialized before any clients.  This distinction allows
  layers to implement Client/Server architecture \i {if required}.  If not,
  layers are free to treat Server and Client contexts identically.

  \value Server The layer is being initialized in the "server" context.
  \value Client The layer is being initialized in the "client" context.
 */

/*!
  \fn QString QAbstractValueSpaceLayer::name()

  Returns the name of the Value Space layer.  This name is only used for
  diagnostics purposes.
 */

/*!
  \fn bool QAbstractValueSpaceLayer::startup(Type type)

  Called by the Value Space system to initialize each layer.  The \a type
  parameter will be set accordingly, and layer implementors can use this to
  implement a client/server architecture if desired.
  Returns true upon success; otherwise returns false.
 */

/*!
  \fn bool QAbstractValueSpaceLayer::restart()

  Called by the Value Space system to restart each layer.
  Returns true upon success; otherwise returns false.
 */

/*!
  \fn void QAbstractValueSpaceLayer::shutdown()

  Called by the Value Space system to uninitialize each layer.  The shutdown
  call can be used to release any resources in use by the layer.
 */

/*!
  \fn QUuid QAbstractValueSpaceLayer::id()

  Return a globally unique id for the layer.  This id is used to break ordering
  ties.
 */

/*!
  \fn unsigned int QAbstractValueSpaceLayer::order()

  Return the position in the Value Space layer stack that this layer should
  reside.  Higher numbers mean the layer has a higher precedence and its values
  will "shadow" those below it.  If two layers specify the same ordering, the
  QAbstractValueSpaceLayer::id() value is used to break the tie.
 */

/*!
  \fn bool QAbstractValueSpaceLayer::value(HANDLE handle, QVariant *data)

  Returns the value for a particular \a handle.  If a value is available, the
  layer will set \a data and return true.  If no value is available, false is
  returned.
  */

/*!
  \fn bool QAbstractValueSpaceLayer::value(HANDLE handle, const QByteArray &subPath, QVariant *data)

  Returns the value for a particular \a subPath of \a handle.  If a value is
  available, the layer will set \a data and return true.  If no value is
  available, false is returned.
  */

/*!
  \fn bool QAbstractValueSpaceLayer::remove(HANDLE handle)

  Process a client side QValueSpaceItem::remove() for the specified \a handle.
  Return true on success and false on failure.
 */

/*!
  \fn bool QAbstractValueSpaceLayer::remove(HANDLE handle, const QByteArray &subPath)

  Process a client side QValueSpaceItem::remove() for the specified \a subPath
  of \a handle.  Return true on success and false on failure.
 */

/*!
  \fn bool QAbstractValueSpaceLayer::setValue(HANDLE handle, const QVariant &value)

  Process a client side QValueSpaceItem::setValue() for the specified \a handle
  and \a value.  Return true on success and false on failure.
  */

/*!
  \fn bool QAbstractValueSpaceLayer::setValue(HANDLE handle, const QByteArray &subPath, const QVariant &value)

  Process a client side QValueSpaceItem::setValue() for the specified \a subPath
  of \a handle and the provided \a value.  Return true on success and false on
  failure.
  */

/*!
  \fn bool QAbstractValueSpaceLayer::syncChanges()

  Commit any changes made through setValue().  Return true on success and false
  on failure.
  */

/*!
  \fn QSet<QByteArray> QAbstractValueSpaceLayer::children(HANDLE handle)

  Returns the set of children of \a handle.  For example, in a layer providing
  the following items:

  \code
  /Device/Configuration/Applications/FocusedApplication
  /Device/Configuration/Buttons/PrimaryInput
  /Device/Configuration/Name
  \endcode

  a request for children of "/Device/Configuration" will return
  { "Applications", "Buttons", "Name" }.
 */

/*!
  \fn HANDLE QAbstractValueSpaceLayer::item(HANDLE parent, const QByteArray &subPath)

  Returns a new opaque handle for the requested \a subPath of \a parent.  If
  \a parent is an InvalidHandle, \a subPath is an absolute path.
  */

/*!
  \fn void QAbstractValueSpaceLayer::setProperty(HANDLE handle, Properties property)

  Apply the specified \a property mask to \a handle.
 */

/*!
  \fn void QAbstractValueSpaceLayer::remHandle(HANDLE handle)

  Releases a \a handle previously returned from QAbstractValueSpaceLayer::item().
 */

/*!
  \fn void QAbstractValueSpaceLayer::handleChanged(unsigned int handle)

  Emitted whenever the \a handle's value, or any sub value, changes.
  */

///////////////////////////////////////////////////////////////////////////////
// define QValueSpace
///////////////////////////////////////////////////////////////////////////////
/*!
  \namespace QValueSpace

  \brief The QValueSpace namespace provides methods that are useful to Value
  Space layer implementors.

  Value Space layers that are available at link time can be automatically
  installed using \c {QVALUESPACE_AUTO_INSTALL_LAYER(name)} macro.  The method
  \c {QAbstractValueSpaceLayer * name::instance()} must exist and return a pointer to
  the layer to install.  The \c {QVALUESPACE_AUTO_INSTALL_LAYER(name)} macro
  will only invoke this method \i {after} QApplication has been constructed,
  making it safe to use any Qt class in your layer's constructor.
 */

/*!
  \typedef QValueSpace::LayerCreateFunc
  \internal
  */
/*!
  \class QValueSpace::AutoInstall

  \internal
  */
/*!
  Initialize the value space.  This method only needs to be called by the value
  space manager process, and should be called before any process in the system
  uses a value space class.
 */
void QValueSpace::initValuespaceManager()
{
    QValueSpaceManager::instance()->initServer();
}

/*!
  Initialize the value space.  This method only needs to be called by layer
  implementers to force initialization of the value space.
  */
void QValueSpace::initValuespace()
{
    QValueSpaceManager::instance()->init();
}

/*!
  Re-initialize the value space.  This method only needs to be called by layer
  implementers to force re-initialization of the value space.
  */
void QValueSpace::reinitValuespace()
{
    QValueSpaceManager::instance()->reinit();
}

/*!
  Used by value space layer implementations to install themselves into the
  system.  \a layer should be a pointer to the layer to install.
  */
void QValueSpace::installLayer(QAbstractValueSpaceLayer * layer)
{
    QValueSpaceManager::instance()->install(layer);
}

/*!
  \internal
  */
void QValueSpace::installLayer(LayerCreateFunc func)
{
    QValueSpaceManager::instance()->install(func);
}


///////////////////////////////////////////////////////////////////////////////
// define QValueSpaceItem
///////////////////////////////////////////////////////////////////////////////
class QValueSpaceItemPrivateProxy : public QObject
{
Q_OBJECT
signals:
    void changed();

public slots:
    void handleChanged(unsigned int handle)
    {
        for(int ii = 0; ii < readers.count(); ++ii)
            if(readers.at(ii).second == handle) {
                emit changed();
                return;
            }
    }

    void objDestroyed()
    {
        connections.remove((QValueSpaceItem *)sender());
    }

public:
    QList<QPair<QAbstractValueSpaceLayer *, QAbstractValueSpaceLayer::HANDLE> > readers;
    QHash<QValueSpaceItem *,int> connections;
};

struct QValueSpaceItemPrivate
{
    enum Type { Data, Write };
    QValueSpaceItemPrivate(Type _t) : type(_t) {}

    Type type;
};

struct QValueSpaceItemPrivateData : public QValueSpaceItemPrivate
{
    QValueSpaceItemPrivateData(const QByteArray &_path)
        : QValueSpaceItemPrivate(Data), refCount(0), connections(0)
    {
        if(*_path.constData() != '/')
            path.append("/");
        path.append(_path);
        if(path.length() != 1 && '/' == *(path.constData() + path.length() - 1))
            path.truncate(path.length() - 1);

        QValueSpaceManager *man = QValueSpaceManager::instance();
        if (!man)
            return;

        const QList<QAbstractValueSpaceLayer *> & readerList = man->getLayers();

        for(int ii = 0; ii < readerList.count(); ++ii) {
            QAbstractValueSpaceLayer * read = readerList.at(ii);
            if(!read) continue;

            QAbstractValueSpaceLayer::HANDLE handle = read->item(QAbstractValueSpaceLayer::InvalidHandle, path);
            if(QAbstractValueSpaceLayer::InvalidHandle != handle) {
                readers.append(qMakePair(read, handle));
            }
        }
    }

    ~QValueSpaceItemPrivateData()
    {
        for(int ii = 0; ii < readers.count(); ++ii)
            readers[ii].first->remHandle(readers[ii].second);

        if(connections)
            delete connections;

    }

    void connect(QValueSpaceItem * space)
    {
        if(!connections) {
            connections = new QValueSpaceItemPrivateProxy;
            connections->readers = readers;
            connections->connections.insert(space,1);
            QObject::connect(space, SIGNAL(destroyed(QObject*)),
                             connections, SLOT(objDestroyed()));
            QObject::connect(connections, SIGNAL(changed()),
                             space, SIGNAL(contentsChanged()));
            for(int ii = 0; ii < readers.count(); ++ii) {
                readers.at(ii).first->setProperty(readers.at(ii).second, QAbstractValueSpaceLayer::Publish);
                QObject::connect(readers.at(ii).first, SIGNAL(handleChanged(uint)), connections, SLOT(handleChanged(uint)));
            }
        } else if(!connections->connections.contains(space)) {
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
        if(connections) {
            QHash<QValueSpaceItem *, int>::Iterator iter =
                connections->connections.find(space);
            if(iter != connections->connections.end()) {
                --(*iter);
                if(!*iter) {
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
        if(!refCount)
            delete this;
    }

    unsigned int refCount;
    QByteArray path;
    QList<QPair<QAbstractValueSpaceLayer *, QAbstractValueSpaceLayer::HANDLE> > readers;
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
  QValueSpaceObject instance exists - that is, they are not persistant.  If
  the object is destroyed, or the application containing it exits (or crashes)
  the items are removed.

  Change notification is modelled in a similar way.  Applications subscribe to
  notifications at a particular object (ie. item) in the tree.  If anything in
  that object (ie. under that item) changes, the application is notified.  This
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

  \i {Note:} The QValueSpaceItem class is not thread safe and may only be used from 
  an application's main thread.
 */

#define QVALUESPACEITEM_D(d) QValueSpaceItemPrivateData *md = (QValueSpaceItemPrivate::Data == d->type)?static_cast<QValueSpaceItemPrivateData *>(d):static_cast<QValueSpaceItemPrivateWrite *>(d)->data;

/*!
  \fn QValueSpaceItem::contentsChanged()

  Emitted whenever the value of this item, or any sub-items changes.
 */
/*!
  Construct a new QValueSpaceItem with the specified \a parent that refers to the same path as \a other.
 */
QValueSpaceItem::QValueSpaceItem(const QValueSpaceItem &other, QObject* parent)
: QObject(parent), d(other.d)
{
    VS_CALL_ASSERT;
    QVALUESPACEITEM_D(other.d);
    if(QValueSpaceItemPrivate::Data == other.d->type) {
        d = md;
    } else {
        d = new QValueSpaceItemPrivateWrite(*static_cast<QValueSpaceItemPrivateWrite *>(other.d));
    }

    md->AddRef();
}

/*!
  Construct a new QValueSpaceItem with the specified \a parent that refers to the root path .
 */
QValueSpaceItem::QValueSpaceItem( QObject* parent )
: QObject( parent ), d(0)
{
    VS_CALL_ASSERT;
    d = new QValueSpaceItemPrivateData("/");
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
  \overload

  Construct a new QValueSpaceItem with the specified parent that refers to the sub-\a path of \a base.
  This constructor is equivalent to \c {QValueSpaceItem(base, path.toUtf8())}.
 */
QValueSpaceItem::QValueSpaceItem(const QValueSpaceItem &base,
                                 const QString &path,
                                 QObject* parent)
: QObject( parent ), d(0)
{
    VS_CALL_ASSERT;
    QVALUESPACEITEM_D(base.d);

    if(path == QLatin1String("/")) {
        if(QValueSpaceItemPrivate::Data == base.d->type) {
            d = md;
        } else {
            d = new QValueSpaceItemPrivateWrite(*static_cast<QValueSpaceItemPrivateWrite *>(base.d));
        }
        md->AddRef();
    } else {
        if("/" == md->path) {
            d = new QValueSpaceItemPrivateData(md->path + path.toUtf8());
        } else  {
            d = new QValueSpaceItemPrivateData(md->path + "/" + path.toUtf8());
        }
        
        static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
    }
}

/*!
  \overload

  Construct a new QValueSpaceItem with the specified \a parent that refers to the sub-\a path of \a base.
  This constructor is equivalent to
  \c {QValueSpaceItem(base, QByteArray(path))}.
 */
QValueSpaceItem::QValueSpaceItem(const QValueSpaceItem &base,
                                 const char *path,
                                 QObject* parent)
: QObject( parent ), d(0)
{
    VS_CALL_ASSERT;
    QVALUESPACEITEM_D(base.d);

    if(1 == strlen(path) && '/' == *path) {
        if(QValueSpaceItemPrivate::Data == base.d->type) {
            d = md;
        } else {
            d = new QValueSpaceItemPrivateWrite(*static_cast<QValueSpaceItemPrivateWrite *>(base.d));
        }
        md->AddRef();
    } else {
        if("/" == md->path)
            d = new QValueSpaceItemPrivateData(md->path + QByteArray(path));
        else
            d = new QValueSpaceItemPrivateData(md->path + "/" + QByteArray(path));
        static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
    }
}

/*!
  Construct a new QValueSpaceItem with the specified \a parent that refers to the sub-\a path of \a base.
 */
QValueSpaceItem::QValueSpaceItem(const QValueSpaceItem &base,
                                 const QByteArray &path,
                                 QObject* parent)
: QObject( parent ), d(0)
{
    VS_CALL_ASSERT;
    QVALUESPACEITEM_D(base.d);

    if(path == "/") {
        if(QValueSpaceItemPrivate::Data == base.d->type) {
            d = md;
        } else {
            d = new QValueSpaceItemPrivateWrite(*static_cast<QValueSpaceItemPrivateWrite *>(base.d));
        }
        md->AddRef();
    } else {
        
        if("/" == md->path)
            d = new QValueSpaceItemPrivateData(md->path + path);
        else
            d = new QValueSpaceItemPrivateData(md->path + "/" + path);
        static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
    }
}

/*!
  Assign \a other to this.
  */
QValueSpaceItem &QValueSpaceItem::operator=(const QValueSpaceItem& other)
{
    VS_CALL_ASSERT;
    if(other.d == d)
        return *this;

    bool reconnect = false;
    {
        QVALUESPACEITEM_D(d);
        reconnect = md->disconnect(this);
        if(d->type == QValueSpaceItemPrivate::Data) {
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

        if(other.d->type == QValueSpaceItemPrivate::Data) {
            d = other.d;
        } else {
            d = new QValueSpaceItemPrivateWrite(*static_cast<QValueSpaceItemPrivateWrite *>(other.d));
        }
    }

    if(reconnect) {
        QVALUESPACEITEM_D(d);
        md->connect(this);
    }

    return *this;
}

/*!
  \overload

  Construct a new QValueSpaceItem with the specified \a parent that refers to \a path.  This constructor is
  equivalent to \c {QValueSpaceItem(path.toUtf8())}.
 */
QValueSpaceItem::QValueSpaceItem(const QString &path, QObject* parent)
: QObject( parent ), d(0)
{
    VS_CALL_ASSERT;
    d = new QValueSpaceItemPrivateData(path.toUtf8());
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}

/*!
  \overload

  Construct a new QValueSpaceItem with the specified \a parent that refers to \a path.  This constructor is
  equivalent to \c {QValueSpaceItem(QByteArray(path))}.
 */
QValueSpaceItem::QValueSpaceItem(const char *path, QObject* parent)
: QObject( parent ), d(0)
{
    VS_CALL_ASSERT;
    d = new QValueSpaceItemPrivateData(path);
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
}
/*!
  Construct a new QValueSpaceItem with the specified \a parent that refers to \a path.
 */
QValueSpaceItem::QValueSpaceItem(const QByteArray &path, QObject* parent)
: QObject(parent), d(0)
{
    VS_CALL_ASSERT;
    d = new QValueSpaceItemPrivateData(path);
    static_cast<QValueSpaceItemPrivateData *>(d)->AddRef();
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
  Returns the item name of this QValueSpaceItem.
  */
QString QValueSpaceItem::itemName() const
{
    VS_CALL_ASSERT;
    QVALUESPACEITEM_D(d);

    return QString::fromUtf8(md->path.constData(), md->path.length());
}

/*!
  Request that the item be removed.  The provider of the item determines whether
  the request is honored or ignored.

  \i {Note:} This call asynchronously \i asks the current provider of the object to 
  change the value.  To explicitly make a change use QValueSpaceObject.
  
  \sa QValueSpaceObject::itemRemove()
 */
void QValueSpaceItem::remove()
{
    VS_CALL_ASSERT;
    remove(QByteArray());
}

/*!
  \overload

  Request that the \a subPath of item be removed.  The provider of the sub path
  determines whether the request is honored or ignored.

  \i {Note:} This call asynchronously \i asks the current provider of the object to 
  change the value.  To explicitly make a change use QValueSpaceObject.
  
  \sa QValueSpaceObject::itemRemove()
 */
void QValueSpaceItem::remove(const QByteArray &subPath)
{
    VS_CALL_ASSERT;
    if(QValueSpaceItemPrivate::Data == d->type) {
        QValueSpaceItemPrivateWrite * write = new QValueSpaceItemPrivateWrite();
        write->data = static_cast<QValueSpaceItemPrivateData *>(d);
        d = write;
    }
    QValueSpaceItemPrivateWrite * write =
        static_cast<QValueSpaceItemPrivateWrite *>(d);

    write->ops.append(QValueSpaceItemPrivateWrite::Op(subPath));
}

/*!
  \overload

  Request that the \a subPath of item be removed.  The provider of the sub path
  determines whether the request is honored or ignored.

  \i {Note:} This call asynchronously \i asks the current provider of the object to 
  change the value.  To explicitly make a change use QValueSpaceObject.
  
  \sa QValueSpaceObject::itemRemove()
 */
void QValueSpaceItem::remove(const char *subPath)
{
    VS_CALL_ASSERT;
    remove(QByteArray(subPath));
}

/*!
  \overload

  Request that the \a subPath of item be removed.  The provider of the sub path
  determines whether the request is honored or ignored.

  \i {Note:} This call asynchronously \i asks the current provider of the object to 
  change the value.  To explicitly make a change use QValueSpaceObject.
  
  \sa QValueSpaceObject::itemRemove()
 */
void QValueSpaceItem::remove(const QString &subPath)
{
    VS_CALL_ASSERT;
    remove(subPath.toUtf8());
}

/*!
  Request that the value of this item be changed to \a value.  The provider of
  the item determines whether the request is honored or ignored.

  \i {Note:} This call asynchronously \i asks the current provider of the object to 
  change the value.  To explicitly make a change use QValueSpaceObject.

  \sa QValueSpaceObject::itemSetValue()
  */
void QValueSpaceItem::setValue(const QVariant &value)
{
    VS_CALL_ASSERT;
    setValue(QByteArray(), value);
}

/*!
  \overload

  Request that the value of the \a subPath of this item be changed to \a value.
  The provider of the sub path determines whether the request is honored or
  ignored.

  \i {Note:} This call asynchronously \i asks the current provider of the object to 
  change the value.  To explicitly make a change use QValueSpaceObject.

  \sa QValueSpaceObject::itemSetValue()
  */
void QValueSpaceItem::setValue(const QByteArray &subPath,
                               const QVariant &value)
{
    VS_CALL_ASSERT;
    if(QValueSpaceItemPrivate::Data == d->type) {
        QValueSpaceItemPrivateWrite * write = new QValueSpaceItemPrivateWrite();
        write->data = static_cast<QValueSpaceItemPrivateData *>(d);
        d = write;
    }
    QValueSpaceItemPrivateWrite * write =
        static_cast<QValueSpaceItemPrivateWrite *>(d);


    write->ops.append(QValueSpaceItemPrivateWrite::Op(subPath, value));
}

/*!
  \overload

  Request that the value of the \a subPath of this item be changed to \a value.
  The provider of the sub path determines whether the request is honored or
  ignored.

  \i {Note:} This call asynchronously \i asks the current provider of the object to 
  change the value.  To explicitly make a change use QValueSpaceObject.

  \sa QValueSpaceObject::itemSetValue()
  */
void QValueSpaceItem::setValue(const char * subPath,
                               const QVariant &value)
{
    VS_CALL_ASSERT;
    setValue(QByteArray(subPath), value);
}

/*!
  \overload

  Request that the value of the \a subPath of this item be changed to \a value.
  The provider of the sub path determines whether the request is honored or
  ignored.

  \i {Note:} This call asynchronously \i asks the current provider of the object to 
  change the value.  To explicitly make a change use QValueSpaceObject.

  \sa QValueSpaceObject::itemSetValue()
  */
void QValueSpaceItem::setValue(const QString & subPath,
                               const QVariant &value)
{
    VS_CALL_ASSERT;
    setValue(subPath.toUtf8(), value);
}

/*!
  Commit all changes made by calls to setValue() or remove().  The return value
  is reserved for future use.
 */
bool QValueSpaceItem::sync()
{
    VS_CALL_ASSERT;
    if(d->type == QValueSpaceItemPrivate::Data)
        return true;

    QValueSpaceItemPrivateWrite * write = static_cast<QValueSpaceItemPrivateWrite *>(d);
    QValueSpaceItemPrivateData * md = write->data;
    QList<QValueSpaceItemPrivateWrite::Op> ops = write->ops;
    write->ops.clear();

    bool rv = true;
    for(int ii = 0; ii < ops.count(); ++ii) {
        const QByteArray & path = ops.at(ii).path;
        const QVariant & value = ops.at(ii).value;

        if(ops.at(ii).type == QValueSpaceItemPrivateWrite::Op::Set) {
            if(path.isEmpty()) {
                for(int ii = md->readers.count(); ii > 0; --ii) {
                    if(!md->readers[ii - 1].first->requestSetValue(md->readers[ii - 1].second,
                            value)) rv = false;
                }
            } else {
                QByteArray vpath =
                    ((*path.constData()) == '/')?path:(QByteArray("/") + path);
                for(int ii = md->readers.count(); ii > 0; --ii) {
                    if(!md->readers[ii - 1].first->requestSetValue(md->readers[ii - 1].second,
                            vpath, value)) rv = false;
                }
            }
        } else {
            if(path.isEmpty()) {
                for(int ii = md->readers.count(); ii > 0; --ii) {
                    if(!md->readers[ii - 1].first->requestRemoveValue(md->readers[ii - 1].second)) rv = false;
                }
            } else {
                QByteArray vpath =
                    ((*path.constData()) == '/')?path:(QByteArray("/") + path);
                for(int ii = md->readers.count(); ii > 0; --ii) {
                    if(!md->readers[ii - 1].first->requestRemoveValue(md->readers[ii - 1].second, vpath)) rv = false;
                }
            }
        }
    }

    for(int ii = md->readers.count(); ii > 0; --ii) {
        if(!md->readers[ii - 1].first->syncChanges())
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
   item if \a subPath is empty.  The following code shows how the item and
   \a subPath relate.

   \code

   QValueSpaceItem base("/Settings");
   QValueSpaceItem equiv("/Settings/Nokia/General/Mappings);

   // Is true
   equiv.value() == base.value("Nokia/General/Mapping");
   \endcode

   If the item does not exist, \a def is returned.
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
        QByteArray vpath =
            ((*subPath.constData()) == '/')?subPath:(QByteArray("/") + subPath);
        for(int ii = md->readers.count(); ii > 0; --ii) {
            if(md->readers[ii - 1].first->value(md->readers[ii - 1].second,
                                                vpath, &value))
                return value;
        }
    }
    return def;
}

/*! \internal */
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

/*! \internal */
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

#include "qvaluespace.moc"
