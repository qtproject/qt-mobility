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


#ifndef PROJWRAPPER_P_H
#define PROJWRAPPER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"
#include <QSharedDataPointer>

QTM_BEGIN_NAMESPACE

class ProjCoordinate;
class QString;

class ProjCoordinateSystemPrivate;
class ProjCoordinateSystem
{
    ProjCoordinateSystem(const QString &projection);
    ProjCoordinateSystem(const ProjCoordinateSystem &other);
    virtual ~ProjCoordinateSystem();

private:
    QSharedDataPointer<ProjCoordinateSystemPrivate> d;

    friend class ProjCoordinate;
};

class ProjCoordinatePrivate;
class ProjCoordinate
{
    ProjCoordinate(double x, double y, double z, const ProjCoordinateSystem &system);
    ProjCoordinate(const ProjCoordinate &other);
    virtual ~ProjCoordinate();

    double x() const;
    double y() const;
    double z() const;

    bool convert(const ProjCoordinateSystem &system);

private:
    const ProjCoordinatePrivate *d;
};

QTM_END_NAMESPACE

#endif // PROJWRAPPER_P_H
