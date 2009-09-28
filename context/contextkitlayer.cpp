/****************************************************************************
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QSet>
#include <QDebug>
#include <contextproperty.h>
#include <contextpropertyinfo.h>
#include <contextregistryinfo.h>

QT_BEGIN_NAMESPACE

#define CONTEXTKIT_LAYER_PREFIX "C"

/* ContextKit layer

   This layer makes ContextKit properties visible in the QValueSpace.
   All properties appear as direct children of "/C".  A property with
   the name "Battery.ChargePercentage", for example, is visible as the
   item names "/C/Battery.ChargePercentage".

   You can not publish values of ContextKit properties using the
   QValueSpace.  This might be fixed later.
*/

// XXX - ContextProperty keys are strings, but ValueSpaceItem paths
//       are ByteArrays.  We assume that the ByteArrays are unicode.

// XXX - Right now, this code assumes that a 'subPath' given to
//       'value' is always the name of a direct child, and never a
//       path to a indirect child, and always starts with "/".  This
//       needs to be checked.  The assumption isn't true for 'item',
//       for example.

// XXX - We do not monitor the ContextKit registry, and we call
//       get_all_props only once.  This means that any changes done to
//       the registry after the first time "/C" has been listed (or
//       similar) are ignored.

// XXX - How can you list children that are in more than one layer?
//       I.e., if one layer claims everything below "/A" and another
//       everything below "/B", what happens when you list "/"?

class ContextKitLayer : public QAbstractValueSpaceLayer
{
    Q_OBJECT

    QHash<QString, ContextProperty *> props;
    bool got_all_props;

public:
    ContextKitLayer();
    virtual ~ContextKitLayer();

    ContextProperty *get_prop (const QByteArray &path);
    ContextProperty *get_prop_from_key (const QString &key);
    void get_all_props ();

    /* ValueSpaceLayer interface - Common functions */
    QString name();
    bool startup(Type);
    QUuid id();
    unsigned int order();
    LayerOptions layerOptions() const;

    Handle item(Handle parent, const QByteArray &);
    void removeHandle(Handle);
    void setProperty(Handle handle, Properties);

    bool value(Handle, QVariant *);
    bool value(Handle, const QByteArray &, QVariant *);
    QSet<QByteArray> children(Handle);

    /* ValueSpaceLayer interface - QValueSpaceItem functions */
    bool notifyInterest(Handle handle, bool interested);
    bool supportsRequests() const { return false; }
    bool requestSetValue(Handle, const QVariant &) { return false; }
    bool requestSetValue(Handle, const QByteArray &, const QVariant &) { return false; }
    bool requestRemoveValue(Handle, const QByteArray &) { return false; }
    bool syncRequests() { return false; }

    /* ValueSpaceLayer interface - QValueSpaceObject functions */
    bool setValue(QValueSpaceObject *, Handle, const QByteArray &, const QVariant &) { return false; }
    bool removeValue(QValueSpaceObject *, Handle, const QByteArray &) { return false; }
    bool removeSubTree(QValueSpaceObject *, Handle) { return false; }
    void addWatch(QValueSpaceObject *, Handle) { return; }
    void removeWatches(QValueSpaceObject *, Handle) { return; }
    void sync() { return; }

    static ContextKitLayer *instance();

private slots:
    void contextPropertyChanged();
};

QVALUESPACE_AUTO_INSTALL_LAYER(ContextKitLayer);

ContextKitLayer::ContextKitLayer ()
    : got_all_props (false)
{
}

ContextKitLayer::~ContextKitLayer ()
{
    foreach (ContextProperty *p, props.values())
        delete p;
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
    // 2c769b9e-d949-4cd1-848f-d32241fe07ff
    return QUuid(0x2c769b9e, 0xd949, 0x4cd1, 0x84, 0x8f,
                 0xd3, 0x22, 0x41, 0xfe, 0x07, 0xff);
}

unsigned int ContextKitLayer::order()
{
    return 0;
}

