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

#include "qvaluespace.h"
#include "qvaluespace_p.h"
#include "qvaluespacemanager_p.h"
#include "qmallocpool_p.h"
#include "qvaluespaceobject.h"
#include "qpacketprotocol_p.h"

#include <QByteArray>
#include <QObject>
#include <QMap>
#include <QPair>
#include <QCoreApplication>
#include <QThread>
#include <QSet>
#include <QString>
#include <QVarLengthArray>

#include <QtCore/qdebug.h>

void QAbstractValueSpaceLayer::emitItemRemove(QValueSpaceObject *object, const QByteArray &path)
{
   emit object->itemRemove(path);
}

void QAbstractValueSpaceLayer::emitItemSetValue(QValueSpaceObject *object, const QByteArray &path, const QVariant &data)
{
   emit object->itemSetValue(path, data);
}

void QAbstractValueSpaceLayer::emitItemNotify(QValueSpaceObject *object, const QByteArray &path, bool interested)
{
    emit object->itemNotify(path, interested);
}


///////////////////////////////////////////////////////////////////////////////
// Layer plug-in interface documentation
///////////////////////////////////////////////////////////////////////////////

/*!
    \class QAbstractValueSpaceLayer
    \brief The QAbstractValueSpaceLayer class provides support for adding new logical data layers
    to the Qt Value Space.
*/

/*!
    \macro QVALUESPACE_AUTO_INSTALL_LAYER(className)

    \relates QAbstractValueSpaceLayer

    This macro installs new value space layer. \a className is the name of the class implementing
    the new layer.

*/

/*!
    \typedef QAbstractValueSpaceLayer::Handle

    The Handle type is an opaque, pointer sized contextual handle used to represent paths within
    value space layers.  Handles are only ever created by QAbstractValueSpaceLayer::item() and are
    always released by calls to QAbstractValueSpaceLayer::removeHandle().  The special value,
    \c {InvalidHandle} is reserved to represent an invalid handle.
 */

/*!
    \enum QAbstractValueSpaceLayer::Type

    Value Space layers are initialized in either a "Server" or a "Client" context.  There is only
    a single server in the value space architecture, and its layers are always initialized before
    any clients.  This distinction allows layers to implement Client/Server architecture
    \i {if required}.  If not, layers are free to treat Server and Client contexts identically.

    \value Server The layer is being initialized in the "server" context.
    \value Client The layer is being initialized in the "client" context.
 */

/*!
    \enum QAbstractValueSpaceLayer::Properties

    To allow for efficient layer implementations, expensive handle operations, currently only
    monitoring for changes, are enabled and disabled as needed on a per-handle basis.  The
    Properties enumeration is a bitmask representing the different properties that can exist on a
    handle.

    \value Publish Enable change notification for the handle.  When set, the layer should emit
                   QAbstractValueSpaceLayer::handleChanged() signals when appropriate for the
                   handle.
*/

/*!
    \fn QString QAbstractValueSpaceLayer::name()

    Returns the name of the Value Space layer.  This name is only used for diagnostics purposes.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::startup(Type type)

    Called by the Value Space system to initialize each layer.  The \a type parameter will be set
    accordingly, and layer implementors can use this to implement a client/server architecture if
    desired.

    Returns true upon success; otherwise returns false.
*/

/*!
    \fn void QAbstractValueSpaceLayer::shutdown()

    Called by the Value Space system to uninitialize each layer.  The shutdown call can be used to
    release any resources in use by the layer.
*/

/*!
    \fn QUuid QAbstractValueSpaceLayer::id()

    Return a globally unique id for the layer.  This id is used to break ordering ties.
*/

/*!
    \fn unsigned int QAbstractValueSpaceLayer::order()

    Return the position in the Value Space layer stack that this layer should reside.  Higher
    numbers mean the layer has a higher precedence and its values will "shadow" those below it.
    If two layers specify the same ordering, the id() value is used to break the tie.
*/

