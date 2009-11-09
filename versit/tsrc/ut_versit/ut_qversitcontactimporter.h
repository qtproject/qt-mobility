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

#ifndef UT_QVERSITCONTACTIMPORTER_H
#define UT_QVERSITCONTACTIMPORTER_H

#include <QObject>
#include <qversitdocument.h>

class QVersitContactImporter;
class QVersitContactImporterPrivate;

class UT_QVersitContactImporter : public QObject
{
    Q_OBJECT

private slots: // Tests
    
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    void testName();
    void testAddress();
    void testTel();    
    void testEmail();
    void testUrl();
    void testUid();
    void testOrganization();
    void testTimeStamp();
    void testAnniversary();
    void testBirthday();
    void testGender();
    void testNickname();
    void testAvatarJpegStored();
    void testAvatarGifStored();
    void testAvatarJpegTwoContactsWithSameName();
    void testAvatarJpegNonexistentPath();
    void testAvatarUrl();
    void testAvatarEncoding();
    void testGeo();
    void testNote();
    void testOnlineAccount();
    void testFamily();
    void testSound();
    void testUnconvertedVersitProperties();

private: 
    
    QVersitDocument createDocumentWithProperty(const QVersitProperty& property);
    QVersitDocument createDocumentWithNameAndPhoto(const QByteArray& name, QByteArray image,
                                                   const QString& photoType, const QString& encoding);

private:
    QVersitContactImporter* mGenerator;
    QVersitContactImporterPrivate* mGeneratorPrivate;
};

#endif // UT_QVERSITCONTACTIMPORTER_H
