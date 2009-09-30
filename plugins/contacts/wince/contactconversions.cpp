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
#include <QDebug>

#include "qcontact_p.h"
#include "qcontactgroup_p.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"

#include "qcontactwincebackend_p.h"

/*!
 * Convert from the supplied CEPROPVAL \a val into a QVariant.
 */
static QVariant CEPropValToQVariant(const CEPROPVAL& val)
{
    if (val.propid != PIMPR_INVALID_ID) {
        switch(TypeFromPropID(val.propid)) {
            case CEVT_BOOL:
                return QVariant(val.val.boolVal);
            case CEVT_I2:
                return QVariant(val.val.iVal);
            case CEVT_UI2:
                return QVariant(val.val.uiVal);
            case CEVT_I4:
                return QVariant(val.val.lVal);
            case CEVT_UI4:
            case CEVT_PIM_AUTO_I4:
                return QVariant((quint32)val.val.ulVal);
            case CEVT_R8:
                return QVariant(val.val.dblVal);
            case CEVT_FILETIME:
            {
                // Convert FILETIME to QDateTime
                if (val.val.filetime.dwHighDateTime != 0 || val.val.filetime.dwLowDateTime != 0) {
                    SYSTEMTIME st;
                    if(FileTimeToSystemTime(&val.val.filetime, &st))
                        return QVariant(QDateTime(QDate(st.wYear, st.wMonth, st.wDay), QTime(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds)));
                }
                break; // Fall through to return at bottom
            }
            case CEVT_LPWSTR:
                return QVariant(QString::fromWCharArray(val.val.lpwstr));

            case CEVT_BLOB: // Not used yet
            case CEVT_PIM_STREAM: // Not used yet
                break;
        }
    }
    return QVariant();
}

/*!
 * Convert the supplied QString \a value into a CEPROPVAL with the supplied \a id.
 *
 * The caller is responsible for freeing the LPWSTR within the CEPROPVAL.
 *
 * The datatype of the \a id is not checked.
 */
static CEPROPVAL convertToCEPropVal(const CEPROPID& id, const QString& value)
{
    CEPROPVAL val;
    val.propid = id;
    val.wFlags = 0;
    val.wLenData = 0;
    val.val.lpwstr = wcsdup(value.utf16()); // XXX leaks

    return val;
}

/*!
 * Convert the supplied QDateTime \a value into a CEPROPVAL with the supplied \a id.
 *
 * The datatype of the \a id is not checked.
 */
static CEPROPVAL convertToCEPropVal(const CEPROPID& id, const QDateTime& value)
{
    CEPROPVAL val;
    val.propid = id;
    val.wFlags = 0;
    val.wLenData = 0;

    SYSTEMTIME st;
    st.wDay = value.date().day();
    st.wMonth = value.date().month();
    st.wYear = value.date().year();
    st.wHour = value.time().hour();
    st.wMinute = value.time().minute();
    st.wSecond = value.time().second();
    st.wMilliseconds = value.time().msec();

    SystemTimeToFileTime(&st, &val.val.filetime); // XXX check for error?

    return val;
}

// Structures for mapping stuffs

// Our fields to POOM
// QMap<definition, {QMap<field, poomid>, datatype, maxnumber}>
typedef void (*processContactPoomElement)(const QContactDetail& detail, QVector<CEPROPVAL>& props);

// POOM to us
// something like
// {PIMPR_,PIMPR_} -> function
// Might then need PIMPR -> bag above

struct PoomContactElement;
typedef void (*processPoomContactElement)(const QVariantList& values, QContact& ret);

struct PoomContactElement {
    QList<CEPROPID> poom;
    processPoomContactElement func;
};

static void setIfNotEmpty(QContactDetail& detail, const QString& field, const QString& value)
{
    if (!value.isEmpty())
        detail.setValue(field, value);
}

static void processName(const QVariantList& values, QContact& ret)
{
    QContactName name;
    setIfNotEmpty(name, QContactName::FieldPrefix, values[0].toString());
    setIfNotEmpty(name, QContactName::FieldFirst, values[1].toString());
    setIfNotEmpty(name, QContactName::FieldMiddle, values[2].toString());
    setIfNotEmpty(name, QContactName::FieldLast, values[3].toString());
    setIfNotEmpty(name, QContactName::FieldSuffix, values[4].toString());
    if (!name.isEmpty())
        ret.saveDetail(&name);
}

