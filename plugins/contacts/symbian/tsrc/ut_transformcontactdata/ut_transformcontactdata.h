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
#include <QObject>

#include <cntdb.h>
#include <cntitem.h>
#include <qtcontacts.h>
#include "transformcontactdata.h"

class TestTransformContactData : public QObject
{
    Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();
	
	void executeTransformEmail();
	void executeTransformName();
	void executeTransformNickname();
	void executeTransformPhonenumber();
	void executeTransformAddress();
	void executeTransformUrl();
	void executeTransformBithday();
	void executeTransformOnlineAccount();
	void executeTransformOrganisation();
	void executeTransformAvatar();
	void executeTransformSyncTarget();
	void executeTransformGender();
	void executeTransformAnniversary();
	void executeTransformGeolocation();
	void executeTransformNote();
	void executeTransformFamily();
	
private:
    void validateTransformEmail(TPtrC16 field, QString detail);
    void validateTransformName(TPtrC16 prefixField, QString prefixDetail,
                               TPtrC16 firstnameField, QString firstnameDetail,
                               TPtrC16 middlenameField, QString middlenameDetail,
                               TPtrC16 lastnameField, QString lastnameDetail,
                               TPtrC16 suffixField, QString suffixDetail);
    void validateTransformNickname(TPtrC16 field, QString detail);
    void validateTransformPhonenumber(TPtrC16 field, QString detail);
    void validateTransformAddress(TPtrC16 countryField, QString countryDetail,
                                  TPtrC16 postcodeField, QString postcodeDetail,
                                  TPtrC16 streetField, QString streetDetail,
                                  TPtrC16 localityField, QString localityDetail,
                                  TPtrC16 regionField, QString regionDetail,
                                  TPtrC16 postOfficeBoxField, QString postOfficeBoxDetail);
    void validateTransformUrl(TPtrC16 field, QString detail);
    void validateTransformBirthday(TTime field, QDate detail);
    void validateTransformOnlineAccount(TPtrC16 sipField, QString sipDetail);
    void validateTransformOrganisation(TPtrC16 companyField, QString companyDetail,
                                       TPtrC16 departmentField, QString departmentDetail,
                                       TPtrC16 jobtitleField, QString jobtitleDetail,
                                       TPtrC16 assistantField, QString assistantDetail);
    void validateTransformAvatar(TPtrC16 field, QString detail);
    void validateTransformSyncTarget(TPtrC16 field, QString detail);
    void validateTransformGender(TPtrC16 field, QString detail);
    void validateTransformAnniversary(TPtrC16 field, QDate dateDetail, QString eventDetail);
    void validateTransformGeolocation(TPtrC16 field, double latitudeDetail, double longitudeDetail);
    void validateTransformNote(TPtrC16 field, QString detail);
    void validateTransformFamily(TPtrC16 spouseField, QString spouseDetail,
                                 TPtrC16 childField, QString childDetail);
    void validateContexts(TransformContactData* transformContactData) const; 
};
