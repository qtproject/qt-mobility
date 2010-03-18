/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QDebug>
#include <QByteArray>
#include <QBuffer>
#include <QUrl>
#include <QPixmap>
#include "qcontactmanager.h"
#include "qtcontacts.h"
#include "qcontactwincebackend_p.h"

QTM_USE_NAMESPACE

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



class WcsdupHelper {
public:
    WcsdupHelper() {}
    ~WcsdupHelper() {
        clear();
    }
    LPWSTR dup(const ushort* str) {
        LPWSTR newStr = wcsdup(str);
        m_list.push_back(newStr);
        return newStr;
    }
    void clear() {
        foreach (const LPWSTR& str, m_list) {
            free(str);
        }
        m_list.clear();
    }
private:
    QList<LPWSTR> m_list; 
};

static WcsdupHelper wcsdupHelper;

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
    if (!value.isEmpty()) {
        val.val.lpwstr = wcsdupHelper.dup(value.utf16());
    } else {
        val.val.lpwstr = 0;
    }

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
typedef bool (*processContactPoomElement)(const QContactWinCEEngine* e, IItem* contact, const QContactDetail& detail, QVector<CEPROPVAL>& props);

// POOM to us
// something like
// {PIMPR_,PIMPR_} -> function
// Might then need PIMPR -> bag above

struct PoomContactElement;
typedef void (*processPoomContactElement)(const QContactWinCEEngine* e, IItem* contact, const QVariantList& values, QContact& ret);

struct PoomContactElement {
    QList<CEPROPID> poom;
    processPoomContactElement func;
};

static void setIfNotEmpty(QContactDetail& detail, const QString& field, const QString& value)
{
    if (!value.isEmpty())
        detail.setValue(field, value);
}

static void processName(const QContactWinCEEngine* /*engine*/, IItem* /*contact*/, const QVariantList& values, QContact& ret)
{
    QContactName name;
    setIfNotEmpty(name, QContactName::FieldPrefix, values[0].toString());
    setIfNotEmpty(name, QContactName::FieldFirst, values[1].toString());
    setIfNotEmpty(name, QContactName::FieldMiddle, values[2].toString());
    setIfNotEmpty(name, QContactName::FieldLast, values[3].toString());
    setIfNotEmpty(name, QContactName::FieldSuffix, values[4].toString());
    setIfNotEmpty(name, QContactName::FieldCustomLabel, values[5].toString());
    if (!name.isEmpty())
        ret.saveDetail(&name);
}

static bool GetStreamSize(IStream* pStream, ULONG* pulSize)
{
    HRESULT hr;
    LARGE_INTEGER  li = {0};
    ULARGE_INTEGER uliZero = {0};
    ULARGE_INTEGER uli;

    if (pStream != NULL && pulSize != NULL) {
        hr = pStream->Seek(li, STREAM_SEEK_END, &uli);
        if (SUCCEEDED(hr)) {
            *pulSize = uli.LowPart;
            hr = pStream->Seek(li, STREAM_SEEK_SET, &uliZero);
            return SUCCEEDED(hr);
        }
    }
    return false;
}

static bool loadAvatarData(IItem* contact, QByteArray* data)
{
    HRESULT   hr;
    SimpleComPointer<IStream>  pStream = NULL;
    ULONG     ulSize;
    // Extract the picture from the contact
    hr = contact->OpenProperty(PIMPR_PICTURE, GENERIC_READ, &pStream);
    if (FAILED(hr))
        return false;

    hr = GetStreamSize(pStream, &ulSize);
    if (FAILED(hr))
        return false;

    // In some cases, the property may exist even if there is no picture.
    // Make sure we can access the stream and don't have a 0 byte stream
    if (ulSize > 0 && pStream != NULL) {

        ULONG     ulSize, readSize;
        if (!GetStreamSize(pStream, &ulSize))
            return false;

        // Prepares the data buffer
        data->resize(ulSize);
        
        // Read all into the data buffer until reach the end of stream
        readSize = 0;
        char* p = data->data();
        while(ulSize && SUCCEEDED(hr)) {
            p += readSize;
            hr = pStream->Read(p, ulSize, &readSize);
            ulSize -= readSize;
        }
        if (FAILED(hr))
            return false;
            
    }
    return true;
}

static bool saveAvatarData(IItem* contact, const QByteArray& data)
{
    HRESULT   hr;
    SimpleComPointer<IStream>  pStream = NULL;

    hr = contact->OpenProperty(PIMPR_PICTURE, GENERIC_WRITE, &pStream);
    if (FAILED(hr))
        return false;

    ULONG     ulWrittenSize;
    pStream->Write(data.data(), data.size(), &ulWrittenSize);

    if (FAILED(hr))
        return false;

    hr = pStream->Commit(0);

    if (FAILED(hr))
        return false;

    hr = contact->Save();

    if (FAILED(hr))
        return false;
        
    return true;
}



