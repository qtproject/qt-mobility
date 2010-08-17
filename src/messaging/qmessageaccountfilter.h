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
#ifndef QMESSAGEACCOUNTFILTER_H
#define QMESSAGEACCOUNTFILTER_H
#include <qmessageglobal.h>
#include <qmessagedatacomparator.h>
#include <qmessageaccountid.h>

QTM_BEGIN_NAMESPACE

class QMessageAccountFilterPrivate;

class Q_MESSAGING_EXPORT QMessageAccountFilter
{
    friend class QMessageAccountFilterPrivate;

public:
    QMessageAccountFilter();
    QMessageAccountFilter(const QMessageAccountFilter &other);
    virtual ~QMessageAccountFilter();

    QMessageAccountFilter& operator=(const QMessageAccountFilter &other);

    void setMatchFlags(QMessageDataComparator::MatchFlags matchFlags);
    QMessageDataComparator::MatchFlags matchFlags() const;

    bool isEmpty() const;
    bool isSupported() const;

    QMessageAccountFilter operator~() const;
    QMessageAccountFilter operator&(const QMessageAccountFilter &other) const;
    QMessageAccountFilter operator|(const QMessageAccountFilter &other) const;
    const QMessageAccountFilter& operator&=(const QMessageAccountFilter &other);
    const QMessageAccountFilter& operator|=(const QMessageAccountFilter &other);

    bool operator==(const QMessageAccountFilter &other) const;
    bool operator!=(const QMessageAccountFilter &other) const;

    static QMessageAccountFilter byId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageAccountFilter byId(const QMessageAccountIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageAccountFilter byId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageAccountFilter byName(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageAccountFilter byName(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

private:
    QMessageAccountFilterPrivate *d_ptr;
};

QTM_END_NAMESPACE
#endif
