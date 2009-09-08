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
#include <QFileSystemWatcher>
#include <QFile>
#include <QSettings>
#include <QUuid>

#include <QDebug>


#include "qcontact_p.h"
#include "qcontactgroup_p.h"
#include "qcontactmanager.h"
#include "qcontactmanager_p.h"

#define INITGUID
#include "qcontactwincebackend_p.h"

QContactWinCEEngine::QContactWinCEEngine(const QMap<QString, QString>& , QContactManager::Error& error)
    : d(new QContactWinCEEngineData)
{
    error = QContactManager::NoError;

    if (SUCCEEDED(d->m_cominit.hr())) {
        if (SUCCEEDED(CoCreateInstance(CLSID_Application, NULL,
                                       CLSCTX_INPROC_SERVER, IID_IPOutlookApp2,
                                       reinterpret_cast<void **>(&d->m_app)))) {
            if(FAILED(d->m_app->Logon(NULL))) {
                qDebug() << "Failed to log on";
                d->m_app = 0;
            } else {
                if(SUCCEEDED(d->m_app->GetDefaultFolder(olFolderContacts, &d->m_folder))) {
                    if(SUCCEEDED(d->m_folder->get_Items(&d->m_collection))) {
                        // get an IPOLItems2 pointer for the collection, too
                        if (SUCCEEDED(d->m_collection->QueryInterface<IPOlItems2>(&d->m_items2))) {
                            HRESULT hr;

                            // Register/retrieve our custom ids
                            LPCWSTR customIds[2] = { L"QTCONTACTS_PHONE_META", L"QTCONTACTS_EMAIL_META" };
                            CEPROPID outIds[2];

                            if (SUCCEEDED(d->m_app->GetIDsFromNames(2, customIds, PIM_CREATE | CEVT_LPWSTR, outIds))) {
                                d->m_phonemeta = outIds[0];
                                d->m_emailmeta = outIds[1];
                            }

                            // Retrieve some ids [don't need this, really]
                            CEPROPID propid = PIMPR_OID;
                            CEPROPVAL *ppropval = 0;

                            int count = 0;
                            d->m_items2->get_Count(&count);

                            ULONG cbSize = 0;

                            // Allocate something to start with
                            ppropval = (CEPROPVAL*) HeapAlloc(GetProcessHeap(), 0, sizeof(CEPROPVAL));

                            for(int i=0; i < count; i++) {
                                hr = d->m_items2->GetProps(i +1, &propid, 0, 1, &ppropval, &cbSize, NULL);
                                if (HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER) == hr) {
                                    ppropval = (CEPROPVAL*) HeapReAlloc(GetProcessHeap(), 0, ppropval, cbSize);
                                    hr = d->m_items2->GetProps(i + 1, &propid, 0, 1, &ppropval, &cbSize, NULL);
                                }
                                if (SUCCEEDED(hr)) {
                                    d->m_ids << (QUniqueId) ppropval->val.ulVal;
                                } else {
                                    qDebug() << QString("Eternal sadness: %1").arg(HRESULT_CODE(hr), 0, 16);
                                }
                            }

                            HeapFree(GetProcessHeap(), 0, ppropval);
                        }
                    } else {
                        qDebug() << "Failed to get items";
                        d->m_collection = 0;
                    }
                } else {
                    qDebug() << "Failed to get contacts folder";
                    d->m_folder = 0;
                }
            }
        }
    }
}

QContactWinCEEngine::QContactWinCEEngine(const QContactWinCEEngine& other)
    : QContactManagerEngine(), d(other.d)
{

}

QContactWinCEEngine& QContactWinCEEngine::operator=(const QContactWinCEEngine& other)
{
    // assign
    d = other.d;

    return *this;
}

QContactWinCEEngine::~QContactWinCEEngine()
{
    if (d->m_app) {
        d->m_app->Logoff();
    }
}

void QContactWinCEEngine::deref()
{
    if (!d->m_refCount.deref())
        delete this;
}


