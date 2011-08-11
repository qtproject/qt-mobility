/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CONTACTBACKENDSDEFS_H
#define CONTACTBACKENDSDEFS_H

/*!
 * The definitions below are to be used as argument when constructiong a QContactManager.
 */
/*!
 * Default backend in Symbian devices.
 */
const QString SYMBIAN_BACKEND     = "qtcontacts:symbian:";

/*!
 * SIM backend. (ADN contacts)
 */
const QString SIM_BACKEND         = "qtcontacts:symbiansim:";

/*!
 * SIM backend for ADN contacts.
 */
const QString SIM_BACKEND_ADN     = "qtcontacts:symbiansim:store=ADN";

/*!
 * SIM backend for SDN contacts.
 */
const QString SIM_BACKEND_SDN     = "qtcontacts:symbiansim:store=SDN";

/*!
 * SIM backend for FDN contacts.
 */
const QString SIM_BACKEND_FDN     = "qtcontacts:symbiansim:store=FDN";

/*!
 * SIM backend for Own Number contacts.
 *
 */
const QString SIM_BACKEND_ON     = "qtcontacts:symbiansim:store=ON";

/*
 * Definition name for MyCard handling.
 *
 */
const QString MYCARD_DEFINTION = "MyCard";

#endif
