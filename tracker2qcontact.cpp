/* * This file is part of qtcontacts-tracker
 * Copyright © 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Contact: Aleksandar Stojiljkovic <aleksandar.stojiljkovic@nokia.com>
 * This software, including documentation, is protected by copyright controlled by
 * Nokia Corporation. All rights are reserved. Copying, including reproducing, storing,
 * adapting or translating, any or all of this material requires the prior written consent
 * of Nokia Corporation. This material also contains confidential information which may
 * not be disclosed to others without the prior written consent of Nokia.
 */

#include "tracker2qcontact.h"

#include <qcontact.h>
#include <qcontactname.h>
#include <qcontactaddress.h>
#include <qcontactanniversary.h>
#include <qcontactavatar.h>
#include <qcontactbirthday.h>
#include <qcontactemailaddress.h>
#include <qcontactphonenumber.h>
#include <qcontacturl.h>
#include <qcontactsynctarget.h>
#include <qcontactgender.h>
#include <qcontactguid.h>

#include <QtTracker/ontologies/nco.h>

using namespace SopranoLive;

/**
   QContactPhoneNumber subtypes
 */
void setSubType(Live<nco::PhoneNumber> ncoPhone, QContactPhoneNumber& detail)
{
    //TODO pick one
    // AttributeSubTypeLandline;
    // AttributeSubTypeMobile;
    // AttributeSubTypeFacsimile;
    // AttributeSubTypePager;
    // AttributeSubTypeVoice;
    // AttributeSubTypeModem;
    // AttributeSubTypeVideo;
    // AttributeSubTypeCar;
    // AttributeSubTypeBulletinBoardSystem;
    // AttributeSubTypeMessagingCapable;

    // BUG: hasType is currently not working which means all numbers will be voice numbers.
    if( ncoPhone.hasType<nco::CellPhoneNumber>() ) {
        detail.setAttribute(QContactPhoneNumber::AttributeSubType, QContactPhoneNumber::AttributeSubTypeMobile);
    } else if ( not ncoPhone.hasType<nco::VoicePhoneNumber>() ) {
        // if it is not a voice number then no attributes are set
    } else {
        detail.setAttribute(QContactPhoneNumber::AttributeSubType, QContactPhoneNumber::AttributeSubTypeVoice);
    }
}

/**
   QContactAddress subtypes
 */
void setSubType(Live<nco::PostalAddress> ncoPostalAddress, QContactAddress& detail)
{
    //TODO pick one
    // AttributeSubTypeParcel;
    // AttributeSubTypePostal;
    // AttributeSubTypeDomestic;
    // AttributeSubTypeInternational;
    // detail.setAttribute(QContactPhoneNumber::AttributeSubType, QContactPhoneNumber::AttributeSubType....);
    Q_UNUSED( ncoPostalAddress );
    Q_UNUSED( detail );
}

/**
   QContactName subtypes
 */
void setSubType(Live<nco::PersonContact> ncoPersonContact, QContactName& detail)
{
    //TODO
    Q_UNUSED( ncoPersonContact );
    Q_UNUSED( detail );
}

/**
   QContactEmailAddress subtypes
*/
void setSubType(Live<nco::EmailAddress> ncoEmail, QContactEmailAddress& detail)
{
    //TODO pick one
    // AttributeSubTypeParcel;
    // AttributeSubTypePostal;
    // AttributeSubTypeDomestic;
    // AttributeSubTypeInternational;
    // detail.setAttribute(QContactPhoneNumber::AttributeSubType, QContactPhoneNumber::AttributeSubType....);
    Q_UNUSED( ncoEmail );
    Q_UNUSED( detail );
}

void setSubType(Live<nfo::FileDataObject> fdo, QContactAvatar& detail)
{
    Q_UNUSED( fdo );
    detail.setAttribute(QContactAvatar::AttributeSubType, QContactAvatar::AttributeSubTypeImage);
}

