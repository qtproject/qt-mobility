/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QVALUESPACE_P_H
#define QVALUESPACE_P_H

#include "qmobilityglobal.h"
#include "qvaluespace.h"

#include <QObject>
#include <QUuid>

QTM_BEGIN_NAMESPACE

class QValueSpacePublisher;

QString qCanonicalPath(const QString &path);

class Q_AUTOTEST_EXPORT QAbstractValueSpaceLayer : public QObject
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

    virtual Handle item(Handle parent, const QString &subPath) = 0;
    virtual void removeHandle(Handle handle) = 0;
    virtual void setProperty(Handle handle, Properties properties) = 0;

    virtual bool value(Handle handle, QVariant *data) = 0;
    virtual bool value(Handle handle, const QString &subPath, QVariant *data) = 0;
    virtual QSet<QString> children(Handle handle) = 0;

    virtual QValueSpace::LayerOptions layerOptions() const = 0;

    /* QValueSpaceSubscriber functions */
    virtual bool supportsInterestNotification() const = 0;
    virtual bool notifyInterest(Handle handle, bool interested) = 0;

    /* QValueSpacePublisher functions */
    virtual bool setValue(QValueSpacePublisher *creator, Handle handle,
                          const QString &subPath, const QVariant &value) = 0;
    virtual bool removeValue(QValueSpacePublisher *creator, Handle handle,
                             const QString &subPath) = 0;
    virtual bool removeSubTree(QValueSpacePublisher *creator, Handle handle) = 0;
    virtual void addWatch(QValueSpacePublisher *creator, Handle handle) = 0;
    virtual void removeWatches(QValueSpacePublisher *creator, Handle parent) = 0;
    virtual void sync() = 0;

protected:
    /* QValueSpacePublisher functions */
    void emitInterestChanged(QValueSpacePublisher *publisher, const QString &attribute,
                             bool interested);

signals:
    void handleChanged(quintptr);
};

namespace QValueSpace {
    typedef QAbstractValueSpaceLayer *(*LayerCreateFunc)();
    Q_AUTOTEST_EXPORT void installLayer(LayerCreateFunc func);
    Q_AUTOTEST_EXPORT void installLayer(QAbstractValueSpaceLayer *layer);

    struct AutoInstall {
        AutoInstall(LayerCreateFunc func) { installLayer(func); }
    };
}

#define QVALUESPACE_AUTO_INSTALL_LAYER(name) \
QAbstractValueSpaceLayer * _qvaluespaceauto_layercreate_ ## name() \
{ \
  return name::instance(); \
} \
static QValueSpace::AutoInstall _qvaluespaceauto_ ## name(_qvaluespaceauto_layercreate_ ## name);

QTM_END_NAMESPACE

#endif // QVALUESPACE_P_H
