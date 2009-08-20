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

typedef QPair<QByteArray, MAPIUID> ServiceDetail;

QList<ServiceDetail> serviceDetails(LPSERVICEADMIN svcAdmin)
{
    QList<ServiceDetail> result;

    IMAPITable *svcTable(0);
    HRESULT rv = svcAdmin->GetMsgServiceTable(0, &svcTable);
    if (HR_SUCCEEDED(rv)) {
        LPSRowSet rows(0);
        SizedSPropTagArray(2, cols) = {2, {PR_SERVICE_NAME_A, PR_SERVICE_UID}};
        rv = HrQueryAllRows(svcTable, reinterpret_cast<LPSPropTagArray>(&cols), 0, 0, 0, &rows);
        if (HR_SUCCEEDED(rv)) {
            for (uint n = 0; n < rows->cRows; ++n) {
                if (rows->aRow[n].lpProps[0].ulPropTag == PR_SERVICE_NAME_A) {
                    QByteArray svcName(rows->aRow[n].lpProps[0].Value.lpszA);
                    MAPIUID svcUid(*(reinterpret_cast<MAPIUID*>(rows->aRow[n].lpProps[1].Value.bin.lpb)));
                    result.append(qMakePair(svcName, svcUid));
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
                    QByteArray recordKey(reinterpret_cast<const char*>(rows->aRow[n].lpProps[1].Value.bin.lpb), rows->aRow[n].lpProps[1].Value.bin.cb); 
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

                        foreach (const ServiceDetail &svc, serviceDetails(svcAdmin)) {
                            if (svc.first.toLower() == QByteArray(providerName).toLower()) {
                                svcUid = svc.second;
                                serviceExists = true;
                                break;
                            }
                        }

                        if (serviceExists) {
                            // Delete the existing service
                            rv = svcAdmin->DeleteMsgService(&svcUid);
                            if (HR_SUCCEEDED(rv)) {
                                serviceExists = false;
                            } else {
                                qWarning() << "DeleteMsgService failed";
                            }
                        }

                        if (!serviceExists) {
                            // Create a message service for this profile using the standard provider
                            rv = svcAdmin->CreateMsgService(reinterpret_cast<LPTSTR>(providerName), reinterpret_cast<LPTSTR>(name.data()), 0, 0);
                            if (HR_SUCCEEDED(rv)) {
                                foreach (const ServiceDetail &svc, serviceDetails(svcAdmin)) {
                                    // Find the name/UID for the service we added
                                    if (svc.first.toLower() == QByteArray(providerName).toLower()) {
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
    Q_UNUSED(params)

    return QMessageFolderId();
}
#endif
QMessageId addMessage(const Parameters &params)
{
    Q_UNUSED(params)

    return QMessageId();
}

}

