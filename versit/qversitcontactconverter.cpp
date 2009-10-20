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
 * \brief The QVersitContactConverter class converts the contact into versit document.
 *
 * A QVersitContactConverter process contact detail and generates the corresponding
 * versit document.
 *
 * \sa QContact, QVersitDocument, QVersitProperty
 */


#include <qcontact.h>
#include <qcontactdetail.h>
#include <qcontactmanager.h>
#include <qcontactname.h>
#include <qcontactaddress.h>
#include <qcontactguid.h>
#include <qcontacturl.h>
#include <qcontacttimestamp.h>
#include <qcontactbirthday.h>
#include <qcontactnote.h>
#include <qcontactgeolocation.h>

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
QVersitDocument QVersitContactConverter::convertContact(const QContact& contact)
{
    QVersitDocument versitDocument;    
    QList<QContactDetail> allDetails = contact.details();
    for (int i = 0; i < allDetails.size(); i++) {
        QContactDetail detail = allDetails.at(i);
        //Encode FieldInfo
        encodeFieldInfo(versitDocument, detail );
    }
    return versitDocument;
}



/*!
 * Encode Contact Field Information into the Versit Document 
 */
void QVersitContactConverter::encodeFieldInfo(QVersitDocument& versitDocument, 
                                                const QContactDetail& detail )
{
    if (detail.definitionName() == QContactName::DefinitionName ){
        encodeName(versitDocument, detail);
    }
    else if (detail.definitionName() == QContactPhoneNumber::DefinitionName){
        encodePhoneNumber(versitDocument, detail);
    }
    else if (detail.definitionName()== QContactEmailAddress::DefinitionName){
        encodeEmailAddress(versitDocument, detail);
    }
    else if (detail.definitionName() == QContactAddress::DefinitionName){
        encodeStreetAddress(versitDocument, detail);
    }
    else if (detail.definitionName() == QContactGuid::DefinitionName){
        encodeUid(versitDocument, detail);
    }
    else if (detail.definitionName() == QContactUrl::DefinitionName){
        encodeUrl(versitDocument, detail);
    }
    else if (detail.definitionName() == QContactTimestamp::DefinitionName){
        encodeRev(versitDocument, detail);
    }
    else if (detail.definitionName() == QContactBirthday::DefinitionName){
        encodeBirthDay(versitDocument, detail);
    }
    else if (detail.definitionName() == QContactGeolocation::DefinitionName){
        encodeGeoLocation(versitDocument, detail);
    }
    else if (detail.definitionName() == QContactNote::DefinitionName){
        encodeNote(versitDocument, detail);
    }

}


/*!
 * Encode Contact Name Field Information into the Versit Document 
 */
void QVersitContactConverter::encodeName(QVersitDocument& versitDocument, 
                                            const QContactDetail& detail)
{   
    QContactName contactName = static_cast<QContactName>(detail);
    QString name = d->mMappingTable.value(detail.definitionName());
    
    QString value = contactName.last() +  QString::fromAscii(";")  + 
                    contactName.first() + QString::fromAscii(";") + 
                    contactName.middle() + QString::fromAscii(";")  + 
                    contactName.prefix() + QString::fromAscii(";") + contactName.suffix();
    
    QVersitProperty versitProperty;
    versitProperty.setName(name);
    versitProperty.setValue(value.toAscii());
    versitDocument.addProperty(versitProperty);
}


/*!
 * Encode Phone Numer Field Information into the Versit Document 
 */
void QVersitContactConverter::encodePhoneNumber(QVersitDocument& versitDocument,
                                                    const QContactDetail& detail )
{
    QContactPhoneNumber phoneNumer = static_cast<QContactPhoneNumber>(detail);
    
    QString name = d->mMappingTable.value(detail.definitionName());
    QString value = phoneNumer.number();
    
    QVersitProperty versitProperty;

    //Add Context 
    QStringList contexts = phoneNumer.contexts();
    encodeParameters(versitProperty, contexts);
    
    //Add SubTypes
    QStringList subTypes = phoneNumer.subTypes();
    encodeParameters(versitProperty, subTypes);
    
    //Add Values
    versitProperty.setName(name);
    versitProperty.setValue(value.toAscii());
    versitDocument.addProperty(versitProperty);
}


/*!
 * Encode Email Address Field Information into the Versit Document 
 */

void QVersitContactConverter::encodeEmailAddress(QVersitDocument& versitDocument,
                                                    const QContactDetail& detail )
{
    QContactEmailAddress emailAddress = static_cast<QContactEmailAddress>(detail);
    QString name = d->mMappingTable.value(detail.definitionName());
    QString value = emailAddress.emailAddress();

    QVersitProperty versitProperty;
    
    //Add Context 
    QStringList contexts = emailAddress.contexts();
    encodeParameters(versitProperty, contexts);
    
    // Add Value:
    versitProperty.setName(name);
    versitProperty.setValue(value.toAscii());
    versitDocument.addProperty(versitProperty);
}


/*!
 * Encode Street Address Field Information into the Versit Document 
 */

