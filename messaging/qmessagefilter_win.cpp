/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"
#include "qvariant.h"
#include "qdebug.h"
#include "winhelpers_p.h"
#include "qmessageaccountid_p.h"
#include "qmessageid_p.h"
#include "addresshelper_p.h"

// Not sure if this will work on WinCE
#ifndef PR_SMTP_ADDRESS
#define PR_SMTP_ADDRESS 0x39FE001E
#endif

void QDateTimeToFileTime(const QDateTime &dt, FILETIME *ft)
{
    SYSTEMTIME st;
    st.wYear = dt.date().year();
    st.wMonth = dt.date().month();
    st.wDay = dt.date().day();
    st.wHour = dt.time().hour();
    st.wMinute = dt.time().minute();
    st.wSecond = dt.time().second();
    st.wMilliseconds = dt.time().msec();
    SystemTimeToFileTime(&st, ft);
}

void QStringToWCharArray(const QString &str, wchar_t **buffer)
{
    delete *buffer;
    *buffer = new wchar_t[str.length() +1];
    memcpy(*buffer, str.utf16(), str.length() * sizeof(wchar_t));
    (*buffer)[str.length()] = 0;
}


MapiFolderIterator::MapiFolderIterator()
    :_store(0)
{
}

MapiFolderIterator::MapiFolderIterator(MapiStorePtr store, MapiFolderPtr root, QSet<QMessage::StandardFolder> standardFoldersInclude, QSet<QMessage::StandardFolder> standardFoldersExclude)
    :_store(store), _standardFoldersInclude(standardFoldersInclude), _standardFoldersExclude(standardFoldersExclude)
{
    _folders.append(root);
}

MapiFolderPtr MapiFolderIterator::next()
{
    while (_store && _store->isValid() && !_folders.isEmpty()) {
        if (_folders.back()->isValid()) {
            // Get the next subfolder of this folder
            QMessageStore::ErrorCode ignored(QMessageStore::NoError);
            MapiFolderPtr folder(_folders.back()->nextSubFolder(&ignored));
            if (ignored == QMessageStore::NoError) {
                if (folder && folder->isValid()) {
                    // Descend into the subfolder
                    _folders.append(folder);

                    QMessage::StandardFolder folderType(folder->standardFolder());
                    if ((_standardFoldersInclude.isEmpty() || _standardFoldersInclude.contains(folderType))
                        && (_standardFoldersExclude.isEmpty() || !_standardFoldersExclude.contains(folderType))) {
                        // This folder is the next one
                        return folder;
                    }
                } else {
                    if (folder) {
                        qWarning() << "Invalid subfolder:" << folder->name();
                    }
                    _folders.pop_back(); // No more subfolders in the current folder
                }
            } else {
                qWarning() << "Error getting next subfolder...";
                _folders.pop_back();
            }
        } else {
            qWarning() << "Invalid folder:" << _folders.back()->name();
            _folders.pop_back();
        }
    }

    return MapiFolderPtr();
}

MapiStoreIterator::MapiStoreIterator()
{
}

MapiStoreIterator::MapiStoreIterator(QList<MapiStorePtr> stores, QSet<QMessageAccountId> accountsInclude, QSet<QMessageAccountId> accountsExclude)
    :_stores(stores), _accountsInclude(accountsInclude), _accountsExclude(accountsExclude)
{
}

MapiStorePtr MapiStoreIterator::next()
{
    while (!_stores.isEmpty()) {
        MapiStorePtr store(_stores.takeFirst());
#ifdef _WIN32_WCE
        QMessageAccountId key(QMessageAccountIdPrivate::from(store->entryId()));
#else
        QMessageAccountId key(QMessageAccountIdPrivate::from(store->recordKey()));
#endif
        if ((_accountsInclude.isEmpty() || _accountsInclude.contains(key))
            && (_accountsExclude.isEmpty() || !_accountsExclude.contains(key))) {
            return store;
        }
    }

    return MapiStorePtr();
}

