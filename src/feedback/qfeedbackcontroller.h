/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef QFEEDBACKCONTROLLER_H
#define QFEEDBACKCONTROLLER_H

#include <QObject>

#include <qmobilityglobal.h>
#include <qfeedbackeffect.h>

QTM_BEGIN_NAMESPACE

class QFeedbackEffect;
class Q_FEEDBACK_EXPORT QFeedbackController : public QObject
{
    Q_OBJECT
public:
    explicit QFeedbackController(QObject *parent = 0);

    // Default factory (smart system controller)
    static QFeedbackController* defaultController();

    // All controllers (e.g. attached gamepads, bluetooth/usb devices, piezo/eccentric mass engines)
    static QList<QFeedbackController*> allControllers();

    // Metadata
    QList<EffectType> supportedEffectTypes(); // might need emulatedTypes

    // Number of channels?? Or some way of describing links to another controller?
    // (e.g. gamepad with two independent eccentric mass engines.. although
    // that can just be handled with an effect that specifies the source location)


    // Type of feedback, for those that care
    enum FeedbackType {
        VibrationTactile,
        PiezoTactile,
        Auditory,
        Visual,
        LinearForce,
        RotatingForce,
        Temperature // Not common yet :)
    };

    QList<FeedbackType> supportedFeedbackTypes() const;

    enum Error {
        NoError,
        PermissionsError,
        DeviceBusyError,
    };
    Error error() const;

signals:

public slots:
    bool performEffect(const QFeedbackEffect& effect) const;
};

QTM_END_NAMESPACE

#endif // QFEEDBACKCONTROLLER_H
