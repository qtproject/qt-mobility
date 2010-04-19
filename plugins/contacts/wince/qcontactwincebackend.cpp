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

#include <QMutex>
#include <QDebug>
#include "qcontactmanager.h"
#include "qcontactchangeset.h"
#include "qtcontacts.h"

#define INITGUID
#include "qcontactwincebackend_p.h"

/*
 * This file is most of the engine plumbing - conversion to/from POOM is in
 * the contactconversions.cpp file.
 */

/*
 * TODOS
 *
 * - refactor (much needed)
 *  - single properties should have common code
 *  - error reporting when we run out of room
 * - use QScopedPointer
 * - pronunciation (needs schema update)
 * - any XXX comments
 * - store extra metadata
 *  - Voice subtype for phone, don't always assume
 * - Address formatting - PIMPR_HOME_ADDRESS seems to be read only
 */


QContactWinCEEngine::QContactWinCEEngine(ContactWinceFactory* factory, const QString& engineName, const QMap<QString, QString>& , QContactManager::Error* error)
    : d(new QContactWinCEEngineData)
{
    *error = QContactManager::NoError;
    
    buildHashForContactDetailToPoomPropId();
    d->m_engineName = engineName;
    d->m_factory = factory;
    if (SUCCEEDED(d->m_cominit.hr())) {
        if (SUCCEEDED(CoCreateInstance(CLSID_Application, NULL,
                                       CLSCTX_INPROC_SERVER, IID_IPOutlookApp2,
                                       reinterpret_cast<void **>(&d->m_app)))) {
            if(FAILED(d->m_app->Logon(NULL))) {
                qWarning() << "Failed to log on";
                d->m_app = 0;
            } else {
                if(SUCCEEDED(d->m_app->GetDefaultFolder(olFolderContacts, &d->m_folder))) {
                    if(SUCCEEDED(d->m_folder->get_Items(&d->m_collection))) {
                        // Register/retrieve our custom ids
                        LPCWSTR customIds[4] = { L"QTCONTACTS_PHONE_META", L"QTCONTACTS_EMAIL_META", L"QTCONTACTS_AVATAR_IMAGE_META",  L"QTCONTACTS_AVATAR_VIDEO_META" };
                        CEPROPID outIds[4];

                        if (SUCCEEDED(d->m_app->GetIDsFromNames(4, customIds, PIM_CREATE | CEVT_LPWSTR, outIds))) {
                            d->m_phonemeta = outIds[0];
                            d->m_emailmeta = outIds[1];
                            d->m_avatarImageMeta = outIds[2];
                            d->m_avatarVideoMeta = outIds[3];
                        }

                        // get an IPOLItems2 pointer for the collection, too
                        if (SUCCEEDED(d->m_collection->QueryInterface<IPOlItems2>(&d->m_items2))) {
                            d->m_ids = convertP2QIdList(d->m_collection);
                        }
                    } else {
                        qWarning() << "Failed to get items";
                        d->m_collection = 0;
                    }
                } else {
                    qWarning() << "Failed to get contacts folder";
                    d->m_folder = 0;
                }
            }
        }
    }
    d->m_requestWorker.start();
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
    d->m_factory->resetEngine();
}

QString QContactWinCEEngine::managerName() const
{
    return d->m_engineName;
}

QContact QContactWinCEEngine::contact(const QContactLocalId& contactId, const QtMobility::QContactFetchHint& hint, QContactManager::Error* error) const
{
    // TODO: implementation for definitionRestrictions!
    Q_UNUSED(hint);
    QContact ret;

    // id == 0 gives a bad argument error from POOM, so don't even try it
    if (contactId != 0) {
        // Fetch!
        SimpleComPointer<IItem> item = 0;
        HRESULT hr = d->m_app->GetItemFromOidEx(contactId, 0, &item);
        if (SUCCEEDED(hr)) {
            if (item) {
                *error = QContactManager::NoError;
                ret = convertToQContact(item);
            } else {
                *error = QContactManager::DoesNotExistError;
            }
        } else {
            if (HRESULT_CODE(hr) == ERROR_NOT_FOUND) {
                *error = QContactManager::DoesNotExistError;
            } else {
                qWarning() << "Failed to retrieve contact:" << HRESULT_CODE(hr);
                *error = QContactManager::UnspecifiedError;
            }
        }
    } else {
        *error = QContactManager::DoesNotExistError;
    }
    return ret;
}