static void processFileAs(const QVariantList& values, QContact& ret)
{
    ret.setDisplayLabel(values[0].toString());
    // isSynthesized gets fixed up after the whole contact is retrieved
}

static void processAddress(const QString& context, const QVariantList& values, QContact& ret)
{
    QContactAddress address;
    address.setContexts(context);
    setIfNotEmpty(address, QContactAddress::FieldStreet, values[0].toString());
    setIfNotEmpty(address, QContactAddress::FieldPostcode, values[1].toString());
    setIfNotEmpty(address, QContactAddress::FieldLocality, values[2].toString());
    setIfNotEmpty(address, QContactAddress::FieldRegion, values[3].toString());
    setIfNotEmpty(address, QContactAddress::FieldCountry, values[4].toString());
    if (!address.isEmpty())
        ret.saveDetail(&address);
}

static void processHomeAddress(const QVariantList& values, QContact& ret)
{
    processAddress(QContactDetail::ContextHome, values, ret);
}

static void processWorkAddress(const QVariantList& values, QContact& ret)
{
    processAddress(QContactDetail::ContextWork, values, ret);
}

static void processOtherAddress(const QVariantList& values, QContact& ret)
{
    processAddress(QContactDetail::ContextOther, values, ret);
}

static void processEmails(const QVariantList& values, QContact& ret)
{
    // First value is our additional metadata..
    // takes the form of a single character for each email address for the context
    // Since email addresses don't bubble up if you remove one, we don't store the
    // full value. but we could..
    QString meta = values[0].toString();

    // Just create an email address for each value (we're expecting 3)
    for (int j=1; j < values.size(); j++) {
        QVariant v = values.at(j);
        if (!v.isNull()) {
            QContactEmailAddress e;
            e.setValue(QContactEmailAddress::FieldEmailAddress, v);
            QChar m = meta.at(j - 1);
            if (m == 'H')
                e.setContexts(QContactDetail::ContextHome);
            else if (m == 'W')
                e.setContexts(QContactDetail::ContextWork);
            ret.saveDetail(&e);
        }
    }
}

static void processPhones(const QVariantList& values, QContact& ret)
{
    // Just like emails, the first value is our additional metadata
    // metadata for phone numbers is somewhat crazy.
    // We use the following characters:

    //
    // space = default for type
    // H = Home context, no type modifier
    // W = Work context, no type modifier
    // O = Other context
    // N = No context

    // In general, they should only apply to PROPIDs that don't fully specify both
    // (e.g. home_fax is fully specified, mobile is not), but we occasionally run out
    // of places for anonymous numbers and stick them in different places.

    // Indexing is the same as the input list

    QString meta = values[0].toString();

    for (int i=0; i < values.count() - 1; i++) {
        const QVariant& v = values.at(i + 1);
        if (!v.isNull()) {
            QContactPhoneNumber number;
            number.setValue(QContactPhoneNumber::FieldNumber, v);
            QChar m = 0;
            switch(i) {
                case 0: // Business phone
                case 1:
                    m = meta.at(i);
                    if (m ==  ' ')
                        m = 'W';
                    number.setSubTypes(QContactPhoneNumber::SubTypeVoice);
                    break;
                case 2: // Car
                    m = meta.at(i);
                    number.setSubTypes(QContactPhoneNumber::SubTypeCar);
                    break;
                case 3: // Mobile
                    m = meta.at(i);
                    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
                    break;
                case 4: // Home phones
                case 5:
                    m = meta.at(i);
                    if (m ==  ' ')
                        m = 'H';
                    number.setSubTypes(QContactPhoneNumber::SubTypeVoice);
                    break;
                case 6: // Pager
                    m = meta.at(i);
                    number.setSubTypes(QContactPhoneNumber::SubTypePager);
                    break;
                case 7: // Radio telephone (??)
                    m = meta.at(i);
                    number.setSubTypes(QContactPhoneNumber::SubTypeMobile);
                    break;
                case 8: // SIM entry
                    break;
                case 9: // Home fax
                    m = meta.at(i);
                    if (m ==  ' ')
                        m = 'H';
                    number.setSubTypes(QContactPhoneNumber::SubTypeFacsimile);
                    break;
                case 10: // Business fax
                    m = meta.at(i);
                    if (m ==  ' ')
                        m = 'W';
                    number.setSubTypes(QContactPhoneNumber::SubTypeFacsimile);
                    break;
            }

            if (m == 'H')
                number.setContexts(QContactDetail::ContextHome);
            else if (m == 'W')
                number.setContexts(QContactDetail::ContextWork);
            else if (m == 'O')
                number.setContexts(QContactDetail::ContextOther);

            ret.saveDetail(&number);
        }
    }
}

