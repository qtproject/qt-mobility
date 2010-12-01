/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (ivan.frade@nokia.com)
**
** This file is part of the QtSparql module (not yet part of the Qt Toolkit).
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** If you have questions regarding the use of this file, please contact
** Nokia at ivan.frade@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef TRACKERNOTIFIER_P_H
#define TRACKERNOTIFIER_P_H

#include <QObject>
#include <QVector>
#include <QStringList>
#include <QDBusConnection>
#include <QDBusArgument>

#include <QtDBus/QtDBus>

class QDBusInterface;
class QDBusPendingCallWatcher;
class QString;
class QTrackerResult;
class QTrackerDriver;

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

struct Quad
{
    int graph;
    int subject;
    int predicate;
    int object;
};

QDBusArgument& operator<<(QDBusArgument& argument, const Quad &t);
const QDBusArgument& operator>>(const QDBusArgument& argument, Quad &t);

class QTrackerChangeNotifier;
class QTrackerChangeNotifierPrivate : public QObject
{
    Q_OBJECT
public:
    QTrackerChangeNotifierPrivate(const QString& className,
                                  QDBusConnection c,
                                  QTrackerChangeNotifier* q);
public slots:
    void changed(QString className, QVector<Quad> deleted, QVector<Quad> inserted);
public:
    QTrackerChangeNotifier* q; // public part
    QString className; // which class is watched
    QDBusConnection connection;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // TRACKERNOTIFIER_P_H

