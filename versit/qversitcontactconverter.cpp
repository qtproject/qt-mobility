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
 * \class QVersitContactConverter
 *
 * \brief The QVersitContactConverter class converts the contacts into versit document.
 *
 * A QVersitContactConverter process contacts list and generates the corresponding
 * versit document.
 *
 * \sa QContact, QVersitDocument, QVersitProperty
 */


#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactmanager.h>
#include <qcontactname.h>
#include <qcontactphonenumber.h>
#include <qcontactemailaddress.h>
#include <qcontactaddress.h>

#include "qversitdefs.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qversitcontactconverter.h"
#include "qversitcontactconverter_p.h"


/*!
 * Constructs a new contact converter
 */
QVersitContactConverter::QVersitContactConverter()
    : d(new QVersitContactConverterPrivate())
{
}

/*!
 * Frees any memory in use by this contact converter
 */
QVersitContactConverter::~QVersitContactConverter()
{
}

/*!
 * Get the error code
 */
QVersitContactConverter::Error QVersitContactConverter::error() const
{
    return d->mError;
}

/*!
 * Get the versit document corresponding to the corresponding contact 
 */
QVersitDocument QVersitContactConverter::convertContacts(const QContact& contact)
{
    QVersitDocument versitDocument;
    int contactCount = 0;
    QContactManager cm;
    QList<QContactDetail> allDetails = contact.details();
    for (int i = 0; i < allDetails.size(); i++) {
        QContactDetail detail = allDetails.at(i);
        QContactDetailDefinition currentDefinition = 
            cm.detailDefinition(detail.definitionName());

        //Encode FieldInfo
        encodeFieldInfo(versitDocument, contact, currentDefinition);
    }
    return versitDocument;
}



/*!
 * Encode Contact Field Information into the Versit Document 
 */
void QVersitContactConverter::encodeFieldInfo(QVersitDocument versitDocument, 
                                                const QContact& contact, 
                                                QContactDetailDefinition definitionName)
{
    if (definitionName.name() == versitContactName )
    {
        encodeName(versitDocument, contact, definitionName);
    } else if (definitionName.name() == versitContactPhoneNumer){
        encodePhoneNumber(versitDocument, contact, definitionName);
    } else if (definitionName.name()== versitContactEmail){
        encodeEmailAddress(versitDocument, contact, definitionName);
    } else if (definitionName.name() == versitContactAddress){
        encodeStreetAddress(versitDocument, contact, definitionName);
    }
}


/*!
 * Encode Contact Name Field Information into the Versit Document 
 */
void QVersitContactConverter::encodeName(QVersitDocument versitDocument, 
                                            const QContact& contact,
                                            QContactDetailDefinition definitionName)
{
    QContactName contactName = contact.detail(definitionName.name());
    QString name = contactName.last() +  ";"  + contactName.first() + ";" + contactName.middle() 
                    + ";"  + contactName.prefix() + ";" + contactName.suffix();
    
    QVersitProperty versitProperty;
    versitProperty.addParameter(versitName, name);
    versitDocument.addProperty(versitProperty);
}

/*!
 * Encode Phone Numer Field Information into the Versit Document 
 */
void QVersitContactConverter::encodePhoneNumber(QVersitDocument versitDocument,
                                                    const QContact& contact,
                                                    QContactDetailDefinition definitionName)
{
    QContactPhoneNumber phoneNumer = contact.detail(definitionName.name());
    QString phone = phoneNumer.number();
    
    QVersitProperty versitProperty;
    versitProperty.addParameter(versitPhoneNumer, phone);
    versitDocument.addProperty(versitProperty);
}


/*!
 * Encode Email Address Field Information into the Versit Document 
 */

void QVersitContactConverter::encodeEmailAddress(QVersitDocument versitDocument,
                                                    const QContact& contact,
                                                    QContactDetailDefinition definitionName)
{
    QContactEmailAddress emailAddress = contact.detail(definitionName.name());
    QString email = emailAddress.emailAddress();

    QVersitProperty versitProperty;
    versitProperty.addParameter(versitEmail, email);
    versitDocument.addProperty(versitProperty);
}


/*!
 * Encode Street Address Field Information into the Versit Document 
 */

void QVersitContactConverter::encodeStreetAddress(QVersitDocument versitDocument,
                                                    const QContact& contact,
                                                    QContactDetailDefinition definitionName)
{
    QContactAddress address = contact.detail(definitionName.name());
    QString addr = address.postOfficeBox() + ";" + address.street() + ";" + address.locality() 
                    + ";" + address.region() + ";" + address.postcode() + ";" + address.country();
    
    QVersitProperty versitProperty;
    versitProperty.addParameter(versitAddress, addr);
    versitDocument.addProperty(versitProperty);
}