static void processAvatar(const QContactWinCEEngine* /*engine*/, IItem* contact, const QVariantList& values, QContact& ret)
{
    QContactAvatar avatar;

    QByteArray data;
    if (loadAvatarData(contact, &data)) {
        if (!data.isEmpty()) {
            QPixmap pixmap;
            pixmap.loadFromData(data, "PNG");
            avatar.setPixmap(pixmap);
        }
    }

    if (values[0].toString().isEmpty()) {
        if (!data.isEmpty()) {
            QUrl url(QUrl::fromEncoded(data.toPercentEncoding()));
            url.setScheme("data");
            avatar.setAvatar(url.toString());
        }
    } else {
        avatar.setAvatar(values[0].toString());
    }

    setIfNotEmpty(avatar, QContactAvatar::FieldSubType, values[1].toString());
    
        
    if (!avatar.isEmpty())
        ret.saveDetail(&avatar);
}

static void processAddress(const QContactWinCEEngine*, const QString& context, const QVariantList& values, QContact& ret)
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

static void processHomeAddress(const QContactWinCEEngine* e, IItem* /*contact*/, const QVariantList& values, QContact& ret)
{
    processAddress(e, QContactDetail::ContextHome, values, ret);
}

static void processWorkAddress(const QContactWinCEEngine* e, IItem* /*contact*/, const QVariantList& values, QContact& ret)
{
    processAddress(e, QContactDetail::ContextWork, values, ret);
}

static void processOtherAddress(const QContactWinCEEngine* e, IItem* /*contact*/, const QVariantList& values, QContact& ret)
{
    processAddress(e, QContactDetail::ContextOther, values, ret);
}

static void processEmails(const QContactWinCEEngine*, IItem* /*contact*/, const QVariantList& values, QContact& ret)
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

static void processPhones(const QContactWinCEEngine*, IItem* /*contact*/, const QVariantList& values, QContact& ret)
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

static void processDates(const QContactWinCEEngine*, IItem* /*contact*/, const QVariantList& values, QContact& ret)
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

static void processId(const QContactWinCEEngine* e, IItem* /*contact*/, const QVariantList& values, QContact& ret)
{
    QContactId id;
    id.setLocalId(values.at(0).toUInt());
    id.setManagerUri(e->managerUri());
    ret.setId(id);
}

static void processNickname(const QContactWinCEEngine*, IItem* /*contact*/, const QVariantList& values, QContact& ret)
{
    QContactNickname nick;
    setIfNotEmpty(nick, QContactNickname::FieldNickname, values[0].toString());

    if (!nick.isEmpty())
        ret.saveDetail(&nick);
}

static void processWebpage(const QContactWinCEEngine*, IItem* /*contact*/, const QVariantList& values, QContact& ret)
{
    QContactUrl url;
    setIfNotEmpty(url, QContactUrl::FieldUrl, values[0].toString());

    if (!url.isEmpty())
        ret.saveDetail(&url);
}

static void processOrganisation(const QContactWinCEEngine*, IItem* /*contact*/, const QVariantList& values, QContact& ret)
{
    QContactOrganization org;
    setIfNotEmpty(org, QContactOrganization::FieldName, values[0].toString());
    setIfNotEmpty(org, QContactOrganization::FieldDepartment, values[1].toString());
    setIfNotEmpty(org, QContactOrganization::FieldLocation, values[2].toString());
    setIfNotEmpty(org, QContactOrganization::FieldTitle, values[3].toString());
    setIfNotEmpty(org, QContactOrganization::FieldAssistantName, values[4].toString());

    if (!org.isEmpty())
        ret.saveDetail(&org);
}

static void processFamily(const QContactWinCEEngine*, IItem* /*contact*/, const QVariantList& values, QContact& ret)
{
    QContactFamily family;
    setIfNotEmpty(family, QContactFamily::FieldSpouse, values[0].toString());
    setIfNotEmpty(family, QContactFamily::FieldChildren, values[1].toString());

    if (!family.isEmpty())
        ret.saveDetail(&family);
}

