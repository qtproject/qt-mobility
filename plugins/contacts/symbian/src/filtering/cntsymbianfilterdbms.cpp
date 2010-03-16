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

#ifndef SYMBIAN_BACKEND_USE_SQLITE

#include <qcontactdetail.h>
#include <QSet>

#include "cntsymbianfilterdbms.h"
#include "cnttransformcontact.h"
#include "cntsymbiantransformerror.h"

#include <cntdb.h>
#include <cntfield.h>
#include <centralrepository.h>

#include "qcontactname.h"
#include "qcontactdetailfilter.h"
#include "qcontactphonenumber.h"
#include "cntsymbiansorterdbms.h"

// Telephony Configuration API
// Keys under this category are used in defining telephony configuration.
const TUid KCRUidTelConfiguration = {0x102828B8};
// Amount of digits to be used in contact matching.
// This allows a customer to variate the amount of digits to be matched.
const TUint32 KTelMatchDigits                               = 0x00000001;
// Default match length
const TInt KDefaultMatchLength(7);

CntSymbianFilter::CntSymbianFilter(CContactDatabase& contactDatabase):
    m_contactDatabase(contactDatabase),
    m_contactSorter(0),
    m_transformContact(0)
{
    // TODO: take CntTransformContact ref as a parameter?
    m_transformContact = new CntTransformContact;
    m_contactSorter = new CntSymbianSorterDbms(m_contactDatabase, *m_transformContact);
}

CntSymbianFilter::~CntSymbianFilter()
{
    delete m_contactSorter;
    delete m_transformContact;
}

/*!
 * The contact database version implementation for QContactManager::contacts
 * function. See filterSupported for the list of supported filters.
 * All the other filtering flags fallback to the generic filtering done
 * in QContactManagerEngine (expected to be done by to the caller). Contacts
 * are sorted only if the sort order is supported by contacts database. See
 * CntSymbianSorterDbms::filterSupportLevel for the list of supported sort
 * orders.
 *
 * \a filter The QContactFilter to be used.
 * \a sortOrders The sort orders to be used. If the sort orders are not
 * supported by contacts database this parameter is ignored and sorting needs
 * to be done by the caller.
 * \a error On return, contains the possible error in filtering/sorting.
 */
QList<QContactLocalId> CntSymbianFilter::contacts(
    const QContactFilter &filter,
    const QList<QContactSortOrder> &sortOrders,
    bool &filterSupportedFlag,
    QContactManager::Error &error)
{
    QList<QContactLocalId> result;

    // No need to proceed if some of the filters in the chain is not supported
    if(!filterSupportedFlag) return result;

    // Intersection filter is handled by a recursive function call for each
    // contained filter (unless at least one requires slow filtering)
    if (filter.type() == QContactFilter::IntersectionFilter) {
        QList<QContactFilter> filters = ((QContactIntersectionFilter) filter).filters();
        for(int i(0); filterSupportedFlag && i < filters.count(); i++) {
            if(result.isEmpty())
                result = contacts(filters[i], sortOrders, filterSupportedFlag, error);
            else
                result = contacts(filters[i], sortOrders, filterSupportedFlag, error).toSet().intersect(result.toSet()).toList();
        }
    // Union filter is handled by a recursive function call for each
    // contained filter (unless at least one requires slow filtering)
    } else if (filter.type() == QContactFilter::UnionFilter) {
        QList<QContactFilter> filters = ((QContactUnionFilter) filter).filters();
        for(int i(0); filterSupportedFlag && i < filters.count(); i++) {
            if(result.isEmpty())
                result = contacts(filters[i], sortOrders, filterSupportedFlag, error);
            else
                result = (contacts(filters[i], sortOrders, filterSupportedFlag, error).toSet() + result.toSet()).toList();
        }
    // Detail filter with a string list is split and re-constructed into
    // an intersection filter
    } else if (filter.type() == QContactFilter::ContactDetailFilter
            && (static_cast<const QContactDetailFilter &>(filter)).value().type() == QVariant::StringList) {
        QStringList values = (static_cast<const QContactDetailFilter &>(filter)).value().toStringList();
        QContactIntersectionFilter intersectionFilter;
        foreach(const QString& value, values) {
            QContactDetailFilter detailFilter = filter;
            detailFilter.setValue(value);
            intersectionFilter.append(detailFilter);
        }
        // The resulting filter is handled with a recursive function call
        result = contacts(intersectionFilter, sortOrders, filterSupportedFlag, error);
    } else {
        if (filterSupportLevel(filter) == Supported) {
            filterSupportedFlag = true;
            // Filter supported, use as the result directly
            result = filterContacts(filter, error);
        } else if (filterSupportLevel(filter) == SupportedPreFilterOnly) {
            // Filter only does pre-filtering, the caller is responsible of
            // removing possible false positives after filtering
            filterSupportedFlag = false;
            result = filterContacts(filter, error);
        } else {
            // Don't do filtering here, return all contact ids and tell the
            // caller to do slow filtering
            filterSupportedFlag = false;
            result = filterContacts(QContactInvalidFilter(), error);
        }
    }

    return result;
}

