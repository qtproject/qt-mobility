/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMobility module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>

#include "qtcontacts.h"

//TESTED_CLASS=
//TESTED_FILES=

class tst_QContactRequest : public QObject
{
Q_OBJECT

public:
    tst_QContactRequest();
    virtual ~tst_QContactRequest();

public slots:
    void init();
    void cleanup();
private slots:
    void contactRequest();
};

tst_QContactRequest::tst_QContactRequest()
{
}

tst_QContactRequest::~tst_QContactRequest()
{
}

void tst_QContactRequest::init()
{
}

void tst_QContactRequest::cleanup()
{
}

void tst_QContactRequest::contactRequest()
{
    QContactManager *cm = new QContactManager("memory");

    QList<QUniqueId> ids;
    ids << 3 << 4 << 5;
    QList<QContact> contacts;

    QContactSortOrder so;
    so.setDetailDefinitionName(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel);

    QContact a,b,c;
    contacts << a << b << c;

    QContactDetailFilter df;
    df.setDetailDefinitionName("Definition", "Field");

    QContactRequest req(cm);

    QVERIFY(req.isFinished() == false);
    QVERIFY(req.status() == QContactAbstractRequest::Inactive);
    QVERIFY(req.error() == QContactManager::NoError);

    QVERIFY(req.type() == QContactAbstractRequest::Contact);

    QVERIFY(!req.sortOrder().isValid());

    /* Sort order */
    req.setSortOrder(so);
    QVERIFY(req.sortOrder().isValid());
    QVERIFY(req.sortOrder().blankPolicy() == so.blankPolicy());
    QVERIFY(req.sortOrder().detailDefinitionName() == so.detailDefinitionName());
    QVERIFY(req.sortOrder().detailFieldName() == so.detailFieldName());

    /* Restrictions */
    QVERIFY(req.restriction() == QContactRequest::NoRestriction);
    QVERIFY(req.detailRestrictions().isEmpty());

    req.restrictToIds();
    QVERIFY(req.restriction() == QContactRequest::RestrictToIds);
    QVERIFY(req.detailRestrictions().isEmpty());

    req.clearRestrictions();
    QVERIFY(req.restriction() == QContactRequest::NoRestriction);
    QVERIFY(req.detailRestrictions().isEmpty());

    req.restrictToDetails(QStringList("Detail"));
    QVERIFY(req.restriction() == QContactRequest::RestrictToDetails);
    QVERIFY(req.detailRestrictions() == QStringList("Detail"));

    req.clearRestrictions();
    QVERIFY(req.restriction() == QContactRequest::NoRestriction);
    QVERIFY(req.detailRestrictions().isEmpty());

    /* Selection type */
    QVERIFY(req.selectionType() == QContactRequest::SelectAll);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection().isEmpty());

    req.selectByFilter(df);
    QVERIFY(req.selectionType() == QContactRequest::SelectByFilter);
    QVERIFY(req.filterSelection() == df);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection().isEmpty());

    req.clearSelection();
    QVERIFY(req.selectionType() == QContactRequest::SelectAll);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection().isEmpty());

    req.selectById(ids);
    QVERIFY(req.selectionType() == QContactRequest::SelectByIds);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection() == ids);

    req.clearSelection();
    QVERIFY(req.selectionType() == QContactRequest::SelectAll);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection().isEmpty());

    req.selectByObject(contacts);
    QVERIFY(req.selectionType() == QContactRequest::SelectByObject);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.idSelection().isEmpty());
    QVERIFY(req.contactSelection() == contacts);

    req.clearSelection();
    QVERIFY(req.selectionType() == QContactRequest::SelectAll);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection().isEmpty());

    /* Without having ever started this request, the results should be empty */
    QVERIFY(req.ids().count() == 0);
    QVERIFY(req.contacts().count() == 0);
    QVERIFY(req.errors().count() == 0);

    /* Now delete the manager and make sure we don't crash */
    delete cm;

    /* A request with no manager is finished, and has an error */
    QVERIFY(req.isFinished() == true);
    QVERIFY(req.status() == QContactAbstractRequest::Finished);
    QVERIFY(req.error() == QContactManager::DoesNotExistError);

    QVERIFY(req.type() == QContactAbstractRequest::Contact);

    QVERIFY(req.sortOrder() == so);

    QVERIFY(req.ids().count() == 0);
    QVERIFY(req.contacts().count() == 0);
    QVERIFY(req.errors().count() == 0);

    /* Restrictions */
    QVERIFY(req.restriction() == QContactRequest::NoRestriction);
    QVERIFY(req.detailRestrictions().isEmpty());

    req.restrictToIds();
    QVERIFY(req.restriction() == QContactRequest::RestrictToIds);
    QVERIFY(req.detailRestrictions().isEmpty());

    req.clearRestrictions();
    QVERIFY(req.restriction() == QContactRequest::NoRestriction);
    QVERIFY(req.detailRestrictions().isEmpty());

    req.restrictToDetails(QStringList("Detail"));
    QVERIFY(req.restriction() == QContactRequest::RestrictToDetails);
    QVERIFY(req.detailRestrictions() == QStringList("Detail"));

    /* Selection type */
    QVERIFY(req.selectionType() == QContactRequest::SelectAll);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection().isEmpty());

    req.selectByFilter(df);
    QVERIFY(req.selectionType() == QContactRequest::SelectByFilter);
    QVERIFY(req.filterSelection() == df);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection().isEmpty());

    req.clearSelection();
    QVERIFY(req.selectionType() == QContactRequest::SelectAll);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection().isEmpty());

    req.selectById(ids);
    QVERIFY(req.selectionType() == QContactRequest::SelectByIds);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection() == ids);

    req.clearSelection();
    QVERIFY(req.selectionType() == QContactRequest::SelectAll);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection().isEmpty());

    req.selectByObject(contacts);
    QVERIFY(req.selectionType() == QContactRequest::SelectByObject);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.idSelection().isEmpty());
    QVERIFY(req.contactSelection() == contacts);

    req.clearSelection();
    QVERIFY(req.selectionType() == QContactRequest::SelectAll);
    QVERIFY(req.filterSelection().type() == QContactFilter::Invalid);
    QVERIFY(req.contactSelection().isEmpty());
    QVERIFY(req.idSelection().isEmpty());

    /* Test creating a request and deleting it before the manager, too */
    cm = new QContactManager("memory");
    QContactRequest* preq = new QContactRequest(cm);
    delete preq;

    delete cm;
}

QTEST_MAIN(tst_QContactRequest)
#include "tst_qcontactrequest.moc"
