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

#ifndef QVALUESPACE_H
#define QVALUESPACE_H

#include <QVariant>
#include <QByteArray>
#include <QList>
#include <QObject>
#include <QUuid>

#include "qcontextglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QValueSpaceObject;

class Q_CFW_EXPORT QAbstractValueSpaceLayer : public QObject
{
    Q_OBJECT

public:
    typedef quintptr Handle;
    static const Handle InvalidHandle = ~Handle(0);

    virtual QString name() = 0;

    enum Type { Server, Client };

    virtual bool startup(Type type) = 0;

    virtual QUuid id() = 0;
    virtual unsigned int order() = 0;

    enum Properties { Publish = 0x00000001 };

    virtual Handle item(Handle parent, const QByteArray &subPath) = 0;
    virtual void removeHandle(Handle handle) = 0;
    virtual void setProperty(Handle handle, Properties properties) = 0;

    virtual bool value(Handle handle, QVariant *data) = 0;
    virtual bool value(Handle handle, const QByteArray &subPath, QVariant *data) = 0;
    virtual QSet<QByteArray> children(Handle handle) = 0;

    enum LayerOption {
        UnspecifiedLayer = 0x0000,
        PermanentLayer = 0x0001,
        NonPermanentLayer = 0x0002,
        // UnspecifiedLayerPermanence = 0x0000,
        // InvalidLayerPermanence = 0x0003,
        WriteableLayer = 0x0004,
        NonWriteableLayer = 0x0008,
        // UnspecifiedLayerWriteability = 0x0000,
        // InvalidLayerWriteability = 0x000C,
    };
    Q_DECLARE_FLAGS(LayerOptions, LayerOption);

    virtual LayerOptions layerOptions() const = 0;

    /* QValueSpaceItem functions */
    virtual bool supportsRequests() const = 0;
    virtual bool notifyInterest(Handle handle, bool interested) = 0;
    virtual bool syncRequests() = 0;

    /* QValueSpaceObject functions */
    virtual bool setValue(QValueSpaceObject *creator, Handle handle, const QByteArray &subPath, const QVariant &value) = 0;
    virtual bool removeValue(QValueSpaceObject *creator, Handle handle, const QByteArray &subPath) = 0;
    virtual bool removeSubTree(QValueSpaceObject *creator, Handle handle) = 0;
    virtual void addWatch(QValueSpaceObject *creator, Handle handle) = 0;
    virtual void removeWatches(QValueSpaceObject *creator, Handle parent) = 0;
    virtual void sync() = 0;

protected:
    /* QValueSpaceObject functions */
    void emitItemNotify(QValueSpaceObject *object, const QByteArray &path, bool interested);

signals:
    void handleChanged(quintptr);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QAbstractValueSpaceLayer::LayerOptions);

namespace QValueSpace {
    Q_CFW_EXPORT void initValueSpaceManager();

    typedef QAbstractValueSpaceLayer *(*LayerCreateFunc)();
    Q_CFW_EXPORT void installLayer(LayerCreateFunc func);
    Q_CFW_EXPORT void installLayer(QAbstractValueSpaceLayer *layer);

    struct AutoInstall {
        AutoInstall(LayerCreateFunc func) { installLayer(func); }
    };

    Q_CFW_EXPORT QList<QUuid> availableLayers();
};

#define QVALUESPACE_SHAREDMEMORY_LAYER QUuid(0xd81199c1, 0x6f60, 0x4432, 0x93, 0x4e, \
                                             0x0c, 0xe4, 0xd3, 0x7e, 0xf2, 0x52)
#define QVALUESPACE_VOLATILEREGISTRY_LAYER QUuid(0x8ceb5811, 0x4968, 0x470f, 0x8f, 0xc2, \
                                                 0x26, 0x47, 0x67, 0xe0, 0xbb, 0xd9)
#define QVALUESPACE_NONVOLATILEREGISTRY_LAYER QUuid(0x8e29561c, 0xa0f0, 0x4e89, 0xba, 0x56, \
                                                    0x08, 0x06, 0x64, 0xab, 0xc0, 0x17)

#define QVALUESPACE_AUTO_INSTALL_LAYER(name) \
    QAbstractValueSpaceLayer * _qvaluespaceauto_layercreate_ ## name() \
    { \
        return name::instance(); \
    } \
    static QValueSpace::AutoInstall _qvaluespaceauto_ ## name(_qvaluespaceauto_layercreate_ ## name);

QT_END_NAMESPACE

QT_END_HEADER

#endif // _QVALUESPACE_H_
