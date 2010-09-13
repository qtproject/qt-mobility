/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qremoteservicecontrol.h"
#include "qremoteservicecontrol_p.h"

QTM_BEGIN_NAMESPACE


/*!
    \class QRemoteServiceControl
    \inmodule QtServiceFramework
    \ingroup servicefw
    \brief The QRemoteServiceControl class manages instances of remote service objects.

    This class instanciates IPC based service objects which have been registered
    via QRemoteServiceClassRegister. It owns each created service object instance and
    ensures that the platform specific IPC mechanism publishes the required service
    object to other processes in the system.
 
    \sa QRemoteServiceClassRegister   
*/

/*!
    Creates a service control instance with the given \a parent.
*/
QRemoteServiceControl::QRemoteServiceControl(QObject* parent)
    : QObject(parent)
{
    d = QRemoteServiceControlPrivate::constructPrivateObject(this);
    connect(d, SIGNAL(lastInstanceClosed()), this, SIGNAL(lastInstanceClosed()));
}

/*!
    Destroys the service control instance
*/
QRemoteServiceControl::~QRemoteServiceControl()
{
}

/*!
    Publishes every service that has been registered using
    \l QRemoteServiceClassRegister::registerType(). \a ident is the service specific
    IPC address under which the service can be reached. This address must match
    the address provided in the services xml descriptor (see <filepath> tag).
*/
void QRemoteServiceControl::publishServices( const QString& ident)
{
    d->publishServices(ident);
}

bool QRemoteServiceControl::quitOnLastInstanceClosed() const
{
  return d->quitOnLastInstanceClosed();
}

void QRemoteServiceControl::setQuitOnLastInstanceClosed(bool quit)
{
  d->setQuitOnLastInstanceClosed(quit);
}

#include "moc_qremoteservicecontrol.cpp"

QTM_END_NAMESPACE
