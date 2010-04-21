/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef UT_QTCONTACTS_TRACKERPLUGIN_DEFINITIONS_H
#define UT_QTCONTACTS_TRACKERPLUGIN_DEFINITIONS_H

#include "ut_qtcontacts_common.h"

QTM_USE_NAMESPACE

class ut_qtcontacts_trackerplugin_definitions : public ut_qtcontacts_common
{
    Q_OBJECT

private slots:
    void checkAllDefitionsTested();

    void saveAndFetchOneAnniversary();
    void saveAndFetchOneAvatar();
    void saveAndFetchOneBirthday();
    void saveAndFetchOneDisplayLabel();
    void saveAndFetchOneEmailAddress();
    void saveAndFetchOneGender();
    void saveAndFetchOneGeoLocation();
    void saveAndFetchOneGuid();
    void saveAndFetchOneName();
    void saveAndFetchOneNickname();
    void saveAndFetchOneNote();
    void saveAndFetchOneOnlineAccount();
    void saveAndFetchOneOrganization();
    void saveAndFetchOnePhoneNumber();
    void saveAndFetchOneStreetAddress();
    void saveAndFetchOneSyncTarget();
    void saveAndFetchOneTimestamp();
    void saveAndFetchOneType();
    void saveAndFetchOneUrl();

    void saveAndFetchManyAnniversary();
    void saveAndFetchManyAvatar();
    void saveAndFetchManyBirthday();
    void saveAndFetchManyDisplayLabel();
    void saveAndFetchManyEmailAddress();
    void saveAndFetchManyGender();
    void saveAndFetchManyGeoLocation();
    void saveAndFetchManyGuid();
    void saveAndFetchManyName();
    void saveAndFetchManyNickname();
    void saveAndFetchManyNote();
    void saveAndFetchManyOnlineAccount();
    void saveAndFetchManyOrganization();
    void saveAndFetchManyPhoneNumber();
    void saveAndFetchManyStreetAddress();
    void saveAndFetchManySyncTarget();
    void saveAndFetchManyTimestamp();
    void saveAndFetchManyType();
    void saveAndFetchManyUrl();

    void saveAndFetchAllAnniversary();
    void saveAndFetchAllAvatar();
    void saveAndFetchAllBirthday();
    void saveAndFetchAllDisplayLabel();
    void saveAndFetchAllEmailAddress();
    void saveAndFetchAllGender();
    void saveAndFetchAllGeoLocation();
    void saveAndFetchAllGuid();
    void saveAndFetchAllName();
    void saveAndFetchAllNickname();
    void saveAndFetchAllNote();
    void saveAndFetchAllOnlineAccount();
    void saveAndFetchAllOrganization();
    void saveAndFetchAllPhoneNumber();
    void saveAndFetchAllStreetAddress();
    void saveAndFetchAllSyncTarget();
    void saveAndFetchAllTimestamp();
    void saveAndFetchAllType();
    void saveAndFetchAllUrl();

private:
    void createTestContacts(const QString &definitionName, QList<QContact> &result);
    void verifyContacts(const QString &definitionName, QList<QContact> &savedContacts,
                        QList<QContact> &fetchedContacts);

    void saveAndFetchOne(const QString &definitionName);
    void saveAndFetchMany(const QString &definitionName);
    void saveAndFetchAll(const QString &definitionName);
};

#endif /* UT_QTCONTACTS_TRACKERPLUGIN_DEFINITIONS_H */
