/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageaccountfilter.h"
#include "qmfhelpers_p.h"

#include <qmailaccountkey.h>

using namespace QTM_PREPEND_NAMESPACE(QmfHelpers);

QTM_BEGIN_NAMESPACE

class QMessageAccountFilterPrivate
{
public:
    QMessageAccountFilterPrivate() : _key(), _matchFlags(0) {}

    QMailAccountKey _key;
    QMessageDataComparator::MatchFlags _matchFlags;

    //static QMessageAccountFilter convert(const QMailAccountKey &key);
    static QMailAccountKey convert(const QMessageAccountFilter &key);
};

/*
QMessageAccountFilter QMessageAccountFilterPrivate::convert(const QMailAccountKey &key)
{
    QMessageAccountFilter result;
    result.d_ptr->_key = key;
    return result;
}
*/

QMailAccountKey QMessageAccountFilterPrivate::convert(const QMessageAccountFilter &filter)
{
    return filter.d_ptr->_key;
}

namespace QmfHelpers {

/*
QMessageAccountFilter convert(const QMailAccountKey &key)
{
    return QMessageAccountFilterPrivate::convert(key);
}
*/

QMailAccountKey convert(const QMessageAccountFilter &filter)
{
    return QMessageAccountFilterPrivate::convert(filter);
}

}

QMessageAccountFilter::QMessageAccountFilter()
    : d_ptr(new QMessageAccountFilterPrivate)
{
}

QMessageAccountFilter::QMessageAccountFilter(const QMessageAccountFilter &other)
    : d_ptr(new QMessageAccountFilterPrivate)
{
    this->operator=(other);
}

QMessageAccountFilter::~QMessageAccountFilter()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageAccountFilter& QMessageAccountFilter::operator=(const QMessageAccountFilter& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
        d_ptr->_matchFlags = other.d_ptr->_matchFlags;
    }

    return *this;
}

void QMessageAccountFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    d_ptr->_matchFlags = matchFlags;
}

QMessageDataComparator::MatchFlags QMessageAccountFilter::matchFlags() const
{
    return d_ptr->_matchFlags;
}

bool QMessageAccountFilter::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageAccountFilter::isSupported() const
{
    return !d_ptr->_matchFlags;
}

QMessageAccountFilter QMessageAccountFilter::operator~() const
{
    QMessageAccountFilter result;
    result.d_ptr->_key = ~d_ptr->_key;
    return result;
}

QMessageAccountFilter QMessageAccountFilter::operator&(const QMessageAccountFilter& other) const
{
    QMessageAccountFilter result;
    result.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    result.d_ptr->_matchFlags = d_ptr->_matchFlags | other.d_ptr->_matchFlags; // matchFlags not supported
    return result;
}

QMessageAccountFilter QMessageAccountFilter::operator|(const QMessageAccountFilter& other) const
{
    QMessageAccountFilter result;
    result.d_ptr->_key = d_ptr->_key | other.d_ptr->_key;
    result.d_ptr->_matchFlags = d_ptr->_matchFlags | other.d_ptr->_matchFlags; // matchFlags not supported
    return result;
}

const QMessageAccountFilter& QMessageAccountFilter::operator&=(const QMessageAccountFilter& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    d_ptr->_matchFlags |= other.d_ptr->_matchFlags; // matchFlags not supported
    return *this;
}

const QMessageAccountFilter& QMessageAccountFilter::operator|=(const QMessageAccountFilter& other)
{
    d_ptr->_key |= other.d_ptr->_key;
    d_ptr->_matchFlags |= other.d_ptr->_matchFlags; // matchFlags not supported
    return *this;
}

bool QMessageAccountFilter::operator==(const QMessageAccountFilter& other) const
{
    return ((d_ptr->_key == other.d_ptr->_key)
            && (d_ptr->_matchFlags == other.d_ptr->_matchFlags));
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_key = QMailAccountKey::id(convert(id), convert(cmp));
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_key = QMailAccountKey::id(convert(ids), convert(cmp));
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountFilter &key, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_key = QMailAccountKey::id(convert(key), convert(cmp));
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_key = QMailAccountKey::name(value, convert(cmp));
    return result;
}

QMessageAccountFilter QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageAccountFilter result;
    result.d_ptr->_key = QMailAccountKey::name(value, convert(cmp));
    return result;
}

QTM_END_NAMESPACE