static void contactP2QTransforms(CEPROPID phoneMeta, CEPROPID emailMeta, CEPROPID avatarMeta, CEPROPID avatarTypeMeta, QHash<CEPROPID, PoomContactElement>& prophash, QVector<CEPROPID>& propids)
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

        // Names
        PoomContactElement name;
        name.poom << PIMPR_TITLE << PIMPR_FIRST_NAME << PIMPR_MIDDLE_NAME << PIMPR_LAST_NAME << PIMPR_SUFFIX << PIMPR_FILEAS;
        name.func = processName;
        list.append(name);

        // Home address
        PoomContactElement homeAddress;
        homeAddress.poom << PIMPR_HOME_ADDRESS_STREET << PIMPR_HOME_ADDRESS_POSTAL_CODE << PIMPR_HOME_ADDRESS_CITY << PIMPR_HOME_ADDRESS_STATE << PIMPR_HOME_ADDRESS_COUNTRY;
        homeAddress.func = processHomeAddress;
        list.append(homeAddress);

        // Work address
        PoomContactElement workAddress;
        workAddress.poom << PIMPR_BUSINESS_ADDRESS_STREET << PIMPR_BUSINESS_ADDRESS_POSTAL_CODE << PIMPR_BUSINESS_ADDRESS_CITY << PIMPR_BUSINESS_ADDRESS_STATE << PIMPR_BUSINESS_ADDRESS_COUNTRY;
        workAddress.func = processWorkAddress;
        list.append(workAddress);

        // Other address
        PoomContactElement otherAddress;
        otherAddress.poom << PIMPR_OTHER_ADDRESS_STREET << PIMPR_OTHER_ADDRESS_POSTAL_CODE << PIMPR_OTHER_ADDRESS_CITY << PIMPR_OTHER_ADDRESS_STATE << PIMPR_OTHER_ADDRESS_COUNTRY;
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
        org.poom << PIMPR_COMPANY_NAME << PIMPR_DEPARTMENT << PIMPR_OFFICE_LOCATION << PIMPR_JOB_TITLE << PIMPR_ASSISTANT_NAME;
        org.func = processOrganisation;
        list.append(org);
        
        // Family
        PoomContactElement family;
        family.poom << PIMPR_SPOUSE <<  PIMPR_CHILDREN;
        family.func = processFamily;
        list.append(family);

        // Avatar
        PoomContactElement avatar;
        avatar.poom << avatarMeta << avatarTypeMeta; //PIMPR_PICTURE need to be handled inside the processAvatar() function separately.
        avatar.func = processAvatar;
        list.append(avatar);


        // XXX Unhandled:
        //
        //  PIMPR_ACCOUNT_NAME
        //  PIMPR_CUSTOMERID
        //  PIMPR_GOVERNMENTID
        //
        //  PIMPR_DISPLAY_NAME
        //
        //  PIMPR_MANAGER
        //  PIMPR_ASSISTANT_TELEPHONE_NUMBER
        //  PIMPR_COMPANY_TELEPHONE_NUMBER
        //  PIMPR_YOMI_COMPANY
        //
        //
        //  PIMPR_IM[123]_ADDRESS
        //
        //  PIMPR_RINGTONE

        //  PIMPR_YOMI_FILEAS
        //  PIMPR_YOMI_FIRSTNAME
        //  PIMPR_YOMI_LASTNAME

        // Now, build the hash
        foreach(const PoomContactElement& e, list) {
            foreach(const CEPROPID& id, e.poom) {
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

static bool processQName(const QContactWinCEEngine*, IItem* /*contact*/, const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    addIfNotEmpty(PIMPR_TITLE, detail.value(QContactName::FieldPrefix), props);
    addIfNotEmpty(PIMPR_FIRST_NAME, detail.value(QContactName::FieldFirst), props);
    addIfNotEmpty(PIMPR_MIDDLE_NAME, detail.value(QContactName::FieldMiddle), props);
    addIfNotEmpty(PIMPR_LAST_NAME, detail.value(QContactName::FieldLast), props);
    addIfNotEmpty(PIMPR_SUFFIX, detail.value(QContactName::FieldSuffix), props);
    addIfNotEmpty(PIMPR_FILEAS, detail.value(QContactName::FieldCustomLabel), props);
    return true;
}

static bool processQAvatar(const QContactWinCEEngine* engine, IItem* contact, const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    QString avatarData = detail.value(QContactAvatar::FieldAvatar);
    QPixmap avatarPixmap = detail.value<QPixmap>(QContactAvatar::FieldAvatarPixmap);

    addIfNotEmpty(engine->metaAvatarType(), detail.value(QContactAvatar::FieldSubType), props);
    addIfNotEmpty(engine->metaAvatar(), avatarData, props);

    if (!avatarPixmap.isNull()) {
        QByteArray data;
        QBuffer buffer(&data);
        buffer.open(QIODevice::WriteOnly);
        if (!avatarPixmap.save(&buffer, "PNG") || !saveAvatarData(contact, data))
            return false;
    }
    return true;
}

static bool processQFamily(const QContactWinCEEngine*, IItem* /*contact*/, const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    addIfNotEmpty(PIMPR_SPOUSE, detail.value(QContactFamily::FieldSpouse), props);
    addIfNotEmpty(PIMPR_CHILDREN, detail.value(QContactFamily::FieldChildren), props);
    return true;
}

static bool validateDate(const QVariant& val)
{
    /*POOM only support date from 1900 to 2999: http://msdn.microsoft.com/en-us/library/aa910841.aspx */
    QDate date = val.toDate();
    return date.year() >= 1900 && date.year() <= 2999;
}

static bool processQBirthday(const QContactWinCEEngine*, IItem* /*contact*/, const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    if (detail.variantValue(QContactBirthday::FieldBirthday).isValid()) {
        if (!validateDate(detail.variantValue(QContactBirthday::FieldBirthday)))
            return false;
        props.append(convertToCEPropVal(PIMPR_BIRTHDAY, detail.variantValue(QContactBirthday::FieldBirthday).toDateTime()));
    }
    return true;
}

static bool processQAnniversary(const QContactWinCEEngine*, IItem* /*contact*/, const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    if (detail.variantValue(QContactAnniversary::FieldOriginalDate).isValid()) {
        if (!validateDate(detail.variantValue(QContactAnniversary::FieldOriginalDate)))
            return false;
        props.append(convertToCEPropVal(PIMPR_ANNIVERSARY, detail.variantValue(QContactAnniversary::FieldOriginalDate).toDateTime()));
    }
    return true;
}

static bool processQNickname(const QContactWinCEEngine*, IItem* /*contact*/, const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    addIfNotEmpty(PIMPR_NICKNAME, detail.value(QContactNickname::FieldNickname), props);
    return true;
}

static bool processQOrganisation(const QContactWinCEEngine*, IItem* /*contact*/, const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    QContactOrganization org(detail);

    addIfNotEmpty(PIMPR_COMPANY_NAME, org.name(), props);
    if (!org.department().isEmpty())
        addIfNotEmpty(PIMPR_DEPARTMENT, org.department().at(0), props);
    addIfNotEmpty(PIMPR_OFFICE_LOCATION, org.location(), props);
    addIfNotEmpty(PIMPR_JOB_TITLE, org.title(), props);
    addIfNotEmpty(PIMPR_ASSISTANT_NAME, org.assistantName(), props);
    return true;
}

static bool processQWebpage(const QContactWinCEEngine*, IItem* /*contact*/, const QContactDetail& detail, QVector<CEPROPVAL>& props)
{
    QContactUrl url(detail);

    addIfNotEmpty(PIMPR_WEB_PAGE, url.url(), props);
    return true;
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
                meta += 'H';
            else if (email.contexts().contains(QContactDetail::ContextWork))
                meta += 'W';
            else
                meta += ' ';
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

static void contactQ2PTransforms(QHash<QString, processContactPoomElement>& ret)
{
    static QHash<QString, processContactPoomElement> hash;
    if (hash.count() == 0) {
        hash.insert(QContactName::DefinitionName, processQName);
        hash.insert(QContactAnniversary::DefinitionName, processQAnniversary);
        hash.insert(QContactBirthday::DefinitionName, processQBirthday);
        hash.insert(QContactNickname::DefinitionName, processQNickname);
        hash.insert(QContactOrganization::DefinitionName, processQOrganisation);
        hash.insert(QContactUrl::DefinitionName, processQWebpage);
        hash.insert(QContactFamily::DefinitionName, processQFamily);
        hash.insert(QContactAvatar::DefinitionName, processQAvatar);
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
    contactP2QTransforms(d->m_phonemeta, d->m_emailmeta, d->m_avatartypemeta, d->m_avatarmeta, hash, props);

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
                qmap.func(this, contact, vl, ret);
            } else {
                qDebug() << "Didn't match property for id:" << QString::number(id, 16);
                // Remove the ignored value so we don't infinite loop
                valueHash.take(id);
            }
        }
        HeapFree(GetProcessHeap(), 0, propvals);
    }

    // Synthesize the display label.
    QContactManager::Error error;
    QString synth = synthesizedDisplayLabel(ret, error);
    ret = setContactDisplayLabel(synth, ret);

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
            if (!func(this, item, detail, props)) {
                error = QContactManager::InvalidDetailError;
                return false;
            }
        }
    }

    // Now the bulk transforms
    processQPhones(contact.details<QContactPhoneNumber>(), d->m_phonemeta, props);
    processQEmails(contact.details<QContactEmailAddress>(), d->m_emailmeta, props);
    processQAddresses(contact.details<QContactAddress>(), props);
    
    // Now set it
    HRESULT hr = item->SetProps(0, props.count(), props.data());
    if (FAILED(hr)) {
        qWarning() << QString("Failed to set props: %1 (%2)").arg(hr, 0, 16).arg(HRESULT_CODE(hr), 0, 16);
        error = QContactManager::UnspecifiedError;
    }
    
    wcsdupHelper.clear();

    return true;
}

