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

QT_BEGIN_NAMESPACE

// XXX - ValueSpaceItems form a hierarchy but ContextProperty:s don't.
//       Right now, we punt by treating every context property as a
//       top-level item with no children.  Maybe that is good enough.

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

QAbstractValueSpaceLayer::Handle ContextKitLayer::item (Handle parent, const QByteArray &subPath)
{
    if (parent == InvalidHandle)
        return (Handle) new ContextProperty (subPath.mid(1));
    else
        return InvalidHandle;
}

void ContextKitLayer::removeHandle (Handle handle)
{
    ContextProperty *p = (ContextProperty *)handle;

    delete p;
}

void ContextKitLayer::setProperty (Handle handle, Properties properties)
{
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
}

bool ContextKitLayer::value(Handle handle, QVariant *data)
{
    ContextProperty *p = (ContextProperty *)handle;
    *data = p->value();
    return true;
}

bool ContextKitLayer::notifyInterest(Handle handle, bool interested)
{
    ContextProperty *p = (ContextProperty *)handle;
    if (interested)
        p->subscribe();
    else
        p->unsubscribe();
    return true;
}

bool ContextKitLayer::value(Handle, const QByteArray &, QVariant *)
{
    return false;
}

QSet<QByteArray> ContextKitLayer::children (Handle)
{
    return QSet<QByteArray>();
}

QT_END_NAMESPACE

#include "contextkitlayer.moc"
