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

#include "support.h"
#include <qmessageaccountid.h>
#include <qmessagefolderid.h>
#include <qmessageid.h>
#include <qmessage_p.h>
#include <qmessagestore.h>
#include <QDataStream>
#include <QFile>
#include <QDebug>

#include <Mapidefs.h>
#include <Mapitags.h>
#include <Mapix.h>
#include <MAPIUtil.h>

// Missing definitions
#ifndef PR_PST_CONFIG_FLAGS
#define PR_PST_CONFIG_FLAGS PROP_TAG( PT_LONG, 0x6770 )
#endif
#ifndef PST_CONFIG_UNICODE
#define PST_CONFIG_UNICODE 0x80000000
#endif
#ifndef PR_PST_PATH_A
#define PR_PST_PATH_A PROP_TAG( PT_STRING8, 0x6700 )
#endif

namespace {

GUID GuidPublicStrings = { 0x00020329, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 };

void doInit()
{
    static QMessageStore *store(0);
    if (!store) {
        store = QMessageStore::instance();
    }
}

IProfAdmin *openProfileAdmin()
{
    IProfAdmin *profAdmin(0);

    HRESULT rv = MAPIAdminProfiles(0, &profAdmin);
    if (HR_FAILED(rv)) {
        qWarning() << "openProfileAdmin: MAPIAdminProfiles failed";
    }

    return profAdmin;
}

IMsgServiceAdmin *openServiceAdmin(const QByteArray &profileName, IProfAdmin *profAdmin)
{
    IMsgServiceAdmin *svcAdmin(0);

    HRESULT rv = profAdmin->AdminServices(reinterpret_cast<LPTSTR>(const_cast<char*>(profileName.data())), 0, 0, 0, &svcAdmin);
    if (HR_FAILED(rv)) {
        qWarning() << "openServiceAdmin: AdminServices failed";
    }

    return svcAdmin;
}

typedef QPair<QByteArray, bool> ProfileDetail;

QList<ProfileDetail> profileDetails(LPPROFADMIN profAdmin)
{
    QList<ProfileDetail> result;

    LPMAPITABLE profileTable(0);
    HRESULT rv = profAdmin->GetProfileTable(0, &profileTable);
    if (HR_SUCCEEDED(rv)) {
        LPSRowSet rows(0);
        SizedSPropTagArray(2, cols) = {2, {PR_DISPLAY_NAME_A, PR_DEFAULT_PROFILE}};
        rv = HrQueryAllRows(profileTable, reinterpret_cast<LPSPropTagArray>(&cols), NULL, NULL, 0, &rows);
        if (HR_SUCCEEDED(rv)) {
            for (uint n = 0; n < rows->cRows; ++n) {
                if (rows->aRow[n].lpProps[0].ulPropTag == PR_DISPLAY_NAME_A) {
                    QByteArray profileName(rows->aRow[n].lpProps[0].Value.lpszA);
                    bool defaultProfile(rows->aRow[n].lpProps[1].Value.b);
                    result.append(qMakePair(profileName, defaultProfile));
                }
            }

            FreeProws(rows);
        } else {
            qWarning() << "profileNames: HrQueryAllRows failed";
        }

        profileTable->Release();
    } else {
        qWarning() << "profileNames: GetProfileTable failed";
    }

    return result;
}

QByteArray findDefaultProfileName(IProfAdmin *profAdmin)
{
    QByteArray defaultProfileName;

    foreach (const ProfileDetail &profile, profileDetails(profAdmin)) {
        if (profile.second) {
            defaultProfileName = profile.first;
            break;
        }
    }

    if (defaultProfileName.isEmpty()) {
        qWarning() << "findDefaultProfileName: no default profile!";
    }

    return defaultProfileName;
}

QByteArray binaryResult(const SPropValue &prop)
{
    return QByteArray(reinterpret_cast<const char*>(prop.Value.bin.lpb), prop.Value.bin.cb);
}

typedef QPair<QPair<QByteArray, QByteArray>, MAPIUID> ServiceDetail;

QList<ServiceDetail> serviceDetails(LPSERVICEADMIN svcAdmin)
{
    QList<ServiceDetail> result;

    IMAPITable *svcTable(0);
    HRESULT rv = svcAdmin->GetMsgServiceTable(0, &svcTable);
    if (HR_SUCCEEDED(rv)) {
        LPSRowSet rows(0);
        SizedSPropTagArray(3, cols) = {3, {PR_SERVICE_NAME_A, PR_DISPLAY_NAME_A, PR_SERVICE_UID}};
        rv = HrQueryAllRows(svcTable, reinterpret_cast<LPSPropTagArray>(&cols), 0, 0, 0, &rows);
        if (HR_SUCCEEDED(rv)) {
            for (uint n = 0; n < rows->cRows; ++n) {
                if (rows->aRow[n].lpProps[0].ulPropTag == PR_SERVICE_NAME_A) {
                    QByteArray svcName(rows->aRow[n].lpProps[0].Value.lpszA);
                    QByteArray displayName;
                    if (rows->aRow[n].lpProps[1].ulPropTag == PR_DISPLAY_NAME_A) {
                        displayName = QByteArray(rows->aRow[n].lpProps[1].Value.lpszA);
                    }
                    MAPIUID svcUid(*(reinterpret_cast<MAPIUID*>(rows->aRow[n].lpProps[2].Value.bin.lpb)));
                    result.append(qMakePair(qMakePair(svcName, displayName), svcUid));
                }
            }

            FreeProws(rows);
        } else {
            qWarning() << "serviceDetails: HrQueryAllRows failed";
        }

        svcTable->Release();
    } else {
        qWarning() << "serviceDetails: GetMsgServiceTable failed";
    }

    return result;
}

typedef QPair<QByteArray, QPair<QByteArray, QByteArray> > StoreDetail;

QList<StoreDetail> storeDetails(LPMAPISESSION session)
{
    QList<StoreDetail> result;

    IMAPITable *storesTable(0);
    HRESULT rv = session->GetMsgStoresTable(0, &storesTable);
    if (HR_SUCCEEDED(rv)) {
        LPSRowSet rows(0);
        SizedSPropTagArray(3, cols) = {3, {PR_DISPLAY_NAME_A, PR_RECORD_KEY, PR_ENTRYID}};
        rv = HrQueryAllRows(storesTable, reinterpret_cast<LPSPropTagArray>(&cols), 0, 0, 0, &rows);
        if (HR_SUCCEEDED(rv)) {
            for (uint n = 0; n < rows->cRows; ++n) {
                SPropValue *props(rows->aRow[n].lpProps);
                if (props[0].ulPropTag == PR_DISPLAY_NAME_A) {
                    QByteArray storeName(props[0].Value.lpszA);
                    QByteArray recordKey(binaryResult(props[1]));
                    QByteArray entryId(binaryResult(props[2]));
                    result.append(qMakePair(storeName, qMakePair(recordKey, entryId)));
                }
            }

            FreeProws(rows);
        } else {
            qWarning() << "storeDetails: HrQueryAllRows failed";
        }

        storesTable->Release();
    } else {
        qWarning() << "storeDetails: GetMsgStoresTable failed";
    }

    return result;
}

QMessageAccountId accountIdFromRecordKey(const QByteArray &recordKey)
{
    QByteArray encodedId;
    {
        QDataStream encodedIdStream(&encodedId, QIODevice::WriteOnly);
        encodedIdStream << recordKey;
    }

    return QMessageAccountId(encodedId.toBase64());
}

QMessageFolderId folderIdFromProperties(const QByteArray &recordKey, const QByteArray &entryId, const QByteArray &storeKey)
{
    QByteArray encodedId;
    {
        QDataStream encodedIdStream(&encodedId, QIODevice::WriteOnly);
        encodedIdStream << recordKey << storeKey;
        if (!entryId.isEmpty()) {
            encodedIdStream << entryId;
        }
    }

    return QMessageFolderId(encodedId.toBase64());
}

QByteArray objectProperty(IMAPIProp *object, ULONG tag)
{
    QByteArray result;

    if (object) {
        SPropValue *prop(0);
        HRESULT rv = HrGetOneProp(object, tag, &prop);
        if (HR_SUCCEEDED(rv)) {
            result = binaryResult(*prop);

            MAPIFreeBuffer(prop);
        } else {
            qWarning() << "objectProperty: HrGetOneProp failed";
        }
    }

    return result;
}

ULONG createNamedProperty(IMAPIProp *object, const QString &name)
{
    ULONG result = 0;

    if (!name.isEmpty()) {
        wchar_t *nameBuffer = new wchar_t[name.length() + 1];
        name.toWCharArray(nameBuffer);
        nameBuffer[name.length()] = 0;

        MAPINAMEID propName = { 0 };
        propName.lpguid = &GuidPublicStrings;
        propName.ulKind = MNID_STRING;
        propName.Kind.lpwstrName = nameBuffer;

        LPMAPINAMEID propNames = &propName;

        SPropTagArray *props;
        HRESULT rv = object->GetIDsFromNames(1, &propNames, MAPI_CREATE, &props);
        if (HR_SUCCEEDED(rv)) {
            result = props->aulPropTag[0] | PT_UNICODE;

            MAPIFreeBuffer(props);
        } else {
            qWarning() << "createNamedProperty: GetIDsFromNames failed";
        }

        delete [] nameBuffer;
    }

    return result;
}

ULONG getNamedPropertyTag(IMAPIProp *object, const QString &name)
{
    ULONG result = 0;

    if (!name.isEmpty()) {
        wchar_t *nameBuffer = new wchar_t[name.length() + 1];
        name.toWCharArray(nameBuffer);
        nameBuffer[name.length()] = 0;

        MAPINAMEID propName = { 0 };
        propName.lpguid = &GuidPublicStrings;
        propName.ulKind = MNID_STRING;
        propName.Kind.lpwstrName = nameBuffer;

        LPMAPINAMEID propNames = &propName;

        SPropTagArray *props;
        HRESULT rv = object->GetIDsFromNames(1, &propNames, 0, &props);
        if (HR_SUCCEEDED(rv)) {
            if (props->aulPropTag[0] != PT_ERROR) {
                result = props->aulPropTag[0] | PT_UNICODE;
            }

            MAPIFreeBuffer(props);
        } else {
            qWarning() << "getNamedPropertyTag: GetIDsFromNames failed";
        }

        delete [] nameBuffer;
    }

    return result;
}

bool setNamedProperty(IMAPIProp *object, ULONG tag, const QString &value)
{
    if (object && tag && !value.isEmpty()) {
        SPropValue prop = { 0 };
        prop.ulPropTag = tag;
        prop.Value.LPSZ = reinterpret_cast<LPTSTR>(const_cast<quint16*>(value.utf16()));

        HRESULT rv = object->SetProps(1, &prop, 0);
        if (HR_SUCCEEDED(rv)) {
            return true;
        } else {
            qWarning() << "setNamedProperty: SetProps failed";
        }
    }

    return false;
}

QString getNamedProperty(IMAPIProp *object, ULONG tag)
{
    QString result;

    if (object && tag) {
        SPropValue *prop(0);
        HRESULT rv = HrGetOneProp(object, tag, &prop);
        if (HR_SUCCEEDED(rv)) {
            result = QString::fromUtf16(reinterpret_cast<quint16*>(prop->Value.LPSZ));

            MAPIFreeBuffer(prop);
        } else if (rv != MAPI_E_NOT_FOUND) {
            qWarning() << "getNamedProperty: HrGetOneProp failed";
        }
    }

    return result;
}

IProviderAdmin *serviceProvider(const MAPIUID &svcUid, LPSERVICEADMIN svcAdmin)
{
    IProviderAdmin *provider(0);

    if (svcAdmin) {
        HRESULT rv = svcAdmin->AdminProviders(const_cast<MAPIUID*>(&svcUid), 0, &provider);
        if (HR_FAILED(rv)) {
            provider = 0;
            qWarning() << "serviceProvider: AdminProviders failed";
        }
    }

    return provider;
}

MAPIUID findProviderUid(const QByteArray &name, IProviderAdmin *providerAdmin)
{
    MAPIUID result = { 0 };
    IMAPITable *providerTable(0);
    HRESULT rv = providerAdmin->GetProviderTable(0, &providerTable);
    if (HR_SUCCEEDED(rv)) {
        LPSRowSet rows(0);
        SizedSPropTagArray(2, cols) = {2, {PR_SERVICE_NAME_A, PR_PROVIDER_UID}};
        rv = HrQueryAllRows(providerTable, reinterpret_cast<LPSPropTagArray>(&cols), 0, 0, 0, &rows);
        if (HR_SUCCEEDED(rv)) {
            for (uint n = 0; n < rows->cRows; ++n) {
                SPropValue *props(rows->aRow[n].lpProps);
                if (props[0].ulPropTag == PR_SERVICE_NAME_A) {
                    QByteArray serviceName(props[0].Value.lpszA);
                    if (name.isEmpty() || (serviceName.toLower() == name.toLower())) {
                        result = *(reinterpret_cast<MAPIUID*>(props[1].Value.bin.lpb));
                        break;
                    }
                }
            }

            FreeProws(rows);
        } else {
            qWarning() << "findProviderUid: HrQueryAllRows failed";
        }

        providerTable->Release();
    } else {
        qWarning() << "findProviderUid: GetProviderTable failed";
    }

    return result;
}

IProfSect *openProfileSection(const MAPIUID &providerUid, IProviderAdmin *providerAdmin)
{
    IProfSect *profileSection(0);

    // Bypass the MAPI_E_NO_ACCESS_ERROR, as described at http://support.microsoft.com/kb/822977
    const ULONG MAPI_FORCE_ACCESS = 0x00080000;

    HRESULT rv = providerAdmin->OpenProfileSection(const_cast<MAPIUID*>(&providerUid), 0, MAPI_FORCE_ACCESS, &profileSection);
    if (HR_FAILED(rv)) {
        qWarning() << "openProfileSection: OpenProfileSection failed";
        profileSection = 0;
    }

    return profileSection;
}

template<typename T>
bool isEmpty(const T &v)
{
    const char empty[sizeof(T)] = { 0 };
    return (memcmp(empty, &v, sizeof(T)) == 0);
}

bool deleteExistingService(const MAPIUID &svcUid, LPSERVICEADMIN svcAdmin)
{
    if (svcAdmin) {
        QByteArray storePath;

        // Find the Provider for this service
        IProviderAdmin *provider = serviceProvider(svcUid, svcAdmin);
        if (provider) {
            MAPIUID providerUid = findProviderUid("MSUPST MS", provider);
            if (!isEmpty(providerUid)) {
                IProfSect *profileSection = openProfileSection(providerUid, provider);
                if (profileSection) {
                    SPropValue *prop(0);
                    HRESULT rv = HrGetOneProp(profileSection, PR_PST_PATH_A, &prop);
                    if (HR_SUCCEEDED(rv)) {
                        storePath = QByteArray(prop->Value.lpszA);

                        MAPIFreeBuffer(prop);
                    } else {
                        qWarning() << "deleteExistingService: HrGetOneProp failed";
                    }

                    profileSection->Release();
                }
            }

            provider->Release();
        }

        if (!storePath.isEmpty()) {
            // Delete the existing service
            HRESULT rv = svcAdmin->DeleteMsgService(const_cast<MAPIUID*>(&svcUid));
            if (HR_SUCCEEDED(rv)) {
                // Delete the storage file
                if (QFile::exists(storePath)) {
                    if (!QFile::remove(storePath)) {
                        qWarning() << "deleteExistingService: Unable to remove PST file at:" << storePath;
                    }
                }
                return true;
            } else {
                qWarning() << "deleteExistingService: DeleteMsgService failed";
            }
        }
    }

    return false;
}

QByteArray defaultProfile()
{
    QByteArray result;

    LPPROFADMIN profAdmin(0);
    HRESULT rv = MAPIAdminProfiles(0, &profAdmin);
    if (HR_SUCCEEDED(rv)) {
        // Find the default profile
        foreach (const ProfileDetail &profile, profileDetails(profAdmin)) {
            if (profile.second) {
                result = profile.first;
                break;
            }
        }
    } else {
        qWarning() << "defaultProfile: MAPIAdminProfiles failed";
    }

    return result;
}

IMAPISession *profileSession(const QByteArray &profileName)
{
    IMAPISession *session(0);

    if (!profileName.isEmpty()) {
        // Open a session on the profile
        QByteArray name(profileName);
        HRESULT rv = MAPILogonEx(0, reinterpret_cast<LPTSTR>(name.data()), 0, MAPI_EXTENDED | MAPI_NEW_SESSION | MAPI_NO_MAIL, &session);
        if (HR_FAILED(rv)) {
            session = 0;
            qWarning() << "profileSession: MAPILogonEx failed";
        }
    }

    return session;
}

IMsgStore *openStore(const QByteArray &entryId, IMAPISession* session)
{
    IMsgStore *store(0);

    if (session && !entryId.isEmpty()) {
        HRESULT rv = session->OpenMsgStore(0, entryId.length(), reinterpret_cast<LPENTRYID>(const_cast<char*>(entryId.data())), 0, MDB_NO_MAIL | MDB_WRITE, reinterpret_cast<LPMDB*>(&store));
        if (HR_FAILED(rv)) {
            store = 0;
            qWarning() << "openStore: OpenMsgStore failed";
        }
    }

    return store;
}

IMsgStore *openStoreByName(const QByteArray &storeName, IMAPISession* session)
{
    IMsgStore *store(0);

    if (session && !storeName.isEmpty()) {
        QByteArray entryId;

        // Find the store with the specified name
        IMAPITable *storesTable(0);
        HRESULT rv = session->GetMsgStoresTable(0, &storesTable);
        if (HR_SUCCEEDED(rv)) {
            LPSRowSet rows(0);
            SizedSPropTagArray(2, cols) = {2, {PR_DISPLAY_NAME_A, PR_ENTRYID}};
            rv = HrQueryAllRows(storesTable, reinterpret_cast<LPSPropTagArray>(&cols), 0, 0, 0, &rows);
            if (HR_SUCCEEDED(rv)) {
                for (uint n = 0; n < rows->cRows; ++n) {
                    if (rows->aRow[n].lpProps[0].ulPropTag == PR_DISPLAY_NAME_A) {
                        QByteArray name(rows->aRow[n].lpProps[0].Value.lpszA);
                        if (name.toLower() == storeName.toLower()) {
                            entryId = binaryResult(rows->aRow[n].lpProps[1]);
                            break;
                        }
                    }
                }

                FreeProws(rows);
            } else {
                qWarning() << "openStoreByName: HrQueryAllRows failed";
            }

            storesTable->Release();
        } else {
            qWarning() << "openStoreByName: GetMsgStoresTable failed";
        }

        if (!entryId.isEmpty()) {
            store = openStore(entryId, session);
        }
    }

    return store;
}

QByteArray rootFolderEntryId(IMsgStore *store)
{
    return objectProperty(store, PR_IPM_SUBTREE_ENTRYID);
}

IMAPIFolder *openFolder(const QByteArray &entryId, IMsgStore *store)
{
    IMAPIFolder *folder(0);

    if (store && !entryId.isEmpty()) {
        ULONG type(0);
        QByteArray entry(entryId);
        HRESULT rv = store->OpenEntry(entry.length(), reinterpret_cast<LPENTRYID>(entry.data()), 0, MAPI_MODIFY, &type, reinterpret_cast<LPUNKNOWN*>(&folder));
        if (HR_FAILED(rv)) {
            folder = 0;
            qWarning() << "openFolder: OpenEntry failed";
        }
    }

    return folder;
}

IMAPIFolder *openFolder(const QByteArray &entryId, IMAPIFolder *container)
{
    IMAPIFolder *folder(0);

    if (container && !entryId.isEmpty()) {
        ULONG type(0);
        QByteArray entry(entryId);
        HRESULT rv = container->OpenEntry(entry.length(), reinterpret_cast<LPENTRYID>(entry.data()), 0, MAPI_MODIFY, &type, reinterpret_cast<LPUNKNOWN*>(&folder));
        if (HR_FAILED(rv)) {
            folder = 0;
            qWarning() << "openFolder: OpenEntry failed";
        }
    }

    return folder;
}

QList<QByteArray> subFolderEntryIds(IMAPIFolder *folder)
{
    QList<QByteArray> result;

    if (folder) {
        IMAPITable *hierarchyTable(0);
        HRESULT rv = folder->GetHierarchyTable(MAPI_UNICODE, &hierarchyTable);
        if (HR_SUCCEEDED(rv)) {
            LPSRowSet rows(0);
            SizedSPropTagArray(2, cols) = {2, {PR_OBJECT_TYPE, PR_ENTRYID}};
            rv = HrQueryAllRows(hierarchyTable, reinterpret_cast<LPSPropTagArray>(&cols), NULL, NULL, 0, &rows);
            if (HR_SUCCEEDED(rv)) {
                for (uint n = 0; n < rows->cRows; ++n) {
                    if ((rows->aRow[n].lpProps[0].ulPropTag == PR_OBJECT_TYPE) &&
                        (rows->aRow[n].lpProps[0].Value.l == MAPI_FOLDER)) {
                        result.append(binaryResult(rows->aRow[n].lpProps[1]));
                    }
                }

                FreeProws(rows);
            } else {
                qWarning() << "subFolderEntryIds: HrQueryAllRows failed";
            }

            hierarchyTable->Release();
        }
    }

    return result;
}

IMAPIFolder *subFolder(const QString &path, IMAPIFolder *folder)
{
    IMAPIFolder *result(0);

    if (folder && !path.isEmpty()) {
        // Find all folders in the current folder
        foreach (const QByteArray &entryId, subFolderEntryIds(folder)) {
            IMAPIFolder *childFolder = openFolder(entryId, folder);
            if (childFolder) {
                ULONG tag = getNamedPropertyTag(childFolder, "path");
                if (tag) {
                    QString childPath(getNamedProperty(childFolder, tag));
                    if (childPath == path) {
                        // This is the folder we're looking for
                        result = childFolder;
                    } else if (path.startsWith(childPath)) {
                        // This must be a parent of the folder we're looking for
                        result = subFolder(path, childFolder);
                    }
                }

                if (childFolder != result) {
                    childFolder->Release();
                }
            }

            if (result) {
                break;
            }
        }
    }

    return result;
}

IMAPIFolder *createFolder(const QString &name, IMAPIFolder *folder)
{
    IMAPIFolder *newFolder(0);

    if (folder && !name.isEmpty()) {
        HRESULT rv = folder->CreateFolder(FOLDER_GENERIC, reinterpret_cast<LPTSTR>(const_cast<quint16*>(name.utf16())), 0, 0, MAPI_UNICODE, &newFolder);
        if (HR_FAILED(rv)) {
            newFolder = 0;
            qWarning() << "createFolder: CreateFolder failed";
        }
    }

    return newFolder;
}

QByteArray folderRecordKey(IMAPIFolder *folder)
{
    return objectProperty(folder, PR_RECORD_KEY);
}

QByteArray folderEntryId(IMAPIFolder *folder)
{
    return objectProperty(folder, PR_ENTRYID);
}

QByteArray storeRecordKey(IMsgStore *store)
{
    return objectProperty(store, PR_RECORD_KEY);
}

}