/*!
 * The contact database version implementation for
 * QContactManager::filterSupport function.
*/
bool CntSymbianFilter::filterSupported(const QContactFilter& filter)
{
    TBool result;

    // Map filter support into a boolean value
    FilterSupport support = filterSupportLevel(filter);
    if (support == Supported || support == SupportedPreFilterOnly) {
        result = true;
    } else {
        result = false;
    }
    return result;
}

/*!
 * The possible return values are Supported, NotSupported and
 * SupportedPreFilterOnly.
 *
 * Supported means that the filtering is implemented directly by the underlying
 * database. NotSupported means that CntSymbianFilter::contacts will
 * return an error. And SupportedPreFilterOnly means that the filter is not
 * fully supported, but the CntSymbianFilter::contacts will act like the
 * filter was supported for performance reasons, returning a result that may
 * contain false positives. This means that the client must filter the
 * pre-filtered set of contacts to see if there are false positives included.
 * Note that the pre-filtering does not give any performance benefits if the
 * result contains all contacts or almost all contacts.
 *
 * \a filter The QContactFilter to be checked.
 * \a return Supported in case the filter is supported. NotSupported in case
 * the filter is not supported. returns
 *
 * SupportedPreFilterOnly is returned in the following cases:
 * 1. matchFlags is set to QContactFilter::MatchExactly (CntSymbianFilter::contacts
 * will use QContactFilter::MatchContains)
 * 2. matchFlags is set to QContactFilter::MatchStartsWith (CntSymbianFilter::contacts
 * will use QContactFilter::MatchContains)
 * 3. matchFlags is set to QContactFilter::MatchEndsWith (CntSymbianFilter::contacts
 * will use QContactFilter::MatchContains)
 * 4. matchFlags is set to QContactFilter::MatchCaseSensitive (CntSymbianFilter::contacts
 * will use QContactFilter::MatchContains)
 */