QMessageFilter QMessageFilterPrivate::from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = field;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilter QMessageFilterPrivate::from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = field;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = Relation;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilter QMessageFilterPrivate::from(QMessageFilterPrivate::Field field, const QVariant &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_field = field;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilterPrivate* QMessageFilterPrivate::implementation(const QMessageFilter &filter)
{
    return filter.d_ptr;
}

MapiFolderIterator QMessageFilterPrivate::folderIterator(const QMessageFilter &filter, QMessageStore::ErrorCode *lastError, const MapiStorePtr &store)
{
    return MapiFolderIterator(store, store->rootFolder(lastError), filter.d_ptr->_standardFoldersInclude, filter.d_ptr->_standardFoldersExclude);
}

MapiStoreIterator QMessageFilterPrivate::storeIterator(const QMessageFilter &filter, QMessageStore::ErrorCode *lastError, const MapiSessionPtr &session)
{
    return MapiStoreIterator(session->allStores(lastError), filter.d_ptr->_accountsInclude, filter.d_ptr->_accountsExclude);
}

QList<QMessageFilter> QMessageFilterPrivate::subFilters(const QMessageFilter &filter)
{
    QList<QMessageFilter> result;
    QList<QMessageFilter> queue;
    queue.append(filter);
    while (!queue.isEmpty()) {
        QMessageFilter top(queue.takeFirst());
        if (!top.d_ptr->_complex) {
            result.append(top);
            continue;
        }
        // Complex so must consist of two subfilters or'd together and an empty containerFiltersPart
        queue.append(*top.d_ptr->_left);
        queue.append(*top.d_ptr->_right);
    }
    return result;
}


MapiRestriction::MapiRestriction(const QMessageFilter &filter)
    :_notRestriction(0),
     _recipientRestriction(0),
     _keyProps(0),
     _restrictions(0),
     _recordKeys(0),
     _valid(false),
     _empty(false),
     _left(0),
     _right(0)
{
    // TODO: Could refactor this code so that different Operators/Fields each have a separate class, with a separate constructor
    QMessageFilterPrivate *d_ptr(QMessageFilterPrivate::implementation(filter));
    if (!d_ptr->_valid) {
        qWarning("Invalid filter application ignored.");
        return;
    }
    if (d_ptr->_options & QMessageDataComparator::FullWord) {
        qWarning("Full word matching not supported on MAPI platforms.");
        return;
    }

    if (d_ptr->_operator != QMessageFilterPrivate::Identity) {
        switch (d_ptr->_operator) {
        case QMessageFilterPrivate::Not: // fall through
        case QMessageFilterPrivate::Nand: // fall through
        case QMessageFilterPrivate::Nor:
            _notRestriction = new SRestriction;
            _notRestriction->rt = RES_NOT;
            _notRestriction->res.resNot.ulReserved = 0;
            _notRestriction->res.resNot.lpRes = &_restriction;
            break;
        default:
            break;
        } //end switch
        switch (d_ptr->_operator) {
        case QMessageFilterPrivate::Not: {
            if (d_ptr->_field == QMessageFilterPrivate::None) {
                _restriction.rt = RES_EXIST;
                _restriction.res.resExist.ulReserved1 = 0;
                _restriction.res.resExist.ulPropTag = PR_ENTRYID; // Should match all, so not this is a non-matching filter
                _restriction.res.resExist.ulReserved2 = 0;
                _valid = true;
                return;
            }
            break;
        }
        case QMessageFilterPrivate::Nand: // fall through
        case QMessageFilterPrivate::And: {
            Q_ASSERT(d_ptr->_left);
            Q_ASSERT(d_ptr->_right);
            if (!d_ptr->_left || !d_ptr->_right)
                return;
            _left = new MapiRestriction(*d_ptr->_left);
            _right = new MapiRestriction(*d_ptr->_right);
            _subRestriction[0] = *_left->sRestriction();
            _subRestriction[1] = *_right->sRestriction();
            _restriction.rt = RES_AND;
            _restriction.res.resAnd.cRes = 2;
            _restriction.res.resAnd.lpRes = &_subRestriction[0];
            _valid = true;
            return;
        }
        case QMessageFilterPrivate::Nor: // fall through
        case QMessageFilterPrivate::Or: {
            Q_ASSERT(d_ptr->_left);
            Q_ASSERT(d_ptr->_right);
            if (!d_ptr->_left || !d_ptr->_right)
                return;
            _left = new MapiRestriction(*d_ptr->_left);
            _right = new MapiRestriction(*d_ptr->_right);
            _subRestriction[0] = *_left->sRestriction();
            _subRestriction[1] = *_right->sRestriction();
            _restriction.rt = RES_OR;
            _restriction.res.resOr.cRes = 2;
            _restriction.res.resOr.lpRes = &_subRestriction[0];
            _valid = true;
            return;
        }
        default:
            Q_ASSERT(false);
            qWarning("Unsupported filter boolean algebra case.");
        } // end switch
    } else { // identity operator
        if (d_ptr->_field == QMessageFilterPrivate::None) {
            _valid = true;
            _empty = true;
            return;
        }
    }

    // Identity or Not filter type

    // Handle Recipients as a special case
#if 1
    if ((d_ptr->_field == QMessageFilterPrivate::RecipientName)
       || (d_ptr->_field == QMessageFilterPrivate::RecipientAddress)) {

        _restriction.rt = RES_SUBRESTRICTION;
        _restriction.res.resSub.ulSubObject = PR_MESSAGE_RECIPIENTS;
        _restriction.res.resSub.lpRes = _recipientRestriction;
        _recipientRestriction = new SRestriction;
        _recipientRestriction->rt = RES_AND;
        _recipientRestriction->res.resAnd.cRes = 2;
        _recipientRestriction->res.resAnd.lpRes = &_subRestriction[0];
        _restriction.res.resSub.lpRes = _recipientRestriction;

        _subRestriction[0].rt = RES_EXIST;
        _subRestriction[0].res.resExist.ulReserved1 = 0;
        _subRestriction[0].res.resExist.ulReserved2 = 0;
   
        _subRestriction[1].rt = RES_CONTENT;
        _subRestriction[1].res.resContent.lpProp = &_keyProp;
        if (d_ptr->_field == QMessageFilterPrivate::RecipientName) {
            _subRestriction[0].res.resExist.ulPropTag = PR_DISPLAY_NAME;
            _subRestriction[1].res.resContent.ulPropTag = PR_DISPLAY_NAME;
            _keyProp.ulPropTag = PR_DISPLAY_NAME;
        } else { // RecipientsAddress
            _subRestriction[0].res.resExist.ulPropTag = PR_SMTP_ADDRESS;
            _subRestriction[1].res.resContent.ulPropTag = PR_SMTP_ADDRESS; // PR_EMAIL_ADDRESS returns unsatisfactory results
            _keyProp.ulPropTag = PR_SMTP_ADDRESS;
        }

        QStringToWCharArray(d_ptr->_value.toString(), &d_ptr->_buffer); 
        _keyProp.Value.LPSZ = d_ptr->_buffer;

        bool negation(false);
        switch (d_ptr->_comparatorType) {
        case QMessageFilterPrivate::Equality: {
            _subRestriction[1].res.resContent.ulFuzzyLevel = FL_FULLSTRING;
            if (static_cast<QMessageDataComparator::EqualityComparator>(d_ptr->_comparatorValue) != QMessageDataComparator::Equal)
                negation = true;
            break;
        }
        case QMessageFilterPrivate::Inclusion: {
            // There is a flaw for Inclusion/Exclusion here,
            // If the search string includes a '<' so that both name and address are specified
            // FL_PREFIX should be used for the address, and (a non-existent) Fl_SUFFIX for name.
            _subRestriction[1].res.resContent.ulFuzzyLevel = FL_SUBSTRING;
            if (static_cast<QMessageDataComparator::InclusionComparator>(d_ptr->_comparatorValue) == QMessageDataComparator::Excludes)
                negation = true;
            break;
        }
        default: { // Relation
            qWarning("Unhandled restriction criteria");
            return;
        }
        }
        if (negation) {
            complement();
        }

        if ((d_ptr->_options & QMessageDataComparator::CaseSensitive) == 0) {
            _subRestriction[1].res.resContent.ulFuzzyLevel |= FL_IGNORECASE;
        }

        _valid = true;
        return;
    }
#endif

    // Handle Subject and/or Sender as a special case
    if ((d_ptr->_field == QMessageFilterPrivate::Subject)
        || (d_ptr->_field == QMessageFilterPrivate::SenderName)
        || (d_ptr->_field == QMessageFilterPrivate::SenderAddress)) {

        _restriction.rt = RES_CONTENT;
        _restriction.res.resContent.lpProp = &_keyProp;

        if (d_ptr->_field == QMessageFilterPrivate::Subject) {
            _restriction.res.resContent.ulPropTag = PR_SUBJECT;
            _keyProp.ulPropTag = PR_SUBJECT;
        } else if (d_ptr->_field == QMessageFilterPrivate::SenderName) {
            _restriction.res.resContent.ulPropTag = PR_SENDER_NAME;
            _keyProp.ulPropTag = PR_SENDER_NAME;
        } else { // SenderAddress
            _restriction.res.resContent.ulPropTag = PR_SENDER_EMAIL_ADDRESS;
            _keyProp.ulPropTag = PR_SENDER_EMAIL_ADDRESS;
        }

        QString subj(d_ptr->_value.toString());
        QStringToWCharArray(subj, &d_ptr->_buffer); 
        _keyProp.Value.LPSZ = d_ptr->_buffer;

        bool negation(false);
        switch (d_ptr->_comparatorType) {
        case QMessageFilterPrivate::Equality: {
            _restriction.res.resContent.ulFuzzyLevel = FL_FULLSTRING;
            if (static_cast<QMessageDataComparator::EqualityComparator>(d_ptr->_comparatorValue) != QMessageDataComparator::Equal)
                negation = true;
            break;
        }
        case QMessageFilterPrivate::Inclusion: {
            _restriction.res.resContent.ulFuzzyLevel = FL_SUBSTRING;
            if (static_cast<QMessageDataComparator::InclusionComparator>(d_ptr->_comparatorValue) == QMessageDataComparator::Excludes)
                negation = true;
            break;
        }
        default: { // Relation
            qWarning("Unhandled restriction criteria");
            return;
        }
        }

        if (negation) {
            complement();
        }

        if ((d_ptr->_options & QMessageDataComparator::CaseSensitive) == 0)
            _restriction.res.resContent.ulFuzzyLevel |= FL_IGNORECASE;

        _valid = true;
        return;
    }

    switch (d_ptr->_comparatorType) {
    case QMessageFilterPrivate::Equality:
    case QMessageFilterPrivate::Relation: {
        _restriction.rt = RES_PROPERTY;
        if (d_ptr->_comparatorType == QMessageFilterPrivate::Equality) {
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(d_ptr->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal)
                _restriction.res.resProperty.relop = RELOP_EQ;
            else
                _restriction.res.resProperty.relop = RELOP_NE;
        } else { // Relation
            QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(d_ptr->_comparatorValue));
            switch (cmp) {
            case QMessageDataComparator::LessThan:
                _restriction.res.resProperty.relop = RELOP_LT;
                break;
            case QMessageDataComparator::LessThanEqual:
                _restriction.res.resProperty.relop = RELOP_LE;
                break;
            case QMessageDataComparator::GreaterThan:
                _restriction.res.resProperty.relop = RELOP_GT;
                break;
            case QMessageDataComparator::GreaterThanEqual:
                _restriction.res.resProperty.relop = RELOP_GE;
                break;
            }
        }
        _restriction.res.resProperty.lpProp = &_keyProp;
        switch (d_ptr->_field) {
        case QMessageFilterPrivate::Size: {
            _restriction.res.resProperty.ulPropTag = PR_MESSAGE_SIZE;
            _keyProp.ulPropTag = PR_MESSAGE_SIZE;
            _keyProp.Value.ul = d_ptr->_value.toInt();
            _valid = true;
            break;
        }
        case QMessageFilterPrivate::ReceptionTimeStamp: {
            _restriction.res.resProperty.ulPropTag = PR_MESSAGE_DELIVERY_TIME;
            _keyProp.ulPropTag = PR_MESSAGE_DELIVERY_TIME;
            QDateTime dt(d_ptr->_value.toDateTime());
            QDateTimeToFileTime(dt, &_keyProp.Value.ft);
            _valid = true;
            break;
        }
        case QMessageFilterPrivate::TimeStamp: {
            _restriction.res.resProperty.ulPropTag = PR_CLIENT_SUBMIT_TIME;
            _keyProp.ulPropTag = PR_CLIENT_SUBMIT_TIME;
            QDateTime dt(d_ptr->_value.toDateTime());
            QDateTimeToFileTime(dt, &_keyProp.Value.ft);
            _valid = true;
            break;
        }
        case QMessageFilterPrivate::Subject: {
            _restriction.res.resProperty.ulPropTag = PR_SUBJECT;
            _keyProp.ulPropTag = PR_SUBJECT;
            QString subj(d_ptr->_value.toString());
            QStringToWCharArray(subj, &d_ptr->_buffer);
            _keyProp.Value.LPSZ = d_ptr->_buffer;
            _valid = true;
            break;
        }
        case QMessageFilterPrivate::Priority: {
            _restriction.res.resProperty.ulPropTag = PR_IMPORTANCE;
            _keyProp.ulPropTag = PR_IMPORTANCE;
            QMessage::Priority priority(static_cast<QMessage::Priority>(d_ptr->_value.toInt()));
            switch (priority) { // TODO: Double check that priority filtering is working
            case QMessage::HighPriority:
                _keyProp.Value.ul = PRIO_URGENT;
                break;
            case QMessage::NormalPriority:
                _keyProp.Value.ul = PRIO_NORMAL;
                break;
            case QMessage::LowPriority:
                _keyProp.Value.ul = PRIO_NONURGENT;
                break;
            default:
                qWarning("Unknown priority encountered during filter processing");
                return;
            }
            _valid = true;
            break;
        }
        default:
            qWarning("Unhandled restriction criteria");
        }
        break;
    }
    case QMessageFilterPrivate::Inclusion: {
        QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(d_ptr->_comparatorValue));
        if (cmp == QMessageDataComparator::Excludes) {
            complement();
        }
        if (d_ptr->_field == QMessageFilterPrivate::Id) {
            QStringList ids(d_ptr->_value.toStringList());
            _recordKeys = new MapiRecordKey[ids.count()];
            _keyProps = new SPropValue[ids.count()];
            _restrictions = new SRestriction[ids.count()];

            _restriction.rt = RES_OR;
            _restriction.res.resOr.cRes = ids.count();
            _restriction.res.resOr.lpRes = &_restrictions[0];
            for (int i = 0; i < ids.count(); ++i) {
                _recordKeys[i] = QMessageIdPrivate::messageRecordKey(QMessageId(ids[i]));
                _keyProps[i].ulPropTag = PR_RECORD_KEY;
                _keyProps[i].Value.bin.cb = _recordKeys[i].count();
                _keyProps[i].Value.bin.lpb = reinterpret_cast<LPBYTE>(const_cast<char*>(_recordKeys[i].data()));
                _restrictions[i].rt = RES_PROPERTY;
                _restrictions[i].res.resProperty.relop = RELOP_EQ;
                _restrictions[i].res.resProperty.ulPropTag = PR_RECORD_KEY;
                _restrictions[i].res.resProperty.lpProp = &_keyProps[i];
            }
            _valid = true;
            return;
        }
        if (d_ptr->_field == QMessageFilterPrivate::Status) {
            _restriction.rt = RES_BITMASK;
            QMessage::Status status(static_cast<QMessage::Status>(d_ptr->_value.toUInt()));
            switch (status) {
            case QMessage::Incoming:
                _restriction.rt = RES_OR;
                _restriction.res.resAnd.cRes = 2;
                _restriction.res.resAnd.lpRes = &_subRestriction[0];
                _subRestriction[0].rt = RES_EXIST;
                _subRestriction[0].res.resExist.ulReserved1 = 0;
                _subRestriction[0].res.resExist.ulPropTag = PR_RECEIVED_BY_ENTRYID;
                _subRestriction[0].res.resExist.ulReserved2 = 0;
                _subRestriction[1].rt = RES_EXIST;
                _subRestriction[1].res.resExist.ulReserved1 = 0;
                _subRestriction[1].res.resExist.ulPropTag = PR_END_DATE;
                _subRestriction[1].res.resExist.ulReserved2 = 0;
                _valid = true;
                return;
            case QMessage::Read:
                _restriction.res.resBitMask.relBMR = BMR_NEZ;
                _restriction.res.resBitMask.ulPropTag = PR_MESSAGE_FLAGS;
                _restriction.res.resBitMask.ulMask = MSGFLAG_READ;
                _valid = true;
                return;
            case QMessage::Removed:
                _restriction.res.resBitMask.relBMR = BMR_NEZ;
                _restriction.res.resBitMask.ulPropTag = PR_MSG_STATUS;
                _restriction.res.resBitMask.ulMask = MSGSTATUS_DELMARKED; // Untested
                _valid = true;
                return;
            case QMessage::HasAttachments:
                _restriction.res.resBitMask.relBMR = BMR_NEZ;
                _restriction.res.resBitMask.ulPropTag = PR_MESSAGE_FLAGS;
                _restriction.res.resBitMask.ulMask = MSGFLAG_HASATTACH; // Found in PR_HASATTACH msdn doc, but not covered in PR_MESSAGE_FLAGS doc
                _valid = true;
                return;
            default:
                qWarning("Unimplemented status filter"); // Has attachments not done
                return;
            }
        }
#if 1
        if (d_ptr->_field == QMessageFilterPrivate::Recipients) {
            _restriction.rt = RES_SUBRESTRICTION;
            _restriction.res.resSub.ulSubObject = PR_MESSAGE_RECIPIENTS;

            QString email(d_ptr->_value.toString()); // TODO split the string into name and address
            QString name; // TODO find name part
//            name = email; // TODO remove these two lines of debugging code.
//            email = "";
            if (name.isEmpty()) {
                _restriction.res.resSub.lpRes = &_subRestriction[1];
            } else if (email.isEmpty()) {
                _restriction.res.resSub.lpRes = &_subRestriction[0];
            } else {
                _recipientRestriction = new SRestriction;
                _recipientRestriction->rt = RES_AND;
                _recipientRestriction->res.resAnd.cRes = 2;
                _recipientRestriction->res.resAnd.lpRes = &_subRestriction[0];
                _restriction.res.resSub.lpRes = _recipientRestriction;
            }

            // Name
            _subRestriction[0].rt = RES_CONTENT;
            if (d_ptr->_options & QMessageDataComparator::FullWord)
                _subRestriction[0].res.resContent.ulFuzzyLevel = FL_FULLSTRING;
            else
                _subRestriction[0].res.resContent.ulFuzzyLevel = FL_SUBSTRING;
            if ((d_ptr->_options & QMessageDataComparator::CaseSensitive) == 0)
                _subRestriction[0].res.resContent.ulFuzzyLevel |= FL_IGNORECASE;
            _subRestriction[0].res.resContent.ulPropTag = PR_DISPLAY_NAME;
            _subRestriction[0].res.resContent.lpProp = &_keyProp;
            _keyProp.ulPropTag = PR_DISPLAY_NAME;
            QStringToWCharArray(name, &d_ptr->_buffer);
            _keyProp.Value.LPSZ = d_ptr->_buffer;

            // Email
            _subRestriction[1].rt = RES_CONTENT;
            if (d_ptr->_options & QMessageDataComparator::FullWord)
                _subRestriction[1].res.resContent.ulFuzzyLevel = FL_FULLSTRING;
            else
                _subRestriction[1].res.resContent.ulFuzzyLevel = FL_SUBSTRING;
            if ((d_ptr->_options & QMessageDataComparator::CaseSensitive) == 0)
                _subRestriction[1].res.resContent.ulFuzzyLevel |= FL_IGNORECASE;
            _subRestriction[1].res.resContent.ulPropTag = PR_SMTP_ADDRESS; // PR_EMAIL_ADDRESS returns unsatisfactory results
            _subRestriction[1].res.resContent.lpProp = &_keyProp2;
            _keyProp2.ulPropTag = PR_SMTP_ADDRESS;
            QStringToWCharArray(email, &d_ptr->_buffer2);
            _keyProp2.Value.LPSZ = d_ptr->_buffer2;
            _valid = true;
            break;
        }
#endif
        qWarning("Unhandled restriction criteria");
        break;
    }
    }
}