QAbstractValueSpaceLayer::LayerOptions ContextKitLayer::layerOptions () const
{
    return NonPermanentLayer | NonWriteableLayer;
}

ContextProperty *ContextKitLayer::get_prop (const QByteArray &path)
{
    return get_prop_from_key (QString::fromUtf8 (path));
}

ContextProperty *ContextKitLayer::get_prop_from_key (const QString &key)
{
    ContextProperty *p = props.value (key);
    if (p == NULL)
    {
        p = new ContextProperty (key);
        props.insert (key, p);
    }
    return p;
}

void ContextKitLayer::get_all_props ()
{
    if (got_all_props)
        return;

    ContextRegistryInfo *registry = ContextRegistryInfo::instance ();
    QStringList keys = registry->listKeys();
    
    foreach (const QString &key, keys)
        get_prop_from_key (key);
    got_all_props = true;
}

QAbstractValueSpaceLayer::Handle ContextKitLayer::item (Handle parent, const QByteArray &subPath)
{
    if (parent == InvalidHandle
        && subPath == "/" CONTEXTKIT_LAYER_PREFIX)
    {
        return (Handle) this;
    }
    else if (parent == InvalidHandle
             && subPath.startsWith ("/" CONTEXTKIT_LAYER_PREFIX "/"))
    {
        return (Handle) get_prop (subPath.mid(strlen (CONTEXTKIT_LAYER_PREFIX)+2));
    }
    else if (parent == (Handle) this)
    {
        return (Handle) get_prop (subPath);
    }
    else
        return InvalidHandle;
}

void ContextKitLayer::removeHandle (Handle handle)
{
    if (handle != (Handle) this)
    {
        ContextProperty *p = (ContextProperty *)handle;
        props.remove (p->key());
        delete p;
    }
}

void ContextKitLayer::setProperty (Handle handle, Properties properties)
{
    if (handle == (Handle) this)
    {
        get_all_props ();
        foreach (ContextProperty *p, props.values())
            setProperty ((Handle) p, properties);
        return;
    }

    ContextProperty *p = (ContextProperty *)handle;

    if (properties & Publish)
        connect (p, SIGNAL(valueChanged()),
                 this, SLOT(contextPropertyChanged()));
    else
        disconnect (p, SIGNAL(valueChanged()),
                    this, SLOT(contextPropertyChanged()));
}

void ContextKitLayer::contextPropertyChanged()
{
    ContextProperty *p = (ContextProperty *)sender();
    emit handleChanged ((Handle) p);
    emit handleChanged ((Handle) this);
}

bool ContextKitLayer::value(Handle handle, QVariant *data)
{
    if (handle == (Handle) this)
        return false;

    ContextProperty *p = (ContextProperty *)handle;
    *data = p->value();
    return true;
}

bool ContextKitLayer::notifyInterest(Handle handle, bool interested)
{
    if (handle == (Handle) this)
        return false;
    else
    {
        ContextProperty *p = (ContextProperty *)handle;
        if (interested)
            p->subscribe();
        else
            p->unsubscribe();
        return true;
    }
}

bool ContextKitLayer::value(Handle handle, const QByteArray &subPath, QVariant *data)
{
    // XXX - can handle be InvalidHandle and subPath be something like
    //       "/C/Battery.Foo"?  Does subPath always start with a slash?

     if (handle == (Handle) this)
    {
        ContextProperty *p = get_prop (subPath.mid(1));
        *data = p->value();
        return true;
    }
    else 
    {
        return false;
    }
}

QSet<QByteArray> ContextKitLayer::children (Handle handle)
{
    if (handle == (Handle) this)
    {
        QSet<QByteArray> kids;

        get_all_props ();
        foreach (const QString &key, props.keys())
            kids.insert (key.toUtf8());
        return kids;
    }
    else
        return QSet<QByteArray>();
}

QT_END_NAMESPACE

#include "contextkitlayer.moc"