QList<QUniqueId> QContactWinCEEngine::contacts(const QList<QContactSortOrder>& sortOrders, QContactManager::Error& error) const
{
    QList<QUniqueId> allCIds = d->m_ids;
    error = QContactManager::NoError;

    return d->m_ids;

    // return the list sorted according to sortOrders
    QContactManager::Error sortError;
    QList<QContact> sorted;
    QList<QUniqueId> sortedIds;
    for (int i = 0; i < allCIds.size(); i++)
        QContactManagerEngine::addSorted(&sorted, contact(allCIds.at(i), sortError), sortOrders);
    for (int i = 0; i < sorted.size(); i++)
        sortedIds.append(sorted.at(i).id());

    return sortedIds;
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
    setIfNotEmpty(address, QContactAddress::FieldDisplayLabel, values[0].toString());
    setIfNotEmpty(address, QContactAddress::FieldStreet, values[1].toString());
    setIfNotEmpty(address, QContactAddress::FieldPostcode, values[2].toString());
    setIfNotEmpty(address, QContactAddress::FieldLocality, values[3].toString());
    setIfNotEmpty(address, QContactAddress::FieldRegion, values[4].toString());
    setIfNotEmpty(address, QContactAddress::FieldCountry, values[5].toString());
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
    // space = nothing
    // H = Home context, no type modifier
    // W = Work context, no type modifier

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

            ret.saveDetail(&number);
        }
    }
}

static void processId(const QVariantList& values, QContact& ret)
{
    ret.setId(values.at(0).toUInt());
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
        homeAddress.poom << PIMPR_HOME_ADDRESS << PIMPR_HOME_ADDRESS_STREET << PIMPR_HOME_ADDRESS_POSTAL_CODE << PIMPR_HOME_ADDRESS_CITY << PIMPR_HOME_ADDRESS_STATE << PIMPR_HOME_ADDRESS_COUNTRY;
        homeAddress.func = processHomeAddress;
        list.append(homeAddress);

        // Work address
        PoomContactElement workAddress;
        workAddress.poom << PIMPR_BUSINESS_ADDRESS << PIMPR_BUSINESS_ADDRESS_STREET << PIMPR_BUSINESS_ADDRESS_POSTAL_CODE << PIMPR_BUSINESS_ADDRESS_CITY << PIMPR_BUSINESS_ADDRESS_COUNTRY;
        workAddress.func = processWorkAddress;
        list.append(workAddress);

        // Other address
        PoomContactElement otherAddress;
        otherAddress.poom << PIMPR_OTHER_ADDRESS << PIMPR_OTHER_ADDRESS_STREET << PIMPR_OTHER_ADDRESS_POSTAL_CODE << PIMPR_OTHER_ADDRESS_CITY << PIMPR_OTHER_ADDRESS_COUNTRY;
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

        // TODO: organization, dates, avatars, notes, account id, customer id, spouse/children
        // government id, IM addresses, .. . ..


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

static CEPROPVAL convertToCEPropVal(const CEPROPID& id, const QString& value)
{
    CEPROPVAL val;
    val.propid = id;
    val.wFlags = 0;
    val.wLenData = 0;
    val.val.lpwstr = wcsdup(value.utf16()); // XXX leaks

    return val;
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

static void processQAddresses(const QList<QContactAddress>& address, QVector<CEPROPVAL>& props)
{

}

static void contactQ2PTransforms(QHash<QString, processContactPoomElement>& ret)
{
    static QHash<QString, processContactPoomElement> hash;
    if (hash.count() == 0) {
        hash.insert(QContactName::DefinitionName, processQName);
        hash.insert(QContactDisplayLabel::DefinitionName, processQLabel);
    }
    ret = hash;
}

static QVariant convertCEPropVal(const CEPROPVAL& val)
{
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
            SYSTEMTIME st;
            FileTimeToSystemTime(&val.val.filetime, &st);
            return QVariant(QDateTime(QDate(st.wYear, st.wMonth, st.wDay), QTime(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds)));
        }
        case CEVT_LPWSTR:
            return QVariant(QString::fromWCharArray(val.val.lpwstr));

        case CEVT_BLOB: // Not used yet
        case CEVT_PIM_STREAM: // Not used yet
            break;
    }
    return QVariant();
}

