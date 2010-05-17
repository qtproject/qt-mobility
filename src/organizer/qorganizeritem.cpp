/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** OrganizerItem: Nokia Corporation (qt-info@nokia.com)
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

#include "qorganizeritem.h"


#include <QSet>
#include <QDebug>

#include "qorganizeritem.h"
#include "qorganizeritem_p.h"
#include "qorganizeritemdetail_p.h"
#include "qorganizeritemmanager_p.h"

#include "qorganizeritemdisplaylabel.h"

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerItem

  \brief The QOrganizerItem class represents an addressbook organizeritem.

  \ingroup organizeritems-main

  Individual organizeritems, groups, and other types of organizeritems are represented with
  a QOrganizerItem object.  In addition to the type, a QOrganizerItem consists of information
  that belongs to the organizeritem.

  A QOrganizerItem object has a collection of details (like a start date, location and
  attendees).  Each detail (which can have multiple fields) is stored
  in an appropriate subclass of QOrganizerItemDetail, and the QOrganizerItem allows
  retrieving these details in various ways.

  A QOrganizerItem instance represents the in-memory version of a calendar organizeritem,
  and has no tie to a specific QOrganizerItemManager.  It is possible for the contents
  of a QOrganizerItem to change independently of the contents that are stored persistently
  in a QOrganizerItemManager.  A QOrganizerItem has an ID associated with it when it is first
  retrieved from a QOrganizerItemManager, or after it has been first saved, and this allows
  clients to track changes using the signals in QOrganizerItemManager.

  A QOrganizerItem has a number of mandatory details:
  \list
   \o A QOrganizerItemType, with the type of the organizeritem (individual event, todo, journal etc)
   \o etc etc
  \endlist

  \sa QOrganizerItemManager, QOrganizerItemDetail
 */

/*!
 * \fn QList<T> QOrganizerItem::details() const
 * Returns a list of details of the template parameter type.  The type must be
 * a subclass of QOrganizerItemDetail.
 *
 * For example:
 *  \snippet doc/src/snippets/qtorganizeritemsdocsample/qtorganizeritemsdocsample.cpp 3
 */

/*!
 * \fn QList<T> QOrganizerItem::details(const QString& fieldName, const QString& value) const
 * Returns a list of details of the template parameter type which have field called \a fieldName, with matching \a value.
 * The type must be a subclass of QOrganizerItemDetail.
 *
 * For example:
 *  \snippet doc/src/snippets/qtorganizeritemsdocsample/qtorganizeritemsdocsample.cpp 4
 */

/*!
 * \fn T QOrganizerItem::detail() const
 * Returns the first detail of the template parameter type, as returned by the template details() function.
 * The type must be a subclass of QOrganizerItemDetail.
 */

/*!
 * \fn QOrganizerItem::operator!=(const QOrganizerItem &other) const
 * Returns true if this organizeritems id or details are different to those of the \a other organizeritem.
 */

/*!
    Construct an empty organizeritem.

    The organizeritem will have an empty display label, an empty id, and have type \l QOrganizerItemType::TypeOrganizerItem.
    The isEmpty() function will return true.
*/
QOrganizerItem::QOrganizerItem()
    : d(new QOrganizerItemData)
{
    clearDetails();
}

/*! Initializes this QOrganizerItem from \a other */
QOrganizerItem::QOrganizerItem(const QOrganizerItem& other)
    : d(other.d)
{
}

/*!
    \internal

    Constructs a new, empty item of the given type \a type.
*/
QOrganizerItem::QOrganizerItem(const char* type)
{
    d = new QOrganizerItemData;
    clearDetails();
    setType(QString(QLatin1String(type)));
}

/*!
    \internal

    Constructs an item that is a copy of \a other if \a other is of the expected type
    identified by \a expectedType, else constructs a new, empty item of the
    type identified by the \a expectedType.

    The \a expectedType pointer must be valid for the lifetime of the program.
*/
QOrganizerItem::QOrganizerItem(const QOrganizerItem& other, const char* expectedType)
{
    if (other.type() == QString(QLatin1String(expectedType))) {
        d = other.d;
    } else {
        d = new QOrganizerItemData;
        setType(QString(QLatin1String(expectedType)));
    }
}