void MapiRestriction::complement()
{
    if (_notRestriction) { // double negative
        delete _notRestriction;
        _notRestriction = 0;
    } else {
        _notRestriction = new SRestriction;
        _notRestriction->rt = RES_NOT;
        _notRestriction->res.resNot.ulReserved = 0;
        _notRestriction->res.resNot.lpRes = &_restriction;
    }

}

MapiRestriction::~MapiRestriction()
{
    delete _notRestriction;
    _notRestriction = 0;
    delete _recipientRestriction;
    _recipientRestriction = 0;
    delete [] _recordKeys;
    _recordKeys = 0;
    delete [] _keyProps;
    _keyProps = 0;
    delete [] _restrictions;
    _restrictions = 0;
    delete _left;
    _left = 0;
    delete _right;
    _right = 0;
}

SRestriction *MapiRestriction::sRestriction()
{
    if (_notRestriction)
        return _notRestriction;
    return &_restriction;
}

QMessageFilterPrivate::QMessageFilterPrivate(QMessageFilter *messageFilter)
    :q_ptr(messageFilter),
     _field(None),
     _comparatorType(Equality),
     _comparatorValue(QMessageDataComparator::Equal),
     _operator(Identity),
     _left(0),
     _right(0),
     _buffer(0),
     _buffer2(0),
     _valid(true),
     _complex(false)
{
}

