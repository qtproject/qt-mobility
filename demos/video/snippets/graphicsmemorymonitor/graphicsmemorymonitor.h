/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef GRAPHICSMEMORYMONITOR_H
#define GRAPHICSMEMORYMONITOR_H

#include <QtCore/QObject>
#include <QtCore/QProcess>

class GraphicsMemoryMonitorPrivate;

class GraphicsMemoryMonitor : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(GraphicsMemoryMonitor)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(qint64 totalMemory READ totalMemory NOTIFY totalMemoryChanged)
    Q_PROPERTY(QString totalMemoryHumanReadable READ totalMemoryHumanReadable NOTIFY totalMemoryHumanReadableChanged)
    Q_PROPERTY(qint64 usedMemory READ usedMemory NOTIFY usedMemoryChanged)
    Q_PROPERTY(QString usedMemoryHumanReadable READ usedMemoryHumanReadable NOTIFY usedMemoryHumanReadableChanged)
    Q_PROPERTY(qint64 availableMemory READ availableMemory NOTIFY availableMemoryChanged)
    Q_PROPERTY(QString availableMemoryHumanReadable READ availableMemoryHumanReadable NOTIFY availableMemoryHumanReadableChanged)
    Q_PROPERTY(qint64 currentProcessUsage READ currentProcessUsage NOTIFY currentProcessUsageChanged)
    Q_PROPERTY(QString currentProcessUsageHumanReadable READ currentProcessUsageHumanReadable NOTIFY currentProcessUsageHumanReadableChanged)
    Q_PROPERTY(qint64 updateInterval READ updateInterval WRITE setUpdateInterval NOTIFY updateIntervalChanged)
public:
    GraphicsMemoryMonitor(QObject *parent = 0);
    ~GraphicsMemoryMonitor();

    static void qmlRegisterType();

    bool active() const;

    qint64 updateInterval() const;
    void setUpdateInterval(qint64 value);

    qint64 totalMemory() const;
    QString totalMemoryHumanReadable() const;

    qint64 usedMemory() const;
    QString usedMemoryHumanReadable() const;

    qint64 availableMemory() const;
    QString availableMemoryHumanReadable() const;

    qint64 currentProcessUsage() const;
    QString currentProcessUsageHumanReadable() const;

    struct ProcessInfo
    {
        qint64 pid;
        QString name;
        qint64 privateUsage;
        qint64 sharedUsage;
    };

    const QList<ProcessInfo> &processList() const;

public slots:
    Q_INVOKABLE void update();
    void trace();

signals:
    void activeChanged();
    void updateIntervalChanged();
    void changed();
    void totalMemoryChanged(qint64);
    void totalMemoryHumanReadableChanged(QString);
    void usedMemoryChanged(qint64);
    void usedMemoryHumanReadableChanged(QString);
    void availableMemoryChanged(qint64);
    void availableMemoryHumanReadableChanged(QString);
    void currentProcessUsageChanged(qint64);
    void currentProcessUsageHumanReadableChanged(QString);
    void processListChanged(const QList<GraphicsMemoryMonitor::ProcessInfo> &);

private:
    GraphicsMemoryMonitorPrivate *d_ptr;
};

#endif // GRAPHICSMEMORYMONITOR_H

