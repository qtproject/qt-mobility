/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef tst_QVERSITCONTACTEXPORTER_H
#define tst_QVERSITCONTACTEXPORTER_H

#include <qcontactdetail.h>
#include <QObject>
#include <qmobilityglobal.h>
#include <qcontact.h>
#include <qversitproperty.h>

QTM_BEGIN_NAMESPACE

class QVersitContactExporter;
class QVersitContactExporterPrivate;
class QVersitDocument;
class QVersitProperty;
class MyQVersitResourceHandler;
class MyQVersitContactExporterDetailHandler;

QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class tst_QVersitContactExporter : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testConvertContact();
    void testContactDetailHandler();
    void testEncodeName();
    void testEncodePhoneNumber();
    void testEncodeEmailAddress();
    void testEncodeStreetAddress();
    void testEncodeUrl();
    void testEncodeParameters();
    void testEncodeUid();
    void testEncodeRev();
    void testEncodeBirthDay();
    void testEncodeNote();
    void testEncodeGeoLocation();
    void testEncodeOrganization();
    void testEncodeEmbeddedContent();
    void testEncodeRingtone();
    void testEncodeGender();
    void testEncodeNickName();
    void testEncodeTag();
    void testEncodeAnniversary();
    void testEncodeOnlineAccount();
    void testEncodeFamily();
    void testEncodeAvatar();
    void testEncodeThumbnail();
    void testEncodeDisplayLabel();
    void testDefaultResourceHandler();

private:
    // Test Utility Functions
    QContact createContactWithName(QString name);
    QContactDetail searchDetail(QList<QContactDetail> details, QString search);
    QVersitProperty findPropertyByName(const QVersitDocument& document,const QString& propertyName);

private: // Data
    QVersitContactExporter* mExporter;
    MyQVersitResourceHandler* mResourceHandler;
    MyQVersitContactExporterDetailHandler* mDetailHandler;
};

#endif // tst_QVERSITCONTACTEXPORTER_H