QMessageFilterPrivate::~QMessageFilterPrivate()
{
    delete _buffer;
    _buffer = 0;
    delete _buffer2;
    _buffer2 = 0;
    delete _left;
    _left = 0;
    delete _right;
    _right = 0;
}

bool QMessageFilterPrivate::containerFiltersAreEmpty()
{
    return (_standardFoldersInclude.isEmpty() 
            && _standardFoldersExclude.isEmpty() 
            && _accountsInclude.isEmpty() 
            && _accountsExclude.isEmpty());
}

bool QMessageFilterPrivate::nonContainerFiltersAreEmpty()
{
    return ((_field == QMessageFilterPrivate::None) 
        && (_operator == QMessageFilterPrivate::Identity));
}

QMessageFilter QMessageFilterPrivate::containerFiltersPart()
{
    QMessageFilter result;
    result.d_ptr->_standardFoldersInclude = _standardFoldersInclude;
    result.d_ptr->_standardFoldersExclude = _standardFoldersExclude;
    result.d_ptr->_accountsInclude = _accountsInclude;
    result.d_ptr->_accountsExclude = _accountsExclude;
    return result;
}

QMessageFilter QMessageFilterPrivate::nonContainerFiltersPart()
{
    QMessageFilter result;
    result.d_ptr->_options = _options;
    result.d_ptr->_field = _field;
    result.d_ptr->_value = _value;
    result.d_ptr->_comparatorType = _comparatorType;
    result.d_ptr->_comparatorValue = _comparatorValue;
    result.d_ptr->_operator = _operator;
    if (_left)
        result.d_ptr->_left = new QMessageFilter(*_left);
    if (_right)
        result.d_ptr->_right = new QMessageFilter(*_right);
    result.d_ptr->_buffer = 0;
    result.d_ptr->_buffer2 = 0;
    result.d_ptr->_valid = _valid;
    result.d_ptr->_complex = _complex;
    return result;
}

