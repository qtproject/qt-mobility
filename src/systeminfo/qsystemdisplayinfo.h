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
#ifndef QSYSTEMDISPLAYINFO_H
#define QSYSTEMDISPLAYINFO_H

#include <QObject>
#include "qmobilityglobal.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QSystemDisplayInfoPrivate;

class  Q_SYSINFO_EXPORT QSystemDisplayInfo : public QObject
{
    Q_OBJECT
    Q_ENUMS(DisplayOrientation)

public:

    explicit QSystemDisplayInfo(QObject *parent = 0);
    ~QSystemDisplayInfo();

    enum DisplayOrientation {
        Unknown = 0,
        Landscape,
        Portrait,
        InvertedLandscape,
        InvertedPortrait
    };

    static int displayBrightness(int screen);
    static int colorDepth(int screen);

    QSystemDisplayInfo::DisplayOrientation getOrientation(int screen);
    float contrast(int screen);
    int getDPIWidth(int screen);
    int getDPIHeight(int screen);
    int physicalHeight(int screen);
    int physicalWidth(int screen);
};


QTM_END_NAMESPACE

QT_END_HEADER

#endif // QSYSTEMDISPLAYINFO_H
