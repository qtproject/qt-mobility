/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__

#include "cntsymbianfilterdbms.h"
#include "cntsymbiantransformerror.h"

#include <cntdb.h>
#include <cntfield.h>
#include <centralrepository.h>
// TODO: the following header is not available in public SDKs
//#include <telephonydomaincrkeys.h>

#include "qcontactname.h"
#include "qcontactdetailfilter.h"
#include "qcontactphonenumber.h"
#include "cntsymbianengine_p.h"
#include "cntsymbiansorterdbms.h"

// Telephony Configuration API
// Keys under this category are used in defining telephony configuration.
const TUid KCRUidTelConfiguration = {0x102828B8};
// Amount of digits to be used in contact matching.
// This allows a customer to variate the amount of digits to be matched.
const TUint32 KTelMatchDigits                               = 0x00000001;
// Default match length
const TInt KDefaultMatchLength(7);

CntSymbianFilterDbms::CntSymbianFilterDbms(CContactDatabase& contactDatabase):
    m_contactDatabase(contactDatabase),
    m_contactSorter(0),
    m_transformContact(0)
{
    // TODO: take CntTransformContact ref as a parameter?
    m_transformContact = new CntTransformContact;
    m_contactSorter = new CntSymbianSorterDbms(m_contactDatabase, *m_transformContact);
}

CntSymbianFilterDbms::~CntSymbianFilterDbms()
{
    delete m_contactSorter;
    delete m_transformContact;
}

/*!
 * The contact database version implementation for QContactManager::contacts
 * function. See filterSupported for the list of supported filters.
 * All the other filtering flags fallback to the generic filtering done
 * in QContactManagerEngine. Contacts are sorted only if the sort order is
 * supported by contacts database. See CntSymbianSorterDbms::sortOrderSupported
 * for the list of supported sort orders.
 *
 * \a filter The QContactFilter to be used.
 * \a sortOrders The sort orders to be used. If the sort orders are not
 * supported by contacts database this parameter is ignored and sorting needs
 * to be done by the caller.
 * \a error On return, contains the possible error in filtering/sorting.
 */
QList<QContactLocalId> CntSymbianFilterDbms::contacts(
            const QContactFilter& filter,
            const QList<QContactSortOrder>& sortOrders,
            QContactManager::Error& error)
{
    QList<QContactLocalId> matches;
    CContactIdArray* idArray(0);

    if (filter.type() == QContactFilter::ContactDetailFilter)
    {
        const QContactDetailFilter &detailFilter = static_cast<const QContactDetailFilter &>(filter);

        // Phone numbers
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
                CntSymbianTransformError::transformError(err, error);
            }
        }
        // Names
        else if (filterSupported(filter) == Supported
                // The pre-filters are implemented as "MatchContains"
                || filterSupported(filter) == SupportedPreFilterOnly) {
            QString name((detailFilter.value()).toString());
            // TODO: this looks ugly
            TPtrC namePtr(reinterpret_cast<const TUint16*>(name.utf16()));
            CContactItemFieldDef *fieldDef(0);
            TRAPD(err, transformDetailFilterL(detailFilter, fieldDef));
            if(err != KErrNone){
                CntSymbianTransformError::transformError(err, error);
            } else {
                Q_ASSERT_X(fieldDef->Count() > 0, "CntSymbianFilterDbms", "Illegal field def");
                TInt err = findContacts(idArray, *fieldDef, namePtr);
                if(err != KErrNone)
                    CntSymbianTransformError::transformError(err, error);
            }
            delete fieldDef;
        } else {
            error = QContactManager::NotSupportedError;
        }
    } else {
        error = QContactManager::NotSupportedError;
    }

    if(idArray && (error == QContactManager::NoError)) {
        // copy the matching contact ids
        for(int i(0); i < idArray->Count(); i++) {
            QContactLocalId id((*idArray)[i]);
            matches.append(QContactLocalId((*idArray)[i]));
        }

        // sort the matching contacts
        if(!sortOrders.isEmpty()) {
            if(m_contactSorter->sortOrderSupported(sortOrders)) {
                matches = m_contactSorter->sort(matches, sortOrders, error);
            }
        }
    }

    delete idArray;

    return matches;
}

/*!
 * The contact database version implementation for
 * QContactManager::filterSupported function. The possible return values are
 * Supported, NotSupported and SupportedPreFilterOnly. Supported means that
 * the filtering is implemented directly by the underlying database.
 * NotSupported means that CntSymbianFilterDbms::contacts will return an
 * error. And SupportedPreFilterOnly means that the filter is not supported,
 * but the CntSymbianFilterDbms::contacts will act like the filter was
 * supported. This means that the client must filter the pre-filtered set of
 * contacts to see if there are false positives included. Note that in some
 * cases the pre-filtering is not very effective.
 *
 * \a filter The QContactFilter to be checked.
 * \a return Supported in case the filter is supported. NotSupported in case
 * the filter is not supported. returns
 *
 * SupportedPreFilterOnly is returned in the following cases:
 * 1. matchFlags is set to QContactFilter::MatchExactly (CntSymbianFilterDbms::contacts
 * will use QContactFilter::MatchContains)
 * 2. matchFlags is set to QContactFilter::MatchStartsWith (CntSymbianFilterDbms::contacts
 * will use QContactFilter::MatchContains)
 * 3. matchFlags is set to QContactFilter::MatchEndsWith (CntSymbianFilterDbms::contacts
 * will use QContactFilter::MatchContains)
 * 4. matchFlags is set to QContactFilter::MatchCaseSensitive (CntSymbianFilterDbms::contacts
 * will use QContactFilter::MatchContains)
 */