static void processDates(const QVariantList& values, QContact& ret)
{
    // We get anniversary, then birthday
    if (!values[0].toDate().isNull()) {
        QContactAnniversary ann;
        ann.setOriginalDate(values[0].toDate());
        ret.saveDetail(&ann);
    }
    if (!values[1].toDate().isNull()) {
        QContactBirthday bday;
        bday.setDate(values[1].toDate());
        ret.saveDetail(&bday);
    }
}

static void processFamily(const QVariantList& values, QContact& ret)
{
    // We get spouse then children
    if (!values[0].toString().isEmpty()) {
        QContactRelationship spouse;
        spouse.setRelationshipType("Spouse"); // XXX need a constant
        spouse.setRelatedContactLabel(values[0].toString());
        ret.saveDetail(&spouse);
    }
    if (!values[1].toString().isEmpty()) {
        QContactRelationship kids;
        kids.setRelationshipType("Children"); // XXX need a constant
        kids.setRelatedContactLabel(values[1].toString());
        ret.saveDetail(&kids);
    }
}

static void processId(const QVariantList& values, QContact& ret)
{
    ret.setId(values.at(0).toUInt());
}

static void processNickname(const QVariantList& values, QContact& ret)
{
    QContactNickname nick;
    setIfNotEmpty(nick, QContactNickname::FieldNickname, values[0].toString());

    if (!nick.isEmpty())
        ret.saveDetail(&nick);
}

static void processWebpage(const QVariantList& values, QContact& ret)
{
    QContactUrl url;
    setIfNotEmpty(url, QContactUrl::FieldUrl, values[0].toString());

    if (!url.isEmpty())
        ret.saveDetail(&url);
}

static void processOrganisation(const QVariantList& values, QContact& ret)
{
    QContactOrganization org;
    setIfNotEmpty(org, QContactOrganization::FieldName, values[0].toString());

    if (!org.isEmpty())
        ret.saveDetail(&org);
}


