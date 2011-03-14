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
#include <QTest>
#include <QDebug>

#include "qmessage.h"
#include "qmessage_p.h"
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"
#include "maemohelpers_p.h"
#include "../support/support.h"

//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE
#ifdef Q_OS_WIN
const QByteArray defaultCharset("UTF-16");
#else
const QByteArray defaultCharset("UTF-8");
#endif

static const QString email_prefix = "MO_";
static const QString sms_prefix = "el";

/*
    Unit test for QMessageFilter class.
*/
class tst_QMessageFilter : public QObject
{
    Q_OBJECT

public:
    tst_QMessageFilter();
    virtual ~tst_QMessageFilter();

private slots:
    void initTestCase();
    void cleanup();
    void cleanupTestCase();

    void testIdFilter();
    void testTypeFilter();
    void testSenderFilter();
    void testRecipientsFilter();
    void testSubjectFilter();
    void testTimeStampFilter();
    void testStatusFilter();
    void testPriorityFilter();
    void testSizeFilter();
    void testParentAccountIdFilter();
    void testStandardFolderFilter();
    void testParentFolderIdFilter();

#if defined(Q_WS_MAEMO_5)
    void testIdPreFilter();
    void testTypePreFilter();
    void testParentAccountIdPreFilter();
    void testParentFolderIdPreFilter();
    void testComplexFiltersPreFilter();
#endif
};

QTEST_MAIN(tst_QMessageFilter)

#include "tst_qmessagestorefiltercache.moc"

tst_QMessageFilter::tst_QMessageFilter()
{
}

tst_QMessageFilter::~tst_QMessageFilter()
{
}

void tst_QMessageFilter::initTestCase()
{
}

void tst_QMessageFilter::cleanup()
{
}

void tst_QMessageFilter::cleanupTestCase()
{
}

void tst_QMessageFilter::testIdFilter()
{
#if defined(Q_WS_MAEMO_5)
    QMessage message;
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_id = QMessageId("123");

    QMessageFilter filter = QMessageFilter::byId(QMessageId("123"), QMessageDataComparator::Equal);
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filter);
    QCOMPARE(pf->filter(message), true);

    filter = QMessageFilter::byId(QMessageId("124"), QMessageDataComparator::Equal);
    pf = QMessageFilterPrivate::implementation(filter);
    QCOMPARE(pf->filter(message), false);

    filter = QMessageFilter::byId(QMessageId("123"), QMessageDataComparator::NotEqual);
    pf = QMessageFilterPrivate::implementation(filter);
    QCOMPARE(pf->filter(message), false);

    filter = QMessageFilter::byId(QMessageId("124"), QMessageDataComparator::NotEqual);
    pf = QMessageFilterPrivate::implementation(filter);
    QCOMPARE(pf->filter(message), true);

    QMessageIdList idList;
    idList.append(QMessageId("123"));
    idList.append(QMessageId("124"));
    idList.append(QMessageId("125"));
    filter = QMessageFilter::byId(idList, QMessageDataComparator::Includes);
    pf = QMessageFilterPrivate::implementation(filter);
    QCOMPARE(pf->filter(message), true);

    filter = QMessageFilter::byId(idList, QMessageDataComparator::Excludes);
    pf = QMessageFilterPrivate::implementation(filter);
    QCOMPARE(pf->filter(message), false);

    idList.clear();
    idList.append(QMessageId("124"));
    idList.append(QMessageId("125"));
    idList.append(QMessageId("126"));
    filter = QMessageFilter::byId(idList, QMessageDataComparator::Includes);
    pf = QMessageFilterPrivate::implementation(filter);
    QCOMPARE(pf->filter(message), false);

    filter = QMessageFilter::byId(idList, QMessageDataComparator::Excludes);
    pf = QMessageFilterPrivate::implementation(filter);
    QCOMPARE(pf->filter(message), true);
#endif
}

void tst_QMessageFilter::testTypeFilter()
{
#if defined(Q_WS_MAEMO_5)
#endif
}

void tst_QMessageFilter::testSenderFilter()
{
#if defined(Q_WS_MAEMO_5)
#endif
}

void tst_QMessageFilter::testRecipientsFilter()
{
#if defined(Q_WS_MAEMO_5)
#endif
}

void tst_QMessageFilter::testSubjectFilter()
{
#if defined(Q_WS_MAEMO_5)
#endif
}

void tst_QMessageFilter::testTimeStampFilter()
{
#if defined(Q_WS_MAEMO_5)
#endif
}

void tst_QMessageFilter::testStatusFilter()
{
#if defined(Q_WS_MAEMO_5)
#endif
}

void tst_QMessageFilter::testPriorityFilter()
{
#if defined(Q_WS_MAEMO_5)
#endif
}

void tst_QMessageFilter::testSizeFilter()
{
#if defined(Q_WS_MAEMO_5)
#endif
}

void tst_QMessageFilter::testParentAccountIdFilter()
{
#if defined(Q_WS_MAEMO_5)
#endif
}

void tst_QMessageFilter::testStandardFolderFilter()
{
#if defined(Q_WS_MAEMO_5)
#endif
}

void tst_QMessageFilter::testParentFolderIdFilter()
{
#if defined(Q_WS_MAEMO_5)
#endif
}

