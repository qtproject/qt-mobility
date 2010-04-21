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

#define QT_STATICPLUGIN
#include "cntsymbianfilterdbms.h"

#include <cntdb.h>

#include <qtcontacts.h>
#include <QContactDetailFilter.h>
#include <QtTest/QtTest>
//#include <QMetaType>

QTM_USE_NAMESPACE

typedef QList<QContactLocalId> QContactIds;

Q_DECLARE_METATYPE(QContactFilter *)
Q_DECLARE_METATYPE(CntAbstractContactFilter::FilterSupport)
Q_DECLARE_METATYPE(QContactIds)

class tst_cntfilteringdbms : public QObject
{
Q_OBJECT

public:
    tst_cntfilteringdbms();
    virtual ~tst_cntfilteringdbms();

private slots:  // Init & cleanup
	void initTestCase();
	void cleanupTestCase();

private slots: // tests
    void filterContacts();
    void filterContacts_data() {addFilterData();}
    
private:
    QContactLocalId addContact(QString firstName, QString lastName, QString phoneNumber);
    void addFilterData();
    void addNewRow(QString defName, QString fieldName, int flags, CntAbstractContactFilter::FilterSupport filterSupport, QString filterCriterion, QList<QContactLocalId> matchingContacts);
    
private:
    QScopedPointer<QContactManager> m_contactManager;
    QScopedPointer<CContactDatabase> m_contactDatabase;
    QScopedPointer<CntSymbianFilter> m_dbmsFilter;
    QList<QContactFilter> m_contactFilters;
};

tst_cntfilteringdbms::tst_cntfilteringdbms()
{
    TRAPD(error, m_contactDatabase.reset(CContactDatabase::OpenL()));
    QVERIFY(error == KErrNone);
    m_dbmsFilter.reset(new CntSymbianFilter(*m_contactDatabase));
    m_contactManager.reset(new QContactManager("symbian"));
}

tst_cntfilteringdbms::~tst_cntfilteringdbms()
{
 
}

void tst_cntfilteringdbms::initTestCase()
{
    // Remove all contacts
    QList<QContactLocalId> contacts = m_contactManager->contactIds();
    m_contactManager->removeContacts(contacts, 0);
}

void tst_cntfilteringdbms::cleanupTestCase()
{
}

void tst_cntfilteringdbms::filterContacts()
{
    QFETCH(QContactFilter *, filter);
    QFETCH(CntAbstractContactFilter::FilterSupport, filterSupport);
    QFETCH(QContactIds, matchingContacts);
    
    QCOMPARE(m_dbmsFilter->filterSupportLevel(*filter), filterSupport);   
    
    QList<QContactSortOrder> sortOrders;
    QStringList definitionRestrictions;
    QList<QContactLocalId> contacts = m_contactManager->contactIds(*filter);
    
    qDebug() << contacts;
    
    QCOMPARE(contacts.count(), matchingContacts.count());
    foreach(QContactLocalId id, matchingContacts) {
        QVERIFY(contacts.contains(id));
    }    
}

QContactLocalId tst_cntfilteringdbms::addContact(QString firstName, QString lastName, QString phoneNumber)
{
    QContact c;
    
    QContactName n;
    n.setFirstName(firstName);
    n.setLastName(lastName);
    c.saveDetail(&n);
    
    QContactPhoneNumber nb;
    nb.setNumber(phoneNumber);
    c.saveDetail(&nb);
    
    m_contactManager->saveContact(&c);
    
    return c.localId();
}

