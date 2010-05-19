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

#include "qvaluespace.h"
#include "qvaluespace_p.h"

#include <QSet>
#include <QDebug>
#include <contextproperty.h>
#include <contextregistryinfo.h>

QTM_BEGIN_NAMESPACE

using namespace QValueSpace;

/* ContextKit layer

   This layer makes ContextKit properties visible in the QValueSpace.

   You can not publish values of ContextKit properties using the
   QValueSpace.  This might be fixed later.

   ContextKit properties have names with dots in them, which allows
   these properties to be arranged in a tree.  The context commander
   does this in its UI, for example.

   However, the ContextKit itself does not pay attention to the dots
   at all: properties just have names and there are no such things as
   parent / child / sibling relations between properties.  For
   example, there is a property called "Battery.ChargeLevel", but the
   ContextKit doesn't know anything about "Battery".  Subscribing to
   "Battery" is exactly the same as subscribing to any other
   non-declared property.  When "Battery.ChargeLevel" changes,
   "Battery" does _not_ receive a change notification.

   The QValueSpace, on the other hand, has a explicit tree-structure:
   the key "/foo" represents all its children, like "/foo/bar".  When
   "/foo/bar" changes, "/foo" _does_ receive a change notification.

   There are two ways to map ContextKit properties into the
   QValueSpace: simply as a long list of siblings (a one-level tree),
   or as the multi-level tree that is implied by their names.

   We do the latter since that is more natural.  For example, the
   ContextKit property "Battery.ChargeLevel" is mapped to
   "/Battery/ChargeLevel", and the key "/Battery" gets a change
   notifications for all its children.
*/

/* ContextKitHandle - contains a ContextKit property and all its
                      direct and indirect children.

   ContextKitHandles do not form a hierarchy between each other; think
   of them as individual little trees that are created in direct
   response to requests to the ContextKitLayer.  The properties in
   these trees can overlap; a single ContextKit property can be
   accessed via many ContextKitHandles.
 */

class ContextKitHandle : public QObject {
    
    Q_OBJECT

public:
    ContextKitHandle (ContextKitHandle *parent, const QString &root);
    ~ContextKitHandle ();

    bool value (const QString &path, QVariant *data);
    void subscribe ();
    void unsubscribe ();
    QSet<QString> children ();

signals:
    void valueChanged();

private:
    QString prefix;
    QHash<QString, ContextProperty *> props;

    void insert (const QString &path, const QString &key);
};

void ContextKitHandle::insert (const QString &path, const QString &key)
{
    ContextProperty *prop = new ContextProperty (key);
    connect (prop, SIGNAL (valueChanged()),
             this, SIGNAL (valueChanged()));
    props.insert (path, prop);
}

ContextKitHandle::ContextKitHandle (ContextKitHandle *parent, const QString &path)
{
    QString key = path;
    if (key.startsWith("/"))
        key = key.mid(1);
    key = key.replace('/', '.');
    if (parent)
        key = parent->prefix + key;

    prefix = (key == "")? "" : key + ".";
    foreach (const QString &k, ContextRegistryInfo::instance()->listKeys())
    {
        if (k == key)
            insert ("", k);
        else if (k.startsWith (prefix))
            insert (k.mid(prefix.length()).replace ('.', '/'), k);
    }
}

ContextKitHandle::~ContextKitHandle ()
{
    foreach (ContextProperty *prop, props.values())
        delete prop;
}

bool
ContextKitHandle::value (const QString &path, QVariant *data)
{
    // path always starts with a "/".
    ContextProperty *p = props.value (path.mid(1));
    if (p)
    {
        *data = p->value();
        return true;
    }
    else
        return false;
}

void
ContextKitHandle::subscribe ()
{
    foreach (ContextProperty *p, props.values())
        p->subscribe ();
}

void
ContextKitHandle::unsubscribe ()
{
    foreach (ContextProperty *p, props.values())
        p->unsubscribe ();
}

QSet<QString>
ContextKitHandle::children ()
{
    QSet<QString> kids;

    foreach (const QString &path, props.keys())
    {
        int pos = path.indexOf("/");
        if (pos >= 0)
            kids.insert (path.left(pos));
        else
            kids.insert (path);
    }
    return kids;
}