static void contactP2QTransforms(CEPROPID phoneMeta, CEPROPID emailMeta, QHash<CEPROPID, PoomContactElement>& prophash, QVector<CEPROPID>& propids)
{
    static QHash<CEPROPID, PoomContactElement> hash;
    static QVector<CEPROPID> ids;

    if (hash.count() == 0) {
        QList<PoomContactElement> list;

        // ID
        PoomContactElement id;
        id.poom << PIMPR_OID;
        id.func = processId;
        list.append(id);

        // Display label
        PoomContactElement fileas;
        fileas.poom << PIMPR_FILEAS;
        fileas.func = processFileAs;
        list.append(fileas);

        // Names
        PoomContactElement name;
        name.poom << PIMPR_TITLE << PIMPR_FIRST_NAME << PIMPR_MIDDLE_NAME << PIMPR_LAST_NAME << PIMPR_SUFFIX;
        name.func = processName;
        list.append(name);

        // Home address
        PoomContactElement homeAddress;
        homeAddress.poom << PIMPR_HOME_ADDRESS_STREET << PIMPR_HOME_ADDRESS_POSTAL_CODE << PIMPR_HOME_ADDRESS_CITY << PIMPR_HOME_ADDRESS_STATE << PIMPR_HOME_ADDRESS_COUNTRY;
        homeAddress.func = processHomeAddress;
        list.append(homeAddress);

        // Work address
        PoomContactElement workAddress;
        workAddress.poom << PIMPR_BUSINESS_ADDRESS_STREET << PIMPR_BUSINESS_ADDRESS_POSTAL_CODE << PIMPR_BUSINESS_ADDRESS_CITY << PIMPR_BUSINESS_ADDRESS_COUNTRY;
        workAddress.func = processWorkAddress;
        list.append(workAddress);

        // Other address
        PoomContactElement otherAddress;
        otherAddress.poom << PIMPR_OTHER_ADDRESS_STREET << PIMPR_OTHER_ADDRESS_POSTAL_CODE << PIMPR_OTHER_ADDRESS_CITY << PIMPR_OTHER_ADDRESS_COUNTRY;
        otherAddress.func = processOtherAddress;
        list.append(otherAddress);

        // Emails
        PoomContactElement emails;
        emails.poom << emailMeta << PIMPR_EMAIL1_ADDRESS << PIMPR_EMAIL2_ADDRESS << PIMPR_EMAIL3_ADDRESS;
        emails.func = processEmails;
        list.append(emails);

        // Phone numbers
        PoomContactElement phones;
        phones.poom << phoneMeta << PIMPR_BUSINESS_TELEPHONE_NUMBER << PIMPR_BUSINESS2_TELEPHONE_NUMBER
            << PIMPR_CAR_TELEPHONE_NUMBER  << PIMPR_MOBILE_TELEPHONE_NUMBER
            << PIMPR_HOME_TELEPHONE_NUMBER << PIMPR_HOME2_TELEPHONE_NUMBER
            << PIMPR_PAGER_NUMBER << PIMPR_RADIO_TELEPHONE_NUMBER
            << PIMPR_SIM_PHONE
            << PIMPR_HOME_FAX_NUMBER << PIMPR_BUSINESS_FAX_NUMBER;
        phones.func = processPhones;
        list.append(phones);

        // Dates
        PoomContactElement dates;
        dates.poom << PIMPR_ANNIVERSARY << PIMPR_BIRTHDAY;
        dates.func = processDates;
        list.append(dates);

        // Spouse and children
        PoomContactElement family;
        family.poom << PIMPR_SPOUSE << PIMPR_CHILDREN;
        family.func = processFamily;
        list.append(family);

        // Nickname
        PoomContactElement nick;
        nick.poom << PIMPR_NICKNAME;
        nick.func = processNickname;
        list.append(nick);

        // Webpage
        PoomContactElement web;
        web.poom << PIMPR_WEB_PAGE;
        web.func = processWebpage;
        list.append(web);

        // Organisation
        PoomContactElement org;
        org.poom << PIMPR_COMPANY_NAME;
        org.func = processOrganisation;
        list.append(org);

        // Unhandled:
        //
        //  PIMPR_ACCOUNT_NAME
        //  PIMPR_CUSTOMERID
        //  PIMPR_GOVERNMENTID
        //
        //  PIMPR_MANAGER
        //  PIMPR_ASSISTANT_NAME
        //  PIMPR_ASSISTANT_TELEPHONE_NUMBER
        //  PIMPR_JOB_TITLE
        //  PIMPR_DEPARTMENT
        //  PIMPR_OFFICE_LOCATION
        //  PIMPR_COMPANY_TELEPHONE_NUMBER
        //  PIMPR_YOMI_COMPANY
        //
        //  PIMPR_DISPLAY_NAME (vs. PIMPR_FILEAS)
        //
        //  PIMPR_IM[123]_ADDRESS
        //
        //  PIMPR_PICTURE
        //  PIMPR_RINGTONE

        //  PIMPR_YOMI_FILEAS
        //  PIMPR_YOMI_FIRSTNAME
        //  PIMPR_YOMI_LASTNAME

        // Now, build the hash
        foreach(const PoomContactElement& e, list) {
            foreach(CEPROPID id, e.poom) {
                ids.append(id);
                hash.insert(id, e);
            }
        }
    }
    propids = ids;
    prophash = hash;
}

static void addIfNotEmpty(const CEPROPID& id, const QString& value, QVector<CEPROPVAL>& props)
{
    if (!value.isEmpty())
        props.append(convertToCEPropVal(id, value));
}

static void processQName(const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    addIfNotEmpty(PIMPR_TITLE, detail.value(QContactName::FieldPrefix), props);
    addIfNotEmpty(PIMPR_FIRST_NAME, detail.value(QContactName::FieldFirst), props);
    addIfNotEmpty(PIMPR_MIDDLE_NAME, detail.value(QContactName::FieldMiddle), props);
    addIfNotEmpty(PIMPR_LAST_NAME, detail.value(QContactName::FieldLast), props);
    addIfNotEmpty(PIMPR_SUFFIX, detail.value(QContactName::FieldSuffix), props);
}