/*!
    \fn Handle QAbstractValueSpaceLayer::item(Handle parent, const QByteArray &subPath)

    Returns a new opaque handle for the requested \a subPath of \a parent.  If \a parent is an
    InvalidHandle, \a subPath is an absolute path.

    The caller should call removeHandle() to free resources used by the handle when it is no longer
    required.
*/

/*!
    \fn void QAbstractValueSpaceLayer::removeHandle(Handle handle)

    Releases a \a handle previously returned from QAbstractValueSpaceLayer::item().
*/

/*!
    \fn void QAbstractValueSpaceLayer::setProperty(Handle handle, Properties property)

    Apply the specified \a property mask to \a handle.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::value(Handle handle, QVariant *data)

    Returns the value for a particular \a handle.  If a value is available, the layer will set
    \a data and return true.  If no value is available, false is returned.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::value(Handle handle, const QByteArray &subPath, QVariant *data)

    Returns the value for a particular \a subPath of \a handle.  If a value is available, the
    layer will set \a data and return true.  If no value is available, false is returned.
*/

/*!
    \fn QSet<QByteArray> QAbstractValueSpaceLayer::children(Handle handle)

    Returns the set of children of \a handle.  For example, in a layer providing the following
    items:

    \code
        /Device/Configuration/Applications/FocusedApplication
        /Device/Configuration/Buttons/PrimaryInput
        /Device/Configuration/Name
    \endcode

    a request for children of "/Device/Configuration" will return
    { "Applications", "Buttons", "Name" }.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::supportsRequests()

    Returns true if the layer supports sending requests via the requestSetValue() and
    requestRemoveValue() functions; otherwise returns false.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::requestSetValue(Handle handle, const QVariant &value)

    Process a client side QValueSpaceItem::setValue() call by sending a request to the provider of
    the value space item identified by \a handle to set it to \a value.

    Returns true if the request was successfully sent; otherwise returns false.

    \sa handleChanged()
*/

/*!
    \fn bool QAbstractValueSpaceLayer::requestSetValue(Handle handle, const QByteArray &subPath, const QVariant &value)

    Process a client side QValueSpaceItem::setValue() call by sending a request to the provider of
    the value space item identified by \a handle and \a subPath to set the value to \a value.

    Returns true if the request was successfully sent; otherwise returns false.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::requestRemoveValue(Handle handle, const QByteArray &subPath)

    Process a client side QValueSpaceItem::remove() call by sending a request to the provider of
    the value space item identified by \a handle and \a subPath to remove the item.

    Returns true if the request was successfully sent; otherwise returns false.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::notifyInterest(Handle handle, bool interested)

    Registers or unregisters that the caller is interested in \a handle and any subpaths under it.
    If \a interested is true interest in \a handle is registered; otherwise it is unregistered.

    The caller should ensure that all calls to this function with \a interested set to true have a
    matching call with \a interested set to false.

    Returns true if the notification was successfully sent; otherwise returns false.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::syncRequests()

    Commit any pending request made through call to requestSetValue() and requestRemoveValue().

    Returns true on success; otherwise returns false.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::setValue(QValueSpaceObject *creator, Handle handle, const QByteArray &subPath, const QVariant &value)

    Process calls to QValueSpaceObject::setAttribute() by setting the value specified by the
    \a subPath under \a handle to \a value.  Ownership of the value space item is assigned to
    \a creator.

    Returns true on success; otherwise returns false.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::removeValue(QValueSpaceObject *creator, Handle handle, const QByteArray &subPath)

    Process calls to QValueSpaceObject::removeAttribute() by removing the value space item
    identified by \a handle and \a subPath and created by \a creator.

    Returns true on success; otherwise returns false.
*/

/*!
    \fn bool QAbstractValueSpaceLayer::removeSubTree(QValueSpaceObject *creator, Handle handle)

    Process calls to QValueSpaceObject::~QValueSpaceObject() by removing the entire sub tree
    created by \a creator under \a handle.

    Returns true on success; otherwise returns false.
*/