/**
 * Convert the given POOM property \a id into a POOM property name string for POOM native query.
 * The complete set of Contact property identifiers and their string names for queries.
 * All the string names copied from: http://msdn.microsoft.com/en-us/library/bb415504.aspx.
 */
static QString getPropertyName(const CEPROPID& id)
{
    switch (id) {
        case PIMPR_ACCOUNT_NAME:
            return "[AccountName]";
        case PIMPR_ANNIVERSARY:
            return "[Anniversary]";
        case PIMPR_ASSISTANT_NAME:
            return "[AssistantName]";
        case PIMPR_ASSISTANT_TELEPHONE_NUMBER:
            return "[AssistantTelephoneNumber]";
        case PIMPR_BIRTHDAY:
            return "[Birthday]";
        case PIMPR_BUSINESS_ADDRESS:
            return "[BusinessAddress]";
        case PIMPR_BUSINESS_ADDRESS_CITY:
            return "[BusinessAddressCity]";
        case PIMPR_BUSINESS_ADDRESS_COUNTRY:
            return "[BusinessAddressCountry]";
        case PIMPR_BUSINESS_ADDRESS_POSTAL_CODE:
            return "[BusinessAddressPostalCode]";
        case PIMPR_BUSINESS_ADDRESS_STATE:
            return "[BusinessAddressState]";
        case PIMPR_BUSINESS_ADDRESS_STREET:
            return "[BusinessAddressStreet]";
        case PIMPR_BUSINESS_FAX_NUMBER:
            return "[BusinessFaxNumber]";
        case PIMPR_BUSINESS_TELEPHONE_NUMBER:
            return "[BusinessTelephoneNumber]";
        case PIMPR_BUSINESS2_TELEPHONE_NUMBER:
            return "[Business2TelephoneNumber]";
        case PIMPR_CAR_TELEPHONE_NUMBER:
            return "[CarTelephoneNumber]";
        case PIMPR_CHILDREN:
            return "[Children]";
        case PIMPR_COMPANY_NAME:
            return "[CompanyName]";
        case PIMPR_COMPANY_TELEPHONE_NUMBER:
            return "[CompanyTelephoneNumber]";
#if defined (PIMPR_CONTACT_TYPE)
        //PIMPR_CONTACT_TYPE is only supported by Windows Mobile 6 and later.
        case PIMPR_CONTACT_TYPE:
            return "[ContactType]";
#endif
        case PIMPR_CUSTOMERID:
            return "[CustomerId]";
        case PIMPR_DEPARTMENT:
            return "[Department]";
        case PIMPR_DISPLAY_NAME:
            return "[DisplayName]";
        case PIMPR_EMAIL1_ADDRESS:
            return "[Email1Address]";
        case PIMPR_EMAIL2_ADDRESS:
            return "[Email2Address]";
        case PIMPR_EMAIL3_ADDRESS:
            return "[Email3Address]";
        case PIMPR_FILEAS:
            return "[FileAs]";
        case PIMPR_FIRST_NAME:
            return "[FirstName]";
        case PIMPR_GOVERNMENTID:
            return "[GovernmentId]";
        case PIMPR_HOME_ADDRESS:
            return "[HomeAddress]";
        case PIMPR_HOME_ADDRESS_CITY:
            return "[HomeAddressCity]";
        case PIMPR_HOME_ADDRESS_COUNTRY:
            return "[HomeAddressCountry]";
        case PIMPR_HOME_ADDRESS_POSTAL_CODE:
            return "[HomeAddressPostalCode]";
        case PIMPR_HOME_ADDRESS_STATE:
            return "[HomeAddressState]";
        case PIMPR_HOME_ADDRESS_STREET:
            return "[HomeAddressStreet]";
        case PIMPR_HOME_FAX_NUMBER:
            return "[HomeFaxNumber]";
        case PIMPR_HOME_TELEPHONE_NUMBER:
            return "[HomeTelephoneNumber]";
        case PIMPR_HOME2_TELEPHONE_NUMBER:
            return "[Home2TelephoneNumber]";
        case PIMPR_IM1_ADDRESS:
            return "[IM1Address]";
        case PIMPR_IM2_ADDRESS:
            return "[IM2Address]";
        case PIMPR_IM3_ADDRESS:
            return "[IM3Address]";
        case PIMPR_JOB_TITLE:
            return "[JobTitle]";
        case PIMPR_LAST_NAME:
            return "[LastName]";
        case PIMPR_MANAGER:
            return "[Manager]";
        case PIMPR_MIDDLE_NAME:
            return "[MiddleName]";
        case PIMPR_MMS:
            return "[Mms]";
        case PIMPR_MOBILE_TELEPHONE_NUMBER:
            return "[MobileTelephoneNumber]";
        case PIMPR_NICKNAME:
            return "[Nickname]";
        case PIMPR_OFFICE_LOCATION:
            return "[OfficeLocation]";
        case PIMPR_OTHER_ADDRESS:
            return "[OtherAddress]";
        case PIMPR_OTHER_ADDRESS_CITY:
            return "[OtherAddressCity]";
        case PIMPR_OTHER_ADDRESS_COUNTRY:
            return "[OtherAddressCountry]";
        case PIMPR_OTHER_ADDRESS_POSTAL_CODE:
            return "[OtherAddressPostalCode]";
        case PIMPR_OTHER_ADDRESS_STATE:
            return "[OtherAddressState]";
        case PIMPR_OTHER_ADDRESS_STREET:
            return "[OtherAddressStreet]";
        case PIMPR_PAGER_NUMBER:
            return "[PagerNumber]";
        case PIMPR_PICTURE:
            return "[Picture]";
        case PIMPR_RADIO_TELEPHONE_NUMBER:
            return "[RadioTelephoneNumber]";
        case PIMPR_RINGTONE:
            return "[RingTone]";
        case PIMPR_SIM_PHONE:
            return "[SIMPhone]";
        case PIMPR_SMARTPROP:
            return "[SmartProperty]";
        case PIMPR_SMS:
            return "[Sms]";
        case PIMPR_SPOUSE:
            return "[Spouse]";
        case PIMPR_SUFFIX:
            return "[Suffix]";
        case PIMPR_TITLE:
            return "[Prefix]";
        case PIMPR_WEB_PAGE:
            return "[WebPage]";
        case PIMPR_YOMI_COMPANY:
            return "[YomiCompanyName]";
        case PIMPR_YOMI_FILEAS:
            return "[YomiFileAs]";
        case PIMPR_YOMI_FIRSTNAME:
            return "[YomiFirstName]";
        case PIMPR_YOMI_LASTNAME:
            return "[YomiLastName]";
    }
    return "";
}

