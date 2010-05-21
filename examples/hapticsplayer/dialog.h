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

#include <qmobilityglobal.h>

#include "ui_dialog.h"

#include <qfeedbackdevice.h>
#include <qfeedbackeffect.h>
QTM_USE_NAMESPACE


#ifndef DIALOG_H_
#define DIALOG_H_

class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog();

private Q_SLOTS:
    void deviceChanged();
    void enabledChanged(bool);
    void playPauseClicked();
    void durationChanged(int);
    void intensityChanged(int);
    void effectStateChanged(QAbstractAnimation::State newState);

    void attackTimeChanged(int);
    void attackIntensityChanged(int);
    void fadeTimeChanged(int);
    void fadeIntensityChanged(int);

    void periodChanged(int value);
    void periodToggled(bool on);

    void instantPlayClicked();

private:
    QFeedbackDevice currentDevice() const;
    Ui_Dialog ui;
    QFeedbackEffect effect;
};

#endif