namespace Support {

void clearMessageStore()
{
    // Ensure the store is instantiated
    doInit();

    // Remove any existing stores that we added previously
    IProfAdmin *profAdmin = openProfileAdmin();
    if (profAdmin) {
        QByteArray defaultProfileName = findDefaultProfileName(profAdmin);
        if (!defaultProfileName.isEmpty()) {
            IMAPISession *session = profileSession(defaultProfileName);
            if (session) {
                IMsgServiceAdmin *svcAdmin = openServiceAdmin(defaultProfileName, profAdmin);
                if (svcAdmin) {
                    char *providerName = "MSUPST MS";
                    QList<MAPIUID> obsoleteUids;

                    foreach (const ServiceDetail &svc, serviceDetails(svcAdmin)) {
                        // Find all services that have our provider
                        if (svc.first.first.toLower() == QByteArray(providerName).toLower()) {
                            IMsgStore *store = openStoreByName(svc.first.second, session);
                            if (store) {
                                // Did we create this store
                                ULONG tag = getNamedPropertyTag(store, "origin");
                                if (tag) {
                                    if (getNamedProperty(store, tag) == "QMF") {
                                        // This is an existing service we need to remove
                                        obsoleteUids.append(svc.second);
                                    }
                                }

                                store->Release();
                            }
                        }
                    }

                    foreach (const MAPIUID &uid, obsoleteUids) {
                        deleteExistingService(uid, svcAdmin);
                    }

                    svcAdmin->Release();
                }

                session->Release();
            }
        }

        profAdmin->Release();
    }
}

QMessageAccountId addAccount(const Parameters &params)
{
    QMessageAccountId result;

    doInit();

    QString accountName(params["name"]);
    QString fromAddress(params["fromAddress"]);

    if (!accountName.isEmpty()) {
        // Profile name must be ASCII
        QByteArray name(accountName.toAscii());

        // See if a profile exists with the given name
        IProfAdmin *profAdmin = openProfileAdmin();
        if (profAdmin) {
            QByteArray defaultProfileName = findDefaultProfileName(profAdmin);
            if (!defaultProfileName.isEmpty()) {
                IMAPISession *session = profileSession(defaultProfileName);
                if (session) {
                    IMsgServiceAdmin *svcAdmin = openServiceAdmin(defaultProfileName, profAdmin);
                    if (svcAdmin) {
                        char *providerName = "MSUPST MS";
                        QList<QByteArray> existingServices;

                        foreach (const ServiceDetail &svc, serviceDetails(svcAdmin)) {
                            // Find all services that have our provider
                            if (svc.first.first.toLower() == QByteArray(providerName).toLower()) {
                                existingServices.append(QByteArray(reinterpret_cast<const char*>(&svc.second), sizeof(MAPIUID)));
                            }
                        }

                        // Create a message service for this profile using the standard provider
                        HRESULT rv = svcAdmin->CreateMsgService(reinterpret_cast<LPTSTR>(providerName), 0, 0, 0);
                        if (HR_SUCCEEDED(rv)) {
                            // Find which of the now-extant services was not in the previous set
                            foreach (const ServiceDetail &svc, serviceDetails(svcAdmin)) {
                                QByteArray uidData(reinterpret_cast<const char*>(&svc.second), sizeof(MAPIUID));
                                if ((svc.first.first.toLower() == QByteArray(providerName).toLower()) &&
                                    !existingServices.contains(uidData)) {
                                    // Create a .PST message store for this service
                                    QByteArray path(QString("%1.pst").arg(name.constData()).toAscii());

                                    SPropValue props[3] = { 0 };
                                    props[0].ulPropTag = PR_DISPLAY_NAME_A;
                                    props[0].Value.lpszA = name.data();
                                    props[1].ulPropTag = PR_PST_PATH_A;
                                    props[1].Value.lpszA = path.data();
                                    props[2].ulPropTag = PR_PST_CONFIG_FLAGS;
                                    props[2].Value.l = PST_CONFIG_UNICODE;

                                    MAPIUID svcUid = svc.second;
                                    rv = svcAdmin->ConfigureMsgService(&svcUid, 0, 0, 3, props);
                                    if (HR_SUCCEEDED(rv)) {
                                        foreach (const StoreDetail &store, storeDetails(session)) {
                                            if (store.first.toLower() == name.toLower()) {
                                                result = accountIdFromRecordKey(store.second.first);

                                                IMsgStore *newStore = openStore(store.second.second, session);
                                                if (newStore) {
                                                    // Add an origin tag to this store
                                                    ULONG originTag = createNamedProperty(newStore, "origin");
                                                    if (originTag) {
                                                        setNamedProperty(newStore, originTag, "QMF");
                                                    }

                                                    if (!fromAddress.isEmpty()) {
                                                        // Try to set the address for this service, as a property of the store
                                                        ULONG addressTag = createNamedProperty(newStore, "fromAddress");
                                                        if (addressTag) {
                                                            setNamedProperty(newStore, addressTag, fromAddress);
                                                        }
                                                    }

                                                    newStore->Release();
                                                }
                                                break;
                                            }
                                        }
                                    } else {
                                        qWarning() << "ConfigureMsgService failed";
                                    }
                                    break;
                                }
                            }
                        } else {
                            qWarning() << "CreateMsgService failed";
                        }

                        svcAdmin->Release();
                    }

                    session->Release();
                }
            }

            profAdmin->Release();
        }
    }

    return result;
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderId addFolder(const Parameters &params)
{
    QMessageFolderId result;

    doInit();

    QString folderPath(params["path"]);
    QString folderName(params["displayName"]);
    QString parentPath(params["parentFolderPath"]);
    QByteArray accountName(params["parentAccountName"].toAscii());

    if (folderName.isEmpty()) {
        int index = folderPath.lastIndexOf('/');
        folderName = folderPath.mid(index + 1);
    }

    if (!folderName.isEmpty() && !folderPath.isEmpty() && !accountName.isEmpty()) {
        // Open a session on the default profile
        IMAPISession *session(profileSession(defaultProfile()));
        if (session) {
            // Open the store for modification
            IMsgStore *store = openStoreByName(accountName, session);
            if (store) {
                // Open the root folder for modification
                IMAPIFolder *folder = openFolder(rootFolderEntryId(store), store);
                if (folder) {
                    // Find the parent folder for the new folder
                    if (!parentPath.isEmpty()) {
                        IMAPIFolder *parentFolder = subFolder(parentPath, folder);
                        folder->Release();
                        folder = parentFolder;
                    }

                    if (folder) {
                        IMAPIFolder *newFolder = createFolder(folderName, folder);
                        if (newFolder) {
                            ULONG tag = createNamedProperty(newFolder, "path");
                            if (tag) {
                                setNamedProperty(newFolder, tag, folderPath);
                            }

                            QByteArray recordKey = folderRecordKey(newFolder);
                            QByteArray entryId = folderEntryId(newFolder);
                            QByteArray storeKey = storeRecordKey(store);
                            result = folderIdFromProperties(recordKey, entryId, storeKey);

                            newFolder->Release();
                        }

                        folder->Release();
                    } else {
                        qWarning() << "Unable to locate parent folder for addition";
                    }
                } else {
                    qWarning() << "Unable to open root folder for addition";
                }

                store->Release();
            }

            session->Release();
        }
    }

    return result;
}
#endif

QMessageId addMessage(const Parameters &params)
{
    QString parentAccountName(params["parentAccountName"]);
    QString parentFolderPath(params["parentFolderPath"]);
    QString to(params["to"]);
    QString from(params["from"]);
    QString date(params["date"]);
    QString receivedDate(params["receivedDate"]);
    QString subject(params["subject"]);
    QString text(params["text"]);
    QString priority(params["priority"]);
    QString size(params["size"]);
    QString type(params["type"]);
    QString read(params["status-read"]);
    QString hasAttachments(params["status-hasAttachments"]);

    if (!to.isEmpty() && !from.isEmpty() && !date.isEmpty() && !subject.isEmpty() &&
        !parentAccountName.isEmpty() && !parentFolderPath.isEmpty()) {
        // Find the named account
        QMessageAccountIdList accountIds(QMessageStore::instance()->queryAccounts(QMessageAccountFilter::byName(parentAccountName)));
#if defined(Q_OS_WIN) && !defined(ACCOUNT_FILTERING_IMPLEMENTED)
{
    // Not implemented yet...
    QMessageAccountIdList::iterator it = accountIds.begin(), end = accountIds.end();
    while (it != end) {
        QMessageAccount acct(*it);
        if (acct.name() == parentAccountName) {
            accountIds.clear();
            accountIds.append(acct.id());
            break;
        }
        if (++it == end) {
            accountIds.clear();
        }
    }
}
#endif
        if (accountIds.count() == 1) {
            // Find the specified folder
            QMessageFolderFilter filter(QMessageFolderFilter::byPath(parentFolderPath) & QMessageFolderFilter::byParentAccountId(accountIds.first()));
            QMessageFolderIdList folderIds(QMessageStore::instance()->queryFolders(filter));
#if defined(Q_OS_WIN) && !defined(FOLDER_FILTERING_IMPLEMENTED)
{
    // Keys aren't implemented yet...
    QMessageFolderIdList::iterator it = folderIds.begin(), end = folderIds.end();
    while (it != end) {
        QMessageFolder fldr(*it);
        if ((fldr.path() == parentFolderPath) && (fldr.parentAccountId() == accountIds.first())) {
            folderIds.clear();
            folderIds.append(fldr.id());
            break;
        }
        if (++it == end) {
            folderIds.clear();
        }
    }
}
#endif
            if (folderIds.count() == 1) {
                QMessage message;

                message.setParentAccountId(accountIds.first());
                // TODO: is this to be added?
                //message.setParentFolderId(folderIds.first());
                message.d_ptr->_parentFolderId = folderIds.first();

                QList<QMessageAddress> toList;
                foreach (const QString &addr, to.split(",")) {
                    toList.append(QMessageAddress(addr.trimmed(), QMessageAddress::Email));
                }
                message.setTo(toList);
                message.setFrom(QMessageAddress(from, QMessageAddress::Email));
                message.setSubject(subject);

                QDateTime dt(QDateTime::fromString(date, Qt::ISODate));
                dt.setTimeSpec(Qt::UTC);
                message.setDate(dt);

                if (type.isEmpty()) {
                    message.setType(QMessage::Email);
                } else {
                    if (type.toLower() == "mms") {
                        message.setType(QMessage::Mms);
                    } else if (type.toLower() == "sms") {
                        message.setType(QMessage::Sms);
                    } else if (type.toLower() == "xmpp") {
                        message.setType(QMessage::Xmpp);
                    } else {
                        message.setType(QMessage::Email);
                    }
                }

                if (!receivedDate.isEmpty()) {
                    QDateTime dt(QDateTime::fromString(receivedDate, Qt::ISODate));
                    dt.setTimeSpec(Qt::UTC);
                    message.setReceivedDate(dt);
                }

                if (!priority.isEmpty()) {
                    if (priority.toLower() == "high") {
                        message.setPriority(QMessage::HighPriority);
                    } else if (priority.toLower() == "low") {
                        message.setPriority(QMessage::LowPriority);
                    }
                }

                /*
                if (!size.isEmpty()) {
                    // TODO: add setSize to QMessageContentCOntainer
                    message.setSize(size.toUInt());
                }
                */

                if (!text.isEmpty()) {
                    message.setContentType("text");
                    message.setContentSubType("plain");
                    message.setContent(text);
                }

                QMessage::StatusFlags flags(0);
                if (read.toLower() == "true") {
                    flags |= QMessage::Read;
                }
                if (hasAttachments.toLower() == "true") {
                    flags |= QMessage::HasAttachments;
                }
                message.setStatus(flags);

                Parameters::const_iterator it = params.begin(), end = params.end();
                /*
                for ( ; it != end; ++it) {
                    if (it.key().startsWith("custom-")) {
                        message.setCustomField(it.key().mid(7), it.value());
                    }
                }*/

                if (!QMessageStore::instance()->addMessage(&message)) {
                    qWarning() << "Unable to addMessage:" << to << from << date << subject;
                } else {
                    return message.id();
                }
            } else {
                qWarning() << "Unable to locate parent folder:" << parentFolderPath;
            }
        } else {
            qWarning() << "Unable to locate parent account:" << parentAccountName;
        }
    } else {
        qWarning() << "Necessary information missing";
    }

    return QMessageId();
}

}