static QString convertToCEPropValString(const CEPROPID& id, const QVariant& val)
{
    if (id != PIMPR_INVALID_ID && val.isValid()) {
        if ((id & CEVT_LPWSTR) || (id & CEVT_FILETIME)) {
            return QString("\"%1\"").arg(val.toString());
        } else {
            return val.toString();
        }
    }
    return QString();
}

// Hash for contact detail definition names to POOM prop ids.
static QHash<QString, CEPROPID> hashForContactDetailToPoomPropId;

#define Q_HASH_CONTACT_DETAIL_TO_POOM_ID(contactClass, contactField, poomId) \
        hashForContactDetailToPoomPropId.insertMulti(((QString)contactClass::DefinitionName).append((QString)contactClass::contactField), poomId)

void QContactWinCEEngine::buildHashForContactDetailToPoomPropId() const
{
    if (hashForContactDetailToPoomPropId.isEmpty()) {
        //QContactName
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactName, FieldPrefix, PIMPR_TITLE);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactName, FieldFirst, PIMPR_FIRST_NAME);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactName, FieldMiddle, PIMPR_MIDDLE_NAME);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactName, FieldLast, PIMPR_LAST_NAME);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactName, FieldSuffix, PIMPR_SUFFIX);

        // Display label
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactDisplayLabel, FieldLabel, PIMPR_FILEAS);

        // Home address
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldStreet, PIMPR_HOME_ADDRESS_STREET);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldPostcode, PIMPR_HOME_ADDRESS_POSTAL_CODE);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldLocality, PIMPR_HOME_ADDRESS_CITY);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldRegion, PIMPR_HOME_ADDRESS_STATE);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldCountry, PIMPR_HOME_ADDRESS_COUNTRY);
        
        // Work address
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldStreet, PIMPR_BUSINESS_ADDRESS_STREET);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldPostcode, PIMPR_BUSINESS_ADDRESS_POSTAL_CODE);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldLocality, PIMPR_BUSINESS_ADDRESS_CITY);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldCountry, PIMPR_BUSINESS_ADDRESS_COUNTRY);

        // Other address
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldStreet, PIMPR_OTHER_ADDRESS_STREET);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldPostcode, PIMPR_OTHER_ADDRESS_POSTAL_CODE);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldLocality, PIMPR_OTHER_ADDRESS_CITY);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAddress, FieldCountry, PIMPR_OTHER_ADDRESS_COUNTRY);

        // Emails
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactEmailAddress, FieldEmailAddress, PIMPR_EMAIL1_ADDRESS);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactEmailAddress, FieldEmailAddress, PIMPR_EMAIL2_ADDRESS);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactEmailAddress, FieldEmailAddress, PIMPR_EMAIL3_ADDRESS);

        // Phone numbers
        //XXX If too many PIM_PR* bind to the same detail field will cause the POOM query string too long...
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactPhoneNumber, FieldNumber, PIMPR_BUSINESS_TELEPHONE_NUMBER);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactPhoneNumber, FieldNumber, PIMPR_BUSINESS2_TELEPHONE_NUMBER);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactPhoneNumber, FieldNumber, PIMPR_CAR_TELEPHONE_NUMBER);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactPhoneNumber, FieldNumber, PIMPR_MOBILE_TELEPHONE_NUMBER);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactPhoneNumber, FieldNumber, PIMPR_HOME_TELEPHONE_NUMBER);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactPhoneNumber, FieldNumber, PIMPR_HOME2_TELEPHONE_NUMBER);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactPhoneNumber, FieldNumber, PIMPR_PAGER_NUMBER);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactPhoneNumber, FieldNumber, PIMPR_RADIO_TELEPHONE_NUMBER);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactPhoneNumber, FieldNumber, PIMPR_SIM_PHONE);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactPhoneNumber, FieldNumber, PIMPR_HOME_FAX_NUMBER);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactPhoneNumber, FieldNumber, PIMPR_BUSINESS_FAX_NUMBER);

        // Dates
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactAnniversary, FieldOriginalDate, PIMPR_ANNIVERSARY);
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactBirthday, FieldBirthday, PIMPR_BIRTHDAY);

        // Nickname
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactNickname, FieldNickname, PIMPR_NICKNAME);

        // Webpage
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactUrl, FieldUrl, PIMPR_WEB_PAGE);

        // Organisation
        Q_HASH_CONTACT_DETAIL_TO_POOM_ID(QContactOrganization, FieldName, PIMPR_COMPANY_NAME);

        // XXX Todo
        // Spouse and children
    }
}