/*!
    \internal

    Assigns this item to \a other if the type of \a other is that identified
    by the given \a expectedType, else assigns this item to be a new, empty
    item of the type identified by the given \a expectedType
*/
QOrganizerItem& QOrganizerItem::assign(const QOrganizerItem& other, const char* expectedType)
{
    if (this != &other) {
        if (other.type() == QString(QLatin1String(expectedType))) {
            d = other.d;
        } else {
            d = new QOrganizerItemData;
            setType(QString(QLatin1String(expectedType)));
        }
    }
    return *this;
}


/*!
 * Returns true if this QOrganizerItem is empty, false if not.
 *
 * An empty QOrganizerItem has an empty label and no extra details.
 * The type of the organizeritem is irrelevant.
 */
bool QOrganizerItem::isEmpty() const
{
    /* Every organizeritem has a display label field.. */
    if (d->m_details.count() > 2)
        return false;

    /* We know we have two details (a display label and a type) */
    const QOrganizerItemDisplayLabel& label = d->m_details.at(0);
    return label.label().isEmpty();
}

/*!
 * Removes all details of the organizeritem.
 * This function does not modify the id or type of the organizeritem.
 * Calling isEmpty() after calling this function will return true.
 * The empty item becomes a \l QOrganizerItemNote.
 */
void QOrganizerItem::clearDetails()
{
    d->m_details.clear();

    // and the organizeritem type detail.
    QOrganizerItemType organizeritemType;
    organizeritemType.setType(QOrganizerItemType::TypeNote);
    organizeritemType.d->m_access = QOrganizerItemDetail::Irremovable;
    d->m_details.insert(0, organizeritemType);
}

/*! Replace the contents of this QOrganizerItem with \a other */
QOrganizerItem& QOrganizerItem::operator=(const QOrganizerItem& other)
{
    d = other.d;
    return *this;
}

/*! Frees the memory used by this QOrganizerItem */
QOrganizerItem::~QOrganizerItem()
{
}

/*!
    Returns the QOrganizerItemId that identifies this organizeritem.

    This may have been set when the organizeritem was retrieved from
    a particular manager, or when the organizeritem was first saved
    in a manager.  The QOrganizerItemId is only valid with a specific
    manager.  See \l QOrganizerItemManager::saveOrganizerItem() for more
    information.

    \sa localId()
 */
QOrganizerItemId QOrganizerItem::id() const
{
    return d->m_id;
}

/*!
    Returns the QOrganizerItemLocalId that identifies this organizeritem within its manager

    This may have been set when the organizeritem was retrieved from
    a particular manager, or when the organizeritem was first saved
    in a manager.  The QOrganizerItemLocalId is associated with a specific
    manager, but other organizeritems with the same local id might exist in
    different managers.

    See \l QOrganizerItemManager::saveOrganizerItem() for more
    information.

   \sa id()
*/
QOrganizerItemLocalId QOrganizerItem::localId() const
{
    return d->m_id.localId();
}

/*!
 * Returns the type of the organizeritem.  Every organizeritem has exactly one type which
 * is either set manually (by saving a modified copy of the QOrganizerItemType
 * in the organizeritem, or by calling \l setType()) or synthesized automatically.
 *
 * \sa setType()
 */
QString QOrganizerItem::type() const
{
    // type is detail 0
    QString type = d->m_details.at(0).value(QOrganizerItemType::FieldType);
    if (type.isEmpty())
        return QString(QLatin1String(QOrganizerItemType::TypeNote));
    return type;
}

/*!
 * Sets the type of the organizeritem to the given \a type.
 */
void QOrganizerItem::setType(const QString& type)
{
    // type is detail 0
    QOrganizerItemType newType;
    newType.setType(type);
    newType.d->m_access = QOrganizerItemDetail::Irremovable;

    d->m_details[0] = newType;
}

/*!
 * Sets the type of the organizeritem to the given \a type detail.
 */
void QOrganizerItem::setType(const QOrganizerItemType& type)
{
    // type is detail 0
    d->m_details[0] = type;
    d->m_details[0].d->m_access = QOrganizerItemDetail::Irremovable;
}