/*!
    \fn void QAbstractValueSpaceLayer::addWatch(QValueSpaceObject *creator, Handle handle)

    Registers \a creator for change notifications to values under \a handle.

    \sa removeWatches(), emitItemRemove(), emitItemSetValue()
*/

/*!
    \fn void QAbstractValueSpaceLayer::removeWatches(QValueSpaceObject *creator, Handle parent)

    Removes all registered change notifications for \a creator under \a parent.

    \sa addWatch(), emitItemRemove(), emitItemSetValue()
*/

/*!
    \fn void QAbstractValueSpaceLayer::sync()

    Flushes all pending changes made by calls to setValue(), removeValue() and removeSubTree().
*/

/*!
    \fn void QAbstractValueSpaceLayer::emitItemRemove(QValueSpaceObject *object, const QByteArray &path)

    Emits the QValueSpaceObject::itemRemove() signal on \a object with \a path.
*/

/*!
    \fn void QAbstractValueSpaceLayer::emitItemSetValue(QValueSpaceObject *object, const QByteArray &path, const QVariant &data)

    Emits the QValueSpaceObject::itemSetValue() signal on \a object with \a path and \a data.
*/

/*!
    \fn void QAbstractValueSpaceLayer::emitItemNotify(QValueSpaceObject *object, const QByteArray &path, bool interested)

    Emits the QValueSpaceObject::itemNotify() signal on \a object with \a path and \a interested.
*/

/*!
    \fn void QAbstractValueSpaceLayer::handleChanged(quintptr handle)

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
  installed using QVALUESPACE_AUTO_INSTALL_LAYER() macro.  The method
  \c {QAbstractValueSpaceLayer * name::instance()} must exist and return a pointer to
  the layer to install.  The QVALUESPACE_AUTO_INSTALL_LAYER() macro
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
void QValueSpace::initValueSpaceManager()
{
    QValueSpaceManager::instance()->initServer();
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

/*!
    \macro QVALUESPACE_SHAREDMEMORY_LAYER
    \relates QValueSpace

    The UUID of the Shared Memory Layer as a QUuid.  The actual UUID value is
    {d81199c1-6f60-4432-934e-0ce4d37ef252}.

    This value can be passed to the constructor of QValueSpaceObject to force the constructed value
    space object to publish its values in the Shared Memory Layer.

    You can test if the Shared Memory Layer is available by checking if the list returned by
    QValueSpace::availableLayers() contains this value.
*/

/*!
    \macro QVALUESPACE_REGISTRY_LAYER
    \relates QValueSpace

    The UUID of the Registry Layer as a QUuid.  The actual UUID value is
    {8ceb5811-4968-470f-8fc2-264767e0bbd9}.

    This value can be passed to the constructor of QValueSpaceObject to force the constructed value
    space object to publish its values in the Registry Layer.

    You can test if the Registry Layer is available by checking if the list returned by
    QValueSpace::availableLayers() contains this value.  The Registry Layer is only available on
    Windows platforms.
*/

/*!
    Returns a List of QUuids of the available layers.
*/
QList<QUuid> QValueSpace::availableLayers()
{
    QList<QAbstractValueSpaceLayer *> layers = QValueSpaceManager::instance()->getLayers();

    QList<QUuid> uuids;

    for (int i = 0; i < layers.count(); ++i)
        uuids.append(layers.at(i)->id());

    return uuids;
}

QByteArray qCanonicalPath(const QByteArray &path)
{
    QByteArray result;
    result.resize(path.length());
    const char *from = path.constData();
    const char *fromend = from + path.length();
    int outc=0;
    char *to = result.data();
    do {
        to[outc++] = '/';
        while (from!=fromend && *from == '/')
            ++from;
        while (from!=fromend && *from != '/')
            to[outc++] = *from++;
    } while (from != fromend);
    if (outc > 1 && to[outc-1] == '/')
        --outc;
    result.resize(outc);
    return result;
}
