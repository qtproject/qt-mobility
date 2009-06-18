/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qabstractsecuritysession.h"

QT_BEGIN_NAMESPACE

/*!
    \class QAbstractSecuritySession
    \brief The QAbstractSecuritySession class provides a generic mechanism to enable
    permission checks for services.

    A QAbstractSecuritySession encapsulates the service client's capabilities. QServiceManager
    can match those capabilites with the capabilites required by a particular service. 
    Service capabilites are declared via the services XML description. 

    The use of a security session is not mandated by the service manager. If the client
    is passing a security session object QServiceManager ensures that the permissions
    are checked before the requested service is loaded and forwards the session to the 
    service in case the service intends to implement additional checks. If no security 
    session is passed to QServiceManager capability checks are not performed.

    QAbstractSecuritySession describes the abstract interface that security/permission
    engines must implement in order to provide capability related functionality.

    \sa QServiceManager, QServicePluginInterface
*/

/*!
    Constrcuts an abstract item model with the given \a parent.
*/
QAbstractSecuritySession::QAbstractSecuritySession(QObject* parent)
    : QObject(parent)
{
}

/*!
    Destroys the abstract security session.
*/
QAbstractSecuritySession::~QAbstractSecuritySession()
{
}

/*! 
    \fn bool QAbstractSecuritySession::isAllowed(const QStringList& capabilities) = 0;

    Returns true if the security session has sufficient rights to access the required
    service \a capabilities. 
*/

QT_END_NAMESPACE
