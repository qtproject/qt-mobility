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
#include <centralrepository.h>
#include <telephonydomaincrkeys.h>

#include "qcontactname.h"
#include "qcontactdetailfilter.h"
#include "qcontactphonenumber.h"

/* ... The macros changed names */
#if QT_VERSION < QT_VERSION_CHECK(4, 6, 0)
#define QT_TRAP_THROWING QT_TRANSLATE_SYMBIAN_LEAVE_TO_EXCEPTION
#define QT_TRYCATCH_LEAVING QT_TRANSLATE_EXCEPTION_TO_SYMBIAN_LEAVE
#endif

const TInt KMaxMatchLength(15);
const TInt KDefaultMatchLength(7);

QContactSymbianFilter::QContactSymbianFilter(CContactDatabase& contactDatabase):
    m_contactDatabase(contactDatabase)
{
}

QContactSymbianFilter::~QContactSymbianFilter()
{
}

/*!
 * The contact database version implementation for QContactManager::contacts
 * function. Symbian backend supports filtering with flag MatchContains.
 * TODO: other constraints.
 * All the other filtering flags fallback to the generic filtering done
 * in QContactManagerEngine.
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

        if (detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName
                && (filterSupported(filter) == Supported
                || filterSupported(filter) == SupportedPreFilterOnly))
        {
            QString number((detailFilter.value()).toString());
            TPtrC commPtr(reinterpret_cast<const TUint16*>(number.utf16()));

            TInt matchLength(KDefaultMatchLength);
            // no need to propagate error, we can use the default match length
            TRAP_IGNORE(getMatchLengthL(matchLength));

            TInt err = matchContacts(idArray, commPtr, matchLength);
            if(err != KErrNone)
            {
                // TODO: map error code
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
                && (filterSupported(filter) == Supported
                // All the name pre-filters are implemented as "MatchContains"
                || filterSupported(filter) == SupportedPreFilterOnly))
        {
            QString name((detailFilter.value()).toString());
            // TODO: this looks ugly
            TPtrC namePtr(reinterpret_cast<const TUint16*>(name.utf16()));
            TUid fieldUid(KNullUid);

            if(detailFilter.detailFieldName() == QContactName::FieldPrefix)
                fieldUid.iUid = KUidContactFieldPrefixNameValue;
            else if(detailFilter.detailFieldName() == QContactName::FieldFirst)
                fieldUid.iUid = KUidContactFieldGivenNameValue;
            else if(detailFilter.detailFieldName() == QContactName::FieldMiddle)
                fieldUid.iUid = KUidContactFieldAdditionalNameValue;
            else if(detailFilter.detailFieldName() == QContactName::FieldLast)
                fieldUid.iUid = KUidContactFieldFamilyNameValue;
            else if(detailFilter.detailFieldName() == QContactName::FieldSuffix)
                fieldUid.iUid = KUidContactFieldSuffixNameValue;

            TInt err = findContacts(idArray, fieldUid, namePtr);
            if(err != KErrNone)
            {
                // TODO: map error codes
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
 * QContactManager::filterSupported function. The possible return values are
 * Supported, NotSupported and SupportedPreFilterOnly. Supported means that
 * the filtering is implemented directly by the underlying database.
 * NotSupported means that QContactSymbianFilter::contacts will return an
 * error. And SupportedPreFilterOnly means that the filter is not supported,
 * but the QContactSymbianFilter::contacts will act like the filter was
 * supported. This means that the client must filter the pre-filtered set of
 * contacts to see if there are false positives included. Note that in some
 * cases the pre-filtering is not very effective.
 *
 * \a filter The QContactFilter to be checked.
 * \a return Supported in case the filter is supported. NotSupported in case
 * the filter is not supported. returns
 *
 * SupportedPreFilterOnly is returned in the following cases:
 * 1. matchFlags is set to Qt::MatchExactly (QContactSymbianFilter::contacts
 * will use Qt::MatchContains)
 * 2. matchFlags is set to Qt::MatchStartsWith (QContactSymbianFilter::contacts
 * will use Qt::MatchContains)
 * 3. matchFlags is set to Qt::MatchEndsWith (QContactSymbianFilter::contacts
 * will use Qt::MatchContains)
 * 4. matchFlags is set to Qt::MatchCaseSensitive (QContactSymbianFilter::contacts
 * will use Qt::MatchContains)
 */
