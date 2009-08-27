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

void QMessageFilterKeyPrivate::filterTable(QMessageStore::ErrorCode *lastError, const QMessageFilterKey &key, LPMAPITABLE messagesTable)
{
    SRestriction restriction;
    SPropValue keyProp;
    bool notRestriction(false);
    bool valid(false);

    switch (key.d_ptr->_comparatorType) {
    case Equality: {
        restriction.rt = RES_PROPERTY;
        QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(key.d_ptr->_comparatorValue));
        if (cmp == QMessageDataComparator::Equal)
            restriction.res.resProperty.relop = RELOP_EQ;
        else
            restriction.res.resProperty.relop = RELOP_NE;
        break;
    }
    case Relation: {
        restriction.rt = RES_PROPERTY;
        QMessageDataComparator::RelationComparator cmp(static_cast<QMessageDataComparator::RelationComparator>(key.d_ptr->_comparatorValue));
        switch (cmp) {
        case QMessageDataComparator::LessThan:
            restriction.res.resProperty.relop = RELOP_LT;
            break;
        case QMessageDataComparator::LessThanEqual:
            restriction.res.resProperty.relop = RELOP_LE;
            break;
        case QMessageDataComparator::GreaterThan:
            restriction.res.resProperty.relop = RELOP_GT;
            break;
        case QMessageDataComparator::GreaterThanEqual:
            restriction.res.resProperty.relop = RELOP_GE;
            break;
        }
        switch (key.d_ptr->_field) {
        case Size:
            restriction.res.resProperty.ulPropTag = PR_MESSAGE_SIZE;
            restriction.res.resProperty.lpProp = &keyProp;
            keyProp.ulPropTag = PR_MESSAGE_SIZE;
            keyProp.Value.ul = key.d_ptr->_value.toInt();
            valid = true;
            break;
        default:
            qWarning("Unhandled restriction criteria");
        }
        break;
    }
    case Inclusion: {
        QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(key.d_ptr->_comparatorValue));
        restriction.rt = RES_CONTENT;
        // May need to complement with SExistRestriction see http://msdn.microsoft.com/en-us/library/aa454981.aspx
        if (key.d_ptr->_options & QMessageDataComparator::FullWord)
            restriction.res.resContent.ulFuzzyLevel = FL_FULLSTRING;
        else
            restriction.res.resContent.ulFuzzyLevel = FL_SUBSTRING;
        if ((key.d_ptr->_options & QMessageDataComparator::CaseSensitive) != 0)
            restriction.res.resContent.ulFuzzyLevel |= FL_IGNORECASE;
        if (cmp == QMessageDataComparator::Excludes)
            notRestriction = true;
        break;
    }
    }

    if (!valid)
        return; //TODO set lastError to unsupported
/*
    TODO set the property tag and value
    restriction.res.resProperty.ulPropTag = PR_RECORD_KEY;
    restriction.res.resProperty.lpProp = &keyProp;
    keyProp.ulPropTag = PR_RECORD_KEY;
    keyProp.Value.bin.cb = key.count();
    keyProp.Value.bin.lpb = reinterpret_cast<LPBYTE>(key.data());
*/

    ULONG flags(0);
    if (messagesTable->Restrict(&restriction, flags) != S_OK)
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
     _right(0)
{
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
    return true; // TODO: Implement
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
    QMessageFilterKey result;
    result.d_ptr->_left = new QMessageFilterKey(*this);
    result.d_ptr->_right = new QMessageFilterKey(other);
    result.d_ptr->_operator = QMessageFilterKeyPrivate::And;
    return result;
}

QMessageFilterKey QMessageFilterKey::operator|(const QMessageFilterKey& other) const
{
    QMessageFilterKey result;
    result.d_ptr->_left = new QMessageFilterKey(*this);
    result.d_ptr->_right = new QMessageFilterKey(other);
    result.d_ptr->_operator = QMessageFilterKeyPrivate::Or;
    return result;
}

const QMessageFilterKey& QMessageFilterKey::operator&=(const QMessageFilterKey& other)
{
    QMessageFilterKey default;
    QMessageFilterKey *left(new QMessageFilterKey(*this));
    QMessageFilterKey *right(new QMessageFilterKey(other));
    *this = default;
    d_ptr->_left = left;
    d_ptr->_right = right;
    d_ptr->_operator = QMessageFilterKeyPrivate::And;
    return *this;
}

const QMessageFilterKey& QMessageFilterKey::operator|=(const QMessageFilterKey& other)
{
    QMessageFilterKey default;
    QMessageFilterKey *left(new QMessageFilterKey(*this));
    QMessageFilterKey *right(new QMessageFilterKey(other));
    *this = default;
    d_ptr->_left = left;
    d_ptr->_right = right;
    d_ptr->_operator = QMessageFilterKeyPrivate::Or;
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

    // TODO: Association, De Morgan's law, and more?
    return false;
}

const QMessageFilterKey& QMessageFilterKey::operator=(const QMessageFilterKey& other)
{
    delete d_ptr->_left;
    d_ptr->_left = 0;
    delete d_ptr->_right;
    d_ptr->_right = 0;
    d_ptr->_field = other.d_ptr->_field;
    d_ptr->_value = other.d_ptr->_value;
    d_ptr->_comparatorType = other.d_ptr->_comparatorType;
    d_ptr->_comparatorValue = other.d_ptr->_comparatorValue;
    d_ptr->_operator = other.d_ptr->_operator;
    if (other.d_ptr->_left)
        d_ptr->_left = new QMessageFilterKey(*other.d_ptr->_left);
    if (other.d_ptr->_right)
        d_ptr->_right = new QMessageFilterKey(*other.d_ptr->_right);
    return *this;
}

QMessageFilterKey QMessageFilterKey::id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Id, QVariant(id.toString()), cmp);
}

QMessageFilterKey QMessageFilterKey::id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QStringList idList;
    foreach(QMessageId id, ids)
        idList << id.toString();
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Id, QVariant(idList), cmp);
}

QMessageFilterKey QMessageFilterKey::id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    //TODO
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::type(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Type, QVariant(type), cmp);
}

QMessageFilterKey QMessageFilterKey::type(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Type, QVariant(type), cmp);
}

QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Sender, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Sender, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Recipients, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Recipients, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Subject, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
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
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Status, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::status(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Status, QVariant(mask), cmp);
}

QMessageFilterKey QMessageFilterKey::priority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::Priority, QVariant(value), cmp);
}

QMessageFilterKey QMessageFilterKey::size(int value, QMessageDataComparator::EqualityComparator cmp)
{
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
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::CustomField, QVariant(nameValue), cmp);
}

QMessageFilterKey QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QStringList nameValue;
    nameValue << name;
    nameValue << value;
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::CustomField, QVariant(nameValue), cmp);
}

QMessageFilterKey QMessageFilterKey::parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::ParentAccountId, QVariant(id.toString()), cmp);
}

QMessageFilterKey QMessageFilterKey::parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    //TODO
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFilterKey QMessageFilterKey::parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::ParentFolderId, QVariant(id.toString()), cmp);
}

QMessageFilterKey QMessageFilterKey::parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    //TODO
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    return QMessageFilterKeyPrivate::from(QMessageFilterKeyPrivate::AncestorFolderIds, QVariant(id.toString()), cmp);
}

QMessageFilterKey QMessageFilterKey::ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    //TODO
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}
#endif
