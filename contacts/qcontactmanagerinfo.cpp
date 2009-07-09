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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactmanagerinfo.h"
#include "qcontactmanagerinfo_p.h"

#include "qcontactmanager_p.h"

/*!
 * \class QContactManagerInfo
 *
 * \brief The QContactManagerInfo class provides information about a particular QContactManager.
 *
 * This class allows managers to report information about their supported functionality to clients.
 * The supported functionality of a manager includes which data types the backend supports,
 * which definitions are natively filterable in the backend (and thus are high performance),
 * and what capabilities the manager supports.
 *
 * If the manager that provided an information object is subsequently destroyed, the
 * QContactManagerInfo object will report empty information.
 *
 * \sa QContactManager
 */

/*! Constructs a new, empty QContactManagerInfo object */
QContactManagerInfo::QContactManagerInfo()
    : d(new QContactManagerInfoPrivate)
{
}

/*! Frees the memory used by this instance */
QContactManagerInfo::~QContactManagerInfo()
{
}

/*! Constructs a new QContactManagerInfo object from the existing \a other object. */
QContactManagerInfo::QContactManagerInfo(const QContactManagerInfo& other)
    : d(other.d)
{

}

/*! Assigns the existing QContactManagerInfo object \a other to this object */
QContactManagerInfo& QContactManagerInfo::operator=(const QContactManagerInfo& other)
{
    if (this != &other) {
        d = other.d;
    }
    return *this;
}

/*!
 * Returns a list of supported capabilities which are supported by
 * the manager associated with this object.
 *
 * TODO add link to schema.
 */
QStringList QContactManagerInfo::capabilities() const
{
    if (d->m_managerdata)
        return d->m_engine->capabilities();
    return QStringList();
}

/*!
 * Returns a list of definition identifiers which are natively (fast) filterable
 * by the manager associated with this object.
 */
QStringList QContactManagerInfo::fastFilterableDefinitions() const
{
    if (d->m_managerdata)
        return d->m_engine->fastFilterableDefinitions();
    return QStringList();
}

/*!
 * Returns the list of data types supported by the manager
 * associated with this object.
 */
QList<QVariant::Type> QContactManagerInfo::supportedDataTypes() const
{
    if (d->m_managerdata)
        return d->m_engine->supportedDataTypes();
    return QList<QVariant::Type>();
}