bool QContactWinCEEngine::saveContact(QContact* contact, QContactManager::Error* error)
{
    if (contact == 0) {
        *error = QContactManager::BadArgumentError;
        return false;
    }

    QContactChangeSet cs;

    // ensure that the contact's details conform to their definitions
    if (!validateContact(*contact, error)) {
        *error = QContactManager::InvalidDetailError;
        return false;
    }

    SimpleComPointer<IItem> icontact;
    bool wasOld = false;
    // Figure out if this is a new or old contact
    if (d->m_ids.contains(contact->localId())) {
        // update existing contact
        HRESULT hr = d->m_app->GetItemFromOidEx(contact->localId(), 0, &icontact);
        if (SUCCEEDED(hr)) {
            wasOld = true;
        } else {
            if (HRESULT_CODE(hr) == ERROR_NOT_FOUND) {
                // Well, doesn't exist any more
                *error = QContactManager::DoesNotExistError;
                d->m_ids.removeAll(contact->localId());
            } else {
                qWarning() << "Didn't get old contact" << HRESULT_CODE(hr);
                *error = QContactManager::UnspecifiedError;
            }
        }
    } else if (contact->localId() == 0) {
        // new contact!
        SimpleComPointer<IDispatch> idisp = 0;
        HRESULT hr = d->m_collection->Add(&idisp);
        if (SUCCEEDED(hr)) {
            // now get an actual item out of it (was IContact, which is not IItem)
            hr = idisp->QueryInterface<IItem>(&icontact);

            if (SUCCEEDED(hr)) {
            } else {
                qWarning() << "Failed to query interface" << HRESULT_CODE(hr);
                *error = QContactManager::UnspecifiedError;
            }
        } else {
            qWarning() << "Failed to create contact: "<< HRESULT_CODE(hr);
            *error = QContactManager::OutOfMemoryError;
        }
    } else {
        // Saving a contact with a non zero id, but that doesn't exist
        *error = QContactManager::DoesNotExistError;
    }

    if (icontact) {
        // Convert our QContact to the Icontact (via setProps)
        if (convertFromQContact(*contact, icontact, *error)) {
            HRESULT hr = icontact->Save();
            if (SUCCEEDED(hr)) {
                // yay! we also need to set the new contact id
                long oid = 0;
                hr = icontact->get_Oid(&oid);
                if (SUCCEEDED(hr)) {
                    *error = QContactManager::NoError;
                    QContact c = this->contact((QContactLocalId)oid, QContactFetchHint(), error);
                    
                    if (*error == QContactManager::NoError) {
                        *contact = c;
                        if (wasOld) {
                            cs.insertChangedContact(contact->localId());
                        } else {
                            cs.insertAddedContact(contact->localId());
                            d->m_ids.append(contact->localId());
                        }
                    }

                    cs.emitSignals(this);
                    return true;
                }
                qWarning() << "Saved contact, but couldn't retrieve id again??" << HRESULT_CODE(hr);
                // Blargh.
                *error = QContactManager::UnspecifiedError;
            } else {
                qWarning() << "Failed to save contact" << HRESULT_CODE(hr);
            }
        } else {
            qWarning() << "Failed to convert contact";
        }
    }

    // error should have been set.
    return false;
}

bool QContactWinCEEngine::removeContact(const QContactLocalId& contactId, QContactManager::Error* error)
{
    // Fetch an IItem* for this
    if (contactId != 0) {
        SimpleComPointer<IItem> item ;
        QContactChangeSet cs;

        HRESULT hr = d->m_app->GetItemFromOidEx(contactId, 0, &item);
        if (SUCCEEDED(hr)) {
            hr = item->Delete();
            if (SUCCEEDED(hr)) {
                *error = QContactManager::NoError;
                d->m_ids.removeAll(contactId);
                cs.insertRemovedContact(contactId);
                cs.emitSignals(this);
                return true;
            }
            qWarning() << "Failed to delete:" << HRESULT_CODE(hr);
            *error = QContactManager::UnspecifiedError;
        } else {
            if (HRESULT_CODE(hr) == ERROR_NOT_FOUND) {
                *error = QContactManager::DoesNotExistError;
            } else {
                qWarning() << "Failed to retrieve item pointer in delete" << HRESULT_CODE(hr);
                *error = QContactManager::UnspecifiedError;
            }
        }
    } else {
        // Id 0 does not exist
        *error = QContactManager::DoesNotExistError;
    }
    return false;
}