static void processQLabel(const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    props.append(convertToCEPropVal(PIMPR_FILEAS, detail.value(QContactDisplayLabel::FieldLabel)));
}

static void processQBirthday(const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    if (detail.variantValue(QContactBirthday::FieldBirthday).isValid())
        props.append(convertToCEPropVal(PIMPR_BIRTHDAY, detail.variantValue(QContactBirthday::FieldBirthday).toDateTime()));
}

static void processQAnniversary(const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    if (detail.variantValue(QContactAnniversary::FieldOriginalDate).isValid())
        props.append(convertToCEPropVal(PIMPR_ANNIVERSARY, detail.variantValue(QContactAnniversary::FieldOriginalDate).toDateTime()));
}

static void processQNickname(const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    addIfNotEmpty(PIMPR_NICKNAME, detail.value(QContactNickname::FieldNickname), props);
}

static void processQOrganisation(const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    QContactOrganization org(detail);

    addIfNotEmpty(PIMPR_COMPANY_NAME, org.name(), props);
}

static void processQWebpage(const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    QContactUrl url(detail);

    addIfNotEmpty(PIMPR_WEB_PAGE, url.url(), props);
}

/* Bulk setters */
static void processQPhones(const QList<QContactPhoneNumber>& nums, CEPROPID metaId, QVector<CEPROPVAL>& props)
{
    QList<CEPROPID> availableIds;
    availableIds << PIMPR_BUSINESS_TELEPHONE_NUMBER << PIMPR_BUSINESS2_TELEPHONE_NUMBER
        << PIMPR_CAR_TELEPHONE_NUMBER  << PIMPR_MOBILE_TELEPHONE_NUMBER
        << PIMPR_HOME_TELEPHONE_NUMBER << PIMPR_HOME2_TELEPHONE_NUMBER
        << PIMPR_PAGER_NUMBER << PIMPR_RADIO_TELEPHONE_NUMBER
        << PIMPR_SIM_PHONE
        << PIMPR_HOME_FAX_NUMBER << PIMPR_BUSINESS_FAX_NUMBER;

    QString meta;
    meta.fill(' ', availableIds.count()); // init with zero metadata

    QList<QContactPhoneNumber> numbers = nums;
    QList<QContactPhoneNumber> deferred;
    for(int j = 0; j < numbers.count() + deferred.count(); /* no j++ */) {
        const QContactPhoneNumber& number = j < numbers.count() ? numbers.at(j) : deferred.at(j - numbers.count());

        CEPROPID id = PIMPR_INVALID_ID;

        // Map from our attributes to ids
        if (number.subTypes().contains(QContactPhoneNumber::SubTypeCar))
            id = PIMPR_CAR_TELEPHONE_NUMBER;
        else if (number.subTypes().contains(QContactPhoneNumber::SubTypeMobile))
            id = PIMPR_MOBILE_TELEPHONE_NUMBER;
        else if (number.subTypes().contains(QContactPhoneNumber::SubTypeFacsimile)) {
            if (number.contexts().contains(QContactDetail::ContextHome))
                id = PIMPR_HOME_FAX_NUMBER;
            else if (number.contexts().contains(QContactDetail::ContextWork))
                id = PIMPR_BUSINESS_FAX_NUMBER;
        } else if (number.subTypes().contains(QContactPhoneNumber::SubTypeVoice)) {
            if (number.contexts().contains(QContactDetail::ContextHome))
                id = availableIds.contains(PIMPR_HOME_TELEPHONE_NUMBER) ? PIMPR_HOME_TELEPHONE_NUMBER : PIMPR_HOME2_TELEPHONE_NUMBER;
            else if (number.contexts().contains(QContactDetail::ContextWork))
                id = availableIds.contains(PIMPR_BUSINESS_TELEPHONE_NUMBER) ? PIMPR_BUSINESS_TELEPHONE_NUMBER : PIMPR_BUSINESS2_TELEPHONE_NUMBER;
        } else if (number.subTypes().contains(QContactPhoneNumber::SubTypePager))
            id = PIMPR_PAGER_NUMBER;
        else if (number.subTypes().isEmpty()) {
            // We do this anonymous number at the end, if we haven't already deferred it
            if (j < numbers.count()) {
                deferred.append(numbers.takeAt(j));
                continue;
            }

            // Well, deferred it once, process it now
            // XXX it might make more sense to reorder these to better match contexts..
            if (availableIds.contains(PIMPR_HOME_TELEPHONE_NUMBER))
                id = PIMPR_HOME_TELEPHONE_NUMBER;
            else if (availableIds.contains(PIMPR_HOME2_TELEPHONE_NUMBER))
                id = PIMPR_HOME2_TELEPHONE_NUMBER;
            else if (availableIds.contains(PIMPR_BUSINESS_TELEPHONE_NUMBER))
                id = PIMPR_BUSINESS_TELEPHONE_NUMBER;
            else if (availableIds.contains(PIMPR_BUSINESS2_TELEPHONE_NUMBER))
                id = PIMPR_BUSINESS2_TELEPHONE_NUMBER;
        }

        // get ready for the next
        j++;

        // Now see if we still have a slot
        if (id == PIMPR_INVALID_ID) {
            qDebug() << "Didn't match source detail:" << number.contexts() << number.subTypes();
        } else {
            if (!availableIds.contains(id)) {
                qDebug() << "Too many phone numbers, store this some other way:" << number.contexts() << number.subTypes();
            } else {
                // Set the meta information
                if (number.contexts().contains(QContactDetail::ContextHome))
                    meta[availableIds.indexOf(id)] = 'H';
                else if (number.contexts().contains(QContactDetail::ContextWork))
                    meta[availableIds.indexOf(id)] = 'W';
                else if (number.contexts().contains(QContactDetail::ContextOther))
                    meta[availableIds.indexOf(id)] = 'O';
                else if (number.contexts().count() == 0)
                    meta[availableIds.indexOf(id)] = 'N';

                props.append(convertToCEPropVal(id, number.number()));
                availableIds.replace(availableIds.indexOf(id), PIMPR_INVALID_ID); // not available any more
            }
        }
    }

    props.append(convertToCEPropVal(metaId, meta));
}