/**
   Copy nco::PersonContact -> QContactName
*/
void copyDetailData(const Live<nco::PersonContact>& ncoContact, QContactName& name)
{
    name.setPrefix     ( ncoContact->getNameHonorificPrefix() );
    name.setFirst      ( ncoContact->getNameGiven() );
    name.setMiddle     ( ncoContact->getNameAdditional() );
    name.setLast       ( ncoContact->getNameFamily() );
    name.setSuffix     ( ncoContact->getNameHonorificSuffix() );
    name.setDisplayName( ncoContact->getFullname() );
    setSubType( ncoContact, name );
}

/**
   Copy nco::PostalAddress -> QContactAddress
 */
void copyDetailData(const Live<nco::PostalAddress>& ncoPostalAddress, QContactAddress& address)
{
    //address.setDisplayLabel ("");
    address.setStreet       ( ncoPostalAddress->getStreetAddress() );
    address.setLocality     ( ncoPostalAddress->getLocality() );
    address.setRegion       ( ncoPostalAddress->getRegion() );
    address.setPostcode     ( ncoPostalAddress->getPostalcode() );
    address.setCountry      ( ncoPostalAddress->getCountry() );
    setSubType( ncoPostalAddress, address );
}

/**
   Copy nco::PhoneNumber -> QContactPhoneNumber
 */
void copyDetailData(const Live<nco::PhoneNumber>& ncoPhoneNumber, QContactPhoneNumber& detail)
{
    detail.setNumber (ncoPhoneNumber->getPhoneNumber());
    setSubType(ncoPhoneNumber, detail);
}

/**
   Copy nco::EmailAddress -> QContactEmailAddress
 */
void copyDetailData(const Live<nco::EmailAddress>& ncoEmail, QContactEmailAddress& detail)
{
    detail.setEmailAddress (ncoEmail->getEmailAddress());
    setSubType(ncoEmail, detail);
}

void copyDetailData(const Live<nfo::FileDataObject>& fdo, QContactAvatar& detail)
{
    if( fdo != 0 ) {
        QUrl url = fdo.toString();
        detail.setAvatar (url.toString());
        setSubType( fdo, detail );
    }
}
// TODO
/*
void copyDetailData(const Live<nco::PersonContact>& ncoContact, QContactAnniversary& detail)
{
    Q_UNUSED(ncoContact);
    Q_UNUSED(detail);
    //detail.setDate (QDate());
}
void copyDetailData(const Live<nco::PersonContact>& ncoContact, QContactGender& detail)
{
    Q_UNUSED(ncoContact);
    Q_UNUSED(detail);
    //const Live< Gender > gender = ncoContact->getGender();
    //detail.setGender (gender->);
}
void copyDetailData(const Live<nco::PersonContact>& ncoContact, QContactBirthday& detail)
{
    detail.setDate (QDate());
}

void copyDetailData(const Live<nco::PersonContact>& ncoContact, QContactGuid& detail)
{
    detail.setGuid (QString());
}
void copyDetailData(const Live<nco::PersonContact>& ncoContact, QContactSyncTarget& detail)
{
    detail.setSyncTarget (QString());
}
void copyDetailData(const Live<nco::PersonContact>& ncoContact, QContactUrl& detail)
{
    detail.setUrl (QString());
}
*/