QContact QContactWinCEEngine::convertContact(IItem *contact) const
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
            QVariant v = convertCEPropVal(propvals[i]);
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

bool QContactWinCEEngine::convertContact(const QContact& contact, IItem* item, QContactManager::Error &error) const
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

    // Now set it
    HRESULT hr = item->SetProps(0, props.count(), props.data());
    if (SUCCEEDED(hr)) {
        return true;
    } else {
        qDebug() << QString("Failed to set props: %1 (%2)").arg(hr, 0, 16).arg(HRESULT_CODE(hr), 0, 16);
    }

    return false;
}

QContact QContactWinCEEngine::contact(const QUniqueId& contactId, QContactManager::Error& error) const
{
    QContact ret;

    // id == 0 gives a bad argument error from POOM, so don't even try it
    if (contactId != 0) {
        // Fetch!
        SimpleComPointer<IItem> item = 0;
        HRESULT hr = d->m_app->GetItemFromOidEx(contactId, 0, &item);
        if (SUCCEEDED(hr)) {
            if (item) {
                error = QContactManager::NoError;
                ret = convertContact(item);
            } else {
                error = QContactManager::DoesNotExistError;
            }
        } else {
            qDebug() << "Failed to retrieve contact:" << HRESULT_CODE(hr);
            if (HRESULT_CODE(hr) == ERROR_NOT_FOUND) {
                error = QContactManager::DoesNotExistError;
            } else {
                error = QContactManager::UnspecifiedError;
            }
        }
    } else {
        error = QContactManager::DoesNotExistError;
    }
    return ret;
}

bool QContactWinCEEngine::saveContact(QContact* contact, QSet<QUniqueId>& contactsAdded, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& groupsChanged, QContactManager::Error& error)
{
    if (contact == 0) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    // ensure that the contact's details conform to their definitions
    if (!validateContact(*contact, error)) {
        qDebug() << "Failed to validate";
        error = QContactManager::InvalidDetailError;
        return false;
    }

    SimpleComPointer<IItem> icontact;
    bool wasOld = false;
    // Figure out if this is a new or old contact
    if (d->m_ids.contains(contact->id())) {
        // update existing contact
        HRESULT hr = d->m_app->GetItemFromOidEx(contact->id(), 0, &icontact);
        if (SUCCEEDED(hr)) {
            wasOld = true;
        } else {
            qDebug() << "Didn't get old contact" << HRESULT_CODE(hr);
            error = QContactManager::UnspecifiedError;
        }
    } else if (contact->id() == 0) {
        // new contact!
        SimpleComPointer<IDispatch> idisp = 0;
        HRESULT hr = d->m_collection->Add(&idisp);
        if (SUCCEEDED(hr)) {
            // now get an actual item out of it (was IContact, which is not IItem)
            hr = idisp->QueryInterface<IItem>(&icontact);

            if (SUCCEEDED(hr)) {
            } else {
                qDebug() << "Failed to query interface" << HRESULT_CODE(hr);
                error = QContactManager::UnspecifiedError;
            }
        } else {
            qDebug() << "Failed to create contact: "<< HRESULT_CODE(hr);
            error = QContactManager::OutOfMemoryError;
        }
    } else {
        // Saving a contact with a non zero id, but that doesn't exist
        qDebug() << "Saving unknown contact id, rejecting";
        error = QContactManager::DoesNotExistError;
    }

    if (icontact) {
        // Convert our QContact to the Icontact (via setProps)
        if (convertContact(*contact, icontact, error)) {
            HRESULT hr = icontact->Save();
            if (SUCCEEDED(hr)) {
                // yay! we also need to set the new contact id
                long oid = 0;
                hr = icontact->get_Oid(&oid);
                if (SUCCEEDED(hr)) {
                    contact->setId((QUniqueId)oid);
                    if (wasOld)
                        contactsChanged.insert(contact->id());
                    else
                        contactsAdded.insert(contact->id());
                    d->m_ids.append(contact->id());
                    error = QContactManager::NoError;
                    return true;
                }
                qDebug() << "Saved contact, but couldn't retrieve id again??" << HRESULT_CODE(hr);
                // Blargh.
                error = QContactManager::UnspecifiedError;
            } else {
                qDebug() << "Failed to save contact" << HRESULT_CODE(hr);
            }
        } else {
            qDebug() << "Failed to convert contact";
        }
    }

    // error should have been set.
    return false;
}


