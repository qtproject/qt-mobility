/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

QTM_USE_NAMESPACE
class QContactQmlPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("QtMobility.contacts"));
        qRegisterMetaType<QContactLocalId>("QContactLocalId");
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
        qmlRegisterType<QDeclarativeContactFamily>(uri, 1, 1, "Family");
        qmlRegisterType<QDeclarativeContactFavorite>(uri, 1, 1, "Favorite");
        qmlRegisterType<QDeclarativeContactGender>(uri, 1, 1, "Gender");
        qmlRegisterType<QDeclarativeContactGeoLocation>(uri, 1, 1, "Location");
        qmlRegisterType<QDeclarativeContactGlobalPresence>(uri, 1, 1, "GlobalPresence");
        qmlRegisterType<QDeclarativeContactGuid>(uri, 1, 1, "Guid");
        qmlRegisterType<QDeclarativeContactName>(uri, 1, 1, "Name");
        qmlRegisterType<QDeclarativeContactNickname>(uri, 1, 1, "Nickname");
        qmlRegisterType<QDeclarativeContactNote>(uri, 1, 1, "Note");
        qmlRegisterType<QDeclarativeContactOnlineAccount>(uri, 1, 1, "OnlineAccount");
        qmlRegisterType<QDeclarativeContactOrganization>(uri, 1, 1, "Organization");
        qmlRegisterType<QDeclarativeContactPhoneNumber>(uri, 1, 1, "PhoneNumber");
        qmlRegisterType<QDeclarativeContactPresence>(uri, 1, 1, "Presence");
        qmlRegisterType<QDeclarativeContactRingtone>(uri, 1, 1, "Ringtone");
        qmlRegisterType<QDeclarativeContactSyncTarget>(uri, 1, 1, "SyncTarget");
        qmlRegisterType<QDeclarativeContactTag>(uri, 1, 1, "Tag");
        qmlRegisterType<QDeclarativeContactTimestamp>(uri, 1, 1, "Timestamp");
        qmlRegisterType<QDeclarativeContactUrl>(uri, 1, 1, "Url");
        qmlRegisterType<QDeclarativeContactHobby>(uri, 1, 1, "Hobby");

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