static void processQEmails(const QList<QContactEmailAddress>& emails, CEPROPID metaId, QVector<CEPROPVAL>& props)
{
    QList<CEPROPID> availableIds;
    availableIds << PIMPR_EMAIL1_ADDRESS << PIMPR_EMAIL2_ADDRESS << PIMPR_EMAIL3_ADDRESS;

    QString meta;

    foreach(const QContactEmailAddress& email, emails) {
        CEPROPID id = availableIds.takeFirst();
        if (id != 0) {
            if (email.contexts().contains(QContactDetail::ContextHome))
                meta += "H";
            else if (email.contexts().contains(QContactDetail::ContextWork))
                meta += "W";
            else
                meta += " ";
            props.append(convertToCEPropVal(id, email.emailAddress()));
        } else {
            qDebug() << "Too many email addresses";
            break;
        }
    }
    props.append(convertToCEPropVal(metaId, meta));
}

static void processQAddresses(const QList<QContactAddress>& addresses, QVector<CEPROPVAL>& props)
{
    // We do 2 passes - first try to match addresses to contexts
    // then use whatever's left for the rest

    bool homeAvailable = true;
    bool workAvailable = true;
    bool otherAvailable = true;

    QList<QContactAddress> deferred;
    foreach(const QContactAddress& address, addresses) {
        if (address.contexts().contains(QContactDetail::ContextHome)) {
            if (homeAvailable) {
                addIfNotEmpty(PIMPR_HOME_ADDRESS_CITY, address.locality(), props);
                addIfNotEmpty(PIMPR_HOME_ADDRESS_COUNTRY, address.country(), props);
                addIfNotEmpty(PIMPR_HOME_ADDRESS_POSTAL_CODE, address.postcode(), props);
                addIfNotEmpty(PIMPR_HOME_ADDRESS_STATE, address.region(), props);
                addIfNotEmpty(PIMPR_HOME_ADDRESS_STREET, address.street(), props);
                homeAvailable = false;
            } else {
                deferred.append(address);
            }
        } else if (address.contexts().contains(QContactDetail::ContextWork)) {
            if (workAvailable) {
                addIfNotEmpty(PIMPR_BUSINESS_ADDRESS_CITY, address.locality(), props);
                addIfNotEmpty(PIMPR_BUSINESS_ADDRESS_COUNTRY, address.country(), props);
                addIfNotEmpty(PIMPR_BUSINESS_ADDRESS_POSTAL_CODE, address.postcode(), props);
                addIfNotEmpty(PIMPR_BUSINESS_ADDRESS_STATE, address.region(), props);
                addIfNotEmpty(PIMPR_BUSINESS_ADDRESS_STREET, address.street(), props);
                workAvailable = false;
            } else {
                deferred.append(address);
            }
        } else {
            if (otherAvailable) {
                addIfNotEmpty(PIMPR_OTHER_ADDRESS_CITY, address.locality(), props);
                addIfNotEmpty(PIMPR_OTHER_ADDRESS_COUNTRY, address.country(), props);
                addIfNotEmpty(PIMPR_OTHER_ADDRESS_POSTAL_CODE, address.postcode(), props);
                addIfNotEmpty(PIMPR_OTHER_ADDRESS_STATE, address.region(), props);
                addIfNotEmpty(PIMPR_OTHER_ADDRESS_STREET, address.street(), props);
                otherAvailable = false;
            } else {
                deferred.append(address);
            }
        }
    }

    // Now the deferred ones
    while(deferred.count() > 0) {
        // If there's nothing left..
        if (!homeAvailable && !workAvailable && !otherAvailable) {
            qDebug() << "Too many addresses";
            return;
        }

        QContactAddress address = deferred.takeFirst();

        // Well, first choice is to use other
        // but we really need to save the contexts XXX
        if (otherAvailable) {
            addIfNotEmpty(PIMPR_OTHER_ADDRESS_CITY, address.locality(), props);
            addIfNotEmpty(PIMPR_OTHER_ADDRESS_COUNTRY, address.country(), props);
            addIfNotEmpty(PIMPR_OTHER_ADDRESS_POSTAL_CODE, address.postcode(), props);
            addIfNotEmpty(PIMPR_OTHER_ADDRESS_STATE, address.region(), props);
            addIfNotEmpty(PIMPR_OTHER_ADDRESS_STREET, address.street(), props);
            otherAvailable = false;
        } else if (workAvailable) {
            addIfNotEmpty(PIMPR_BUSINESS_ADDRESS_CITY, address.locality(), props);
            addIfNotEmpty(PIMPR_BUSINESS_ADDRESS_COUNTRY, address.country(), props);
            addIfNotEmpty(PIMPR_BUSINESS_ADDRESS_POSTAL_CODE, address.postcode(), props);
            addIfNotEmpty(PIMPR_BUSINESS_ADDRESS_STATE, address.region(), props);
            addIfNotEmpty(PIMPR_BUSINESS_ADDRESS_STREET, address.street(), props);
            workAvailable = false;
        } else {
            addIfNotEmpty(PIMPR_HOME_ADDRESS_CITY, address.locality(), props);
            addIfNotEmpty(PIMPR_HOME_ADDRESS_COUNTRY, address.country(), props);
            addIfNotEmpty(PIMPR_HOME_ADDRESS_POSTAL_CODE, address.postcode(), props);
            addIfNotEmpty(PIMPR_HOME_ADDRESS_STATE, address.region(), props);
            addIfNotEmpty(PIMPR_HOME_ADDRESS_STREET, address.street(), props);
            homeAvailable = false;
        }
    }
}

