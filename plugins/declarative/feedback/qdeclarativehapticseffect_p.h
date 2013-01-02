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
#ifndef QDECLARATIVEHAPTICSEFFECT_P_H
#define QDECLARATIVEHAPTICSEFFECT_P_H

#include <QtDeclarative/qdeclarative.h>
#include "qdeclarativefeedbackeffect_p.h"
#include "qdeclarativefeedbackactuator_p.h"

QTM_USE_NAMESPACE

class QDeclarativeHapticsEffect : public QDeclarativeFeedbackEffect
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeFeedbackActuator> availableActuators READ availableActuators)
    Q_PROPERTY(qreal intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)
    Q_PROPERTY(int attackTime READ attackTime WRITE setAttackTime NOTIFY attackTimeChanged)
    Q_PROPERTY(qreal attackIntensity READ attackIntensity WRITE setAttackIntensity NOTIFY attackIntensityChanged)
    Q_PROPERTY(int fadeTime READ fadeTime WRITE setFadeTime NOTIFY fadeTimeChanged)
    Q_PROPERTY(qreal fadeIntensity READ fadeIntensity WRITE setFadeIntensity NOTIFY fadeIntensityChanged)
    Q_PROPERTY(int period READ period WRITE setPeriod NOTIFY periodChanged)
    Q_PROPERTY(QDeclarativeFeedbackActuator* actuator READ actuator WRITE setActuator NOTIFY actuatorChanged)

public:

    explicit QDeclarativeHapticsEffect(QObject *parent = 0);

    void setDuration(int msecs);
    int duration() const;
    void setIntensity(qreal intensity);
    qreal intensity() const;
    //the envelope
    void setAttackTime(int msecs);
    int attackTime() const;
    void setAttackIntensity(qreal intensity);
    qreal attackIntensity() const;
    void setFadeTime(int msecs);
    int fadeTime() const;
    void setFadeIntensity(qreal intensity);
    qreal fadeIntensity() const;
    void setPeriod(int msecs);
    int period() const;
    void setActuator(QDeclarativeFeedbackActuator *actuator);
    QDeclarativeFeedbackActuator* actuator() const;
    QDeclarativeListProperty<QDeclarativeFeedbackActuator> availableActuators();
    static int actuator_count(QDeclarativeListProperty<QDeclarativeFeedbackActuator> *prop);
    static QDeclarativeFeedbackActuator *actuator_at(QDeclarativeListProperty<QDeclarativeFeedbackActuator> *prop, int index);
signals:
    void intensityChanged();
    void attackTimeChanged();
    void attackIntensityChanged();
    void fadeTimeChanged();
    void fadeIntensityChanged();
    void periodChanged();
    void actuatorChanged();
public slots:
private:
    QFeedbackHapticsEffect* d;
    QList<QDeclarativeFeedbackActuator*> m_actuators;
    QDeclarativeFeedbackActuator* m_actuator;
};

QML_DECLARE_TYPE(QDeclarativeHapticsEffect);

#endif // QDECLARATIVEHAPTICSEFFECT_P_H