static QList<CEPROPID> convertToCEPropIds(const QString& detailDefinitionName, const QString& detailFieldName)
{
    return hashForContactDetailToPoomPropId.values(QString(detailDefinitionName).append(detailFieldName));
}


/*!
 * Convert from the supplied QContactFilter \a filter into a POOM query string.
 * Return empty string if any error occurred.
 */
QString QContactWinCEEngine::convertFilterToQueryString(const QContactFilter& filter) const
{
    QString ret;
    switch(filter.type()) {
        case QContactFilter::InvalidFilter:
            {
                //Always FALSE
                ret = "([Oid] = 0 AND [Oid] <> 0)";
            }
            break;
        case QContactFilter::DefaultFilter:
            {
                //Always TRUE?
                ret = "[Oid] <> 0";
            }
            break;

        case QContactFilter::LocalIdFilter:
            {
                const QContactLocalIdFilter idf(filter);
                QList<QContactLocalId> ids = idf.ids();
                if (!ids.isEmpty())
                {
                    QStringList idList;
                    foreach(const QContactLocalId id, ids) {
                        idList << QString("[Oid] = %1").arg(id);
                    }
                    ret = idList.join(" OR ");
                    ret.prepend('(').append(')');
                }
            }
            break;

        case QContactFilter::ContactDetailFilter:
            {
                const QContactDetailFilter cdf(filter);
                //XXX Only exact match can be supported?
                if (cdf.matchFlags() == QContactFilter::MatchExactly && cdf.value().isValid()) {
                    QList<CEPROPID> ids = convertToCEPropIds(cdf.detailDefinitionName(), cdf.detailFieldName());
                    if (!ids.isEmpty()) {
                        QStringList strList;
                        foreach (const CEPROPID& id, ids) {
                            strList << QString("%1 = %2").arg(getPropertyName(id))
                                                .arg(convertToCEPropValString(id, cdf.value()));
                        }
                        ret = QString("(%1)").arg(strList.join(" OR "));
                    }
                }
                // Fall through to end
            }
            break;

        case QContactFilter::ContactDetailRangeFilter:
            {
                const QContactDetailRangeFilter cdf(filter);
                //XXX Only exact match can be supported?
                if (cdf.matchFlags() == QContactFilter::MatchExactly && (cdf.minValue().isValid() || cdf.maxValue().isValid())) {
                    QList<CEPROPID> ids = convertToCEPropIds(cdf.detailDefinitionName(), cdf.detailFieldName());
                    if (!ids.isEmpty()) {
                        const QString minComp = cdf.rangeFlags() & QContactDetailRangeFilter::ExcludeLower ? ">" : ">=";
                        const QString maxComp = cdf.rangeFlags() & QContactDetailRangeFilter::IncludeUpper ? "<=" : "<";

                        QString minCompString, maxCompString;
                        QStringList strList;

                        foreach (const CEPROPID& id, ids) {
                            if (cdf.minValue().isValid()) {
                                minCompString = QString("%1 %2 %3").arg(getPropertyName(id))
                                                                   .arg(minComp)
                                                                   .arg(convertToCEPropValString(id, cdf.minValue()));
                            }
                            if (cdf.maxValue().isValid()) {
                                maxCompString = QString("%1 %2 %3").arg(getPropertyName(id))
                                                                   .arg(maxComp)
                                                                   .arg(convertToCEPropValString(id, cdf.maxValue()));
                            }
                            
                            if (!minCompString.isEmpty() && !maxCompString.isEmpty()) {
                                strList << QString("(%1 AND %2)").arg(minCompString).arg(maxCompString);
                            } else {
                                strList << (minCompString.isEmpty() ? maxCompString : minCompString);
                            }
                        }
                        ret = QString("(%1)").arg(strList.join(" OR "));
                    }
                }
                // Fall through to end
            }
            break;

        case QContactFilter::RelationshipFilter:
            //XXX Group detail is not supported by WinCE backend
            break;

        case QContactFilter::ChangeLogFilter:
            //XXX Timestamp detail is not supported by WinCE backend
            break;

        case QContactFilter::ActionFilter:
            {
                // Find any matching actions, and do a union filter on their filter objects
                QContactActionFilter af(filter);
                QList<QContactActionDescriptor> descriptors = QContactAction::actionDescriptors(af.actionName(), af.vendorName(), af.implementationVersion());
                
                QString str;
                QStringList strList;
                for (int j = 0; j < descriptors.count(); j++) {
                    QContactAction* action = QContactAction::action(descriptors.at(j));

                    QContactFilter d = action->contactFilter(af.value());
                    delete action; // clean up.
                    if (!QContactManagerEngine::validateActionFilter(d))
                        return QString();
                    
                    str = convertFilterToQueryString(d);
                    if (str.isEmpty())
                        return QString();
                    strList << str;
                }

                if (!strList.isEmpty()) {
                    ret =QString("(%1)").arg(strList.join(" OR "));
                }
                // Fall through to end
            }
            break;

        case QContactFilter::IntersectionFilter:
            {
                const QContactIntersectionFilter bf(filter);
                const QList<QContactFilter>& terms = bf.filters();
                if (terms.count() > 0) {
                    QString str;
                    QStringList strList;
                    for(int j = 0; j < terms.count(); j++) {
                        str = convertFilterToQueryString(terms.at(j));
                        if (str.isEmpty())
                            return QString();
                        strList << str;
                    }
                    if (!strList.isEmpty()) {
                        ret =QString("(%1)").arg(strList.join(" AND "));
                    }
                }
                // Fall through to end
            }
            break;

        case QContactFilter::UnionFilter:
            {
                const QContactUnionFilter bf(filter);
                const QList<QContactFilter>& terms = bf.filters();
                if (terms.count() > 0) {
                    QString str;
                    QStringList strList;
                    for(int j = 0; j < terms.count(); j++) {
                        str = convertFilterToQueryString(terms.at(j));
                        if (str.isEmpty())
                            return QString();
                    }
                    if (!strList.isEmpty()) {
                        ret =QString("(%1)").arg(strList.join(" OR "));
                    }
                }
                // Fall through to end
            }
            break;
    }
    return ret;
}