QMessageFilter::QMessageFilter()
    :d_ptr(new QMessageFilterPrivate(this))
{
}

QMessageFilter::QMessageFilter(const QMessageFilter &other)
    :d_ptr(new QMessageFilterPrivate(this))
{
    this->operator=(other);
}

QMessageFilter::~QMessageFilter()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageFilter& QMessageFilter::operator=(const QMessageFilter& other)
{
    if (&other == this)
        return *this;
    delete d_ptr->_left;
    d_ptr->_left = 0;
    delete d_ptr->_right;
    d_ptr->_right = 0;
    d_ptr->_options = other.d_ptr->_options;
    d_ptr->_field = other.d_ptr->_field;
    d_ptr->_value = other.d_ptr->_value;
    d_ptr->_comparatorType = other.d_ptr->_comparatorType;
    d_ptr->_comparatorValue = other.d_ptr->_comparatorValue;
    d_ptr->_operator = other.d_ptr->_operator;
    d_ptr->_buffer = 0; // Delay construction of buffer until it is used
    d_ptr->_buffer2 = 0;
    d_ptr->_valid = other.d_ptr->_valid;
    d_ptr->_standardFoldersInclude = other.d_ptr->_standardFoldersInclude;
    d_ptr->_standardFoldersExclude = other.d_ptr->_standardFoldersExclude;
    d_ptr->_accountsInclude = other.d_ptr->_accountsInclude;
    d_ptr->_accountsExclude = other.d_ptr->_accountsExclude;
    d_ptr->_complex = other.d_ptr->_complex;

    if (other.d_ptr->_left)
        d_ptr->_left = new QMessageFilter(*other.d_ptr->_left);
    if (other.d_ptr->_right)
        d_ptr->_right = new QMessageFilter(*other.d_ptr->_right);
    return *this;
}

void QMessageFilter::setOptions(QMessageDataComparator::Options options)
{
    d_ptr->_options = options;
    if (d_ptr->_options & QMessageDataComparator::FullWord) {
        qWarning("Full word matching not supported on MAPI platforms.");
        d_ptr->_valid = false;
    }
}

QMessageDataComparator::Options QMessageFilter::options() const
{
    return d_ptr->_options;
}

bool QMessageFilter::isEmpty() const
{
    return d_ptr->nonContainerFiltersAreEmpty() && d_ptr->containerFiltersAreEmpty();
}

bool QMessageFilter::isSupported() const
{
    return d_ptr->_valid;
}