bool Tracker2QContact::copyContactData(const Live<nco::PersonContact>& ncoContact, QContact& qcontact )
{
    bool ok;
    QUniqueId id = QString(ncoContact->getContactUID()).toUInt(&ok);
    if( not ok ) {
        qWarning() << "Could not convert id to number";
        return false;
    }
    qcontact.setId(id);

    QContactName detail;
    copyDetailData(ncoContact, detail);
    qcontact.saveDetail(&detail);

    LiveNodes addresses = ncoContact->getHasPostalAddresss(); //Home addresses
    foreach( const Live<nco::PostalAddress>& address, addresses ) {
        QContactAddress detail;
        copyDetailData(address, detail);
        detail.setAttribute(QContactAddress::AttributeContext, QContactAddress::AttributeContextHome);
        qcontact.saveDetail(&detail);
    }

    LiveNodes phoneNumbers = ncoContact->getHasPhoneNumbers(); //Home phone numbers
    foreach( const Live<nco::PhoneNumber>& phoneNumber, phoneNumbers) {
        QContactPhoneNumber detail;
        copyDetailData(phoneNumber, detail);
        detail.setAttribute(QContactPhoneNumber::AttributeContext, QContactPhoneNumber::AttributeContextHome);
        qcontact.saveDetail(&detail);
    }

    LiveNodes emailAddresses = ncoContact->getHasEmailAddresss(); //Home email addresses
    foreach( const Live<nco::EmailAddress>& ncoEmail, emailAddresses) {
        QContactEmailAddress detail;
        copyDetailData(ncoEmail, detail);
        detail.setAttribute(QContactEmailAddress::AttributeContext, QContactEmailAddress::AttributeContextHome);
        detail.setAttribute(QContactEmailAddress::AttributeSubType, QContactEmailAddress::AttributeSubTypeInternet);
        qcontact.saveDetail(&detail);
    }

    foreach( Live<nfo::FileDataObject> fdo, ncoContact->getPhotos() ) { //Home avatars
        if( fdo != 0 ) {
            QContactAvatar detail;
            copyDetailData(fdo, detail);
            detail.setAttribute(QContactAvatar::AttributeContext, QContactAvatar::AttributeContextHome);
            qcontact.saveDetail(&detail);
        }
    }


    // Work addresses+phonenumbers+emails+avatars

    LiveNodes affiliations = ncoContact->getHasAffiliations();
    foreach( const Live<nco::Affiliation>& affiliation, affiliations ) {
        Live<nco::OrganizationContact> org = affiliation->getOrg();
        LiveNodes addresses = org->getHasPostalAddresss();//Work addresses
        foreach( const Live<nco::PostalAddress>& address, addresses ) {
            QContactAddress detail;
            copyDetailData(address, detail);
            detail.setAttribute(QContactAddress::AttributeContext, QContactAddress::AttributeContextWork);
            qcontact.saveDetail(&detail);
        }
        LiveNodes phoneNumbers = org->getHasPhoneNumbers(); //Work phone
        foreach( const Live<nco::PhoneNumber>& phoneNumber, phoneNumbers) {
            QContactPhoneNumber detail;
            copyDetailData(phoneNumber, detail);
            detail.setAttribute(QContactPhoneNumber::AttributeContext, QContactPhoneNumber::AttributeContextWork);
            qcontact.saveDetail(&detail);
        }
        LiveNodes emailAddresses = org->getHasEmailAddresss(); //Work email addresses
        foreach( const Live<nco::EmailAddress>& ncoEmail, emailAddresses) {
            QContactEmailAddress detail;
            copyDetailData(ncoEmail, detail);
            detail.setAttribute(QContactEmailAddress::AttributeContext, QContactEmailAddress::AttributeContextWork);
            detail.setAttribute(QContactEmailAddress::AttributeSubType, QContactEmailAddress::AttributeSubTypeInternet);
            qcontact.saveDetail(&detail);
        }
        foreach( Live<nfo::FileDataObject> fdo, org->getPhotos() ) //Work avatars
        {
            if( fdo != 0 ) {
                QContactAvatar detail;
                copyDetailData(fdo, detail);
                detail.setAttribute(QContactAvatar::AttributeContext, QContactAvatar::AttributeContextWork);
                qcontact.saveDetail(&detail);
            }
        }
    }

    /*{ //TODO
        QContactAnniversary detail;
        copyDetailData(ncoContact, detail);
        qcontact.saveDetail(&detail);
    }{
        QContactGender detail;
        copyDetailData(ncoContact, detail);
        qcontact.saveDetail(&detail);
    }{
        QContactBirthday detail;
        copyDetailData(ncoContact, detail);
        qcontact.saveDetail(&detail);
    }{
        QContactGuid detail;
        copyDetailData(ncoContact, detail);
        qcontact.saveDetail(&detail);
    }{
        QContactSyncTarget detail;
        copyDetailData(ncoContact, detail);
        qcontact.saveDetail(&detail);
    }{
        QContactUrl detail;
        copyDetailData(ncoContact, detail);
        qcontact.saveDetail(&detail);
    }
    */
    return true;
}

