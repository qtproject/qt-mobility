/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QApplication>
#include <QtDeclarative>
#include <QDeclarativeExtensionPlugin>
#include <QDebug>
#include "qdeclarativecontactmodel_p.h"
#include "qdeclarativecontact_p.h"
#include "qdeclarativecontactdetail_p.h"
#include "qdeclarativecontactdetails_p.h"
#include "qdeclarativecontactfilter_p.h"
#include "qdeclarativecontactfilters_p.h"
#include "qdeclarativecontactimageprovider_p.h"
#include "qdeclarativecontactsortorder_p.h"
#include "qdeclarativecontactrelationship_p.h"
#include "qdeclarativecontactfetchhint_p.h"

QT_USE_NAMESPACE


class QContactQmlPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("QtMobility.contacts"));
        qmlRegisterType<QDeclarativeContactModel>(uri, 1, 1, "ContactModel");
        qmlRegisterType<QDeclarativeContact>(uri, 1, 1, "Contact");
        qmlRegisterType<QDeclarativeContactFetchHint>(uri, 1, 1, "FetchHint");
        qmlRegisterType<QDeclarativeContactRelationship>(uri, 1, 1, "Relationship");
        qmlRegisterType<QDeclarativeContactSortOrder>(uri, 1, 1, "SortOrder");

        //details
        qmlRegisterType<QDeclarativeContactDetail>(uri, 1, 1, "ContactDetail");
        qmlRegisterType<QDeclarativeContactAddress>(uri, 1, 1, "Address");
        qmlRegisterType<QDeclarativeContactAnniversary>(uri, 1, 1, "Anniversary");
        qmlRegisterType<QDeclarativeContactAvatar>(uri, 1, 1, "Avatar");
        qmlRegisterType<QDeclarativeContactBirthday>(uri, 1, 1, "Birthday");
        qmlRegisterType<QDeclarativeContactDisplayLabel>(uri, 1, 1, "DisplayLabel");
        qmlRegisterType<QDeclarativeContactEmailAddress>(uri, 1, 1, "EmailAddress");
        qmlRegisterType<QDeclarativeContactFamily>(uri, 1, 1, "ContactFamily");
        qmlRegisterType<QDeclarativeContactFavorite>(uri, 1, 1, "Favorite");
        qmlRegisterType<QDeclarativeContactGender>(uri, 1, 1, "Gender");
        qmlRegisterType<QDeclarativeContactGeoLocation>(uri, 1, 1, "Location");
        qmlRegisterType<QDeclarativeContactGlobalPresence>(uri, 1, 1, "GlobalPresence");
        qmlRegisterType<QDeclarativeContactGuid>(uri, 1, 1, "ContactGuid");
        qmlRegisterType<QDeclarativeContactName>(uri, 1, 1, "ContactName");
        qmlRegisterType<QDeclarativeContactNickname>(uri, 1, 1, "Nickname");
        qmlRegisterType<QDeclarativeContactNote>(uri, 1, 1, "ContactNote");
        qmlRegisterType<QDeclarativeContactOnlineAccount>(uri, 1, 1, "OnlineAccount");
        qmlRegisterType<QDeclarativeContactOrganization>(uri, 1, 1, "Organization");
        qmlRegisterType<QDeclarativeContactPhoneNumber>(uri, 1, 1, "PhoneNumber");
        qmlRegisterType<QDeclarativeContactPresence>(uri, 1, 1, "Presence");
        qmlRegisterType<QDeclarativeContactRingtone>(uri, 1, 1, "Ringtone");
        qmlRegisterType<QDeclarativeContactSyncTarget>(uri, 1, 1, "SyncTarget");
        qmlRegisterType<QDeclarativeContactTag>(uri, 1, 1, "ContactTag");
        qmlRegisterType<QDeclarativeContactTimestamp>(uri, 1, 1, "Timestamp");
        qmlRegisterType<QDeclarativeContactUrl>(uri, 1, 1, "ContactUrl");
        qmlRegisterType<QDeclarativeContactHobby>(uri, 1, 1, "ContactHobby");

        //filters
        qmlRegisterType<QDeclarativeContactFilter>(uri, 1, 1, "Filter");
        qmlRegisterType<QDeclarativeContactActionFilter>(uri, 1, 1, "ActionFilter");
        qmlRegisterType<QDeclarativeContactChangeLogFilter>(uri, 1, 1, "ChangeLogFilter");
        qmlRegisterType<QDeclarativeContactDetailFilter>(uri, 1, 1, "DetailFilter");
        qmlRegisterType<QDeclarativeContactDetailRangeFilter>(uri, 1, 1, "DetailRangeFilter");
        qmlRegisterType<QDeclarativeContactLocalIdFilter>(uri, 1, 1, "IdFilter");
        qmlRegisterType<QDeclarativeContactRelationshipFilter>(uri, 1, 1, "RelationshipFilter");
        qmlRegisterType<QDeclarativeContactIntersectionFilter>(uri, 1, 1, "IntersectionFilter");
        qmlRegisterType<QDeclarativeContactUnionFilter>(uri, 1, 1, "UnionFilter");
    }

    void initializeEngine(QDeclarativeEngine *engine, const char *uri) {
        Q_UNUSED(uri);
        engine->addImageProvider("thumbnail", new ContactThumbnailImageProvider);
    }
};

#include "plugin.moc"

Q_EXPORT_PLUGIN2(qcontactqmlplugin, QContactQmlPlugin);