CntAbstractContactFilter::FilterSupport CntSymbianFilter::filterSupportLevel(const QContactFilter& filter)
{
    if (filter.type() == QContactFilter::ContactDetailFilter) {
        const QContactDetailFilter &detailFilter = static_cast<const QContactDetailFilter &>(filter);
        QContactFilter::MatchFlags matchFlags = detailFilter.matchFlags();
        const QString defName = detailFilter.detailDefinitionName();
        const QString fieldName = detailFilter.detailFieldName();
        
        // Filter must target a certain field
        if (fieldName.isEmpty()) {
            return NotSupported;
        }         

        // Phone numbers
        if (defName == QContactPhoneNumber::DefinitionName) {
            
            if (matchFlags == QContactFilter::MatchPhoneNumber) {
                return Supported;
            }
            
            if (matchFlags == QContactFilter::MatchExactly ||
                matchFlags == QContactFilter::MatchEndsWith ||
                matchFlags == QContactFilter::MatchFixedString) {
                return SupportedPreFilterOnly;
            }
        // Names
        } else if (defName == QContactName::DefinitionName
                || defName == QContactNickname::DefinitionName
                || defName == QContactEmailAddress::DefinitionName) {

            if (matchFlags == QContactFilter::MatchContains) {
                return Supported;
            }

            // Don't care about case sensitivity flag because: 
            // 1) We do not support it. 2) We are doing prefiltering only.
            matchFlags &= ~QContactFilter::MatchFlags(QContactFilter::MatchCaseSensitive);
            
            if (matchFlags == QContactFilter::MatchExactly ||
                matchFlags == QContactFilter::MatchContains ||
                matchFlags == QContactFilter::MatchStartsWith ||
                matchFlags == QContactFilter::MatchEndsWith ||
                matchFlags == QContactFilter::MatchFixedString) {
                return SupportedPreFilterOnly;
            }            
        // display label, this is a special case that contains several name
        // fields and company name
        //TODO: "unnamed" display label is not supported currently
        } else if (defName == QContactDisplayLabel::DefinitionName) {
            
            if (matchFlags == QContactFilter::MatchStartsWith) {
                return Supported;
            }
            
            // Don't care about case sensitivity flag because: 
            // 1) We do not support it. 2) We are doing prefiltering only.
            matchFlags &= ~QContactFilter::MatchFlags(QContactFilter::MatchCaseSensitive);
            
            if (matchFlags == QContactFilter::MatchStartsWith) {
                return SupportedPreFilterOnly;
            }              
        }
    }
    return NotSupported;
}

QList<QContactLocalId> CntSymbianFilter::filterContacts(
    const QContactFilter& filter,
    QContactManager::Error& error)
{
    QList<QContactLocalId> matches;
    CContactIdArray* idArray(0);

    if (filter.type() == QContactFilter::InvalidFilter ){
        TTime epoch(0);
        idArray = m_contactDatabase.ContactsChangedSinceL(epoch); // return all contacts
    } else if(filterSupportLevel(filter) == NotSupported) {
        error = QContactManager::NotSupportedError;
    } else if (filter.type() == QContactFilter::ContactDetailFilter) {
        const QContactDetailFilter &detailFilter = static_cast<const QContactDetailFilter &>(filter);

        // Phone numbers
        if (detailFilter.detailDefinitionName() == QContactPhoneNumber::DefinitionName) {
            QString number((detailFilter.value()).toString());
            TPtrC commPtr(reinterpret_cast<const TUint16*>(number.utf16()));

            TInt matchLength(KDefaultMatchLength);
            // no need to propagate error, we can use the default match length
            TRAP_IGNORE(getMatchLengthL(matchLength));

            TInt err = matchContacts(idArray, commPtr, matchLength);
            if(err != KErrNone) {
                CntSymbianTransformError::transformError(err, error);
            }
        // Names, e-mail, display label (other flags)
        } else {
            QString name((detailFilter.value()).toString());
            TPtrC namePtr(reinterpret_cast<const TUint16*>(name.utf16()));
            CContactItemFieldDef *fieldDef(0);
            TRAPD(err, transformDetailFilterL(detailFilter, fieldDef));
            if(err != KErrNone){
                CntSymbianTransformError::transformError(err, error);
            } else {
                Q_ASSERT_X(fieldDef->Count() > 0, "CntSymbianFilter", "Illegal field def");
                TInt err = findContacts(idArray, *fieldDef, namePtr);
                if(err != KErrNone) {
                    CntSymbianTransformError::transformError(err, error);
                }

                // Display label special case with "starts with" flag;
                // False positives are removed here for performance reasons
                // (this is a very common use case in a name list view)
                //
                // Another option might be to use CContactDatabase::FindInTextDefLC
                // for filtering with display label
                if (detailFilter.detailDefinitionName() == QContactDisplayLabel::DefinitionName
                    && detailFilter.matchFlags() == QContactFilter::MatchStartsWith) {

                    // Remove false positives
                    for(TInt i(0); i < idArray->Count(); ++i) {
                        CContactItem* contactItem = m_contactDatabase.ReadContactLC((*idArray)[i]);
                        const CContactItemFieldSet& fieldSet(contactItem->CardFields());
                        if(isFalsePositive(fieldSet, KUidContactFieldGivenName, namePtr)
                           && isFalsePositive(fieldSet, KUidContactFieldFamilyName, namePtr)
                           && isFalsePositive(fieldSet, KUidContactFieldCompanyName, namePtr)
                           && isFalsePositive(fieldSet, KUidContactFieldSecondName, namePtr)){
                            idArray->Remove(i);
                            i--;
                        }
                        CleanupStack::PopAndDestroy(contactItem);
                    }
                }
            }
            delete fieldDef;
        }
    }

    if(idArray && (error == QContactManager::NoError)) {
        // copy the matching contact ids
        for(int i(0); i < idArray->Count(); i++) {
            matches.append(QContactLocalId((*idArray)[i]));
        }
    }

    delete idArray;

    return matches;
}

