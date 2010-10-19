/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef QDECLARATIVEHAPTICSEFFECT_H
#define QDECLARATIVEHAPTICSEFFECT_H

#include <QtDeclarative/qdeclarative.h>
#include <qfeedbackeffect.h>
#include <qfeedbackactuator.h>

QTM_USE_NAMESPACE

class QDeclarativeHapticsEffect : public QFeedbackHapticsEffect
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QFeedbackActuator> actuators READ actuators NOTIFY actuatorsChanged)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)

public:
    QDeclarativeHapticsEffect(QObject *parent = 0) : QFeedbackHapticsEffect(parent), running_(false), paused_(false) {
        actuators_ = QFeedbackActuator::actuators();
        QObject::connect(this, SIGNAL(stateChanged()), this, SLOT(updateState()));
    }

    bool isRunning() const { return running_; }
    bool isPaused() const { return paused_; }
    void setRunning(bool running) {
        State currentState = state();
        if (currentState != Running && running) {
            start();
        } else if (currentState != Stopped && !running) {
            stop();
        }
    }

    void setPaused(bool paused) {
        State currentState = state();
        if (currentState == Paused && !paused) {
            start();
        } else if (currentState == Running && paused) {
            pause();
        }
    }

    QDeclarativeListProperty<QFeedbackActuator> actuators() {
        return QDeclarativeListProperty<QFeedbackActuator>(this,
                0,
                actuator_append,
                actuator_count,
                actuator_at,
                0);
    }
    static void actuator_append(QDeclarativeListProperty<QFeedbackActuator> *prop, QFeedbackActuator *actuator)
    {
        static_cast<QDeclarativeHapticsEffect*>(prop->object)->actuators_.append(actuator);
        emit static_cast<QDeclarativeHapticsEffect*>(prop->object)->actuatorsChanged();
    }
    static int actuator_count(QDeclarativeListProperty<QFeedbackActuator> *prop)
    {
        return static_cast<QDeclarativeHapticsEffect*>(prop->object)->actuators_.size();
    }
    static QFeedbackActuator *actuator_at(QDeclarativeListProperty<QFeedbackActuator> *prop, int index)
    {
        return static_cast<QDeclarativeHapticsEffect*>(prop->object)->actuators_.at(index);
    }

signals:
    void runningChanged();
    void pausedChanged();
    void actuatorsChanged();
public slots:
    void updateState() {
        bool running = state() == Running;
        bool paused = state() == Paused;
        if (running != running_) {
            running_ = running;
            emit runningChanged();
        }
        if (paused != paused_) {
            paused_ = paused;
            emit pausedChanged();
        }
    }
public:
    bool running_;
    bool paused_;
    QList<QFeedbackActuator*> actuators_;
};

QML_DECLARE_TYPE(QDeclarativeHapticsEffect);

#endif // QDECLARATIVEHAPTICSEFFECT_H
