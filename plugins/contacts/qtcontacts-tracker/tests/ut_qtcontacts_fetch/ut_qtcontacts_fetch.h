/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef UT_QTCONTACTS_TRACKERPLUGIN_FETCH_H
#define UT_QTCONTACTS_TRACKERPLUGIN_FETCH_H

#include "ut_qtcontacts_common.h"

QTM_USE_NAMESPACE

class ut_qtcontacts_fetch : public ut_qtcontacts_common
{
    Q_OBJECT

public:
    ut_qtcontacts_fetch();

private slots:
    void checkDatabaseEmpty();

    void testSaveContact();
    void testSaveContactCopy();
    void testFetchSavedContact();

private:
    void setupTestContact(QContact &contact);

    QContactDetailFilter mNameFilter;

    QString mUuid;
    QString mFirstName;
    QString mLastName;
    QString mWebPage;
};

#endif /* UT_QTCONTACTS_TRACKERPLUGIN_FETCH_H */
