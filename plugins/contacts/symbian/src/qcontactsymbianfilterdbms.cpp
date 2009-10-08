/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__

#include "qcontactsymbianfilterdbms.h"

#include <cntdb.h>

#include <cntfield.h>

#include "qcontactname.h"
#include "qcontactdetailfilter.h"
#include "qcontactphonenumber.h"

/* ... The macros changed names */
#if QT_VERSION < QT_VERSION_CHECK(4, 6, 0)
#define QT_TRAP_THROWING QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION
#define QT_TRYCATCH_LEAVING QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE
#endif

QContactSymbianFilter::QContactSymbianFilter(CContactDatabase& contactDatabase):
    m_contactDatabase(contactDatabase)
{
}

QContactSymbianFilter::~QContactSymbianFilter()
{
}

/*!
 * The contact database version implementation for
 * QContactManager::contacts function.
 *
 * \a filter The QContactFilter to be used.
 * \a sortOrders The sort order to be used.
 * \a error On return, contains the possible error in filtering/sorting.
 */
QList<QUniqueId> QContactSymbianFilter::contacts(
            const QContactFilter& filter,
            const QList<QContactSortOrder>& sortOrders,
            QContactManager::Error& error)
{
    // TODO: handle also sort orders

    QList<QUniqueId> matches;
    CContactIdArray* idArray(0);

    if (filter.type() == QContactFilter::ContactDetailFilter)
    {
        const QContactDetailFilter &detailFilter = static_cast<const QContactDetailFilter &>(filter);

        if (detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName)
        {
            QString number((detailFilter.value()).toString());
            TPtrC commPtr(reinterpret_cast<const TUint16*>(number.utf16()));
            TInt err = matchContacts(idArray, commPtr, 7);
            if(err != KErrNone)
            {
                // TODO: error code
                error = QContactManager::UnspecifiedError;
            }
            else
            {
                for(int i(0); i < idArray->Count(); i++) {
                    matches.append(QUniqueId((*idArray)[i]));
                }
            }
        }
        else if (detailFilter.detailDefinitionName() == QContactName::DefinitionName
                && detailFilter.detailFieldName() == QContactName::FieldFirst
                && detailFilter.matchFlags() == Qt::MatchContains)
        {
            QString name((detailFilter.value()).toString());
            // TODO: this looks ugly
            TPtrC namePtr(reinterpret_cast<const TUint16*>(name.utf16()));
            TInt err = findContacts(idArray, namePtr);
            if(err != KErrNone)
            {
                // TODO: map error code
                error = QContactManager::UnspecifiedError;
            }
            else
            {
                for(int i = 0; i < idArray->Count(); i++) {
                    matches.append(QUniqueId((*idArray)[i]));
                }
            }
        }
        else
        {
            error = QContactManager::NotSupportedError;
        }
    }
    else
    {
        error = QContactManager::NotSupportedError;
    }

    delete idArray;

    return matches;
}

/*!
 * The contact database version implementation for
 * QContactManager::filterSupported function.
 * \a filter The QContactFilter to be checked.
 */
bool QContactSymbianFilter::filterSupported(const QContactFilter& filter)
{
    TBool supported(false);
    if (filter.type() == QContactFilter::ContactDetailFilter) {
        const QContactDetailFilter &detailFilter = static_cast<const QContactDetailFilter &>(filter);
        if (detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName)
            supported = true;
        else if(detailFilter.detailDefinitionName() == QContactName::DefinitionName
                && detailFilter.detailFieldName() == QContactName::FieldFirst
                && detailFilter.matchFlags() == Qt::MatchContains)
            supported = true;
    }
    return supported;
}

TInt QContactSymbianFilter::findContacts(CContactIdArray*& idArray, const TDesC& text) const
{
    CContactIdArray* idArrayTmp(0);
    TRAPD( err, idArrayTmp = findContactsL(text));
    if(err == KErrNone)
    {
        idArray = idArrayTmp;
    }
    return err;
}

/*!
 * \a text The text to be searched for.
 */
CContactIdArray* QContactSymbianFilter::findContactsL(const TDesC& text) const
{
    CContactItemFieldDef* fieldDef = new (ELeave) CContactItemFieldDef();
    CleanupStack::PushL(fieldDef);
    fieldDef->SetReserveL(2);
    TUid uid;
    //if( detailFilter. )
    uid.iUid = KUidContactFieldGivenNameValue;
    fieldDef->AppendL(uid);
    // TODO: the fields?
    //uid.iUid = KUidContactFieldFamilyNameValue;
    //fieldDef->AppendL(uid);
    CContactIdArray* idsArray = m_contactDatabase.FindLC(text, fieldDef);
    CleanupStack::Pop(idsArray); // Ownership transferred
    CleanupStack::PopAndDestroy(fieldDef);
    return idsArray;
}

TInt QContactSymbianFilter::matchContacts(
        CContactIdArray*& idArray,
        const TDesC& phoneNumber,
        const TInt matchLength)
{
    CContactIdArray* idArrayTmp(0);
    TRAPD( err, idArrayTmp = m_contactDatabase.MatchPhoneNumberL(phoneNumber, matchLength));
    if(err == KErrNone)
    {
        idArray = idArrayTmp;
    }
    return err;
}

#endif