static void processQRelationships(const QList<QContactRelationship>& relationships, QVector<CEPROPVAL>& props)
{
    foreach(const QContactRelationship& rel, relationships) {
        if (rel.relationshipType() == "Spouse") { // XXX constant
            props.append(convertToCEPropVal(PIMPR_SPOUSE, rel.relatedContactLabel()));
        } else if (rel.relationshipType() == "Children") { // XXX constant
            props.append(convertToCEPropVal(PIMPR_CHILDREN, rel.relatedContactLabel()));
        } else {
            qDebug() << "unsupported relationship:" << rel.relationshipType();
        }
    }
}

static void contactQ2PTransforms(QHash<QString, processContactPoomElement>& ret)
{
    static QHash<QString, processContactPoomElement> hash;
    if (hash.count() == 0) {
        hash.insert(QContactName::DefinitionName, processQName);
        hash.insert(QContactDisplayLabel::DefinitionName, processQLabel);
        hash.insert(QContactAnniversary::DefinitionName, processQAnniversary);
        hash.insert(QContactBirthday::DefinitionName, processQBirthday);
        hash.insert(QContactNickname::DefinitionName, processQNickname);
        hash.insert(QContactOrganization::DefinitionName, processQOrganisation);
        hash.insert(QContactUrl::DefinitionName, processQWebpage);
    }
    ret = hash;
}