/* ContextKitLayer - implements QAbstractValueSpaceLayer interface to
                     hook ContextKit into QValueSpace.
   
   It mainly creates ContextKitHandles and dispatches to them.
*/

class ContextKitLayer : public QAbstractValueSpaceLayer
{
    Q_OBJECT

public:
    ContextKitLayer();
    virtual ~ContextKitLayer();

    /* ValueSpaceLayer interface - Common functions */
    QString name();
    bool startup(Type);
    QUuid id();
    unsigned int order();
    LayerOptions layerOptions() const;

    Handle item(Handle parent, const QString &);
    void removeHandle(Handle);
    void setProperty(Handle handle, Properties);

    bool value(Handle, QVariant *);
    bool value(Handle, const QString &, QVariant *);
    QSet<QString> children(Handle);

    /* ValueSpaceLayer interface - QValueSpaceSubscriber functions */
    bool supportsInterestNotification() const { return true; }
    bool notifyInterest(Handle handle, bool interested);

    /* ValueSpaceLayer interface - QValueSpacePublisher functions */
    bool setValue(QValueSpacePublisher *, Handle, const QString &, const QVariant &) { return false; }
    bool removeValue(QValueSpacePublisher *, Handle, const QString &) { return false; }
    bool removeSubTree(QValueSpacePublisher *, Handle) { return false; }
    void addWatch(QValueSpacePublisher *, Handle) { return; }
    void removeWatches(QValueSpacePublisher *, Handle) { return; }
    void sync() { return; }

    static ContextKitLayer *instance();

private slots:
    void contextHandleChanged();
};

QVALUESPACE_AUTO_INSTALL_LAYER(ContextKitLayer);

ContextKitLayer::ContextKitLayer ()
{
}

ContextKitLayer::~ContextKitLayer ()
{
}

Q_GLOBAL_STATIC(ContextKitLayer, contextKitLayer);
ContextKitLayer *ContextKitLayer::instance ()
{
    return contextKitLayer ();
}

QString ContextKitLayer::name()
{
    return "ContextKit Layer";
}

bool ContextKitLayer::startup(Type)
{
    return true;
}

QUuid ContextKitLayer::id()
{
    return QVALUESPACE_CONTEXTKIT_LAYER;
}

unsigned int ContextKitLayer::order()
{
    return 0;
}

LayerOptions ContextKitLayer::layerOptions () const
{
    return TransientLayer | ReadOnlyLayer;
}

QAbstractValueSpaceLayer::Handle ContextKitLayer::item (Handle parent, const QString &subPath)
{
    return (Handle) new ContextKitHandle ((parent == InvalidHandle)
                                          ? NULL
                                          : (ContextKitHandle *) parent,
                                          subPath);
}

void ContextKitLayer::removeHandle (Handle handle)
{
    ContextKitHandle *h = (ContextKitHandle *)handle;
    delete h;
}

void ContextKitLayer::setProperty (Handle handle, Properties properties)
{
    ContextKitHandle *h = (ContextKitHandle *)handle;

    if (properties & Publish)
        connect (h, SIGNAL(valueChanged()),
                 this, SLOT(contextHandleChanged()));
    else
        disconnect (h, SIGNAL(valueChanged()),
                    this, SLOT(contextHandleChanged()));
}

void ContextKitLayer::contextHandleChanged()
{
    ContextKitHandle *h = (ContextKitHandle *)sender();
    emit handleChanged ((Handle) h);
}

bool ContextKitLayer::value (Handle handle, QVariant *data)
{
    ContextKitHandle *h = (ContextKitHandle *)handle;
    return h->value ("", data);
}

bool ContextKitLayer::value (Handle handle, const QString &subPath, QVariant *data)
{
    ContextKitHandle *h = (ContextKitHandle *)handle;
    return h->value (subPath, data);
}

bool ContextKitLayer::notifyInterest(Handle handle, bool interested)
{
    ContextKitHandle *h = (ContextKitHandle *)handle;

    if (interested)
        h->subscribe();
    else
        h->unsubscribe();
    return true;
}

QSet<QString> ContextKitLayer::children (Handle handle)
{
    ContextKitHandle *h = (ContextKitHandle *)handle;
    return h->children ();
}

#include "contextkitlayer.moc"
QTM_END_NAMESPACE

