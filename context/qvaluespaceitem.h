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

#include <QObject>
#include <QVariant>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

struct QValueSpaceItemPrivate;
class Q_CFW_EXPORT QValueSpaceItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString path READ path WRITE setPath);
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY contentsChanged);

public:
    QValueSpaceItem(const QValueSpaceItem &base, const QByteArray &path, QObject *parent = 0);
    QValueSpaceItem(const QValueSpaceItem &base, const QString &path, QObject *parent = 0);
    QValueSpaceItem(const QValueSpaceItem &base, const char * path, QObject *parent = 0);
    QValueSpaceItem(const QValueSpaceItem &other, QObject *parent = 0);
    explicit QValueSpaceItem(const QByteArray &path, QObject *parent = 0);
    explicit QValueSpaceItem(const QString &path, QObject *parent = 0);
    explicit QValueSpaceItem(const char *path, QObject *parent = 0);
    explicit QValueSpaceItem(QObject *parent = 0);
    virtual ~QValueSpaceItem();

    void setPath(const QString &path);
    QString path() const;

    QList<QString> subPaths() const;

    bool remove();
    bool remove(const QByteArray &subPath);
    bool remove(const char *subPath);
    bool remove(const QString &subPath);

    bool setValue(const QVariant &value);
    bool setValue(const QByteArray &subPath, const QVariant &value);
    bool setValue(const char *subPath, const QVariant &value);
    bool setValue(const QString &subPath, const QVariant &value);

    bool sync();

    QVariant value(const QByteArray &subPath = QByteArray(),
                   const QVariant &def = QVariant()) const;
    QVariant value(const char *subPath, const QVariant &def = QVariant()) const;
    QVariant value(const QString &subPath, const QVariant &def = QVariant()) const;

    QValueSpaceItem &operator=(const QValueSpaceItem &other);

signals:
    void contentsChanged();

protected:
    virtual void connectNotify(const char *signal);
    virtual void disconnectNotify(const char *signal);

private:
    QValueSpaceItemPrivate *d;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QVALUESPACEITEM_H
