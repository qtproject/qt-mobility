/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEVALUESPACESUBSCRIBER_P_H
#define QDECLARATIVEVALUESPACESUBSCRIBER_P_H

#include <QObject>
#include "qvaluespacesubscriber.h"

QTM_USE_NAMESPACE

class QDeclarativeValueSpaceSubscriberPrivate;
class QDeclarativeValueSpaceSubscriber : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QVariant value READ value NOTIFY contentsChanged)
    Q_PROPERTY(bool connected READ isConnected)
    Q_PROPERTY(QStringList subPaths READ subPaths)

public:
    QDeclarativeValueSpaceSubscriber();
    ~QDeclarativeValueSpaceSubscriber();

    QString path() const;

    QVariant value(const QString &subPath = QString(), const QVariant &def=QVariant()) const;

    QStringList subPaths() const;

    bool isConnected() const;

public slots:
    void setPath(QString path);

signals:
    void contentsChanged();
    void pathChanged();

private:
    QDeclarativeValueSpaceSubscriberPrivate *d;
};

#endif // QDECLARATIVEVALUESPACESUBSCRIBER_P_H