void tst_cntfilteringdbms::addFilterData()
{
    QTest::addColumn<QContactFilter *>("filter");
    QTest::addColumn<CntAbstractContactFilter::FilterSupport>("filterSupport");
    QTest::addColumn<QContactIds>("matchingContacts");
    
    QContactLocalId abc = addContact("abc", "def", "123");
    QContactLocalId bcd = addContact("bcd", "efg", "1234567");
    QContactLocalId cde = addContact("cde", "fgh", "1234567890");
    QContactLocalId Abc = addContact("Abc", "Def", "+3581234567890");
    QContactLocalId Bcd = addContact("Bcd", "Efg", "0987654321");
    QContactLocalId Cde = addContact("Cde", "Fgh", "1111111111");
    
    qDebug() << "abc =" << abc;
    qDebug() << "bcd =" << bcd;
    qDebug() << "cde =" << cde;
    qDebug() << "Abc =" << Abc;
    qDebug() << "Bcd =" << Bcd;
    qDebug() << "Cde =" << Cde;
    
    QList<QContactLocalId> allContacts;
    allContacts << abc << bcd << cde << Abc << Bcd << Cde;
    
    addNewRow(QContactPhoneNumber::DefinitionName, QString(), QContactFilter::MatchExactly, CntAbstractContactFilter::NotSupported, "foobar", allContacts);
    addNewRow(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber, QContactFilter::MatchExactly, CntAbstractContactFilter::NotSupported, "123", QContactIds() << abc);
    addNewRow(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber, QContactFilter::MatchExactly, CntAbstractContactFilter::SupportedPreFilterOnly, "1234567", QContactIds() << bcd);
    addNewRow(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber, QContactFilter::MatchExactly, CntAbstractContactFilter::SupportedPreFilterOnly, "1234567890", QContactIds() << cde);
    addNewRow(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber, QContactFilter::MatchContains, CntAbstractContactFilter::NotSupported, "123", QContactIds() << abc << bcd << cde << Abc);
    addNewRow(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber, QContactFilter::MatchStartsWith, CntAbstractContactFilter::NotSupported, "123", QContactIds() << abc << bcd << cde);
    addNewRow(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber, QContactFilter::MatchEndsWith, CntAbstractContactFilter::NotSupported, "567", QContactIds() << bcd );
    addNewRow(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber, QContactFilter::MatchEndsWith, CntAbstractContactFilter::SupportedPreFilterOnly, "4567890", QContactIds() << cde << Abc );
    addNewRow(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber, QContactFilter::MatchFixedString, CntAbstractContactFilter::NotSupported, "123", QContactIds() << abc);
    addNewRow(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber, QContactFilter::MatchPhoneNumber, CntAbstractContactFilter::Supported, "1234567890", QContactIds() << cde << Abc);
    addNewRow(QContactPhoneNumber::DefinitionName, QContactPhoneNumber::FieldNumber, QContactFilter::MatchKeypadCollation, CntAbstractContactFilter::NotSupported, "123", QContactIds() << abc);
    
    addNewRow(QContactName::DefinitionName, QString(), QContactFilter::MatchExactly, CntAbstractContactFilter::NotSupported, "foobar", allContacts);
    addNewRow(QContactName::DefinitionName, QContactName::FieldFirstName, QContactFilter::MatchExactly, CntAbstractContactFilter::SupportedPreFilterOnly, "Abc", QContactIds() << abc << Abc);
    addNewRow(QContactName::DefinitionName, QContactName::FieldFirstName, QContactFilter::MatchExactly | QContactFilter::MatchCaseSensitive, CntAbstractContactFilter::SupportedPreFilterOnly, "Abc", QContactIds() << Abc);
    addNewRow(QContactName::DefinitionName, QContactName::FieldFirstName, QContactFilter::MatchContains, CntAbstractContactFilter::Supported, "Ab", QContactIds() << abc << Abc);
    addNewRow(QContactName::DefinitionName, QContactName::FieldFirstName, QContactFilter::MatchContains | QContactFilter::MatchCaseSensitive, CntAbstractContactFilter::SupportedPreFilterOnly, "Ab", QContactIds() << Abc);
    addNewRow(QContactName::DefinitionName, QContactName::FieldFirstName, QContactFilter::MatchStartsWith, CntAbstractContactFilter::SupportedPreFilterOnly, "Ab", QContactIds() << abc << Abc);
    addNewRow(QContactName::DefinitionName, QContactName::FieldFirstName, QContactFilter::MatchStartsWith | QContactFilter::MatchCaseSensitive, CntAbstractContactFilter::SupportedPreFilterOnly, "Ab", QContactIds() << Abc);
    addNewRow(QContactName::DefinitionName, QContactName::FieldFirstName, QContactFilter::MatchEndsWith, CntAbstractContactFilter::SupportedPreFilterOnly, "Cde", QContactIds() << cde << Cde);
    addNewRow(QContactName::DefinitionName, QContactName::FieldFirstName, QContactFilter::MatchEndsWith | QContactFilter::MatchCaseSensitive, CntAbstractContactFilter::SupportedPreFilterOnly, "Cde", QContactIds() << Cde);
    addNewRow(QContactName::DefinitionName, QContactName::FieldFirstName, QContactFilter::MatchFixedString, CntAbstractContactFilter::SupportedPreFilterOnly, "abc", QContactIds() << abc << Abc);
    addNewRow(QContactName::DefinitionName, QContactName::FieldFirstName, QContactFilter::MatchFixedString | QContactFilter::MatchCaseSensitive, CntAbstractContactFilter::SupportedPreFilterOnly, "abc", QContactIds() << abc);
    addNewRow(QContactName::DefinitionName, QContactName::FieldFirstName, QContactFilter::MatchKeypadCollation, CntAbstractContactFilter::NotSupported, "223", QContactIds() << bcd << Bcd);
    
    addNewRow(QContactDisplayLabel::DefinitionName, QString(), QContactFilter::MatchExactly, CntAbstractContactFilter::NotSupported, "foobar", allContacts);
    addNewRow(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel, QContactFilter::MatchExactly, CntAbstractContactFilter::NotSupported, "abc def", QContactIds() << abc << Abc);
    addNewRow(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel, QContactFilter::MatchExactly | QContactFilter::MatchCaseSensitive, CntAbstractContactFilter::NotSupported, "abc def", QContactIds() << abc);
    addNewRow(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel, QContactFilter::MatchContains, CntAbstractContactFilter::NotSupported, "d e", QContactIds() << bcd << Bcd);
    addNewRow(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel, QContactFilter::MatchContains | QContactFilter::MatchCaseSensitive, CntAbstractContactFilter::NotSupported, "d e", QContactIds() << bcd);
    addNewRow(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel, QContactFilter::MatchStartsWith, CntAbstractContactFilter::Supported, "B", QContactIds() << bcd << Bcd);
    addNewRow(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel, QContactFilter::MatchStartsWith | QContactFilter::MatchCaseSensitive, CntAbstractContactFilter::SupportedPreFilterOnly, "B", QContactIds() << Bcd);
    addNewRow(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel, QContactFilter::MatchEndsWith, CntAbstractContactFilter::NotSupported, "d Efg", QContactIds() << bcd << Bcd);
    addNewRow(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel, QContactFilter::MatchEndsWith | QContactFilter::MatchCaseSensitive, CntAbstractContactFilter::NotSupported, "d Efg", QContactIds() << Bcd);
    addNewRow(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel, QContactFilter::MatchFixedString, CntAbstractContactFilter::NotSupported, "abc def", QContactIds() << abc << Abc);
    addNewRow(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel, QContactFilter::MatchFixedString | QContactFilter::MatchCaseSensitive, CntAbstractContactFilter::NotSupported, "abc def", QContactIds() << abc);
    addNewRow(QContactDisplayLabel::DefinitionName, QContactDisplayLabel::FieldLabel, QContactFilter::MatchKeypadCollation, CntAbstractContactFilter::NotSupported, "223 334", QContactIds() << bcd << Bcd);
}



void tst_cntfilteringdbms::addNewRow(QString defName, QString fieldName, int flags, CntAbstractContactFilter::FilterSupport filterSupport, QString filterCriterion, QList<QContactLocalId> matchingContacts)
{
    QString s;
    foreach(QContactLocalId id, matchingContacts)
        s.append(QString("%1 ").arg(id));
    
    QString title = QString("Detail filter : defName=%1 fieldName=%2 matchFlags=0x%3 filterCriterion=%4 matchingContacts=%5").arg(defName).arg(fieldName).arg(flags,0,16).arg(filterCriterion).arg(s);
    
    QContactDetailFilter *f = new QContactDetailFilter();
    f->setDetailDefinitionName(defName, fieldName);
    f->setMatchFlags(QContactFilter::MatchFlags(flags));
    f->setValue(QVariant(filterCriterion));
    
    QTest::newRow(title.toAscii().constData()) << (QContactFilter*) f << filterSupport << matchingContacts;
}

QTEST_MAIN(tst_cntfilteringdbms);
#include "tst_cntfilteringdbms.moc"

