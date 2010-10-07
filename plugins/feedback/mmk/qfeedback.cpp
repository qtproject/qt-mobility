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

#include "qfeedback.h"
#include <QtCore/QtPlugin>
#include <QtCore/QCoreApplication>

Q_EXPORT_PLUGIN2(feedback_mmk, QFeedbackMMK)

QFeedbackMMK::QFeedbackMMK() : QObject(qApp)
{
}

QFeedbackMMK::~QFeedbackMMK()
{
}

void QFeedbackMMK::setLoaded(QFeedbackFileEffect *effect, bool load)
{
    Q_UNUSED(effect);
    // XXX TODO
    if (load) {
    } else {
    }
}

void QFeedbackMMK::setEffectState(QFeedbackFileEffect *effect, QFeedbackEffect::State state)
{
    Q_UNUSED(effect);
    Q_UNUSED(state);
    // XXX TODO
}

QFeedbackEffect::State QFeedbackMMK::effectState(const QFeedbackFileEffect *effect)
{
    Q_UNUSED(effect);
    // XXX TODO
    return QFeedbackEffect::Stopped;
}

int QFeedbackMMK::effectDuration(const QFeedbackFileEffect *effect)
{
    Q_UNUSED(effect);
    // XXX TODO
    return -1;
}

QStringList QFeedbackMMK::supportedMimeTypes()
{
    return QStringList() << "audio/x-wav"; // XXX todo - need an API for this
}