QString QOrganizerItem::displayLabel() const
{
    QOrganizerItemDisplayLabel dl = detail<QOrganizerItemDisplayLabel>();
    return dl.label();
}
void QOrganizerItem::setDisplayLabel(const QString& label)
{
    QOrganizerItemDisplayLabel dl = detail<QOrganizerItemDisplayLabel>();
    dl.setLabel(label);
    saveDetail(&dl);
}
void QOrganizerItem::setDisplayLabel(const QOrganizerItemDisplayLabel& label)
{
    QOrganizerItemDisplayLabel dl = detail<QOrganizerItemDisplayLabel>();
    dl.setLabel(label.label());
    saveDetail(&dl);
}

QString QOrganizerItem::description() const
{
    QOrganizerItemDescription descr = detail<QOrganizerItemDescription>();
    return descr.description();
}
void QOrganizerItem::setDescription(const QString& description)
{
    QOrganizerItemDescription descr = detail<QOrganizerItemDescription>();
    descr.setDescription(description);
    saveDetail(&descr);
}
void QOrganizerItem::setDescription(const QOrganizerItemDescription& description)
{
    QOrganizerItemDescription descr = detail<QOrganizerItemDescription>();
    descr.setDescription(description.description());
    saveDetail(&descr);
}


/*!
 * Sets the id of this organizeritem to \a id.
 *
 * Note that this only affects this object, not any corresponding structures stored
 * by a QOrganizerItemManager.
 *
 * If you change the id of a organizeritem and save the organizeritem
 * in a manager, the previously existing organizeritem will still
 * exist.  You can do this to create copies (possibly modified)
 * of an existing organizeritem, or to save a organizeritem in a different manager.
 *
 * \sa QOrganizerItemManager::saveOrganizerItem()
 */
void QOrganizerItem::setId(const QOrganizerItemId& id)
{
    d->m_id = id;
}

/*!
    \fn QOrganizerItemDetail QOrganizerItem::detail(const QLatin1Constant& definitionName) const
    Returns the first detail stored in the organizeritem which with the given \a definitionName.
    The \a definitionName argument is typically the detail name constant provided by a
    specific subclass of QOrganizerItemDetail.  For example:

    \snippet doc/src/snippets/qtorganizeritemsdocsample/qtorganizeritemsdocsample.cpp 0

    It would usually be more convenient to use the template version of this function, in
    the following manner:

    \snippet doc/src/snippets/qtorganizeritemsdocsample/qtorganizeritemsdocsample.cpp 1
*/

/*!
    \fn QList<QOrganizerItemDetail> QOrganizerItem::details(const QLatin1Constant& definitionName) const
    Returns a list of details of the given \a definitionName.

    The \a definitionName argument is typically the detail name constant provided by a
    specific subclass of QOrganizerItemDetail.  For example:

    \snippet doc/src/snippets/qtorganizeritemsdocsample/qtorganizeritemsdocsample.cpp 2

    It would usually be more convenient to use the template version of this function, in
    the following manner:

    \snippet doc/src/snippets/qtorganizeritemsdocsample/qtorganizeritemsdocsample.cpp 3
*/

/*!
    \fn QList<QOrganizerItemDetail> QOrganizerItem::details(const QLatin1Constant& definitionName, const QLatin1Constant& fieldName, const QString& value)
    Returns a list of details of the given \a definitionName, with fields named \a fieldName and with value \a value.
*/

/*!
    \fn QList<T> QOrganizerItem::details(const char* fieldName, const QString& value) const
    \internal

    Returns a list of details of the template type which match the \a fieldName and \a value criteria
*/

/*!
    Returns the first detail stored in the organizeritem with the given \a definitionName
*/
QOrganizerItemDetail QOrganizerItem::detail(const QString& definitionName) const
{
    if (definitionName.isEmpty())
        return d->m_details.first();

    // build the sub-list of matching details.
    for (int i = 0; i < d->m_details.size(); i++) {
        const QOrganizerItemDetail& existing = d->m_details.at(i);
        if (QOrganizerItemDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName) {
            return existing;
        }
    }

    return QOrganizerItemDetail();
}

/*! Returns a list of details with the given \a definitionName
    The definitionName string can be determined by the DefinitionName attribute
    of defined objects (e.g. QOrganizerItemPhoneNumber::DefinitionName) or by
    requesting a list of all the definition names using
    \l {QOrganizerItemManager::detailDefinitions()}{detailDefinitions()} or the
    asynchronous \l
    {QOrganizerItemDetailDefinitionFetchRequest::definitionNames()}{definitionNames()}.*/