QMap<QString, QContactDetailDefinition> QContactWinCEEngine::detailDefinitions(const QString& contactType, QContactManager::Error* error) const
{
    *error = QContactManager::NoError;
    QMap<QString, QMap<QString, QContactDetailDefinition> > defns = QContactManagerEngine::schemaDefinitions();

    // Remove the details we don't support
    defns[contactType].remove(QContactSyncTarget::DefinitionName);
    defns[contactType].remove(QContactGeoLocation::DefinitionName);
    defns[contactType].remove(QContactTimestamp::DefinitionName);
    defns[contactType].remove(QContactGuid::DefinitionName);
    defns[contactType].remove(QContactGender::DefinitionName); // ? Surprising

    QMap<QString, QContactDetailFieldDefinition> fields = defns[contactType][QContactAnniversary::DefinitionName].fields();
    // Simple anniversarys
    fields.remove(QContactAnniversary::FieldCalendarId);
    fields.remove(QContactAnniversary::FieldEvent);
    fields.remove(QContactAnniversary::FieldSubType);
    defns[contactType][QContactAnniversary::DefinitionName].setFields(fields);

    // No logo for organisation
    fields = defns[contactType][QContactOrganization::DefinitionName].fields();
    fields.remove(QContactOrganization::FieldLogoUrl);
    defns[contactType][QContactOrganization::DefinitionName].setFields(fields);

    // No subtypes for these details
    fields = defns[contactType][QContactUrl::DefinitionName].fields();
    fields.remove(QContactUrl::FieldSubType);
    defns[contactType][QContactUrl::DefinitionName].setFields(fields);

    // No contexts for these details
    fields =  defns[contactType][QContactAvatar::DefinitionName].fields();
    fields.remove(QContactDetail::FieldContext);
    defns[contactType][QContactAvatar::DefinitionName].setFields(fields);

    // Simple phone number types (non multiple)
    // defns[QContactPhoneNumber::DefinitionName].fields()[QContactPhoneNumber::FieldSubTypes].dataType = QVariant::String; // XXX doesn't work
    fields = defns[contactType][QContactPhoneNumber::DefinitionName].fields();
    fields[QContactPhoneNumber::FieldSubTypes].allowableValues().removeAll(QString(QLatin1String(QContactPhoneNumber::SubTypeBulletinBoardSystem)));
    fields[QContactPhoneNumber::FieldSubTypes].allowableValues().removeAll(QString(QLatin1String(QContactPhoneNumber::SubTypeLandline)));
    fields[QContactPhoneNumber::FieldSubTypes].allowableValues().removeAll(QString(QLatin1String(QContactPhoneNumber::SubTypeMessagingCapable)));
    fields[QContactPhoneNumber::FieldSubTypes].allowableValues().removeAll(QString(QLatin1String(QContactPhoneNumber::SubTypeModem)));
    fields[QContactPhoneNumber::FieldSubTypes].allowableValues().removeAll(QString(QLatin1String(QContactPhoneNumber::SubTypeVideo)));
    defns[contactType][QContactPhoneNumber::DefinitionName].setFields(fields);
    
    // XXX temporary definitions that we should support but don't yet.
    defns[contactType].remove(QContactOnlineAccount::DefinitionName);

    return defns[contactType];
}


/*! \reimp */
void QContactWinCEEngine::requestDestroyed(QContactAbstractRequest* req)
{
    d->m_requestWorker.removeRequest(req);
}

/*! \reimp */
bool QContactWinCEEngine::startRequest(QContactAbstractRequest* req)
{
    return d->m_requestWorker.addRequest(req);
}

/*! \reimp */
bool QContactWinCEEngine::cancelRequest(QContactAbstractRequest* req)
{
    return  d->m_requestWorker.cancelRequest(req);
}


/*! \reimp */
bool QContactWinCEEngine::waitForRequestFinished(QContactAbstractRequest* req, int msecs)
{
    return d->m_requestWorker.waitRequest(req, msecs) && req->isFinished();
}

/*! \reimp */
bool QContactWinCEEngine::hasFeature(QContactManager::ManagerFeature feature, const QString& contactType) const
{
    Q_UNUSED(contactType);
    // The Windows CE backend is an "isolated" backend
    if (feature == QContactManager::Anonymous)
        return true;

    // Windows CE backend does not support Mutable Definitions, Relationships or Action Preferences
    return false;
}

