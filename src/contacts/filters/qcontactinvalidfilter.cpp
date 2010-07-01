/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
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

#include "qcontactinvalidfilter.h"
#include "qcontactfilter_p.h"

/*!
  \class QContactInvalidFilter
  \brief The QContactInvalidFilter class matches no contacts.
   \ingroup contacts-filters
 
  This class provides a filter which will never match any contacts
 */

QTM_BEGIN_NAMESPACE

class QContactInvalidFilterPrivate : public QContactFilterPrivate
{
public:
    QContactInvalidFilterPrivate()
       : QContactFilterPrivate()
    {
    }

    bool compare(const QContactFilterPrivate*) const
    {
        return true; // all invalid filters are alike
    }

    QDataStream& outputToStream(QDataStream& stream) const
    {
        return stream;
    }

    QDataStream& inputFromStream(QDataStream& stream)
    {
        return stream;
    }

    /* There is no way this can be called - d is never detached */
    QContactFilterPrivate* clone() const
    {
        return new QContactInvalidFilterPrivate();
    }

    QContactFilter::FilterType type() const
    {
        return QContactFilter::InvalidFilter;
    }
};

/*!
 * Constructs a new invalid filter
 */
QContactInvalidFilter::QContactInvalidFilter()
    : QContactFilter(new QContactInvalidFilterPrivate)
{
}

/*!
 * Constructs a new invalid filter, ignoring the \a other filter
 */
QContactInvalidFilter::QContactInvalidFilter(const QContactFilter& other)
    : QContactFilter(new QContactInvalidFilterPrivate)
{
    // Initializing a QCIF from anything is the same as just constructing a QCIF
    Q_UNUSED(other);
}

QTM_END_NAMESPACE