void QVersitContactConverter::encodeStreetAddress(QVersitDocument& versitDocument,
                                                    const QContactDetail& detail )
{
    QContactAddress address = static_cast<QContactAddress>(detail);
    
    QString name = d->mMappingTable.value(detail.definitionName());
    QString value = address.postOfficeBox() + QString::fromAscii(";") +
                    QString::fromAscii(";") + address.street() +
                    QString::fromAscii(";") + address.locality() + 
                    QString::fromAscii(";") + address.region() + 
                    QString::fromAscii(";") + address.postcode() + 
                    QString::fromAscii(";") + address.country();
    
    QVersitProperty versitProperty;
    
    //Add Context 
    QStringList contexts = address.contexts();
    encodeParameters(versitProperty, contexts);
    
    //Add SubTypes
    QStringList subTypes = address.subTypes();
    encodeParameters(versitProperty, subTypes);
    
    versitProperty.setName(name);
    versitProperty.setValue(value.toAscii());
    versitDocument.addProperty(versitProperty);
}


/*!
 * Encode URL Field Information into the Versit Document
 */

void QVersitContactConverter::encodeUrl(QVersitDocument& versitDocument,
                                        const QContactDetail& detail )
{
    QContactUrl contactUrl = static_cast<QContactUrl>(detail);

    QString name = d->mMappingTable.value(detail.definitionName());
    QString value = contactUrl.url();

    QVersitProperty versitProperty;

    //Add Context
    QStringList contexts = contactUrl.contexts();
    encodeParameters(versitProperty, contexts);

    //Encode Sub Types.
    QString type = contactUrl.subType();
    QStringList subTypes;
    subTypes.append(type);
    encodeParameters(versitProperty, subTypes);

    //Add Values
    versitProperty.setName(name);
    versitProperty.setValue(value.toAscii());
    versitDocument.addProperty(versitProperty);
}



/*!
 * Encode Uid Field Information into the Versit Document
 */

void QVersitContactConverter::encodeUid(QVersitDocument& versitDocument,
                                        const QContactDetail& detail )
{
    QContactGuid uid = static_cast<QContactGuid>(detail);

    QString name = d->mMappingTable.value(detail.definitionName());
    QString value = uid.guid();

    QVersitProperty versitProperty;

    //Add Values
    versitProperty.setName(name);
    versitProperty.setValue(value.toAscii());
    versitDocument.addProperty(versitProperty);
}


/*!
 * Encode REV Field Information into the Versit Document
 */

void QVersitContactConverter::encodeRev(QVersitDocument& versitDocument,
                                        const QContactDetail& detail )
{
    QContactTimestamp  rev = static_cast<QContactTimestamp >(detail);
    QString name = d->mMappingTable.value(detail.definitionName());

    QString value = rev.lastModified().toString(Qt::ISODate);

    if ( !value.size() ) {
     value = rev.created().toString(Qt::ISODate);
    }

    QVersitProperty versitProperty;

    //Add Values
    versitProperty.setName(name);
    versitProperty.setValue(value.toAscii());
    versitDocument.addProperty(versitProperty);
}


/*!
 * Encode BirthDay Field Information into the Versit Document
 */

void QVersitContactConverter::encodeBirthDay(QVersitDocument& versitDocument,
                                              const QContactDetail& detail )
{
    QContactBirthday bday = static_cast<QContactBirthday >(detail);
    QString name = d->mMappingTable.value(detail.definitionName());
    QString value = bday.date().toString(Qt::ISODate);

    QVersitProperty versitProperty;

    //Add Values
    versitProperty.setName(name);
    versitProperty.setValue(value.toAscii());
    versitDocument.addProperty(versitProperty);
}



/*!
 * Encode Comment i.e. Note Field Information into the Versit Document
 */

void QVersitContactConverter::encodeNote(QVersitDocument& versitDocument,
                                         const QContactDetail& detail )
{
    QContactNote contactNote = static_cast<QContactNote >(detail);
    QString name = d->mMappingTable.value(detail.definitionName());
    QString value = contactNote.note();

    QVersitProperty versitProperty;

    //Add Values
    versitProperty.setName(name);
    versitProperty.setValue(value.toAscii());
    versitDocument.addProperty(versitProperty);
}


/*!
 * Encode Geo Prpoperties Field Information into the Versit Document
 */

void QVersitContactConverter::encodeGeoLocation(QVersitDocument& versitDocument,
                                                const QContactDetail& detail )
{
    QContactGeolocation geoLocation = static_cast<QContactGeolocation >(detail);
    QString name = d->mMappingTable.value(detail.definitionName());

    QString longitude, latitude;
    QString value = longitude.setNum(geoLocation.longitude()) +
                    QString::fromAscii(",") +
                    latitude.setNum(geoLocation.latitude());

    QVersitProperty versitProperty;

    //Add Values
    versitProperty.setName(name);
    versitProperty.setValue(value.toAscii());
    versitDocument.addProperty(versitProperty);
}



/*!
 * Encode Parameter for versit property if its supported in Versit Document 
 */
void QVersitContactConverter::encodeParameters(
    QVersitProperty& property,
    const QStringList& paramList)
{
    foreach (QString type, paramList) {
        QString parameterValue = d->mMappingTable.value(type);
        if (parameterValue.size())
            property.addParameter(QString::fromAscii(versitType),parameterValue);
    }
}


/*!
 * Get Versit contact mapping table 
 */

const QHash<QString,QString>& QVersitContactConverter::getMappingTable()
{
    return d->mMappingTable;
}
