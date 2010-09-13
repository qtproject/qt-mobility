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

//TESTED_COMPONENT=src/telephony

#include <qmobilityglobal.h>
#include <qtelephonycalllist_win_p.h>
#include <qtelephonycallinfo_p.h>
#include <QtTest/QtTest>
#include <QtCore>
#include <QDebug>

#define private public
#include "../qsfwtestutil.h"

#if defined(Q_OS_SYMBIAN)
# define TESTDATA_DIR "."
#endif

QTM_USE_NAMESPACE

QTM_BEGIN_NAMESPACE

class tst_TelefonyCallList : public QTelephonyCallList
{
    Q_OBJECT
public:
    tst_TelefonyCallList() : QTelephonyCallList(), pcallInfoPrivate(0)
    {
        connect(this, SIGNAL(activeCallStatusChanged(const QTelephonyCallInfo&)), this, SLOT(activeCallStatusChangedSlot(const QTelephonyCallInfo&)));
        connect(this, SIGNAL(activeCallRemoved(const QTelephonyCallInfo&)), this, SLOT(activeCallRemovedSlot(const QTelephonyCallInfo&)));
        connect(this, SIGNAL(activeCallAdded(const QTelephonyCallInfo&)), this, SLOT(activeCallAddedSlot(const QTelephonyCallInfo&)));
    }
    QTelephonyCallListPrivate* getPrivateTelefonyList() { return d; }
private slots:
    void activeCallStatusChangedSlot(const QTelephonyCallInfo& call)
    {
        if(values.contains("activeCallStatusChangedSlot_RefCount"))
            values.remove("activeCallStatusChangedSlot_RefCount");
        if(pcallInfoPrivate)
            values.insert("activeCallStatusChangedSlot_RefCount", QVariant(pcallInfoPrivate->ref));
    }
    void activeCallRemovedSlot(const QTelephonyCallInfo& call)
    {
        if(values.contains("activeCallRemovedSlot_RefCount"))
            values.remove("activeCallRemovedSlot_RefCount");
        if(pcallInfoPrivate)
            values.insert("activeCallRemovedSlot_RefCount", QVariant(pcallInfoPrivate->ref));
    }
    void activeCallAddedSlot(const QTelephonyCallInfo& call)
    {
        if(values.contains("aactiveCallAddedSlot_RefCount"))
            values.remove("activeCallAddedSlot_RefCount");
        if(pcallInfoPrivate)
            values.insert("activeCallAddedSlot_RefCount", QVariant(pcallInfoPrivate->ref));
    }
public:
    QHash<QString, QVariant> values;
    QTelephonyCallInfoPrivate* pcallInfoPrivate;
};

class tst_Telephony: public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    //Test cases
    void checkSharedPointer();
    void cleanupTestCase();

private:
    tst_TelefonyCallList* pCallList;
};

void tst_Telephony::initTestCase()
{
    qDebug("called before everything else");
    pCallList = new tst_TelefonyCallList();
}

//Test cases
void tst_Telephony::checkSharedPointer()
{
    QTelephonyCallListPrivate* p = pCallList->getPrivateTelefonyList();
    QVERIFY(p != 0);
    pCallList->pcallInfoPrivate = new QTelephonyCallInfoPrivate();

    //Adding to list should increment the ref counter to 1
    p->callInfoList.append(QExplicitlySharedDataPointer<QTelephonyCallInfoPrivate>(pCallList->pcallInfoPrivate));
    QVERIFY(pCallList->pcallInfoPrivate->ref == 1);

    int refcount = -1;

    //Now emit the activeCallAdded signal this should increment the ref counter to 2
    qDebug("emit ActiveCallAdded");
    p->emitActiveCallAdded(*pCallList->pcallInfoPrivate);
    if(pCallList->values.contains("activeCallAddedSlot_RefCount")){
        QVariant var = pCallList->values.value("activeCallAddedSlot_RefCount");
        refcount = var.toInt();
    }
    QVERIFY(refcount == 2);
    qDebug("- ref counting OK");

    //Now emit the activeCallStatusChanged signal this should increment the ref counter to 2
    qDebug("emit ActiveCallStatusChanged");
    p->emitActiveCallStatusChanged(*pCallList->pcallInfoPrivate);
    refcount = -1;
    if(pCallList->values.contains("activeCallStatusChangedSlot_RefCount")){
        QVariant var = pCallList->values.value("activeCallStatusChangedSlot_RefCount");
        refcount = var.toInt();
    }
    QVERIFY(refcount == 2);
    qDebug("- ref counting OK");

    //Now emit the activeCallRemoved signal this should increment the ref counter to 2
    qDebug("emit ActiveCallRemoved");
    p->emitActiveCallRemoved(*pCallList->pcallInfoPrivate);
    refcount = -1;
    if(pCallList->values.contains("activeCallRemovedSlot_RefCount")){
        QVariant var = pCallList->values.value("activeCallRemovedSlot_RefCount");
        refcount = var.toInt();
    }
    QVERIFY(refcount == 2);
    qDebug("- ref counting OK");

    //After emitint we should have ref counter == 1
    QVERIFY(pCallList->pcallInfoPrivate->ref == 1);
}

void tst_Telephony::cleanupTestCase()
{
    qDebug("called after myFirstTest and mySecondTest");
    delete pCallList;
}
QTM_END_NAMESPACE

QTEST_MAIN(tst_Telephony)

#include "tst_telephony_win.moc"
