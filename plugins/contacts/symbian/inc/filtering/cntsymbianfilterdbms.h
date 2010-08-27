/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef CNTSYMBIANFILTERDBMS_H
#define CNTSYMBIANFILTERDBMS_H

#ifndef SYMBIAN_BACKEND_USE_SQLITE

#include "cntabstractcontactfilter.h"
#include <e32cmn.h>
#include <cntdef.h>

class CContactDatabase;
class CContactIdArray;
class CntAbstractContactSorter;
class CntTransformContact;
class CContactItemFieldDef;
class CContactItemFieldSet;

QTM_BEGIN_NAMESPACE
class QContactDetailFilter;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE
class CntSymbianFilter : public CntAbstractContactFilter
{
public:
    CntSymbianFilter(CContactDatabase& contactDatabase);
    ~CntSymbianFilter();

    /* from CntAbstractContactFilter */
    QList<QContactLocalId> contacts(const QContactFilter& filter,
                                    const QList<QContactSortOrder>& sortOrders,
                                    bool &filterSupported,
                                    QContactManager::Error* error);
    bool filterSupported(const QContactFilter& filter);

private:
    FilterSupport filterSupportLevel(const QContactFilter& filter);
    QList<QContactLocalId> filterContacts(const QContactFilter& filter,
                                          QContactManager::Error* error);
    void transformDetailFilterL(const QContactDetailFilter& detailFilter, CContactItemFieldDef*& fieldDef);
    TInt findContacts(
            CContactIdArray*& idArray,
            const CContactItemFieldDef& fieldDef,
            const TDesC& text) const;
    CContactIdArray* findContactsL(
            const CContactItemFieldDef& fieldDef,
            const TDesC& text) const;
    TInt matchContacts(
            CContactIdArray*& idArray,
            const TDesC& phoneNumber,
            const TInt matchLength);
    bool isFalsePositive(const CContactItemFieldSet& fieldSet, const TUid& fieldTypeUid, const TDesC& searchString);
    bool contactExists(const TContactItemId &contactId);
    void getMatchLengthL(TInt& matchLength);
    CContactDatabase &m_contactDatabase;
    CntAbstractContactSorter *m_contactSorter;
    CntTransformContact *m_transformContact;
#ifdef PBK_UNIT_TEST
    friend class tst_cntfilteringdbms;
#endif
};

#endif /*SYMBIAN_BACKEND_USE_SQLITE*/

#endif /* CNTSYMBIANFILTERDBMS_H */