/* Synthesise the display label of a contact */
QString QContactWinCEEngine::synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error) const
{
    Q_UNUSED(error)
    // The POOM API (well, lack thereof) makes this a bit strange.
    // It's basically just "Last, First" or "Company", if "FileAs" is not set.
    QContactName name = contact.detail<QContactName>();
    QContactOrganization org = contact.detail<QContactOrganization>();

    // Basically we ignore any existing labels for this contact, since we're being
    // asked what the synthesized label would be

    // XXX For greatest accuracy we might be better off converting this contact to
    // a real item (but don't save it), and then retrieve it...
    if (!name.customLabel().isEmpty()) {
        return name.customLabel();
    }
    else if (!name.lastName().isEmpty()) {
        if (!name.firstName().isEmpty()) {
            return QString(QLatin1String("%1, %2")).arg(name.lastName()).arg(name.firstName());
        } else {
            // Just last
            return name.lastName();
        }
    } else if (!name.firstName().isEmpty()) {
        return name.firstName();
    } else if (!org.name().isEmpty()) {
        return org.name();
    } else {
        // XXX grargh.
        return QLatin1String("Unnamed");
    }
}



PROPID QContactWinCEEngine::metaAvatarImage() const
{
    return d->m_avatarImageMeta;
}

PROPID QContactWinCEEngine::metaAvatarVideo() const
{
    return d->m_avatarVideoMeta;
}

PROPID QContactWinCEEngine::metaEmail() const
{
    return d->m_emailmeta;
}

PROPID QContactWinCEEngine::metaPhone() const
{
    return d->m_phonemeta;
}

/*! \reimp */
bool QContactWinCEEngine::isFilterSupported(const QContactFilter& filter) const
{
    switch (filter.type()) {
        case QContactFilter::InvalidFilter:
        case QContactFilter::DefaultFilter:
        case QContactFilter::LocalIdFilter:
        case QContactFilter::ContactDetailFilter:
        case QContactFilter::ContactDetailRangeFilter:
        case QContactFilter::ActionFilter:
        case QContactFilter::IntersectionFilter:
        case QContactFilter::UnionFilter:
            return true;
    }
    return false;
}

/*! \reimp */
int QContactWinCEEngine::managerVersion() const
{
    return QTCONTACTS_VERSION;
}

/*! \reimp */
QList<QContact> QContactWinCEEngine::contacts(const QContactFilter& filter, const QList<QContactSortOrder>& sortOrders, const QContactFetchHint& fetchHint, QContactManager::Error* error) const
{
    QList<QContactLocalId> ids = contactIds(filter, sortOrders, error);
    QList<QContact> cs;
    if (*error == QContactManager::NoError) {
        foreach (const QContactLocalId& id, ids) {
            cs << contact(id, fetchHint, error);
        }
    }
    return cs;
}

/*! \reimp */
bool QContactWinCEEngine::saveContacts(QList<QContact>* contacts, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    bool ret = true;

    for (int j = 0; j < contacts->size(); j++) {
        if (!saveContact(&((*contacts)[j]), error)) {
            ret = false;
        }
        if (*error != QContactManager::NoError) {
            errorMap->insert(j, *error);
        }
    }
    return ret;
}

/*! \reimp */
bool QContactWinCEEngine::removeContacts(const QList<QContactLocalId>& contactIds, QMap<int, QContactManager::Error>* errorMap, QContactManager::Error* error)
{
    bool ret = true;

    for (int j = 0; j < contactIds.size(); j++) {
        if (!removeContact(contactIds[j], error)) {
            ret = false;
        }

        if (*error != QContactManager::NoError) {
            errorMap->insert(j, *error);
        }
    }
    return ret;
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

/* The default constructor of wince contact manager engine factory */
ContactWinceFactory::ContactWinceFactory()
:m_engine(0)
{
}

/* Factory lives here in the basement */
QContactManagerEngine* ContactWinceFactory::engine(const QMap<QString, QString>& parameters, QContactManager::Error* error)
{
    QMutexLocker locker(&m_mutex);
    if (!m_engine) {
        m_engine = new QContactWinCEEngine(this, managerName(), parameters, error);
    }
    return m_engine;
}

QString ContactWinceFactory::managerName() const
{
    return QString("wince");
}

 void ContactWinceFactory::resetEngine()
 {
     QMutexLocker locker(&m_mutex);
     m_engine = 0;
 }
Q_EXPORT_PLUGIN2(qtcontacts_wince, ContactWinceFactory);
