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

#include "qservicecontrol.h"
#ifdef Q_OS_SYMBIAN
#include "qservicecontrol_s60_p.h"
#else
#include "qservicecontrol_p.h"
#endif

QTM_BEGIN_NAMESPACE


/*!
    \class QServiceControl
    \ingroup servicefw
    \brief The QServiceControl class manages instances of remote service objects.

    This class instanciates IPC based service objects which have been registered
    via QServiceTypeRegister. It owns each created service object instance and
    ensures that the platform specific IPC mechanism publishes the required service
    object to other processes in the system.
 
    \sa QServiceTypeRegister   
*/

/*!
    Creates a service control instance with the given \a parent.
*/
QServiceControl::QServiceControl(QObject* parent)
    : QObject(parent)
{
    d = new QServiceControlPrivate(this);
}

/*!
    Destroys the service control instance
*/
QServiceControl::~QServiceControl()
{
}

/*!
    Publishes every service that has been registered using
    \l QServiceTypeRegister::registerType(). \a ident is the service specific
    IPC address under which the service can be reached. This address must match
    the address provide in the services xml descriptor (see <filepath> tag).
*/
void QServiceControl::publishServices( const QString& ident)
{
    d->publishServices(ident);
}


#include "moc_qservicecontrol.cpp"

QTM_END_NAMESPACE
