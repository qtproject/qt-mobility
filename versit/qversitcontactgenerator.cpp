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
#include <qcontactaddress.h>
#include <QHash>


QVersitContactGenerator::QVersitContactGenerator()
{
    d = new QVersitContactGeneratorPrivate();
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
            detail = d->addName(property);
        }
        else if (property.name() == QString::fromAscii(versitAddressId)) {
            detail = createAddress(property);
        }
        if (detail)
            contact.saveDetail(detail);
    }  
    return contact;
}

/*!
 * Creates a QContactAddress from \a property
 */
QContactDetail* QVersitContactGenerator::createAddress(
    const QVersitProperty& property) const
{
    QContactAddress* address = new QContactAddress();
    
    QStringList types = 
        property.parameters().values(QString::fromAscii("TYPE"));
    address->setContexts(extractContexts(types));
    
    QStringList subTypes;
    foreach (QString type, types) {
        if (type == QString::fromAscii(versitDomesticId)) {
            subTypes.append(QContactAddress::SubTypeDomestic);
        }
        else if (type == QString::fromAscii(versitInternationalId)) {
            subTypes.append(QContactAddress::SubTypeInternational);
        }
        else if (type == QString::fromAscii(versitPostalId)) {
            subTypes.append(QContactAddress::SubTypePostal);
        }
        else if (type == QString::fromAscii(versitParcelId)) {
            subTypes.append(QContactAddress::SubTypeParcel);
        }
        else {
            // NOP
        }
    }
    address->setSubTypes(subTypes);
    
    QList<QByteArray> addressParts = property.value().split(';');
    if (!addressParts.isEmpty())
        address->setPostOfficeBox(QString::fromAscii(addressParts.takeFirst()));
    if (!addressParts.isEmpty())
        addressParts.removeFirst(); // TODO: Where to set the Extended Address?
    if (!addressParts.isEmpty())
        address->setStreet(QString::fromAscii(addressParts.takeFirst()));
    if (!addressParts.isEmpty())
        address->setLocality(QString::fromAscii(addressParts.takeFirst()));
    if (!addressParts.isEmpty()) 
        address->setRegion(QString::fromAscii(addressParts.takeFirst()));
    if (!addressParts.isEmpty())
        address->setPostcode(QString::fromAscii(addressParts.takeFirst()));
    if (!addressParts.isEmpty())
        address->setCountry(QString::fromAscii(addressParts.takeFirst()));
    
    return address;
}

/*!
 * Extracts the list of contexts from \a types
 */
QStringList QVersitContactGenerator::extractContexts(
    const QStringList& types) const
{   
    QStringList contexts;
    foreach (QString type, types) {
        if (type == QString::fromAscii(versitContextHomeId)) {
            contexts.append(QContactDetail::ContextHome);
        }
        else if (type == QString::fromAscii(versitContextWorkId)) {
            contexts.append(QContactDetail::ContextWork);
        }
        else {
            // NOP
        }
    }
    return contexts;    
}
