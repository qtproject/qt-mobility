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

#ifndef QVALUESPACEITEM_H
#define QVALUESPACEITEM_H

#include "qcontextglobal.h"
#include "qvaluespace.h"

#include <QObject>
#include <QVariant>
#include <QStringList>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

struct QValueSpaceItemPrivate;
class Q_CFW_EXPORT QValueSpaceItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString path READ path WRITE setPath);
    Q_PROPERTY(QVariant value READ valuex NOTIFY contentsChanged);

public:
    explicit QValueSpaceItem(QObject *parent = 0);
    explicit QValueSpaceItem(const QString &path, QObject *parent = 0);
    explicit QValueSpaceItem(const char *path, QObject *parent = 0);

    QValueSpaceItem(const QString &path, QValueSpace::LayerOptions filter, QObject *parent = 0);
    QValueSpaceItem(const char *path, QValueSpace::LayerOptions filter, QObject *parent = 0);

    QValueSpaceItem(const QString &path, const QUuid &uuid, QObject *parent = 0);
    QValueSpaceItem(const char *path, const QUuid &uuid, QObject *parent = 0);

    virtual ~QValueSpaceItem();

    void setPath(const QString &path);
    void setPath(QValueSpaceItem *item);
    QString path() const;
    void cd(const QString &path);
    void cdUp();

    bool isValid() const;

    QStringList subPaths() const;

    QVariant valuex(const QVariant &def = QVariant()) const;
    QVariant value(const QByteArray &subPath = QByteArray(),
                   const QVariant &def = QVariant()) const;
    QVariant value(const char *subPath, const QVariant &def = QVariant()) const;
    QVariant value(const QString &subPath, const QVariant &def = QVariant()) const;

signals:
    void contentsChanged();

protected:
    virtual void connectNotify(const char *signal);
    virtual void disconnectNotify(const char *signal);

private:
    Q_DISABLE_COPY(QValueSpaceItem)
    QValueSpaceItemPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QVALUESPACEITEM_H