/*!
 * Return a list of QContact ids from the given POOM item \a collection.
 */
QList<QContactLocalId> QContactWinCEEngine::convertP2QIdList(const SimpleComPointer<IPOutlookItemCollection>& collection) const
{
    SimpleComPointer<IPOlItems2> items;
    QList<QContactLocalId> ids;
    if (SUCCEEDED(collection->QueryInterface<IPOlItems2>(&items))) {
        CEPROPID propid = PIMPR_OID;
        CEPROPVAL *ppropval = 0;

        int count = 0;
        items->get_Count(&count);

        ULONG cbSize = 0;

        // Allocate something to start with
        ppropval = (CEPROPVAL*) HeapAlloc(GetProcessHeap(), 0, sizeof(CEPROPVAL));
        
        HRESULT hr;
        for(int i=0; i < count; i++) {
            hr = items->GetProps(i +1, &propid, 0, 1, &ppropval, &cbSize, NULL);
            if (HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER) == hr) {
                ppropval = (CEPROPVAL*) HeapReAlloc(GetProcessHeap(), 0, ppropval, cbSize);
                hr = items->GetProps(i + 1, &propid, 0, 1, &ppropval, &cbSize, NULL);
            }
            if (SUCCEEDED(hr)) {
                ids << (QContactLocalId) ppropval->val.ulVal;
            } else {
                qDebug() << QString("Eternal sadness: %1").arg(HRESULT_CODE(hr), 0, 16);
            }
        }

        HeapFree(GetProcessHeap(), 0, ppropval);
    }
    return ids;
}

