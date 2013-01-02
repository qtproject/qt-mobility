/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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

    static QMessageAccountFilter byName(const QString &pattern, QMessageDataComparator::LikeComparator cmp);
    static QMessageAccountFilter byName(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageAccountFilter byName(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

private:
    QMessageAccountFilterPrivate *d_ptr;
};

QTM_END_NAMESPACE
#endif