QContact QContactWinCEEngine::convertToQContact(IItem *contact) const
{
    QContact ret;

    // Several choices for converting a contact:
    // - IContact and lots of get_XXXXX
    // - IItem and one or more GetProps calls
    // - etc
    // No idea which one is faster.. needs a benchmark
    // use IItem for now, since it's a bit more generic and we can
    // use PIMPR_* rather than the different explicit function calls

    unsigned long cbSize = 0;

    // Map information
    QHash<CEPROPID, PoomContactElement> hash;
    QVector<CEPROPID> props;

    // Get our mapping tables
    contactP2QTransforms(d->m_phonemeta, d->m_emailmeta, hash, props);

    CEPROPVAL *propvals = 0;
    HRESULT hr = contact->GetProps(props.constData(), CEDB_ALLOWREALLOC, props.count(), &propvals, &cbSize, GetProcessHeap());

    if (SUCCEEDED(hr)) {
        // Loop over each property, adding to a map
        // since there should only be juicy data retrieved
        QHash<CEPROPID, QVariant> valueHash;
        for (int i=0; i < props.count(); i++) {
            QVariant v = CEPropValToQVariant(propvals[i]);
            if (!v.isNull()) {
                valueHash.insert(propvals[i].propid, v);
            }
        }

        // Now process our map transform elements, removing things as we go
        while(valueHash.count() > 0) {
            CEPROPID id = valueHash.constBegin().key();

            const PoomContactElement& qmap = hash.value(id);
            if (qmap.func) {
                // We need to create values for each of qmap.poom
                // (which means we need to find each value of qmap.poom)
                // and we remove them as we go
                QVariantList vl;
                foreach(const CEPROPID& id, qmap.poom) {
                    vl << valueHash.take(id);
                }
                qmap.func(vl, ret);
            } else {
                qDebug() << "Didn't match property for id:" << QString::number(id, 16);
                // Remove the ignored value so we don't infinite loop
                valueHash.take(id);
            }
        }

        HeapFree(GetProcessHeap(), 0, propvals);
    }

    // Now, we need to check whether we got a display label
    QContactDisplayLabel label = ret.displayLabel();
    QContactManager::Error error;
    QString synth = synthesiseDisplayLabel(ret, error);
    if (label.label().isEmpty() || label.label() == synth) {
        label.setLabel(synth);
        label.setSynthesised(true);
        ret.saveDetail(&label);
    }

    return ret;
}

bool QContactWinCEEngine::convertFromQContact(const QContact& contact, IItem* item, QContactManager::Error &error) const
{
    // We have to create a whole bunch of CEPROPVALs for each detail
    // This is slightly hampered by the limited storage slots

    QList<QContactDetail> details = contact.details();
    QHash<QString, processContactPoomElement> transforms;

    contactQ2PTransforms(transforms);
    processContactPoomElement func;

    QVector<CEPROPVAL> props;

    foreach (const QContactDetail& detail, details) {
        func = transforms.value(detail.definitionName());
        if (func) {
            func(detail, props);
        }
    }

    // Now the bulk transforms
    processQPhones(contact.details<QContactPhoneNumber>(), d->m_phonemeta, props);
    processQEmails(contact.details<QContactEmailAddress>(), d->m_emailmeta, props);
    processQAddresses(contact.details<QContactAddress>(), props);
    //processQRelationships(contact.details<QContactRelationship>(), props);

    // Now set it
    HRESULT hr = item->SetProps(0, props.count(), props.data());
    if (SUCCEEDED(hr)) {
        return true;
    } else {
        qDebug() << QString("Failed to set props: %1 (%2)").arg(hr, 0, 16).arg(HRESULT_CODE(hr), 0, 16);
        error = QContactManager::UnspecifiedError;
    }

    return false;
}

