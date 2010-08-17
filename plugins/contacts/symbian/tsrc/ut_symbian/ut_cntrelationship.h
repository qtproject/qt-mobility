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
#ifndef TESTRELATIONSHIP_H
#define TESTRELATIONSHIP_H

#include <QObject>
#include "cntsymbianengine.h"

class CContactDatabase;
class CntRelationship;

class TestCntRelationship : public QObject
{
    Q_OBJECT

private slots:
	void initTestCase();
	void cleanupTestCase();

	void init();
	void cleanup();

	void invalidRelationship();
	void validGroupRelationship();
	void validGroupRelationships();
	void invalidGroupRelationship();
	void invalidFirstContactGroupRelationship();
	void invalidSecondContactGroupRelationship();
	void invalidFirstAndSecondContactGroupRelationship();

private:
    bool validateRelationshipFilter(const QContactRelationship::Role role, const QContactId contactId, const QList<QContactLocalId> expectedContacts);

private:
     CntSymbianEngine *m_manager;
     CContactDatabase *m_database;
     CntRelationship  *m_relationship;
};

#endif