QAbstractContactFilter::FilterSupport QContactSymbianFilter::filterSupported(const QContactFilter& filter)
{
    FilterSupport filterSupported(NotSupported);
    if (filter.type() == QContactFilter::ContactDetailFilter) {
        const QContactDetailFilter &detailFilter = static_cast<const QContactDetailFilter &>(filter);
        Qt::MatchFlags matchFlags = detailFilter.matchFlags();

        // Phone numbers
        if (detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName) {
            if (matchFlags == Qt::MatchEndsWith)
                filterSupported = Supported;
            else if (matchFlags == Qt::MatchExactly)
                filterSupported = SupportedPreFilterOnly;
        }
        // Names
        else if (detailFilter.detailDefinitionName() == QContactName::DefinitionName) {
            Qt::MatchFlags supportedPreFilterFlags =
                Qt::MatchExactly
                & Qt::MatchStartsWith
                & Qt::MatchEndsWith
                & Qt::MatchCaseSensitive;
            if (matchFlags == Qt::MatchContains) {
                filterSupported = Supported;
            }
            else if (matchFlags | supportedPreFilterFlags ==
                supportedPreFilterFlags ) {
                filterSupported = SupportedPreFilterOnly;
            }
        }
    }
    return filterSupported;
}

/*!
 * Find contacts based on a contact field contents.
 * \a idArray On return contains the ids of the contacts that have the field
 * defined that contains the find text.
 * \a fieldUid The UID of the contact database field to be searched.
 * \a text The text to be searched for.
 * \return Symbian error code.
 */
TInt QContactSymbianFilter::findContacts(
        CContactIdArray*& idArray,
        const TUid fieldUid,
        const TDesC& text) const
{
    CContactIdArray* idArrayTmp(0);
    TRAPD( err, idArrayTmp = findContactsL(fieldUid, text));
    if(err == KErrNone)
    {
        idArray = idArrayTmp;
    }
    return err;
}

/*!
 * Leaving implementation called by findContacts.
 */
CContactIdArray* QContactSymbianFilter::findContactsL(
        const TUid fieldUid,
        const TDesC& text) const
{
    CContactItemFieldDef* fieldDef = new (ELeave) CContactItemFieldDef();
    CleanupStack::PushL(fieldDef);
    fieldDef->SetReserveL(2);
    //TUid uid;
    //if( detailFilter. )
    //uid.iUid = KUidContactFieldGivenNameValue;
    fieldDef->AppendL(fieldUid);
    // TODO: the fields?
    //uid.iUid = KUidContactFieldFamilyNameValue;
    //fieldDef->AppendL(uid);
    CContactIdArray* idsArray = m_contactDatabase.FindLC(text, fieldDef);
    CleanupStack::Pop(idsArray); // Ownership transferred
    CleanupStack::PopAndDestroy(fieldDef);
    return idsArray;
}

/*
 * Find contacts based on a phone number.
 * \a idArray On return contains the ids of the contacts that match the filter.
 * \a phoneNumber The phone number to match
 * \a matchLength Match length; digits from right.
 */
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

/*
 * Get the match length setting. Digits to be used in matching (counted from
 * right).
 */
void QContactSymbianFilter::getMatchLengthL(TInt& matchLength)
{
    //Get number of digits used to match
    CRepository* repository = CRepository::NewL(KCRUidTelConfiguration);
    CleanupStack::PushL(repository);
    User::LeaveIfError(repository->Get(KTelMatchDigits, matchLength));
    CleanupStack::PopAndDestroy(repository);
}

#endif
