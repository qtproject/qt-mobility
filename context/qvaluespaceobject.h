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

#ifndef QVALUESPACEOBJECT_H
#define QVALUESPACEOBJECT_H

#include "qcontextglobal.h"
#include "qvaluespace.h"

#include <QObject>
#include <QUuid>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QValueSpaceObjectPrivate;
class Q_CFW_EXPORT QValueSpaceObject : public QObject
{
    friend class QAbstractValueSpaceLayer;

    Q_OBJECT

public:
    explicit QValueSpaceObject(const QString &path, QObject *parent = 0);
    explicit QValueSpaceObject(const char *path, QObject *parent = 0);

    QValueSpaceObject(const QString &path, QValueSpace::LayerOptions filter, QObject *parent = 0);
    QValueSpaceObject(const char *path, QValueSpace::LayerOptions filter, QObject *parent = 0);

    QValueSpaceObject(const QString &objectPath, const QUuid &uuid, QObject *parent = 0);
    QValueSpaceObject(const char *objectPath, const QUuid &uuid, QObject *parent = 0);

    ~QValueSpaceObject();

    QString path() const;

    bool isConnected() const;

    static void sync();

signals:
    void itemNotify(const QByteArray &path, bool interested);

public slots:
    void setAttribute(const char *attribute, const QVariant &data);
    void removeAttribute(const char *attribute);
    void setAttribute(const QString &attribute, const QVariant &data);
    void removeAttribute(const QString &attribute);
    void setAttribute(const QByteArray &attribute, const QVariant &data);
    void removeAttribute(const QByteArray &attribute);

protected:
    virtual void connectNotify(const char *);

private:
    Q_DISABLE_COPY(QValueSpaceObject)
    QValueSpaceObjectPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QVALUESPACEOBJECT_H