QMessageFilter QMessageFilter::operator~() const
{
    QMessageFilter result;
    if (d_ptr->containerFiltersAreEmpty() && !d_ptr->_complex) {
        // Simple case of a native filter
        result = *this;
        int op = static_cast<int>(d_ptr->_operator) + static_cast<int>(QMessageFilterPrivate::Not);
        op = op % static_cast<int>(QMessageFilterPrivate::OperatorEnd);
        result.d_ptr->_operator = static_cast<QMessageFilterPrivate::Operator>(op);
    } else if (d_ptr->_complex) {
        // A filter can be in one of two forms, either
        // 1) An account and/or standard folder restriction &'d with a 'native' filter part 
        //  that can be evaluated using a MAPI SRestriction.
        // or 2) a 'complex' filter part that consists of two subparts |'d together
        //     at least one of which is non-native. That is at least one subpart is either
        //     complex itself, or has an account and/or standard folder restriction.
        //
        // On Windows, only account and folder filters are non-native, i.e. can't be evaluated 
        //  using a MAPI SRestriction.
        // 
        // So ~(X|Y)) -> ~X&~Y -> and & will transform further

        if (!d_ptr->containerFiltersAreEmpty())
            qWarning("Complex filter has non empty container filter part");
        result = ~*d_ptr->_left & ~*d_ptr->_right;
    } else {
        switch (d_ptr->_operator)
        {
            case QMessageFilterPrivate::Identity: // fall through
            case QMessageFilterPrivate::Not:
            {
                if (d_ptr->nonContainerFiltersAreEmpty()) {
                    // ~F
                    result.d_ptr->_standardFoldersInclude = d_ptr->_standardFoldersExclude;
                    result.d_ptr->_standardFoldersExclude = d_ptr->_standardFoldersInclude;
                    result.d_ptr->_accountsInclude = d_ptr->_accountsExclude;
                    result.d_ptr->_accountsExclude = d_ptr->_accountsInclude;
                    break;
                }
                //~(F&A) -> ~F|~A  Identity case
                //~(F&~A) -> ~F|~~A -> ~F|A  Not case
                result.d_ptr->_operator = QMessageFilterPrivate::Or;
                result.d_ptr->_left = new QMessageFilter();
                // Find ~F, swap includes and excludes
                result.d_ptr->_left->d_ptr->_standardFoldersInclude = d_ptr->_standardFoldersExclude;
                result.d_ptr->_left->d_ptr->_standardFoldersExclude = d_ptr->_standardFoldersInclude;
                result.d_ptr->_left->d_ptr->_accountsInclude = d_ptr->_accountsExclude;
                result.d_ptr->_left->d_ptr->_accountsExclude = d_ptr->_accountsInclude;
                // Find ~A or ~~A
                result.d_ptr->_right = new QMessageFilter(~d_ptr->nonContainerFiltersPart());
                result.d_ptr->_complex = true;
                break;
            }
            case QMessageFilterPrivate::And: // fall through
            case QMessageFilterPrivate::Nand: // fall through
            case QMessageFilterPrivate::Or: // fall through
            case QMessageFilterPrivate::Nor:
            {
                // Find ~(F&(A?B)), where F is non-native filter part, A and B are native filters, and '?' is a boolean operator
                // ~(F&(A?B) -> ~F|~(A?B)
                QMessageFilter result;
                QMessageFilter resultL;
                QMessageFilter resultR;
                result.d_ptr->_operator = QMessageFilterPrivate::Or;
                result.d_ptr->_left = &resultL;
                result.d_ptr->_right = &resultR;
                result.d_ptr->_complex = true;
                // Find ~F swap inclusion and exclusions
                resultL.d_ptr->_left->d_ptr->_standardFoldersInclude = d_ptr->_standardFoldersExclude;
                resultL.d_ptr->_left->d_ptr->_standardFoldersExclude = d_ptr->_standardFoldersInclude;
                resultL.d_ptr->_left->d_ptr->_accountsInclude = d_ptr->_accountsExclude;
                resultL.d_ptr->_left->d_ptr->_accountsExclude = d_ptr->_accountsInclude;
                // Use DeMorgan's law to find ~(A?B)
                if (d_ptr->_operator == QMessageFilterPrivate::And) {
                    //~(F&(A&B)) -> ~F|(~A|~B)
                    resultR.d_ptr->_operator = QMessageFilterPrivate::Or;
                    resultR.d_ptr->_left = new QMessageFilter(~*d_ptr->_left);
                    resultR.d_ptr->_right = new QMessageFilter(~*d_ptr->_right);
                } else if (d_ptr->_operator == QMessageFilterPrivate::Nand) {
                    //~(F&~(A&B)) -> ~F|(A&B)
                    resultR.d_ptr->_operator = QMessageFilterPrivate::And;
                    resultR.d_ptr->_left = new QMessageFilter(*d_ptr->_left);
                    resultR.d_ptr->_right = new QMessageFilter(*d_ptr->_right);
                } else if (d_ptr->_operator == QMessageFilterPrivate::Or) {
                    //~(F&(A|B)) -> ~F|(~A&~B)
                    resultR.d_ptr->_operator = QMessageFilterPrivate::And;
                    resultR.d_ptr->_left = new QMessageFilter(~*d_ptr->_left);
                    resultR.d_ptr->_right = new QMessageFilter(~*d_ptr->_right);
                } else if (d_ptr->_operator == QMessageFilterPrivate::Nor) {
                    //~(F&~(A|B)) -> ~F|(A|B)
                    resultR.d_ptr->_operator = QMessageFilterPrivate::Or;
                    resultR.d_ptr->_left = new QMessageFilter(*d_ptr->_left);
                    resultR.d_ptr->_right = new QMessageFilter(*d_ptr->_right);
                }
                break;
            }
            default: 
            {
                qWarning() << "Unhandled filter negation case.";
                break;
            }
        }
    }
    return result;
}

QMessageFilter QMessageFilter::operator&(const QMessageFilter& other) const
{
    QMessageFilter result(*this);
    result &= other;
    return result;
}

QMessageFilter QMessageFilter::operator|(const QMessageFilter& other) const
{
    QMessageFilter result(*this);
    result |= other;
    return result;
}

const QMessageFilter& QMessageFilter::operator&=(const QMessageFilter& other)
{
    if (&other == this)
        return *this;
    if (isEmpty()) {
        *this = other;
        return *this;
    }
    if (other.isEmpty())
        return *this;
    if (!d_ptr->_valid || !other.d_ptr->_valid) {
        d_ptr->_valid = false;
        return *this;
    }

    QMessageFilter result;
    if (!d_ptr->_complex && !other.d_ptr->_complex) {
        // A&B
        // intersect includes and union excludes

        // empty include set means include all
        if (this->d_ptr->_standardFoldersInclude.isEmpty()) {
            result.d_ptr->_standardFoldersInclude = other.d_ptr->_standardFoldersInclude;
        } else {
            result.d_ptr->_standardFoldersInclude = this->d_ptr->_standardFoldersInclude;
            if (!other.d_ptr->_standardFoldersInclude.isEmpty())
                result.d_ptr->_standardFoldersInclude &= other.d_ptr->_standardFoldersInclude;
        }
        result.d_ptr->_standardFoldersExclude = this->d_ptr->_standardFoldersExclude;
        result.d_ptr->_standardFoldersExclude |= other.d_ptr->_standardFoldersExclude;
        if (this->d_ptr->_accountsInclude.isEmpty()) {
            result.d_ptr->_accountsInclude = other.d_ptr->_accountsInclude;
        } else {
            result.d_ptr->_accountsInclude = this->d_ptr->_accountsInclude;
            if (!other.d_ptr->_accountsInclude.isEmpty())
                result.d_ptr->_accountsInclude &= other.d_ptr->_accountsInclude;
        }
        result.d_ptr->_accountsExclude = this->d_ptr->_accountsExclude;
        result.d_ptr->_accountsExclude |= other.d_ptr->_accountsExclude;

        if (this->d_ptr->nonContainerFiltersPart().isEmpty() 
            || other.d_ptr->nonContainerFiltersPart().isEmpty()) {
            // Degenerate case, empty this or other nonContainerFiltersPart can be thrown away
            if (this->d_ptr->nonContainerFiltersPart().isEmpty()) {
                *this = other;
            } // else throw away empty other nonContainerFiltersPart

            // Just update the containerFiltersPart
            d_ptr->_standardFoldersInclude = result.d_ptr->_standardFoldersInclude;
            d_ptr->_standardFoldersExclude = result.d_ptr->_standardFoldersExclude;
            d_ptr->_accountsInclude = result.d_ptr->_accountsInclude;
            d_ptr->_accountsExclude = result.d_ptr->_accountsExclude;
        } else {
            QMessageFilter *left(new QMessageFilter(this->d_ptr->nonContainerFiltersPart()));
            QMessageFilter *right(new QMessageFilter(other.d_ptr->nonContainerFiltersPart()));
            *this = result;
            d_ptr->_left = left;
            d_ptr->_right = right;
            d_ptr->_operator = QMessageFilterPrivate::And;
        }
    } else if (d_ptr->_complex) { // other maybe complex
        // (X|Y)&Z -> X&Z | Y&Z  Query optimizer is not a priority
        result.d_ptr->_left = new QMessageFilter(*this->d_ptr->_left & other); // recursive evaluation
        result.d_ptr->_right = new QMessageFilter(*this->d_ptr->_right & other); // recursive evaluation
        result.d_ptr->_operator = QMessageFilterPrivate::Or;
        result.d_ptr->_complex = true;
        *this = result;
    } else { // this is not complex, other is complex
        // A&(X|Y) -> A&X | A&Y  Query optimizer is not a priority
        result.d_ptr->_left = new QMessageFilter(*this & *other.d_ptr->_left); // recursive evaluation
        result.d_ptr->_right = new QMessageFilter(*this & *other.d_ptr->_right); // recursive evaluation
        result.d_ptr->_operator = QMessageFilterPrivate::Or;
        result.d_ptr->_complex = true;
        *this = result;
    }
    return *this;
}

