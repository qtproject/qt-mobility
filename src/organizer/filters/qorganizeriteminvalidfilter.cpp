/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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

#include "qorganizeriteminvalidfilter.h"
#include "qorganizeritemfilter_p.h"

/*!
  \class QOrganizerItemInvalidFilter
  \brief The QOrganizerItemInvalidFilter class matches no organizeritems.
  \inmodule QtOrganizer
  \ingroup organizer-filters
 
  This class provides a filter which will never match any organizeritems
 */

QTM_BEGIN_NAMESPACE

class QOrganizerItemInvalidFilterPrivate : public QOrganizerItemFilterPrivate
{
public:
    QOrganizerItemInvalidFilterPrivate()
       : QOrganizerItemFilterPrivate()
    {
    }

    bool compare(const QOrganizerItemFilterPrivate*) const
    {
        return true; // all invalid filters are alike
    }

    QDataStream& outputToStream(QDataStream& stream, quint8 formatVersion) const
    {
        Q_UNUSED(formatVersion)
        return stream;
    }

    QDataStream& inputFromStream(QDataStream& stream, quint8 formatVersion)
    {
        Q_UNUSED(formatVersion)
        return stream;
    }

    /* There is no way this can be called - d is never detached */
    QOrganizerItemFilterPrivate* clone() const
    {
        return new QOrganizerItemInvalidFilterPrivate();
    }

    QOrganizerItemFilter::FilterType type() const
    {
        return QOrganizerItemFilter::InvalidFilter;
    }

    QList<QOrganizerItemFilter> m_filters;
};

/*!
 * Constructs a new invalid filter
 */
QOrganizerItemInvalidFilter::QOrganizerItemInvalidFilter()
    : QOrganizerItemFilter(new QOrganizerItemInvalidFilterPrivate)
{
}

/*!
 * Constructs a new invalid filter, ignoring the \a other filter
 */
QOrganizerItemInvalidFilter::QOrganizerItemInvalidFilter(const QOrganizerItemFilter& other)
    : QOrganizerItemFilter(new QOrganizerItemInvalidFilterPrivate)
{
    // Initializing a QCIF from anything is the same as just constructing a QCIF
    Q_UNUSED(other);
}

QTM_END_NAMESPACE
