/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <proj_api.h>

int main(int argc, char **argv)
{
    projPJ pj_merc, pj_latlong;
    double x, y;
    int p;

    if (!(pj_merc = pj_init_plus("+proj=tmerc +lat_0=-27.5796 +lon_0=153.1 +k=1.0 +x_0=0 +y_0=0")))
        return 1;
    if (!(pj_latlong = pj_init_plus("+proj=latlong +ellps=WGS84")))
        return 1;

    x = -13.0;
    y = 21.0;
    p = pj_transform(pj_merc, pj_latlong, 1, 1, &x, &y, NULL);
    if (p)
        return 1;

    return 0;
}
