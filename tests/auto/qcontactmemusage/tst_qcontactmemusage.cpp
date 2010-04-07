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

#include <malloc.h>
#include <QtTest/QtTest>

#include "qtcontacts.h"
#include "qcontactmanagerdataholder.h" //QContactManagerDataHolder

//TESTED_CLASS=
//TESTED_FILES=


// Define global op new so we can track some amount of allocations
static qulonglong cAllocated = 0;
static qulonglong nAllocations = 0;

/*
void * operator new(size_t sz)
{
    nAllocations++;
    cAllocated += sz;
    return malloc(sz);
}
void * operator new[](size_t sz)
{
    nAllocations++;
    cAllocated += sz;
    return malloc(sz);
}
void operator delete(void * block)
{
    if (block)
        free(block);
}

void operator delete[](void *block)
{
    if (block)
        free(block);
}
*/


// Also hook malloc (with glibc)
void *(*old_hook)(size_t, const void*);
void *qmallochook(size_t size, const void* )
{
    void * ret;
    nAllocations++;
    cAllocated += size;
    __malloc_hook = old_hook;
    ret = malloc(size);
    __malloc_hook = qmallochook;
    return ret;
}

// I can't seem to get the __malloc_init_hook method working (C++ mangling?)
static int qmallocinithook()
{
    old_hook = __malloc_hook;
    __malloc_hook = qmallochook;
    return 1;
}

Q_CONSTRUCTOR_FUNCTION(qmallocinithook);

class QMemUsed
{
public:
    QMemUsed() : mLabel(0), mAllocated(cAllocated), mAllocations(nAllocations), mItems(0)
    {
    }
    QMemUsed(const char* name) : mLabel(name), mAllocated(cAllocated), mAllocations(nAllocations), mItems(0)
    {
    }
    QMemUsed(const char* name, int nItems) : mLabel(name), mAllocated(cAllocated), mAllocations(nAllocations), mItems(nItems)
    {
    }
    ~QMemUsed()
    {
        mAllocated = cAllocated - mAllocated;
        mAllocations = nAllocations - mAllocations;
        QString allocations = mAllocations > 1 ? QString("allocations") : QString("allocation");
        if (mItems > 1) {
            QString msg("%1 bytes in %2 %8 (for %3 x %6 -> %4 bytes, %5 allocations per %7)");
            qDebug() << msg.arg(mAllocated).arg(mAllocations).arg(mItems).arg(((double)mAllocated) / mItems).arg(((double) mAllocations) / mItems).arg(mLabel).arg(mLabel).arg(allocations).toLatin1().constData();
        } else if (mLabel.latin1()){
            QString msg("%1 bytes in %2 %4 for %3");
            qDebug() << msg.arg(mAllocated).arg(mAllocations).arg(mLabel).arg(allocations).toLatin1().constData();
        } else {
            QString msg("%1 bytes in %2 %3");
            qDebug() << msg.arg(mAllocated).arg(mAllocations).arg(allocations).toLatin1().constData();
        }
    }
    QLatin1String mLabel;
    qulonglong mAllocated;
    qulonglong mAllocations;
    int mItems;
};

QTM_USE_NAMESPACE
class tst_QContactMemUsage : public QObject
{
Q_OBJECT

public:
    tst_QContactMemUsage();
    virtual ~tst_QContactMemUsage();

private:
    QContactManagerDataHolder managerDataHolder;

public slots:
    void init();
    void cleanup();

private slots:
    void single();
    void multiple();

private:
    QMemUsed mem;
};

tst_QContactMemUsage::tst_QContactMemUsage()
{
}

tst_QContactMemUsage::~tst_QContactMemUsage()
{
}

void tst_QContactMemUsage::init()
{
}

void tst_QContactMemUsage::cleanup()
{
}

void tst_QContactMemUsage::single()
{
    // Try single allocations
    {
        QMemUsed m("PhoneNumber");
        QContactPhoneNumber p1;
    }

    {
        QMemUsed m("Address");
        QContactAddress a;
    }

    {
        QMemUsed m("Name");
        QContactName p1;
    }
}

void tst_QContactMemUsage::multiple()
{
    // First 10
    {
        QMemUsed m("PhoneNumber", 10);
        QContactPhoneNumber p1;
        QContactPhoneNumber p2;
        QContactPhoneNumber p3;
        QContactPhoneNumber p4;
        QContactPhoneNumber p5;
        QContactPhoneNumber p6;
        QContactPhoneNumber p7;
        QContactPhoneNumber p8;
        QContactPhoneNumber p9;
        QContactPhoneNumber p10;
    }

    {
        QMemUsed m("Address", 10);
        QContactAddress p1;
        QContactAddress p2;
        QContactAddress p3;
        QContactAddress p4;
        QContactAddress p5;
        QContactAddress p6;
        QContactAddress p7;
        QContactAddress p8;
        QContactAddress p9;
        QContactAddress p10;
    }

    {
        QMemUsed m("Address", 10);
        QContactAddress p1;
        QContactAddress p2;
        QContactAddress p3;
        QContactAddress p4;
        QContactAddress p5;
        QContactAddress p6;
        QContactAddress p7;
        QContactAddress p8;
        QContactAddress p9;
        QContactAddress p10;
    }

    {
        QMemUsed m("Name", 10);
        QContactName p1;
        QContactName p2;
        QContactName p3;
        QContactName p4;
        QContactName p5;
        QContactName p6;
        QContactName p7;
        QContactName p8;
        QContactName p9;
        QContactName p10;
    }

    {
        QList<QContactDetail> details;
        QMemUsed m("PhoneNumber", 100);
        for (int i = 0; i < 100; i++) {
            QContactPhoneNumber a;
            details.append(a);
        }
    }


    {
        QList<QContactDetail> details;
        QMemUsed m("Address", 100);
        for (int i = 0; i < 100; i++) {
            QContactAddress a;
            details.append(a);
        }
    }


    {
        QList<QContactDetail> details;
        QMemUsed m("Name", 100);
        for (int i = 0; i < 100; i++) {
            QContactName a;
            details.append(a);
        }
    }


    // Now set some values
    {
        QString v("1234");
        QMemUsed m("PhoneNumber with number");
        QContactPhoneNumber p;
        p.setNumber(v);
    }

    {
        QVariant v("1234");
        QMemUsed m("PhoneNumber with number as variant");
        QContactPhoneNumber p;
        p.setValue(QContactPhoneNumber::FieldNumber, v);
    }


    {
        QMemUsed m("PhoneNumber with number and subtype");
        QContactPhoneNumber p;
        p.setNumber("1234");
        p.setSubTypes(QContactPhoneNumber::SubTypeMobile);
    }

    {
        QMemUsed m("PhoneNumber with custom fields");
        QContactPhoneNumber p;
        QVariant v = QString("1234");
        p.setValue(QContactAddress::FieldCountry, v);
        p.setValue(QContactUrl::FieldUrl, v);
        p.setValue(QContactEmailAddress::FieldEmailAddress, v);
        p.setValue(QContactName::FieldFirstName, v);
        p.setValue(QContactName::FieldLastName, v);
        p.setValue(QContactName::FieldMiddleName, v);
        p.setValue(QContactName::FieldPrefix, v);
        p.setValue(QContactName::FieldSuffix, v);
        p.setValue(QContactAddress::FieldLocality, v);
        p.setValue(QContactAddress::FieldRegion, v);
    }


}

QTEST_MAIN(tst_QContactMemUsage)
#include "tst_qcontactmemusage.moc"
