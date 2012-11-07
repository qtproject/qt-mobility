/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
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