/*!
 * Return a list of contacts from the given POOM \a collection.
 */
bool QContactWinCEEngine::convertP2QContacts(const SimpleComPointer<IPOutlookItemCollection>& collection, QList<QContact>* contacts) const
{
    int itemCount;
    HRESULT hr;
    hr = collection->get_Count(&itemCount);
    if (SUCCEEDED(hr)) {
        SimpleComPointer<IDispatch> idisp = 0;
        SimpleComPointer<IItem> iItem;
        for (int i = 1; i <= itemCount; i++) {
            HRESULT hr = collection->Item(i, &idisp);
            if (SUCCEEDED(hr) &&
                SUCCEEDED(hr = idisp->QueryInterface<IItem>(&iItem))) {
                    contacts->append(convertToQContact(iItem));
            } else {
                qDebug() << QString("Eternal sadness: %1").arg(HRESULT_CODE(hr), 0, 16);
                return false;
            }
        }
        return true;
    }
    return false;
}

static bool sortPOOMContacts(const SimpleComPointer<IPOutlookItemCollection>& collection, const QContactSortOrder& sortOrder)
{
    HRESULT hr = S_FALSE;
    QList<CEPROPID> ids = convertToCEPropIds(sortOrder.detailDefinitionName(), sortOrder.detailFieldName());

    if (!ids.isEmpty()) {
        QString prop = getPropertyName(ids.at(0));
        int descending = sortOrder.direction() == Qt::DescendingOrder ? 1 : 0;
        hr = collection->Sort((BSTR)prop.constData(), descending);

        if (!SUCCEEDED(hr)) {
            qDebug() << QString("Failed to sort contacts: %1 (%2)").arg(hr, 0, 16).arg(HRESULT_CODE(hr), 0, 16);
        }
    } else {
        qDebug() << QString("This detail field(%1:%2) is not supported by wince native sorting")
                   .arg(sortOrder.detailDefinitionName())
                   .arg(sortOrder.detailFieldName());
    }

    return SUCCEEDED(hr);
}

QList<QContactLocalId> QContactWinCEEngine::contactIds(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
    QString query = convertFilterToQueryString(filter);

    if (!query.isEmpty()) {
        *error = QContactManager::NoError;
        //Filtering contacts with POOM API
        SimpleComPointer<IPOutlookItemCollection> collection;
        HRESULT hr = d->m_collection->Restrict((BSTR)(query.constData()), &collection);

        if (SUCCEEDED(hr)) {
            QList<QContactLocalId> ids;

            //Try native sorting first...
            if (sortOrders.size() == 1 && sortPOOMContacts(collection, sortOrders.at(0))) {
                ids = convertP2QIdList(collection);
            } else {
                //Multi sort orders or native sorting failed, fall back to the generic sorting
                QList<QContact> filteredContacts;
                if (convertP2QContacts(collection, &filteredContacts)) {
                    ids = sortContacts(filteredContacts, sortOrders);
                } else {
                    *error = QContactManager::UnspecifiedError;
                    qDebug() << "Wince contact manager internal error";
                }
            }
            return ids;
        } else {
            //Should we fail back to generic filtering here?
            qDebug() << "Can't filter contacts with query string:" << query << ", HRESULT=" << HRESULT_CODE(hr);
        }
    }
    //Fail back to generic filtering
    return QContactManagerEngine::contactIds(filter, sortOrders, error);
}

QList<QContactLocalId> QContactWinCEEngine::contactIds(const QList<QContactSortOrder>& sortOrders, QContactManager::Error* error) const
{
    QList<QContactLocalId> ids;
    *error = QContactManager::NoError;
    if (sortOrders.isEmpty()) {
        ids = d->m_ids;
    } else {
        SimpleComPointer<IPOutlookItemCollection> newCollection;
        HRESULT hr = d->m_collection->Restrict(TEXT("[Oid] <> 0"), &newCollection);

        if (SUCCEEDED(hr)) {
            //Try native sorting first...
            if (sortOrders.size() == 1 && sortPOOMContacts(newCollection, sortOrders.at(0))) {
                ids = convertP2QIdList(newCollection);
                return ids;
            } 
        }

        //Multi sort orders or native sorting failed, fall back to the generic sorting
        QList<QContact> contacts;
        if (convertP2QContacts(d->m_collection, &contacts)) {
            ids = sortContacts(contacts, sortOrders);
        } else {
            *error = QContactManager::UnspecifiedError;
            qDebug() << "Wince contact manager internal error";
        }
    }
    return ids;
}

