/****************************************************************************
**
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

#ifndef QDECLARATIVEFEEDBACKEFFECT_P_H
#define QDECLARATIVEFEEDBACKEFFECT_P_H

#include <QtDeclarative/qdeclarative.h>
#include <qfeedbackeffect.h>

QTM_USE_NAMESPACE

class QDeclarativeFeedbackEffect : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(ErrorType error READ error NOTIFY errorChanged)
    Q_ENUMS(Duration)
    Q_ENUMS(State)
    Q_ENUMS(ErrorType)

public:
    enum Duration {
        Infinite = QFeedbackEffect::Infinite
    };

    enum State {
        Stopped = QFeedbackEffect::Stopped,
        Paused = QFeedbackEffect::Paused,
        Running = QFeedbackEffect::Running,
        Loading = QFeedbackEffect::Loading
    };

    enum ErrorType {
        UnknownError = QFeedbackEffect::UnknownError,
        DeviceBusy = QFeedbackEffect::DeviceBusy
    };

    QDeclarativeFeedbackEffect(QObject *parent = 0);
    void setFeedbackEffect(QFeedbackEffect* effect);
    QFeedbackEffect* feedbackEffect();

    bool isRunning() const;
    bool isPaused() const;
    void setRunning(bool running);
    void setPaused(bool paused);
    virtual State state() const;
    virtual int duration() const;
    virtual void setState(State newState);
    virtual void setDuration(int newDuration);
    ErrorType error() const;

signals:
    void runningChanged();
    void pausedChanged();
    void durationChanged();
    void stateChanged();
    void errorChanged();
public slots:
    void updateState();
    void start();
    void stop();
    void pause();
private slots:
    void _error(QFeedbackEffect::ErrorType err);
private:
    bool m_running;
    bool m_paused;
    QFeedbackEffect* m_effect;
    ErrorType m_error;
};

QML_DECLARE_TYPE(QDeclarativeFeedbackEffect);

#endif // QDECLARATIVEFEEDBACKEFFECT_P_H
