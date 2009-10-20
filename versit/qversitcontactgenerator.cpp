/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

/*!
 * \class QContact
 *
 * \brief The QContact class provides an addressbook contact.
 *
 * A QContact consists of zero or more details.
 *
 * An instance of the QContact class represents an in-memory contact,
 * and may not reflect the state of that contact found in persistent
 * storage until the appropriate synchronisation method is called
 * on the QContactManager (i.e., saveContact, removeContact).
 *
 * \sa QContactManager, QContactDetail
 */

#include "qversitdefs.h"
#include <qversitcontactgenerator.h>
#include "qversitcontactgenerator_p.h"
#include <qversitdocument.h>
#include <qversitproperty.h>
#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactname.h>
#include <qcontactphonenumber.h>
#include <qcontactaddress.h>
#include <qcontactemailaddress.h>
#include <qcontactorganization.h>
#include <qcontacturl.h>
#include <qcontactguid.h>
#include <qcontacttimestamp.h>
#include <QHash>

QVersitContactGenerator::QVersitContactGenerator() 
    : d(new QVersitContactGeneratorPrivate)
{
}

QVersitContactGenerator::~QVersitContactGenerator()
{
    delete d;
}

QContact QVersitContactGenerator::generateContact(
    const QVersitDocument& versitDocument)
{
    QContact contact;
    const QList<QVersitProperty> properties = versitDocument.properties();
    foreach(const QVersitProperty& property,properties) {
        QContactDetail* detail = 0;
        if (property.name() == QString::fromAscii(versitNameId)) {
            detail = d->createName(property);
        }
        else if (property.name() == QString::fromAscii(versitPhoneId)) {
            detail = d->createPhone(property);
        }        
        else if (property.name() == QString::fromAscii(versitAddressId)) {
            detail = d->createAddress(property);
        }
        else if (property.name() == QString::fromAscii(versitOrganizationId)) {
            detail = d->createOrganization(property);
        }
        else if (property.name() == QString::fromAscii(versitEmailId)) {
            detail = d->createEmail(property);
        }
        else if (property.name() == QString::fromAscii(versitUrlId)) {
            detail = d->createUrl(property);
        }
        else if (property.name() == QString::fromAscii(versitUidId)) {
            detail = d->createUid(property);
        }
        else if (property.name() == QString::fromAscii(versitRevisionId)) {
            detail = d->createTimeStamp(property);
        }
        else if (property.name() == QString::fromAscii(versitAnniversaryId)) {
            detail = d->createAnniversary(property);
        }
        else {
            // NOP
        }
            
        if (detail) {
            detail->setContexts(d->extractContexts(property));
            contact.saveDetail(detail);
        }
    } 
    return contact;
}
