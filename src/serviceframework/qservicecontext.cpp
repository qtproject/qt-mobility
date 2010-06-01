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

#include "qservicecontext.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QServiceContext
    \ingroup servicefw
    \brief The QServiceContext class provides context information to 
    services.

    A service context is created by clients and passed on to the service.
    It enables the opportunity to pass additional context information 
    and errors between services, clients and the service framework.
    
    Clients must implement this abstract class to receive context information.

    \sa QServiceManager

*/

/*!
    \enum QServiceContext::ContextType

    This enum describes the type of context information.

    \value  DisplayContext              The service provides user visible display 
                                        text such as an error message.
    \value  ScriptContext               The service provides a script which may 
                                        be executed by the client.
    \value  UserDefined                 The first context type that can be used for service
                                        specific context information.
*/

/*!
    \fn void QServiceContext::notify(ContextType type, const QVariant& data) = 0

    Services may call this function to notify the service client about service related
    context information of the given \a type. The contextual information is stored in \a data.  
*/

/*!
    Constrcuts a service context with the given \a parent.
*/
QServiceContext::QServiceContext(QObject* parent)
    : QObject(parent)
{

}

/*!
    Destroys the service context object.
*/
QServiceContext::~QServiceContext() 
{
}

/*!
    \property QServiceContext::clientId
    \brief the id of the client using the service.

    By default, this value is empty but you can change this by calling
    setClientId().
*/
QString QServiceContext::clientId() const
{
    return m_id;
}

/*!
    Sets the \a id of the client using the service.
*/
void QServiceContext::setClientId(const QString& id)
{
    m_id = id;
}

/*!
    \property QServiceContext::clientName
    \brief the name of the client using the service.

    By default, this value is empty but you can change this by calling
    setClientName(). This string is translated and can be shown to the user.
*/
QString QServiceContext::clientName() const
{
    return m_displayName;
}

void QServiceContext::setClientName(const QString& name)
{
    m_displayName = name;
}

#include "moc_qservicecontext.cpp"

QTM_END_NAMESPACE