bool QContactWinCEEngine::removeContact(const QUniqueId& contactId, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& , QContactManager::Error& error)
{
    // Fetch an IItem* for this
    SimpleComPointer<IItem> item ;
    HRESULT hr = d->m_app->GetItemFromOidEx(contactId, 0, &item);
    if (SUCCEEDED(hr)) {
        hr = item->Delete();
        if (SUCCEEDED(hr)) {
            contactsChanged.insert(contactId);
            error = QContactManager::NoError;
            d->m_ids.removeAll(contactId);
            return true;
        }
        qDebug() << "Failed to delete:" << HRESULT_CODE(hr);
        error = QContactManager::UnspecifiedError;
    } else {
        qDebug() << "Failed to retrieve item pointer" << HRESULT_CODE(hr);
        error = QContactManager::DoesNotExistError;
    }
    return false;
}


QMap<QString, QContactDetailDefinition> QContactWinCEEngine::detailDefinitions(QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    return QContactManagerEngine::schemaDefinitions();
}

/*! \reimp */
bool QContactWinCEEngine::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    switch (feature) {
        case QContactManagerInfo::ReadOnlyDetails:
        case QContactManagerInfo::Synchronous:
            return true;
        default:
            return false;
    }
}

/* Synthesise the display label of a contact */
QString QContactWinCEEngine::synthesiseDisplayLabel(const QContact& contact, QContactManager::Error& error) const
{
    // The POOM API (well, lack thereof) makes this a bit strange.
    // It's basically just "Last, First" or "Company", if "FileAs" is not set.
    QContactName name = contact.detail<QContactName>();
    QContactOrganisation org = contact.detail<QContactOrganisation>();

    // Basically we ignore any existing labels for this contact, since we're being
    // asked what the synthesized label would be

    // XXX For greatest accuracy we might be better off converting this contact to
    // a real item (but don't save it), and then retrieve it...
    if (!name.last().isEmpty()) {
        if (!name.first().isEmpty()) {
            return QString(QLatin1String("%1, %2")).arg(name.last()).arg(name.first());
        } else {
            // Just last
            return name.last();
        }
    } else if (!name.first().isEmpty()) {
        return name.first();
    } else if (!org.displayLabel().isEmpty()) {
        return org.displayLabel();
    } else {
        // XXX grargh.
        return QLatin1String("Unnamed");
    }
}


/*!
 * Returns the list of data types supported by the WinCE engine
 */
QList<QVariant::Type> QContactWinCEEngine::supportedDataTypes() const
{
    QList<QVariant::Type> st;
    st.append(QVariant::String);
    st.append(QVariant::Int);
    st.append(QVariant::UInt);
    st.append(QVariant::Double);
    st.append(QVariant::Date);
    st.append(QVariant::DateTime);

    return st;
}


/* Factory lives here in the basement */
QContactManagerEngine* ContactWinceFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
{
    return new QContactWinCEEngine(parameters, error);
}

QString ContactWinceFactory::managerName() const
{
    return QString("wince");
}
Q_EXPORT_PLUGIN2(contacts_wince, ContactWinceFactory);
