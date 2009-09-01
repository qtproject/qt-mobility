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
#include "qmessagefilterkey.h"
#include "qmessagefilterkey_p.h"
#include "qvariant.h"
#include "winhelpers_p.h"

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

QMessageFilterKey QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Field field, const QVariant &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_field = field;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = Equality;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilterKey QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Field field, const QVariant &value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_field = field;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = Relation;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilterKey QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Field field, const QVariant &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_field = field;
    result.d_ptr->_value = value;
    result.d_ptr->_comparatorType = Inclusion;
    result.d_ptr->_comparatorValue = static_cast<int>(cmp);
    return result;
}

QMessageFilterKeyPrivate* QMessageFilterKeyPrivate::implementation(const QMessageFilterKey &key)
{
    return key.d_ptr;
}

class MapiRestriction {
public:
    MapiRestriction(const QMessageFilterKey &key);
    ~MapiRestriction();
    SRestriction *sRestriction();
    bool isValid() { return _valid; }
    bool isEmpty() { return _empty; }

private:
    SRestriction _restriction;
    SRestriction _subRestriction[2];
    SPropValue _keyProp;
    SPropValue _keyProp2;
    SRestriction *_notRestriction;
    SRestriction *_recipientRestriction;
    bool _valid;
    bool _empty;
    MapiRestriction *_left;
    MapiRestriction *_right;
};

