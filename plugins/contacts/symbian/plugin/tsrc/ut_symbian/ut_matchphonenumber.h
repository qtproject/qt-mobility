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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QObject>
#include <QHash>
#include <qcontactfilter.h>
#include <cntdb.h>
#include "cntsymbianengine.h"


QTM_USE_NAMESPACE
QTM_BEGIN_NAMESPACE
QTM_END_NAMESPACE

class TestMatchPhoneNumber : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
	void initTestCase();
	void cleanupTestCase();

private:
	void cleanupContactsDb();
	void setupBestMatchingContactsDb1();
	void setupBestMatchingContactsDb2();
	void setupBestMatchingContactsDb3();
	void setupBestMatchingContactsDb4();
	void setupBestMatchingContactsDb5();
	void setupBestMatchingContactsDb6();
	void setupBestMatchingContactsDb7();
	void setupBestMatchingContactsDb8();
	void setupBestMatchingContactsDb9();
	void setupBestMatchingContactsDb10();
	void setupBestMatchingContactsDb11();
	void setupBestMatchingContactsDb12();
	void setupBestMatchingContactsDb13();
	void setupBestMatchingContactsDb14();
	void setupBestMatchingContactsDb15();
	void setupBestMatchingContactsDb16();
	void setupBestMatchingContactsDb17();
	
    void addContact(QString firstname,QString lastname,QString phonenumber);
    
    void testBestMatching1();
    void testBestMatching2();
    void testBestMatching3();
    void testBestMatching4();
    void testBestMatching5();
    void testBestMatching6();
    void testBestMatching7();
    void testBestMatching8();
    void testBestMatching9();
    void testBestMatching10();
    void testBestMatching11();
    void testBestMatching12();
    void testBestMatching13();
    void testBestMatching14();
    void testBestMatching15();
    void testBestMatching16();
    void testBestMatching17();
    
    void testBestMatchingWithUnionFilter();
    void testBestMatchingWithIntersectionFilter();
private slots:  // Test cases
    void testMatchPhoneNumber();
private:
    CContactDatabase *m_database;
    CntSymbianEngine *m_engine;
};
