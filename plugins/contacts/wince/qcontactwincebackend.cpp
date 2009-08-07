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
** contact Nokia at http://www.qtsoftware.com/contact.
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

QContactWinCEEngine::QContactWinCEEngine(const QMap<QString, QString>& parameters, QContactManager::Error& error)
    : d(new QContactWinCEEngineData)
{
    error = QContactManager::NoError;

    if (SUCCEEDED(CoInitializeEx( NULL, 0))) {
        qDebug() << "Initialized COM";
        if (SUCCEEDED(CoCreateInstance(CLSID_Application, NULL,
                                       CLSCTX_INPROC_SERVER, IID_IPOutlookApp2,
                                       reinterpret_cast<void **>(&d->m_app)))) {
            qDebug() << "Created IPOutlookApp2";
            if(FAILED(d->m_app->Logon(NULL)))
            {
                qDebug() << "Failed to log on";
                d->m_app->Release();
                d->m_app = 0;
            } else {
                qDebug() << "logged on ok";
                if(SUCCEEDED(d->m_app->GetDefaultFolder(olFolderContacts, &d->m_folder))) {
                    if(SUCCEEDED(d->m_folder->get_Items(&d->m_collection))) {
                        qDebug() << "Got item collection";

                        // get an IPOLItems2 pointer for the collection, too
                        if (SUCCEEDED(d->m_collection->QueryInterface<IPOlItems2>(&d->m_items2))) {
                            qDebug() << "Got items2";

                            // Retrieve some ids
                            CEPROPID propid = PIMPR_OID;
                            CEPROPVAL *ppropval = 0;

                            int count = 0;
                            d->m_items2->get_Count(&count);
                            qDebug() << "there are " << count << "items";

                            ULONG cbSize = 0;
                            HRESULT hr;

                            // Allocate something to start with
                            ppropval = (CEPROPVAL*) HeapAlloc(GetProcessHeap(), 0, sizeof(CEPROPVAL));

                            for(int i=0; i < count; i++) {
                                hr = d->m_items2->GetProps(i +1, &propid, 0, 1, &ppropval, &cbSize, NULL);
                                if (HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER) == hr) {
                                    qDebug() << "Not enough buffer size, reallocating";
                                    ppropval = (CEPROPVAL*) HeapReAlloc(GetProcessHeap(), 0, ppropval, cbSize);
                                    hr = d->m_items2->GetProps(i + 1, &propid, 0, 1, &ppropval, &cbSize, NULL);
                                }
                                if (SUCCEEDED(hr)) {
                                    qDebug() << "Succeeded getting something";
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
        d->m_app->Release();
    }
    CoUninitialize();
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
typedef void (*processContactPoomElement)(const QContactDetail& detail, int index, QVector<CEPROPVAL>& props);

// POOM to us
// something like
// {PIMPR_,PIMPR_} -> function
// Might then need PIMPR -> bag above

struct PoomContactElement;
typedef void (*processPoomContactElement)(const PoomContactElement& e, const QVariantList& values, QContact& ret);

struct PoomContactElement {
    QList<CEPROPID> poom;
    processPoomContactElement func;
};

static void processName(const PoomContactElement& e, const QVariantList& values, QContact& ret)
{
    QContactName name;
    name.setValue(QContactName::FieldPrefix, values[0]);
    name.setValue(QContactName::FieldFirst, values[1]);
    name.setValue(QContactName::FieldMiddle, values[2]);
    name.setValue(QContactName::FieldLast, values[3]);
    name.setValue(QContactName::FieldSuffix, values[4]);
    ret.saveDetail(&name);
}

static void processFileAs(const PoomContactElement& e, const QVariantList& values, QContact& ret)
{
    ret.setDisplayLabel(values[0].toString());
}

static void processAddress(const QString& context, const QVariantList& values, QContact& ret)
{
    QContactAddress address;
    address.setContextAttribute(context);
    address.setDisplayLabel(values[0].toString());
    address.setValue(QContactAddress::FieldStreet, values[1]);
    address.setValue(QContactAddress::FieldPostcode, values[2]);
    address.setValue(QContactAddress::FieldLocality, values[3]);
    address.setValue(QContactAddress::FieldRegion, values[4]);
    address.setValue(QContactAddress::FieldCountry, values[5]);
    ret.saveDetail(&address);
}

static void processHomeAddress(const PoomContactElement& e, const QVariantList& values, QContact& ret)
{
    processAddress(QContactDetail::AttributeContextHome, values, ret);
}

static void processWorkAddress(const PoomContactElement& e, const QVariantList& values, QContact& ret)
{
    processAddress(QContactDetail::AttributeContextWork, values, ret);
}

static void processOtherAddress(const PoomContactElement& e, const QVariantList& values, QContact& ret)
{
    processAddress(QContactDetail::AttributeContextOther, values, ret);
}

static void processEmails(const PoomContactElement& e, const QVariantList& values, QContact& ret)
{
    // Just create an email address for each one
    foreach (const QVariant& v, values) {
        if (!v.isNull()) {
            QContactEmailAddress e;
            e.setValue(QContactEmailAddress::FieldEmailAddress, v);
            ret.saveDetail(&e);
        }
    }
}

static void processPhones(const PoomContactElement& e, const QVariantList& values, QContact& ret)
{
    // Crazy ordering, here
    for (int i=0; i < values.count(); i++) {
        const QVariant& v = values.at(i);
        if (!v.isNull()) {
            QContactPhoneNumber number;
            number.setValue(QContactPhoneNumber::FieldNumber, v);
            switch(i) {
                case 0: // Business phone
                case 1:
                    number.setContextAttribute(QContactDetail::AttributeContextWork);
                    number.setSubTypeAttribute(QContactPhoneNumber::AttributeSubTypeVoice);
                    break;
                case 2: // Car
                    number.setSubTypeAttribute(QContactPhoneNumber::AttributeSubTypeCar);
                    break;
                case 3: // Mobile
                    number.setSubTypeAttribute(QContactPhoneNumber::AttributeSubTypeMobile);
                    break;
                case 4: // Home phones
                case 5:
                    number.setContextAttribute(QContactDetail::AttributeContextHome);
                    number.setSubTypeAttribute(QContactPhoneNumber::AttributeSubTypeVoice);
                    break;
                case 6: // MMS & SMS (??)
                case 7:
                    number.setSubTypeAttribute(QContactPhoneNumber::AttributeSubTypeMessagingCapable);
                    break;
                case 8: // Pager
                    number.setSubTypeAttribute(QContactPhoneNumber::AttributeSubTypePager);
                    break;
                case 9: // Radio telephone (??)
                    number.setSubTypeAttribute(QContactPhoneNumber::AttributeSubTypeMobile);
                    break;
                case 10: // SIM entry
                    break;
                case 11: // Home fax
                    number.setContextAttribute(QContactDetail::AttributeContextHome);
                    number.setSubTypeAttribute(QContactPhoneNumber::AttributeSubTypeFacsimile);
                    break;
                case 12: // Business fax
                    number.setContextAttribute(QContactDetail::AttributeContextWork);
                    number.setSubTypeAttribute(QContactPhoneNumber::AttributeSubTypeFacsimile);
                    break;
            }
            ret.saveDetail(&number);
        }
    }
}

static void contactP2QTransforms(QHash<CEPROPID, PoomContactElement>& prophash, QVector<CEPROPID>& propids)
{
    static QHash<CEPROPID, PoomContactElement> hash;
    static QVector<CEPROPID> ids;

    if (hash.count() == 0) {
        QList<PoomContactElement> list;

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
        emails.poom << PIMPR_EMAIL1_ADDRESS << PIMPR_EMAIL2_ADDRESS << PIMPR_EMAIL3_ADDRESS;
        emails.func = processEmails;
        list.append(emails);

        // Phone numbers
        PoomContactElement phones;
        phones.poom << PIMPR_BUSINESS_TELEPHONE_NUMBER << PIMPR_BUSINESS2_TELEPHONE_NUMBER
            << PIMPR_CAR_TELEPHONE_NUMBER  << PIMPR_MOBILE_TELEPHONE_NUMBER
            << PIMPR_HOME_TELEPHONE_NUMBER << PIMPR_HOME2_TELEPHONE_NUMBER
            << PIMPR_MMS << PIMPR_SMS << PIMPR_PAGER_NUMBER << PIMPR_RADIO_TELEPHONE_NUMBER
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
    if (!value.isEmpty()) {
        props.append(convertToCEPropVal(id, value));
    }
}

static void processQName(const QContactDetail& detail, int index, QVector<CEPROPVAL>& props)
{
    if (index == 0) {
        addIfNotEmpty(PIMPR_TITLE, detail.value(QContactName::FieldPrefix), props);
        addIfNotEmpty(PIMPR_FIRST_NAME, detail.value(QContactName::FieldFirst), props);
        addIfNotEmpty(PIMPR_MIDDLE_NAME, detail.value(QContactName::FieldMiddle), props);
        addIfNotEmpty(PIMPR_LAST_NAME, detail.value(QContactName::FieldLast), props);
        addIfNotEmpty(PIMPR_SUFFIX, detail.value(QContactName::FieldSuffix), props);
    } else {
        qDebug() << "Extra name!??";
    }
}

static void contactQ2PTransforms(QHash<QString, processContactPoomElement>& ret)
{
    static QHash<QString, processContactPoomElement> hash;
    if (hash.count() == 0) {
        hash.insert(QContactName::DefinitionName, processQName);
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
    contactP2QTransforms(hash, props);

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

            qDebug() << "Property:" << id << TypeFromPropID(id) << valueHash.value(id);
            const PoomContactElement& qmap = hash.value(id);
            if (qmap.func) {
                // We need to create values for each of qmap.poom
                // (which means we need to find each value of qmap.poom)
                // and we remove them as we go
                QVariantList vl;
                foreach(const CEPROPID& id, qmap.poom) {
                    vl << valueHash.take(id);
                }
                qmap.func(qmap, vl, ret);
            } else {
                qDebug() << "Didn't match property";
                // Remove the ignored value so we don't infinite loop
                valueHash.take(id);
            }
        }

        HeapFree(GetProcessHeap(), 0, propvals);
    }

    return ret;
}

bool QContactWinCEEngine::convertContact(const QContact& contact, IItem* item, QContactManager::Error &error) const
{
    // We have to create a whole bunch of CEPROPVALs for each detail
    // This is slightly hampered by the limited storage slots

    QList<QContactDetail> details = contact.details();
    QHash<QString, int> indices;
    QHash<QString, processContactPoomElement> transforms;

    contactQ2PTransforms(transforms);
    processContactPoomElement func;

    QVector<CEPROPVAL> props;

    foreach (const QContactDetail& detail, details) {
        func = transforms.value(detail.definitionName());
        if (func) {
            int idx = indices.value(detail.definitionName(), 0);
            func(detail, idx, props);
            indices.insert(detail.definitionName(), idx + 1);
        }
    }

    // Now set it
    qDebug() << "Trying to set" << props.count() << "properties";
    HRESULT hr = item->SetProps(0, props.count(), props.data());
    if (SUCCEEDED(hr)) {
        qDebug() << "Set props ok!";
        return true;
    } else {
        qDebug() << "Failed to set props:" << HRESULT_CODE(hr);
    }
    return false;
}

QContact QContactWinCEEngine::contact(const QUniqueId& contactId, QContactManager::Error& error) const
{
    error = QContactManager::NoError;
    QContact ret;

    // Fetch!
    IItem* item = 0;
    qDebug() << "trying to fetch contact:" << contactId;
    HRESULT hr = d->m_app->GetItemFromOidEx(contactId, 0, &item);
    if (SUCCEEDED(hr)) {
        if (item) {
            ret = convertContact(item);
        } else {
            error = QContactManager::DoesNotExistError;
        }
    } else {
        qDebug() << "Failed to retrieve contact:" << HRESULT_CODE(hr);
        if (HRESULT_CODE(hr) == ERROR_INVALID_PARAMETER) {
            error = QContactManager::DoesNotExistError;
        } else {
            error = QContactManager::UnspecifiedError;
        }
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
    /*
    if (!validateContact(*contact, error)) {
        qDebug() << "Failed to validate";
        error = QContactManager::InvalidDetailError;
        return false;
    }*/

    IItem *icontact = 0;
    bool wasOld = false;
    // Figure out if this is a new or old contact
    if (d->m_ids.contains(contact->id())) {
        // update existing contact
        HRESULT hr = d->m_app->GetItemFromOidEx(contact->id(), 0, &icontact);
        if (SUCCEEDED(hr)) {
            wasOld = true;
            qDebug() << "Got old item ok";
        } else {
            qDebug() << "Didn't get old contact" << HRESULT_CODE(hr);
            error = QContactManager::UnspecifiedError;
        }
    } else {
        // new contact!
        HRESULT hr = d->m_items2->Add((IDispatch**) &icontact);
        if (SUCCEEDED(hr)) {
            qDebug() << "Succeeded creating contact";
        } else {
            qDebug() << "Failed to create contact: "<< HRESULT_CODE(hr);
            error = QContactManager::OutOfMemoryError;
        }
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
                    icontact->Release();
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
        icontact->Release();
    }

    // error should have been set.
    return false;
}


bool QContactWinCEEngine::removeContact(const QUniqueId& contactId, QSet<QUniqueId>& contactsChanged, QSet<QUniqueId>& , QContactManager::Error& error)
{
    // Fetch an IItem* for this
    IItem* item = 0;
    HRESULT hr = d->m_app->GetItemFromOidEx(contactId, 0, &item);
    if (SUCCEEDED(hr)) {
        hr = item->Delete();
        if (SUCCEEDED(hr)) {
            contactsChanged.insert(contactId);
            item->Release();
            error = QContactManager::NoError;
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

QContactDetailDefinition QContactWinCEEngine::detailDefinition(const QString& definitionName, QContactManager::Error& error) const
{
    error = QContactManager::DoesNotExistError;
    return QContactDetailDefinition();
}

bool QContactWinCEEngine::saveDetailDefinition(const QContactDetailDefinition& def, QContactManager::Error& error)
{
    if (!validateDefinition(def, error)) {
        return false;
    }
    error = QContactManager::NoError;
    return true;
}

bool QContactWinCEEngine::removeDetailDefinition(const QString& definitionId, QContactManager::Error& error)
{
    if (definitionId.isEmpty()) {
        error = QContactManager::BadArgumentError;
        return false;
    }

    error = QContactManager::DoesNotExistError;
    return false;
}

/*! \reimp */
bool QContactWinCEEngine::hasFeature(QContactManagerInfo::ManagerFeature feature) const
{
    switch (feature) {
        case QContactManagerInfo::ReadOnlyDetails:
//        case QContactManagerInfo::MutableDefinitions:
        case QContactManagerInfo::Synchronous:
            return true;
        default:
            return false;
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