MapiRestriction::MapiRestriction(const QMessageFilterKey &key)
    :_notRestriction(0),
    _recipientRestriction(0),
     _valid(false),
     _empty(false),
     _left(0),
     _right(0)
{
    // TODO: Could refactor this code so that different Operators/Fields each have a separate class, with a separate constructor
    QMessageFilterKeyPrivate *d_ptr(QMessageFilterKeyPrivate::implementation(key));
    if (!d_ptr->_valid) {
        qWarning("Invalid filter application ignored.");
        return;
    }
    if (d_ptr->_operator != QMessageFilterKeyPrivate::Identity) {
        switch (d_ptr->_operator) {
        case QMessageFilterKeyPrivate::Not: // fall through
        case QMessageFilterKeyPrivate::Nand: // fall through
        case QMessageFilterKeyPrivate::Nor:
            _notRestriction = new SRestriction;
            _notRestriction->rt = RES_NOT;
            _notRestriction->res.resNot.ulReserved = 0;
            _notRestriction->res.resNot.lpRes = &_restriction;
            break;
        default:
            break;
        } //end switch
        switch (d_ptr->_operator) {
        case QMessageFilterKeyPrivate::Not: {
            if (d_ptr->_field == QMessageFilterKeyPrivate::None) {
                _restriction.rt = RES_EXIST;
                _restriction.res.resExist.ulReserved1 = 0;
                _restriction.res.resExist.ulPropTag = PR_ENTRYID; // Should match all, so not this is a non-matching key
                _restriction.res.resExist.ulReserved2 = 0;
                _valid = true;
                return;
            }
            break;
        }
        case QMessageFilterKeyPrivate::Nand: // fall through
        case QMessageFilterKeyPrivate::And: {
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
        case QMessageFilterKeyPrivate::Nor: // fall through
        case QMessageFilterKeyPrivate::Or: {
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
        if (d_ptr->_field == QMessageFilterKeyPrivate::None) {
            _valid = true;
            _empty = true;
            return;
        }
    }
    switch (d_ptr->_comparatorType) {
    case QMessageFilterKeyPrivate::Equality:
    case QMessageFilterKeyPrivate::Relation: {
        _restriction.rt = RES_PROPERTY;
        if (d_ptr->_comparatorType == QMessageFilterKeyPrivate::Equality) {
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
        case QMessageFilterKeyPrivate::Size: {
            _restriction.res.resProperty.ulPropTag = PR_MESSAGE_SIZE;
            _keyProp.ulPropTag = PR_MESSAGE_SIZE;
            _keyProp.Value.ul = d_ptr->_value.toInt();
            _valid = true;
            break;
        }
        case QMessageFilterKeyPrivate::ReceptionTimeStamp: {
            _restriction.res.resProperty.ulPropTag = PR_MESSAGE_DELIVERY_TIME;
            _keyProp.ulPropTag = PR_MESSAGE_DELIVERY_TIME;
            QDateTime dt(d_ptr->_value.toDateTime());
            QDateTimeToFileTime(dt, &_keyProp.Value.ft);
            _valid = true;
            break;
        }
        case QMessageFilterKeyPrivate::TimeStamp: {
            _restriction.res.resProperty.ulPropTag = PR_CLIENT_SUBMIT_TIME;
            _keyProp.ulPropTag = PR_CLIENT_SUBMIT_TIME;
            QDateTime dt(d_ptr->_value.toDateTime());
            QDateTimeToFileTime(dt, &_keyProp.Value.ft);
            _valid = true;
            break;
        }
        case QMessageFilterKeyPrivate::Subject: {
            _restriction.res.resProperty.ulPropTag = PR_SUBJECT;
            _keyProp.ulPropTag = PR_SUBJECT;
            QString subj(d_ptr->_value.toString());
            QStringToWCharArray(subj, &d_ptr->_buffer); 
            _keyProp.Value.LPSZ = d_ptr->_buffer;
            _valid = true;
            break;
        }
        case QMessageFilterKeyPrivate::Priority: {
            _restriction.res.resProperty.ulPropTag = PR_IMPORTANCE;
            _keyProp.ulPropTag = PR_IMPORTANCE;
            QMessage::Priority priority(static_cast<QMessage::Priority>(d_ptr->_value.toInt()));
            switch (priority) { // TODO: Double check that priority filtering is working
            case QMessage::High:
                _keyProp.Value.ul = PRIO_URGENT;
                break;
            case QMessage::Normal:
                _keyProp.Value.ul = PRIO_NORMAL;
                break;
            case QMessage::Low:
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
    case QMessageFilterKeyPrivate::Inclusion: {
        QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(d_ptr->_comparatorValue));
        if (cmp == QMessageDataComparator::Excludes) {
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
        if (d_ptr->_field == QMessageFilterKeyPrivate::Status) {
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
        if (d_ptr->_field == QMessageFilterKeyPrivate::Recipients) {
            _restriction.rt = RES_SUBRESTRICTION;
            _restriction.res.resSub.ulSubObject = PR_MESSAGE_RECIPIENTS;

            QString email(d_ptr->_value.toString()); // TODO split the string into name and email address
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

        _restriction.rt = RES_CONTENT;
        if (d_ptr->_options & QMessageDataComparator::FullWord)
            _restriction.res.resContent.ulFuzzyLevel = FL_FULLSTRING;
        else
            _restriction.res.resContent.ulFuzzyLevel = FL_SUBSTRING;
        if ((d_ptr->_options & QMessageDataComparator::CaseSensitive) == 0)
            _restriction.res.resContent.ulFuzzyLevel |= FL_IGNORECASE;
        switch (d_ptr->_field) {
        case QMessageFilterKeyPrivate::Subject: {
            _restriction.res.resContent.ulPropTag = PR_SUBJECT;
            _restriction.res.resContent.lpProp = &_keyProp;
            _keyProp.ulPropTag = PR_SUBJECT;
            QString subj(d_ptr->_value.toString());
            QStringToWCharArray(subj, &d_ptr->_buffer); 
            _keyProp.Value.LPSZ = d_ptr->_buffer;
            _valid = true;
            break;
        }
        case QMessageFilterKeyPrivate::Sender: {
            //TODO: Split search address into name and address part
            //TODO: Look for the name part in PR_SENDER_NAME
            //TODO: And address part in PR_SENDER_EMAIL_ADDRESS
            _restriction.res.resContent.ulPropTag = PR_SENDER_EMAIL_ADDRESS;
            _restriction.res.resContent.lpProp = &_keyProp;
            _keyProp.ulPropTag = PR_SENDER_EMAIL_ADDRESS;
            QString subj(d_ptr->_value.toString());
            QStringToWCharArray(subj, &d_ptr->_buffer); 
            _keyProp.Value.LPSZ = d_ptr->_buffer;
            _subRestriction[1] = _restriction;
            _restriction.rt = RES_AND;
            _restriction.res.resAnd.cRes = 2;
            _restriction.res.resAnd.lpRes = &_subRestriction[0];
            _subRestriction[0].rt = RES_EXIST;
            _subRestriction[0].res.resExist.ulReserved1 = 0;
            _subRestriction[0].res.resExist.ulPropTag = PR_SENDER_EMAIL_ADDRESS;
            _subRestriction[0].res.resExist.ulReserved2 = 0;
            _valid = true;
            break;
        }
        default:
            qWarning("Unhandled restriction criteria");
        }
        break;
    }
    }
}

MapiRestriction::~MapiRestriction()
{
    delete _notRestriction;
    _notRestriction = 0;
    delete _recipientRestriction;
    _recipientRestriction = 0;
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

void QMessageFilterKeyPrivate::filterTable(QMessageStore::ErrorCode *lastError, const QMessageFilterKey &key, LPMAPITABLE messagesTable)
{
    MapiRestriction restriction(key);
    if (!restriction.isValid())
        return;
    if (restriction.isEmpty())
        return; // nothing to do

    ULONG flags(0);
    if (messagesTable->Restrict(restriction.sRestriction(), flags) != S_OK)
        *lastError = QMessageStore::ConstraintFailure;
}

void QMessageFilterKey::setOptions(QMessageDataComparator::Options options)
{
    d_ptr->_options = options;
}

QMessageDataComparator::Options QMessageFilterKey::options() const
{
    return d_ptr->_options;
}

QMessageFilterKeyPrivate::QMessageFilterKeyPrivate(QMessageFilterKey *messageFilterKey)
    :q_ptr(messageFilterKey),
     _field(None),
     _comparatorType(Equality),
     _comparatorValue(QMessageDataComparator::Equal),
     _operator(Identity),
     _left(0),
     _right(0),
     _buffer(0),
     _buffer2(0),
     _valid(true)
{
}

QMessageFilterKeyPrivate::~QMessageFilterKeyPrivate()
{
    delete _buffer;
    _buffer = 0;
    delete _buffer2;
    _buffer2 = 0;
}

QMessageFilterKey::QMessageFilterKey()
    :d_ptr(new QMessageFilterKeyPrivate(this))
{
}

QMessageFilterKey::QMessageFilterKey(const QMessageFilterKey &other)
    :d_ptr(new QMessageFilterKeyPrivate(this))
{
    this->operator=(other);
}

QMessageFilterKey::~QMessageFilterKey()
{
    delete d_ptr;
    d_ptr = 0;
}

bool QMessageFilterKey::isEmpty() const
{
    return ((d_ptr->_field == QMessageFilterKeyPrivate::None) 
        && (d_ptr->_operator == QMessageFilterKeyPrivate::Identity));
}

bool QMessageFilterKey::isSupported() const
{
    return d_ptr->_valid;
}

QMessageFilterKey QMessageFilterKey::operator~() const
{
    QMessageFilterKey result(*this);
    int op = static_cast<int>(d_ptr->_operator) + static_cast<int>(QMessageFilterKeyPrivate::Not);
    op = op % static_cast<int>(QMessageFilterKeyPrivate::OperatorEnd);
    result.d_ptr->_operator = static_cast<QMessageFilterKeyPrivate::Operator>(op);

    return result;
}

QMessageFilterKey QMessageFilterKey::operator&(const QMessageFilterKey& other) const
{
    if (isEmpty())
        return QMessageFilterKey(other);
    if (other.isEmpty())
        return QMessageFilterKey(*this);
    QMessageFilterKey result;
    result.d_ptr->_left = new QMessageFilterKey(*this);
    result.d_ptr->_right = new QMessageFilterKey(other);
    result.d_ptr->_operator = QMessageFilterKeyPrivate::And;
    if (!result.d_ptr->_left->d_ptr->_valid || !result.d_ptr->_right->d_ptr->_valid)
        result.d_ptr->_valid = false;
    return result;
}

QMessageFilterKey QMessageFilterKey::operator|(const QMessageFilterKey& other) const
{
    if (isEmpty())
        return QMessageFilterKey(*this);
    if (other.isEmpty())
        return QMessageFilterKey(other);
    QMessageFilterKey result;
    result.d_ptr->_left = new QMessageFilterKey(*this);
    result.d_ptr->_right = new QMessageFilterKey(other);
    result.d_ptr->_operator = QMessageFilterKeyPrivate::Or;
    if (!result.d_ptr->_left->d_ptr->_valid || !result.d_ptr->_right->d_ptr->_valid)
        result.d_ptr->_valid = false;
    return result;
}

const QMessageFilterKey& QMessageFilterKey::operator&=(const QMessageFilterKey& other)
{
    if (&other == this)
        return *this;
    if (isEmpty()) {
        *this = other;
        return *this;
    }
    if (other.isEmpty())
        return *this;
    QMessageFilterKey default;
    QMessageFilterKey *left(new QMessageFilterKey(*this));
    QMessageFilterKey *right(new QMessageFilterKey(other));
    *this = default;
    d_ptr->_left = left;
    d_ptr->_right = right;
    d_ptr->_operator = QMessageFilterKeyPrivate::And;
    if (!d_ptr->_left->d_ptr->_valid || !d_ptr->_right->d_ptr->_valid)
        d_ptr->_valid = false;
    return *this;
}

const QMessageFilterKey& QMessageFilterKey::operator|=(const QMessageFilterKey& other)
{
    if (&other == this)
        return *this;
    if (isEmpty())
        return *this;
    if (other.isEmpty()) {
        *this = other;
        return *this;
    }
    QMessageFilterKey default;
    QMessageFilterKey *left(new QMessageFilterKey(*this));
    QMessageFilterKey *right(new QMessageFilterKey(other));
    *this = default;
    d_ptr->_left = left;
    d_ptr->_right = right;
    d_ptr->_operator = QMessageFilterKeyPrivate::Or;
    if (!d_ptr->_left->d_ptr->_valid || !d_ptr->_right->d_ptr->_valid)
        d_ptr->_valid = false;
    return *this;
}

bool QMessageFilterKey::operator==(const QMessageFilterKey& other) const
{
    if (d_ptr->_operator == QMessageFilterKeyPrivate::Identity) {
        if (other.d_ptr->_operator != QMessageFilterKeyPrivate::Identity)
            return false;
        return (d_ptr->_field == other.d_ptr->_field
            && d_ptr->_value == other.d_ptr->_value
            && d_ptr->_comparatorType == other.d_ptr->_comparatorType
            && d_ptr->_comparatorValue == other.d_ptr->_comparatorValue);
    }
    if (other.d_ptr->_operator != d_ptr->_operator)
        return false;
    if (d_ptr->_left == other.d_ptr->_left 
        && d_ptr->_right == other.d_ptr->_right)
        return true;
    if (d_ptr->_left == other.d_ptr->_right 
        && d_ptr->_right == other.d_ptr->_left)
        return true;

    // TODO: For a system of determining equality of boolean algebra expressions see:
    // TODO:  Completely distributed normal form http://wikpedia.org/wiki/Boolean_algebra(logic)
    return false;
}

const QMessageFilterKey& QMessageFilterKey::operator=(const QMessageFilterKey& other)
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
    d_ptr->_valid = other.d_ptr->_valid;

    if (other.d_ptr->_left)
        d_ptr->_left = new QMessageFilterKey(*other.d_ptr->_left);
    if (other.d_ptr->_right)
        d_ptr->_right = new QMessageFilterKey(*other.d_ptr->_right);
    return *this;
}

QMessageFilterKey QMessageFilterKey::id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    // Not implemented
    QMessageFilterKey result(QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Id, QVariant(id.toString()), cmp)); // stub
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilterKey QMessageFilterKey::id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    QStringList idList;
    foreach(QMessageId id, ids)
        idList << id.toString();
    QMessageFilterKey result(QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Id, QVariant(idList), cmp)); // stub
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilterKey QMessageFilterKey::id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    QMessageFilterKey result; // stub
    result.d_ptr->_valid = false; // Will require synchronous filter evaluation
    return result;
}

QMessageFilterKey QMessageFilterKey::type(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
{
    // Not implemented
    QMessageFilterKey result(QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Type, QVariant(type), cmp)); // stub
    result.d_ptr->_valid = false; // Not natively implementable
    return result;
}

QMessageFilterKey QMessageFilterKey::type(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    QMessageFilterKey result(QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Type, QVariant(type), cmp)); // stub
    result.d_ptr->_valid = false; // Not natively implementable
    return result;
}

QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    QMessageFilterKey result(QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Sender, QVariant(value), cmp)); // stub
    result.d_ptr->_valid = false; // Not implemented
    return result;
}

QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    // Partially implemented, search email address but not name
    if (value.isEmpty()) {
        if (cmp == QMessageDataComparator::Includes)
            return QMessageFilterKey();
        return ~QMessageFilterKey();
    }
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Sender, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    // Not implemented
    QMessageFilterKey result(QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Recipients, QVariant(value), cmp));
    result.d_ptr->_valid = false; // Not supported
    return result;
}

QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    if (value.isEmpty()) {
        if (cmp == QMessageDataComparator::Includes)
            return QMessageFilterKey();
        return ~QMessageFilterKey();
    }
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Recipients, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    // TODO: Test this filter
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Subject, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    if (value.isEmpty()) {
        if (cmp == QMessageDataComparator::Includes)
            return QMessageFilterKey();
        return ~QMessageFilterKey();
    }
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Subject, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::TimeStamp, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::TimeStamp, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::ReceptionTimeStamp, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::ReceptionTimeStamp, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::status(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageDataComparator::InclusionComparator comparator(QMessageDataComparator::Excludes);
    if (cmp == QMessageDataComparator::Equal)
        comparator = QMessageDataComparator::Includes;
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Status, QVariant(value), comparator);
}

