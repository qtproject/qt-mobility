/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMEDIAENDPOINTINTERFACE_H
#define QMEDIAENDPOINTINTERFACE_H

#include <QtCore/qplugin.h>

#include "qmultimediaglobal.h"

class Q_MEDIA_EXPORT QMediaEndpointInterface
{
public:
    enum Direction
    {
        Input = 0x01,
        Output = 0x02,
        InputOutput = Input | Output
    };

    virtual ~QMediaEndpointInterface();

    virtual Direction direction() const = 0;
};

#define QMediaEndpointInterface_iid "com.nokia.Qt.QMediaEndpointInterface/1.0"

Q_DECLARE_INTERFACE(QMediaEndpointInterface, QMediaEndpointInterface_iid);

template <typename T> const char *qmediaendpoint_iid() { return 0; }

#define Q_MEDIA_DECLARE_ENDPOINT(IFace, IId) \
    template <> inline const char *qmediaendpoint_iid<IFace *>() { return IId; }

#endif