QList<QOrganizerItemDetail> QOrganizerItem::details(const QString& definitionName) const
{
    // build the sub-list of matching details.
    QList<QOrganizerItemDetail> sublist;

    // special case
    if (definitionName.isEmpty()) {
        sublist = d->m_details;
    } else {
        for (int i = 0; i < d->m_details.size(); i++) {
            const QOrganizerItemDetail& existing = d->m_details.at(i);
            if (QOrganizerItemDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName) {
                sublist.append(existing);
            }
        }
    }

    return sublist;
}

/*!
    Returns a list of details of the given \a definitionName, with fields named \a fieldName and with value \a value.
    The definitionName string can be determined by the DefinitionName attribute
    of defined objects (e.g. QOrganizerItemPhoneNumber::DefinitionName) or by
    requesting a list of all the definition names using
    \l {QOrganizerItemManager::detailDefinitions()}{detailDefinitions()} or the
    asynchronous \l
    {QOrganizerItemDetailDefinitionFetchRequest::definitionNames()}{definitionNames()}.*/
QList<QOrganizerItemDetail> QOrganizerItem::details(const QString& definitionName, const QString& fieldName, const QString& value) const
{
    // build the sub-list of matching details.
    QList<QOrganizerItemDetail> sublist;

    // special case
    if (fieldName.isEmpty()) {
        sublist = details(definitionName);
    } else {
        for (int i = 0; i < d->m_details.size(); i++) {
            const QOrganizerItemDetail& existing = d->m_details.at(i);
            if (QOrganizerItemDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName
                && existing.hasValue(fieldName) && value == existing.value(fieldName)) {
                sublist.append(existing);
            }
        }
    }

    return sublist;
}

/*!
    \internal
    Returns the first detail stored in the organizeritem which with the given \a definitionName
*/
QOrganizerItemDetail QOrganizerItem::detail(const char* definitionName) const
{
    if (definitionName == 0)
        return d->m_details.first();

    // build the sub-list of matching details.
    for (int i = 0; i < d->m_details.size(); i++) {
        const QOrganizerItemDetail& existing = d->m_details.at(i);
        if (QOrganizerItemDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName) {
            return existing;
        }
    }

    return QOrganizerItemDetail();
}

/*!
    \internal
    Returns a list of details with the given \a definitionName
*/
QList<QOrganizerItemDetail> QOrganizerItem::details(const char* definitionName) const
{
    // build the sub-list of matching details.
    QList<QOrganizerItemDetail> sublist;

    // special case
    if (definitionName == 0) {
        sublist = d->m_details;
    } else {
        for (int i = 0; i < d->m_details.size(); i++) {
            const QOrganizerItemDetail& existing = d->m_details.at(i);
            if (QOrganizerItemDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName) {
                sublist.append(existing);
            }
        }
    }

    return sublist;
}

/*!
    \internal
    Returns a list of details with the given \a definitionName, \a fieldName and field \a value
*/
QList<QOrganizerItemDetail> QOrganizerItem::details(const char* definitionName, const char* fieldName, const QString& value) const
{
    // build the sub-list of matching details.
    QList<QOrganizerItemDetail> sublist;

    // special case
    if (fieldName == 0) {
        sublist = details(definitionName);
    } else {
        for (int i = 0; i < d->m_details.size(); i++) {
            const QOrganizerItemDetail& existing = d->m_details.at(i);
            if (QOrganizerItemDetailPrivate::detailPrivate(existing)->m_definitionName == definitionName
                && existing.hasValue(fieldName) && value == existing.value(fieldName)) {
                sublist.append(existing);
            }
        }
    }

    return sublist;
}