const QMessageFilter& QMessageFilter::operator|=(const QMessageFilter& other)
{
    if (&other == this)
        return *this;
    if (isEmpty())
        return *this;
    if (other.isEmpty()) {
        *this = other;
        return *this;
    }

    QMessageFilter result;
    if (!d_ptr->_valid || !other.d_ptr->_valid) {
        result.d_ptr->_valid = false;
        *this = result;
        return *this;
    }
    if (!d_ptr->_complex 
        && !other.d_ptr->_complex 
        && (d_ptr->containerFiltersPart() == other.d_ptr->containerFiltersPart())) {
        // F&A|F&B - > F&(A|B)
        QMessageFilter *left(new QMessageFilter(this->d_ptr->nonContainerFiltersPart()));
        QMessageFilter *right(new QMessageFilter(other.d_ptr->nonContainerFiltersPart()));
        result.d_ptr->_standardFoldersInclude = d_ptr->_standardFoldersInclude;
        result.d_ptr->_standardFoldersExclude = d_ptr->_standardFoldersExclude;
        result.d_ptr->_accountsInclude = d_ptr->_accountsInclude;
        result.d_ptr->_accountsExclude = d_ptr->_accountsExclude;
        *this = result;
        d_ptr->_left = left;
        d_ptr->_right = right;
        d_ptr->_operator = QMessageFilterPrivate::Or;
    // } else { Could have special case for neither complex and for both nonContainerFiltersIsEmpty is true
    } else {
        // X|Y
        QMessageFilter *left(new QMessageFilter(*this));
        QMessageFilter *right(new QMessageFilter(other));
        *this = result;
        d_ptr->_left = left;
        d_ptr->_right = right;
        d_ptr->_operator = QMessageFilterPrivate::Or;
        result.d_ptr->_complex = true;
    }

    return *this;
}

bool QMessageFilter::operator==(const QMessageFilter& other) const
{
    if (d_ptr->_standardFoldersInclude != other.d_ptr->_standardFoldersInclude)
        return false;
    if (d_ptr->_standardFoldersExclude != other.d_ptr->_standardFoldersExclude)
        return false;
    if (d_ptr->_accountsInclude != other.d_ptr->_accountsInclude)
        return false;
    if (d_ptr->_accountsExclude != other.d_ptr->_accountsExclude)
        return false;
    if (d_ptr->_complex != other.d_ptr->_complex)
        return false;

    if (other.d_ptr->_operator != d_ptr->_operator)
        return false;

    if (d_ptr->_operator == QMessageFilterPrivate::Identity) {
        if (other.d_ptr->_operator != QMessageFilterPrivate::Identity)
            return false;
        return (d_ptr->_field == other.d_ptr->_field
            && d_ptr->_value == other.d_ptr->_value
            && d_ptr->_comparatorType == other.d_ptr->_comparatorType
            && d_ptr->_comparatorValue == other.d_ptr->_comparatorValue);
    }

    if (d_ptr->_left == other.d_ptr->_left 
        && d_ptr->_right == other.d_ptr->_right)
        return true;
    if (d_ptr->_left == other.d_ptr->_right 
        && d_ptr->_right == other.d_ptr->_left)
        return true; // Commutativity

    // TODO: For a system of determining equality of boolean algebra expressions see:
    // TODO:  Completely distributed normal form http://wikpedia.org/wiki/Boolean_algebra(logic)
    return false;
}

QMessageFilter QMessageFilter::byId(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageDataComparator::InclusionComparator inclCmp(QMessageDataComparator::Includes);
    if (cmp == QMessageDataComparator::NotEqual)
        inclCmp = QMessageDataComparator::Excludes;
    QMessageIdList ids;
    ids << id;

    return QMessageFilter::byId(ids, inclCmp);
}

QMessageFilter QMessageFilter::byId(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;

    if (ids.isEmpty())
        return ~result; // match nothing;

#ifdef _WIN32_WCE
    QMap<MapiEntryId, QStringList> storeIds;
    foreach(QMessageId id, ids)
        storeIds[QMessageIdPrivate::storeRecordKey(id)].append(id.toString());

    QMapIterator<MapiEntryId, QStringList> i(storeIds);
#else
    QMap<MapiRecordKey, QStringList> storeIds;
    foreach(QMessageId id, ids)
        storeIds[QMessageIdPrivate::storeRecordKey(id)].append(id.toString());

    QMapIterator<MapiRecordKey, QStringList> i(storeIds);
#endif
    bool first(true);
    while (i.hasNext()) {
        i.next();
        QMessageFilter tmp(QMessageFilter::byParentAccountId(QMessageAccountIdPrivate::from(i.key())));
        tmp &= QMessageFilterPrivate::from(QMessageFilterPrivate::Id, QVariant(i.value()), cmp);
        if (first)
            result = tmp;
        else
            result |= tmp;
        first = false;
    }
    
    return result;
}

QMessageFilter QMessageFilter::byId(const QMessageFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    QMessageFilter result; // stub
    result.d_ptr->_valid = false; // Will require synchronous filter evaluation
    return result;
}

QMessageFilter QMessageFilter::byType(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
{
    // Not implemented
    QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::Type, QVariant(type), cmp)); // stub
    result.d_ptr->_valid = false; // Not natively implementable
    return result;
}

QMessageFilter QMessageFilter::byType(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::Type, QVariant(type), cmp)); // stub
    result.d_ptr->_valid = false; // Not natively implementable
    return result;
}

