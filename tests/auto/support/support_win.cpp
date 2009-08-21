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

typedef QPair<QByteArray, QByteArray> StoreDetail;

QList<StoreDetail> storeDetails(LPMAPISESSION session)
{
    QList<StoreDetail> result;

    IMAPITable *storesTable(0);
    HRESULT rv = session->GetMsgStoresTable(0, &storesTable);
    if (HR_SUCCEEDED(rv)) {
        LPSRowSet rows(0);
        SizedSPropTagArray(2, cols) = {2, {PR_DISPLAY_NAME_A, PR_RECORD_KEY}};
        rv = HrQueryAllRows(storesTable, reinterpret_cast<LPSPropTagArray>(&cols), 0, 0, 0, &rows);
        if (HR_SUCCEEDED(rv)) {
            for (uint n = 0; n < rows->cRows; ++n) {
                if (rows->aRow[n].lpProps[0].ulPropTag == PR_DISPLAY_NAME_A) {
                    QByteArray storeName(rows->aRow[n].lpProps[0].Value.lpszA);
                    QByteArray recordKey(binaryResult(rows->aRow[n].lpProps[1]));
                    result.append(qMakePair(storeName, recordKey));
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
            result = PROP_TAG( PT_UNICODE, props->aulPropTag[0] );

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
            result = PROP_TAG( PT_UNICODE, props->aulPropTag[0] );

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
        prop.ulPropTag = PROP_TAG( PT_UNICODE, tag );
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
        HRESULT rv = HrGetOneProp(object, PROP_TAG( PT_UNICODE, tag ), &prop);
        if (HR_SUCCEEDED(rv)) {
            result = QString::fromUtf16(reinterpret_cast<quint16*>(prop->Value.LPSZ));

            MAPIFreeBuffer(prop);
        } else if (rv != MAPI_E_NOT_FOUND) {
            qWarning() << "getNamedProperty: HrGetOneProp failed";
        }
    }

    return result;
}

IProviderAdmin *serviceProvider(MAPIUID &svcUid, LPSERVICEADMIN svcAdmin)
{
    IProviderAdmin *provider(0);

    if (svcAdmin) {
        HRESULT rv = svcAdmin->AdminProviders(&svcUid, 0, &provider);
        if (HR_FAILED(rv)) {
            provider = 0;
            qWarning() << "serviceProvider: AdminProviders failed";
        }
    }

    return provider;
}

bool deleteExistingService(MAPIUID &svcUid, LPSERVICEADMIN svcAdmin)
{
    if (svcAdmin) {
        QByteArray storePath;

        // Find the Provider for this service
        IProviderAdmin *provider = serviceProvider(svcUid, svcAdmin);
        if (provider) {
            IMAPITable *providerTable(0);
            HRESULT rv = provider->GetProviderTable(0, &providerTable);
            if (HR_SUCCEEDED(rv)) {
                MAPIUID providerUid = { 0 };
                bool foundProvider(false);

                LPSRowSet rows(0);
                SizedSPropTagArray(2, cols) = {2, {PR_SERVICE_NAME_A, PR_PROVIDER_UID}};
                rv = HrQueryAllRows(providerTable, reinterpret_cast<LPSPropTagArray>(&cols), 0, 0, 0, &rows);
                if (HR_SUCCEEDED(rv)) {
                    for (uint n = 0; n < rows->cRows; ++n) {
                        if (rows->aRow[n].lpProps[0].ulPropTag == PR_SERVICE_NAME_A) {
                            QByteArray serviceName(rows->aRow[n].lpProps[0].Value.lpszA);
                            if (serviceName.toUpper() == "MSUPST MS") {
                                providerUid = *(reinterpret_cast<MAPIUID*>(rows->aRow[n].lpProps[1].Value.bin.lpb));
                                foundProvider = true;
                                break;
                            }
                        }
                    }

                    FreeProws(rows);
                } else {
                    qWarning() << "deleteExistingService: HrQueryAllRows failed";
                }

                providerTable->Release();

                if (foundProvider) {
                    // Bypass the MAPI_E_NO_ACCESS_ERROR, as described at http://support.microsoft.com/kb/822977
                    const ULONG MAPI_FORCE_ACCESS = 0x00080000;

                    IProfSect *profileSection(0);
                    HRESULT rv = provider->OpenProfileSection(&providerUid, 0, MAPI_FORCE_ACCESS, &profileSection);
                    if (HR_SUCCEEDED(rv)) {
                        SPropValue *prop(0);
                        rv = HrGetOneProp(profileSection, PR_PST_PATH_A, &prop);
                        if (HR_SUCCEEDED(rv)) {
                            storePath = QByteArray(prop->Value.lpszA);

                            MAPIFreeBuffer(prop);
                        } else {
                            qWarning() << "deleteExistingService: HrGetOneProp failed";
                        }

                        profileSection->Release();
                    } else {
                        qWarning() << "deleteExistingService: OpenProfileSection failed";
                    }
                }
            } else {
                qWarning() << "deleteExistingService: GetProviderTable failed";
            }

            provider->Release();
        }

        if (!storePath.isEmpty()) {
            // Delete the existing service
            HRESULT rv = svcAdmin->DeleteMsgService(&svcUid);
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

LPMAPISESSION profileSession(const QByteArray &profileName)
{
    LPMAPISESSION session(0);

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


IMsgStore *openStore(const QByteArray &storeName, IMAPISession* session)
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
                qWarning() << "openStore: HrQueryAllRows failed";
            }

            storesTable->Release();
        } else {
            qWarning() << "openStore: GetMsgStoresTable failed";
        }

        if (!entryId.isEmpty()) {
            rv = session->OpenMsgStore(0, entryId.length(), reinterpret_cast<LPENTRYID>(entryId.data()), 0, MDB_NO_MAIL | MDB_WRITE, reinterpret_cast<LPMDB*>(&store));
            if (HR_FAILED(rv)) {
                store = 0;
                qWarning() << "openStore: OpenMsgStore failed";
            }
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
}

QMessageAccountId addAccount(const Parameters &params)
{
    QMessageAccountId result;

    doInit();

    QString accountName(params["name"]);
    if (!accountName.isEmpty()) {
        // Profile name must be ASCII
        QByteArray name(accountName.toAscii());

        // See if a profile exists with the given name
        LPPROFADMIN profAdmin(0);
        HRESULT rv = MAPIAdminProfiles(0, &profAdmin);
        if (HR_SUCCEEDED(rv)) {
            // Find the default profile
            QByteArray defaultProfileName;
            foreach (const ProfileDetail &profile, profileDetails(profAdmin)) {
                if (profile.second) {
                    defaultProfileName = profile.first;
                    break;
                }
            }

            if (!defaultProfileName.isEmpty()) {
                // Open a session on the profile
                LPMAPISESSION session(0);
                rv = MAPILogonEx(0, reinterpret_cast<LPTSTR>(defaultProfileName.data()), 0, MAPI_EXTENDED | MAPI_NEW_SESSION | MAPI_NO_MAIL, &session);
                if (HR_SUCCEEDED(rv)) {
                    LPSERVICEADMIN svcAdmin(0);
                    rv = profAdmin->AdminServices(reinterpret_cast<LPTSTR>(defaultProfileName.data()), 0, 0, 0, &svcAdmin);
                    if (HR_SUCCEEDED(rv)) {
                        char *providerName = "MSUPST MS";
                        bool serviceExists(false);
                        MAPIUID svcUid = { 0 };
                        QList<QByteArray> existingServices;

                        foreach (const ServiceDetail &svc, serviceDetails(svcAdmin)) {
                            // Find all services that have our provider
                            if (svc.first.first.toLower() == QByteArray(providerName).toLower()) {
                                if (svc.first.second.toLower() == name.toLower()) {
                                    // This is existing service we need to remove
                                    svcUid = svc.second;
                                    serviceExists = true;
                                } else {
                                    existingServices.append(QByteArray(reinterpret_cast<const char*>(&svc.second), sizeof(MAPIUID)));
                                }
                            }
                        }

                        if (serviceExists) {
                            if (deleteExistingService(svcUid, svcAdmin)) {
                                serviceExists = false;
                            }
                        }

                        if (!serviceExists) {
                            // Create a message service for this profile using the standard provider
                            rv = svcAdmin->CreateMsgService(reinterpret_cast<LPTSTR>(providerName), 0, 0, 0);
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

                                        svcUid = svc.second;
                                        rv = svcAdmin->ConfigureMsgService(&svcUid, 0, 0, 3, props);
                                        if (HR_SUCCEEDED(rv)) {
                                            serviceExists = true;
                                        } else {
                                            qWarning() << "ConfigureMsgService failed";
                                        }
                                        break;
                                    }
                                }
                            } else {
                                qWarning() << "CreateMsgService failed";
                            }
                        }

                        if (serviceExists) {
                            foreach (const StoreDetail &store, storeDetails(session)) {
                                if (store.first.toLower() == name.toLower()) {
                                    result = accountIdFromRecordKey(store.second);
                                    break;
                                }
                            }
                        }

                        svcAdmin->Release();
                    } else {
                        qWarning() << "AdminServices failed";
                    }

                    session->Release();
                } else {
                    qWarning() << "MAPILogonEx failed";
                }
            } else {
                qWarning() << "No default profile found!";
            }

            profAdmin->Release();
        } else {
            qWarning() << "MAPIAdminProfiles failed";
        }
    }

    return result;
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderId addFolder(const Parameters &params)
{
    QMessageFolderId result;

    doInit();

    QString folderName(params["displayName"]);
    QString folderPath(params["path"]);
    QString parentPath(params["parentFolderPath"]);
    QByteArray accountName(params["parentAccountName"].toAscii());

    if (!folderName.isEmpty() && !folderPath.isEmpty() && !accountName.isEmpty()) {
        // Open a session on the default profile
        LPMAPISESSION session(profileSession(defaultProfile()));
        if (session) {
            // Open the store for modification
            IMsgStore *store = openStore(accountName, session);
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
    Q_UNUSED(params)

    return QMessageId();
}

}