#if defined(Q_WS_MAEMO_5)
void tst_QMessageFilter::testIdPreFilter()
{
    QMessageFilter filter = QMessageFilter::byId(QMessageId(email_prefix+"123"), QMessageDataComparator::Equal);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), false);

    filter = QMessageFilter::byId(QMessageId(sms_prefix+"123"), QMessageDataComparator::Equal);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), false);

    filter = QMessageFilter::byId(QMessageId(email_prefix+"123"), QMessageDataComparator::NotEqual);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byId(QMessageId(), QMessageDataComparator::Equal);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byId(QMessageId(), QMessageDataComparator::NotEqual);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    QMessageIdList idList;
    idList.append(QMessageId(email_prefix+"123"));
    idList.append(QMessageId(email_prefix+"124"));
    filter = QMessageFilter::byId(idList, QMessageDataComparator::Includes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), false);

    filter = QMessageFilter::byId(idList, QMessageDataComparator::Excludes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    idList.clear();
    idList.append(QMessageId(sms_prefix+"123"));
    idList.append(QMessageId(sms_prefix+"124"));
    filter = QMessageFilter::byId(idList, QMessageDataComparator::Includes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), false);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byId(idList, QMessageDataComparator::Excludes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    idList.clear();
    idList.append(QMessageId(email_prefix+"123"));
    idList.append(QMessageId(sms_prefix+"124"));
    filter = QMessageFilter::byId(idList, QMessageDataComparator::Includes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byId(idList, QMessageDataComparator::Excludes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);
}

void tst_QMessageFilter::testTypePreFilter()
{
    QMessageFilter filter = QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Equal);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), false);

    filter = QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::Equal);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), false);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byType(QMessage::Email, QMessageDataComparator::NotEqual);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), false);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::NotEqual);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), false);

    filter = QMessageFilter::byType(QMessage::NoType, QMessageDataComparator::Equal);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byType(QMessage::NoType, QMessageDataComparator::NotEqual);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byType(QMessage::Email | QMessage::Mms, QMessageDataComparator::Includes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), false);

    filter = QMessageFilter::byType(QMessage::Sms | QMessage::Mms, QMessageDataComparator::Includes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), false);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byType(QMessage::Email | QMessage::Sms, QMessageDataComparator::Includes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byType(QMessage::Email | QMessage::Mms, QMessageDataComparator::Excludes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), false);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byType(QMessage::Sms | QMessage::Mms, QMessageDataComparator::Excludes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), false);

    filter = QMessageFilter::byType(QMessage::Email | QMessage::Sms, QMessageDataComparator::Excludes);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), false);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), false);
}

void tst_QMessageFilter::testParentAccountIdPreFilter()
{
    QMessageFilter filter = QMessageFilter::byParentAccountId(QMessageAccountId(email_prefix+"123"), QMessageDataComparator::Equal);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), false);

    filter = QMessageFilter::byParentAccountId(QMessageAccountId(sms_prefix+"123"), QMessageDataComparator::Equal);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), false);

    filter = QMessageFilter::byParentAccountId(QMessageAccountId(email_prefix+"123"), QMessageDataComparator::NotEqual);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byParentAccountId(QMessageAccountId(sms_prefix+"123"), QMessageDataComparator::NotEqual);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);
}

void tst_QMessageFilter::testParentFolderIdPreFilter()
{
    QMessageFilter filter = QMessageFilter::byParentFolderId(QMessageFolderId(email_prefix+"123"), QMessageDataComparator::Equal);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), false);

    filter = QMessageFilter::byParentFolderId(QMessageFolderId(sms_prefix+"123"), QMessageDataComparator::Equal);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), false);

    filter = QMessageFilter::byParentFolderId(QMessageFolderId(email_prefix+"123"), QMessageDataComparator::NotEqual);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);

    filter = QMessageFilter::byParentFolderId(QMessageFolderId(sms_prefix+"123"), QMessageDataComparator::NotEqual);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(filter, QMessage::Sms), true);
}

void tst_QMessageFilter::testComplexFiltersPreFilter()
{
    QMessageFilter filter1 = QMessageFilter::byId(QMessageId(email_prefix+"123"), QMessageDataComparator::Equal);
    QMessageFilter filter2 = QMessageFilter::byId(QMessageId(sms_prefix+"123"), QMessageDataComparator::Equal);
    QMessageFilter filter3 = QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Equal);
    QMessageFilter filter4 = QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::Equal);
    QMessageFilter filter5 = QMessageFilter::byType(QMessage::Mms, QMessageDataComparator::Equal);
    QMessageFilter filter6 = QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1999-04-01T10:31:00Z", Qt::ISODate), QMessageDataComparator::Equal);
    QMessageFilter filter7 = QMessageFilter::byStatus(QMessage::HasAttachments, QMessageDataComparator::NotEqual);

    QMessageFilter complexFilter = (filter1 & filter6) | filter3;
    QCOMPARE(MessagingHelper::preFilter(complexFilter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(complexFilter, QMessage::Sms), false);

    complexFilter = (filter4 & filter6) | (filter2 & filter7);
    QCOMPARE(MessagingHelper::preFilter(complexFilter, QMessage::Email), false);
    QCOMPARE(MessagingHelper::preFilter(complexFilter, QMessage::Sms), true);

    complexFilter = filter3 | (filter2 & filter7);
    QCOMPARE(MessagingHelper::preFilter(complexFilter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(complexFilter, QMessage::Sms), true);

    complexFilter = (filter1 & filter6) | (filter4 & filter6);
    QCOMPARE(MessagingHelper::preFilter(complexFilter, QMessage::Email), true);
    QCOMPARE(MessagingHelper::preFilter(complexFilter, QMessage::Sms), true);

    complexFilter = (filter5 & filter6) | (filter5 & filter7);
    QCOMPARE(MessagingHelper::preFilter(complexFilter, QMessage::Email), false);
    QCOMPARE(MessagingHelper::preFilter(complexFilter, QMessage::Sms), false);


}
#endif