/*!
 * Checks if the contact's field set includes field \a fieldTypeUid and if the
 * field contents matches the search string. The features currently:
 * 1. only checks the first field instance
 * 2. supports only "starts with"
 * 3. searches every word inside the field
 * 4. supports only "not case-sensitive" matching
 */
bool CntSymbianFilter::isFalsePositive(const CContactItemFieldSet& fieldSet, const TUid& fieldTypeUid, const TDesC& searchString)
{
    bool value(true);
    TInt index = fieldSet.Find(fieldTypeUid);
    if(index >= 0) {
        const CContactItemField& field(fieldSet[index]);
        CContactTextField* storage = field.TextStorage();
        TPtrC text = storage->Text();
        index = text.FindC(searchString);
        // Check if this is the first word beginning with search string
        if(index == 0)
            value = false;
        // Check if this is in the beginning of a word (the preceding
        // character is a space)
        else if(index > 0 && TChar(text[index-1]) == TChar(0x20))
            value = false;
    }
    return value;
}

/*!
 * Transform detail filter into a contact item field definition that can be
 * used with CContactDatabase finds.
 */
void CntSymbianFilter::transformDetailFilterL(
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
    // Display label
    else if(detailFilter.detailDefinitionName() == QContactDisplayLabel::DefinitionName) {
        // in S60 display label is constructed with "nick name", "first name",
        // "last name" and/or "company name"
        tempFieldDef->SetReserveL(nameFieldsCount);
        tempFieldDef->AppendL(KUidContactFieldSecondName);
        tempFieldDef->AppendL(KUidContactFieldGivenName);
        tempFieldDef->AppendL(KUidContactFieldFamilyName);
        tempFieldDef->AppendL(KUidContactFieldCompanyName);
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
TInt CntSymbianFilter::findContacts(
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
CContactIdArray* CntSymbianFilter::findContactsL(
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
TInt CntSymbianFilter::matchContacts(
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
 * Get the match length setting used in MatchPhoneNumber type filtering.
 * \a matchLength Phone number digits to be used in matching (counted from
 * right).
 */
void CntSymbianFilter::getMatchLengthL(TInt& matchLength)
{
    //Get number of digits used to match
    CRepository* repository = CRepository::NewL(KCRUidTelConfiguration);
    CleanupStack::PushL(repository);
    User::LeaveIfError(repository->Get(KTelMatchDigits, matchLength));
    CleanupStack::PopAndDestroy(repository);
}

#endif