CntAbstractContactFilter::FilterSupport CntSymbianFilterDbms::filterSupported(const QContactFilter& filter)
{
    FilterSupport filterSupported(NotSupported);
    if (filter.type() == QContactFilter::ContactDetailFilter) {
        const QContactDetailFilter &detailFilter = static_cast<const QContactDetailFilter &>(filter);
        QContactFilter::MatchFlags matchFlags = detailFilter.matchFlags();

        // Phone numbers
        if (detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName) {
            if (matchFlags == QContactFilter::MatchEndsWith)
                filterSupported = Supported;
            else if (matchFlags == QContactFilter::MatchExactly)
                filterSupported = SupportedPreFilterOnly;
        }
        // Names
        else if (detailFilter.detailDefinitionName() == QContactName::DefinitionName
                || detailFilter.detailDefinitionName() == QContactNickname::DefinitionName
                || detailFilter.detailDefinitionName() == QContactEmailAddress::DefinitionName) {
            QContactFilter::MatchFlags supportedPreFilterFlags =
                QContactFilter::MatchExactly
                & QContactFilter::MatchStartsWith
                & QContactFilter::MatchEndsWith
                & QContactFilter::MatchCaseSensitive;
            if (matchFlags == QContactFilter::MatchContains) {
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

void CntSymbianFilterDbms::transformDetailFilterL(
        const QContactDetailFilter &detailFilter,
        CContactItemFieldDef *&fieldDef)
{
    const TInt defaultReserve(1);
    const TInt nameFieldsCount(5);

    CContactItemFieldDef* tempFieldDef = new (ELeave) CContactItemFieldDef();
    CleanupStack::PushL(tempFieldDef);
    tempFieldDef->SetReserveL(defaultReserve);

    // TODO: Refactor to use the transform classes
    // Names
    if(detailFilter.detailDefinitionName() == QContactName::DefinitionName) {
        if(detailFilter.detailFieldName() == QContactName::FieldPrefix) {
            tempFieldDef->AppendL(KUidContactFieldPrefixName);
        } else if(detailFilter.detailFieldName() == QContactName::FieldFirst) {
            tempFieldDef->AppendL(KUidContactFieldGivenName);
        } else if(detailFilter.detailFieldName() == QContactName::FieldMiddle) {
            tempFieldDef->AppendL(KUidContactFieldAdditionalName);
        } else if(detailFilter.detailFieldName() == QContactName::FieldLast) {
            tempFieldDef->AppendL(KUidContactFieldFamilyName);
        } else if(detailFilter.detailFieldName() == QContactName::FieldSuffix) {
            tempFieldDef->AppendL(KUidContactFieldSuffixName);
        } else {
            // default to all name fields
            tempFieldDef->SetReserveL(nameFieldsCount);
            tempFieldDef->AppendL(KUidContactFieldPrefixName);
            tempFieldDef->AppendL(KUidContactFieldGivenName);
            tempFieldDef->AppendL(KUidContactFieldAdditionalName);
            tempFieldDef->AppendL(KUidContactFieldFamilyName);
            tempFieldDef->AppendL(KUidContactFieldSuffixName);
        }
    }
    // Nick name
    else if(detailFilter.detailDefinitionName() == QContactNickname::DefinitionName) {
        tempFieldDef->AppendL(KUidContactFieldSecondName);
    }
    // Email
    else if(detailFilter.detailDefinitionName() == QContactEmailAddress::DefinitionName) {
        tempFieldDef->AppendL(KUidContactFieldEMail);
    }

    CleanupStack::Pop(tempFieldDef);
    fieldDef = tempFieldDef;
}

/*!
 * Find contacts based on a contact field contents.
 * \a idArray On return contains the ids of the contacts that have the field
 * defined that contains the find text.
 * \a fieldUid The UID of the contact database field to be searched.
 * \a text The text to be searched for.
 * \return Symbian error code.
 */
TInt CntSymbianFilterDbms::findContacts(
        CContactIdArray*& idArray,
        const CContactItemFieldDef& fieldDef,
        const TDesC& text) const
{
    CContactIdArray* idArrayTmp(0);
    TRAPD( err, idArrayTmp = findContactsL(fieldDef, text));
    if(err == KErrNone)
    {
        idArray = idArrayTmp;
    }
    return err;
}

/*!
 * Leaving implementation called by findContacts.
 */
CContactIdArray* CntSymbianFilterDbms::findContactsL(
        const CContactItemFieldDef& fieldDef,
        const TDesC& text) const
{
    CContactIdArray* idsArray = m_contactDatabase.FindLC(text, &fieldDef);
    CleanupStack::Pop(idsArray); // Ownership transferred
    return idsArray;
}

/*
 * Find contacts based on a phone number.
 * \a idArray On return contains the ids of the contacts that match the filter.
 * \a phoneNumber The phone number to match
 * \a matchLength Match length; digits from right.
 */
TInt CntSymbianFilterDbms::matchContacts(
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
void CntSymbianFilterDbms::getMatchLengthL(TInt& matchLength)
{
    //Get number of digits used to match
    CRepository* repository = CRepository::NewL(KCRUidTelConfiguration);
    CleanupStack::PushL(repository);
    User::LeaveIfError(repository->Get(KTelMatchDigits, matchLength));
    CleanupStack::PopAndDestroy(repository);
}

#endif
