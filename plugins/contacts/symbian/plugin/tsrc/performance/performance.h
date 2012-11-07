/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QObject>
#include <qmobilityglobal.h>
#include <qtcontacts.h>

QTM_BEGIN_NAMESPACE
#define NO_OF_CONTACTS 1000
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class SymbianPluginPerfomance : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
	void initTestCase();
	void cleanupTestCase();

private slots:  // Test cases
    void createSimpleContacts();
    void removeSimpleContacts();
    void createComplexContacts();
    void sortContacts();
    void fetchAllNames();
    void filterContacts();
    void filterUnions();
    void filterNameListStyle();
    void filterPhoneNumberMatch();

    // Data providers
	void filterNameListStyle_data();
	void filterPhoneNumberMatch_data();

	/*
	void createViews();
	void operation1();
	void operation2();
	void asyncOperation1();    // requests
	void asyncOperation2();
	// feel free to add more...
	*/
    void removeComplexContacts();

    void createComplexContactsWithOnlineAccount();
    void sortContactsWithOnlineAccount();
    void removeComplextContactsWithOnlineAccount();   
private:
    int measureContactsFetch(
            QString debugMessage,
            const QContactFilter &filter,
            const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>());
    int measureNamesFetch(
            QString debugMessage,
            const QContactFilter &filter,
            const QList<QContactSortOrder>& sortOrders = QList<QContactSortOrder>());
    QContactManager     *mCntMng;
    QTime               mTime;
};