QMessageFilter QMessageFilter::bySender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QString sender(value);
    QString name;
    QString address;
    QString suffix;
    bool startDelimeterFound;
    QMessageAddress::parseEmailAddress(sender, &name, &address, &suffix, &startDelimeterFound);
    if (startDelimeterFound) {
        QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::SenderName, QVariant(name), QMessageDataComparator::Equal));
        result &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(address), QMessageDataComparator::Equal);
        if (cmp == QMessageDataComparator::Equal) {
            return result;
        } else {
            return ~result;
        }
    } else {
        // value could be name or address, both are set by parseEmailAddress to the same value
        QMessageFilter result1(QMessageFilterPrivate::from(QMessageFilterPrivate::SenderName, QVariant(name), QMessageDataComparator::Equal));
        result1 &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(""), QMessageDataComparator::Equal);
        QMessageFilter result2(QMessageFilterPrivate::from(QMessageFilterPrivate::SenderName, QVariant(""), QMessageDataComparator::Equal));
        result2 &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(address), QMessageDataComparator::Equal);
        if (cmp == QMessageDataComparator::Equal) {
            return result1 | result2;
        } else {
            return ~(result1 | result2);
        }
    }
}

QMessageFilter QMessageFilter::bySender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    if (value.isEmpty()) {
        if (cmp == QMessageDataComparator::Includes)
            return QMessageFilter();
        return ~QMessageFilter();
    }

    QString sender(value);
    QString name;
    QString address;
    QString suffix;
    bool startDelimeterFound;
    bool endDelimeterFound;
    QMessageAddress::parseEmailAddress(sender, &name, &address, &suffix, &startDelimeterFound, &endDelimeterFound);
    if (startDelimeterFound) {

        QMessageFilter result;
        if (endDelimeterFound) {
            result &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(address), QMessageDataComparator::Equal);
        } else {
            result &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(address), QMessageDataComparator::Includes);
        }

        // Need additional matches check, this should be a Suffix comparision rather than just an Includes
        // Futhermore this seems to trigger some kind of MAPI bug, results are being missed, 
        // so comment out for now, TODO need matches function for desktop as well as mobile
        result &= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderName, QVariant(name), QMessageDataComparator::Includes);

        if (cmp == QMessageDataComparator::Includes) {
            return result;
        } else {
            return ~result;
        }
    } else {
        QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::SenderName, QVariant(name), QMessageDataComparator::Includes));
        result |= QMessageFilterPrivate::from(QMessageFilterPrivate::SenderAddress, QVariant(address), QMessageDataComparator::Includes);
        if (cmp == QMessageDataComparator::Includes) {
            return result;
        } else {
            return ~result;
        }
    }
}

QMessageFilter QMessageFilter::byRecipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    if (value.isEmpty()) {
        if (cmp == QMessageDataComparator::Includes)
            return QMessageFilter();
        return ~QMessageFilter();
    }
    // Unable to get sensible results yet from either code path below
#if 1
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Recipients, QVariant(value), cmp);
#else
    QString sender(value);
    QString name;
    QString address;
    QString suffix;
    bool startDelimeterFound;
    bool endDelimeterFound;
    QMessageAddress::parseEmailAddress(sender, &name, &address, &suffix, &startDelimeterFound, &endDelimeterFound);
    if (startDelimeterFound) {

        QMessageFilter result;
        if (endDelimeterFound) {
            result &= QMessageFilterPrivate::from(QMessageFilterPrivate::RecipientAddress, QVariant(address), QMessageDataComparator::Equal);
        } else {
            result &= QMessageFilterPrivate::from(QMessageFilterPrivate::RecipientAddress, QVariant(address), QMessageDataComparator::Includes);
        }

        // Need additional matches check, this should be a Suffix comparision rather than just an Includes
        result &= QMessageFilterPrivate::from(QMessageFilterPrivate::RecipientName, QVariant(name), QMessageDataComparator::Includes);

        if (cmp == QMessageDataComparator::Includes) {
            return result;
        } else {
            return ~result;
        }
    } else {
        QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::RecipientName, QVariant(name), QMessageDataComparator::Includes));
        result |= QMessageFilterPrivate::from(QMessageFilterPrivate::RecipientAddress, QVariant(address), QMessageDataComparator::Includes);
        if (cmp == QMessageDataComparator::Includes) {
            return result;
        } else {
            return ~result;
        }
    }
#endif
}

QMessageFilter QMessageFilter::bySubject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    // TODO: Test this filter
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Subject, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::bySubject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    if (value.isEmpty()) {
        if (cmp == QMessageDataComparator::Includes)
            return QMessageFilter();
        return ~QMessageFilter();
    }
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Subject, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::TimeStamp, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::TimeStamp, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::ReceptionTimeStamp, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::ReceptionTimeStamp, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byStatus(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageDataComparator::InclusionComparator comparator(QMessageDataComparator::Excludes);
    if (cmp == QMessageDataComparator::Equal)
        comparator = QMessageDataComparator::Includes;
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Status, QVariant(value), comparator);
}

QMessageFilter QMessageFilter::byStatus(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    QMessageDataComparator::EqualityComparator comparator(QMessageDataComparator::NotEqual);
    if (cmp == QMessageDataComparator::Includes)
        comparator = QMessageDataComparator::Equal;
    if (mask & QMessage::Incoming)
        result &= QMessageFilter::byStatus(QMessage::Incoming, comparator);
    if (mask & QMessage::Read)
        result &= QMessageFilter::byStatus(QMessage::Read, comparator);
    if (mask & QMessage::Removed)
        result &= QMessageFilter::byStatus(QMessage::Removed, comparator);
    if (mask & QMessage::HasAttachments)
        result &= QMessageFilter::byStatus(QMessage::HasAttachments, comparator);
    if (result.isEmpty()) // Be consistent with QMF, but seems wrong. TODO verify correctness
        return ~result;
    return result;
}

QMessageFilter QMessageFilter::byPriority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Priority, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::bySize(int value, QMessageDataComparator::EqualityComparator cmp)
{
    // TODO: Test this filter
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Size, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::bySize(int value, QMessageDataComparator::RelationComparator cmp)
{
    return QMessageFilterPrivate::from(QMessageFilterPrivate::Size, QVariant(value), cmp);
}

QMessageFilter QMessageFilter::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    if (cmp == QMessageDataComparator::Equal)
        result.d_ptr->_accountsInclude += id;
    else
        result.d_ptr->_accountsExclude += id;
    return result;
}

QMessageFilter QMessageFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    QMessageFilter result; // stub
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilter QMessageFilter::byStandardFolder(QMessage::StandardFolder folder, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(folder)
    Q_UNUSED(cmp)
    QMessageFilter result; // stub
    result.d_ptr->_valid = false;
    return result;
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFilter QMessageFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    // Not implemented
    QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::ParentFolderId, QVariant(id.toString()), cmp)); // stub
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilter QMessageFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    QMessageFilter result; // stub
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    QMessageFilter result(QMessageFilterPrivate::from(QMessageFilterPrivate::AncestorFolderIds, QVariant(id.toString()), cmp));
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    QMessageFilter result; // stub
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}
#endif