QMessageFilterKey QMessageFilterKey::status(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    QMessageDataComparator::EqualityComparator comparator(QMessageDataComparator::NotEqual);
    if (cmp == QMessageDataComparator::Includes)
        comparator = QMessageDataComparator::Equal;
    if (mask & QMessage::Incoming)
        result &= QMessageFilterKey::status(QMessage::Incoming, comparator);
    if (mask & QMessage::Read)
        result &= QMessageFilterKey::status(QMessage::Read, comparator);
    if (mask & QMessage::Removed)
        result &= QMessageFilterKey::status(QMessage::Removed, comparator);
    if (mask & QMessage::HasAttachments)
        result &= QMessageFilterKey::status(QMessage::HasAttachments, comparator);
    if (result.isEmpty()) // Be consistent with QMF, but seems wrong. TODO verify correctness
        return ~result;
    return result;
}

QMessageFilterKey QMessageFilterKey::priority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Priority, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::size(int value, QMessageDataComparator::EqualityComparator cmp)
{
    // TODO: Test this filer
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Size, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::size(int value, QMessageDataComparator::RelationComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Size, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QStringList nameValue;
    nameValue << name;
    nameValue << value;
    QMessageFilterKey result(QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::CustomField, QVariant(nameValue), cmp));
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilterKey QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QStringList nameValue;
    nameValue << name;
    nameValue << value;
    QMessageFilterKey result(QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::CustomField, QVariant(nameValue), cmp));
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilterKey QMessageFilterKey::parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    // Not implemented
    QMessageFilterKey result(QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::ParentAccountId, QVariant(id.toString()), cmp));
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilterKey QMessageFilterKey::parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    QMessageFilterKey result; // stub
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFilterKey QMessageFilterKey::parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    // Not implemented
    QMessageFilterKey result(QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::ParentFolderId, QVariant(id.toString()), cmp)); // stub
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilterKey QMessageFilterKey::parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    QMessageFilterKey result; // stub
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilterKey QMessageFilterKey::ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    QMessageFilterKey result(QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::AncestorFolderIds, QVariant(id.toString()), cmp));
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}

QMessageFilterKey QMessageFilterKey::ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    // Not implemented
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    QMessageFilterKey result; // stub
    // Not natively implementable?
    result.d_ptr->_valid = false;
    return result;
}
#endif