/*!
 * Saves the given \a detail in the list of stored details, and sets the detail's id.
 * If another detail of the same type and id has been previously saved in
 * this organizeritem, that detail is overwritten.  Otherwise, a new id is generated
 * and set in the detail, and the detail is added to the organizeritem.
 *
 * If the detail's access constraint includes \c QOrganizerItemDetail::ReadOnly,
 * this function will return true and save the detail in the organizeritem,
 * however attempting to save the organizeritem in a manager may fail (if that manager
 * decides that the read only detail should not be updated).
 * Details with the \c QOrganizerItemDetail::ReadOnly constraint set are typically provided
 * in a organizeritem by the manager, and are usually information that is either
 * synthesized, or not intended to be changed by the user (e.g. presence information
 * for other organizeritems).
 *
 * If \a detail is a QOrganizerItemType, the existing organizeritem type will
 * be overwritten with \a detail.  There is never more than one organizeritem type
 * in a organizeritem.
 *
 * If \a detail is a QOrganizerItemDisplayLabel, the organizeritem will not be updated,
 * and the function will return false.  Since the display label formatting is specific
 * to each manager, use the QOrganizerItemManager::synthesizeOrganizerItemDisplayLabel() function
 * instead.
 *
 * Returns true if the detail was saved successfully, otherwise returns false.
 *
 * Note that the caller retains ownership of the detail.
 * \sa QOrganizerItemManager::synthesizeOrganizerItemDisplayLabel()
 */
bool QOrganizerItem::saveDetail(QOrganizerItemDetail* detail)
{
    if (!detail)
        return false;

    /* Also handle organizeritem type specially - only one of them. */
    if (QOrganizerItemDetailPrivate::detailPrivate(*detail)->m_definitionName == QOrganizerItemType::DefinitionName.latin1()) {
        detail->d->m_access |= QOrganizerItemDetail::Irremovable;
        d->m_details[0] = *detail; // type is zeroth detail.
        return true;
    }

    /* And display label.. */
    if (QOrganizerItemDetailPrivate::detailPrivate(*detail)->m_definitionName == QOrganizerItemDisplayLabel::DefinitionName.latin1()) {
        return false;
    }

    // try to find the "old version" of this field
    // ie, the one with the same type and id, but different value or attributes.
    for (int i = 0; i < d->m_details.size(); i++) {
        const QOrganizerItemDetail& curr = d->m_details.at(i);
        if (detail->d->m_definitionName == curr.d->m_definitionName && detail->d->m_id == curr.d->m_id) {
            // update the detail constraints of the supplied detail
            detail->d->m_access = d->m_details[i].accessConstraints();
            // Found the old version.  Replace it with this one.
            d->m_details[i] = *detail;
            return true;
        }
    }

    // this is a new detail!  add it to the organizeritem.
    d->m_details.append(*detail);
    return true;
}

/*!
 * Removes the \a detail from the organizeritem.
 *
 * The detail in the organizeritem which has the same key as that of the given \a detail
 * will be removed if it exists.  Only the key is used for comparison - that is, the
 * information in the detail may be different.
 *
 * If the detail's access constraint includes \c QOrganizerItemDetail::Irremovable,
 * this function will return false.
 *
 * Returns true if the detail was removed successfully, false if an error occurred.
 *
 * Note that the caller retains ownership of the detail.
 */
bool QOrganizerItem::removeDetail(QOrganizerItemDetail* detail)
{
    if (!detail)
        return false;

    // find the detail stored in the organizeritem which has the same key as the detail argument
    int removeIndex = -1;
    for (int i = 0; i < d->m_details.size(); i++) {
        if (d->m_details.at(i).key() == detail->key()) {
            removeIndex = i;
            break;
        }
    }

    // make sure the detail exists (in some form) in the organizeritem.
    if (removeIndex < 0)
        return false;

    if (detail->accessConstraints() & QOrganizerItemDetail::Irremovable)
        return false;

    if (!d->m_details.contains(*detail))
        return false;

    // then remove the detail.
    d->m_details.removeAt(removeIndex);
    return true;
}

/*! Returns true if this organizeritem is equal to the \a other organizeritem, false if either the id or stored details are not the same */
bool QOrganizerItem::operator==(const QOrganizerItem& other) const
{
    return other.d->m_id == d->m_id &&
        other.d->m_details == d->m_details;
}

/*!
    \relates QOrganizerItem
    Returns the hash value for \a key.
*/
uint qHash(const QOrganizerItem &key)
{
    uint hash = qHash(key.id());
    foreach (const QOrganizerItemDetail& detail, key.details()) {
        hash += qHash(detail);
    }
    return hash;
}

QDebug operator<<(QDebug dbg, const QOrganizerItem& organizeritem)
{
    dbg.nospace() << "QOrganizerItem(" << organizeritem.id() << ")";
    foreach (const QOrganizerItemDetail& detail, organizeritem.details()) {
        dbg.space() << '\n' << detail;
    }
    return dbg.maybeSpace();
}

QTM_END_NAMESPACE
