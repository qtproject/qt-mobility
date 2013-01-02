/**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60VIDEOFRAMERATE_H
#define S60VIDEOFRAMERATE_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

class QElapsedTimer;
class QTimer;

/**
 * Class for measuring video frame rate
 *
 * Delivery of a new video frame is notified by the client via the notify() slot.
 * This class performs regular sampling of the event frequency and calculates a
 * rolling average which is published via the frequency property.
 */
class S60VideoFrameRate : public QObject
{
    Q_OBJECT

public:
    S60VideoFrameRate(QObject *parent = 0);
    ~S60VideoFrameRate();
    qreal frequency() const;

signals:
    void frequencyChanged(qreal freq) const;

public slots:
    void notify();

private slots:
    void sample();
    void trace();

private:
    QTimer *m_sampleTimer;
    QTimer *m_traceTimer;
    QScopedPointer<QElapsedTimer> m_elapsedTimer;
    int m_count;
    QVector<qreal> m_history;
    int m_historyIndex;
    int m_historyCount;
    qreal m_frequencySum;
    qreal m_frequency;
};

#endif // S60VIDEOFRAMERATE_H
