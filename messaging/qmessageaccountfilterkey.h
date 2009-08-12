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
#ifndef QMESSAGEACCOUNTFILTERKEY_H
#define QMESSAGEACCOUNTFILTERKEY_H
#include <qmessageglobal.h>
#include <qmessagedatacomparator.h>
#include <qmessageaccountid.h>

class QMessageAccountFilterKeyPrivate;

class Q_MESSAGING_EXPORT QMessageAccountFilterKey
{
    Q_DECLARE_PRIVATE(QMessageAccountFilterKey)

public:
    void setOptions(QMessageDataComparator::Options options);
    QMessageDataComparator::Options options() const;

    QMessageAccountFilterKey();
    QMessageAccountFilterKey(const QMessageAccountFilterKey &other);
    virtual ~QMessageAccountFilterKey();
    bool isEmpty() const;

    QMessageAccountFilterKey operator~() const;
    QMessageAccountFilterKey operator&(const QMessageAccountFilterKey &other) const;
    QMessageAccountFilterKey operator|(const QMessageAccountFilterKey &other) const;
    const QMessageAccountFilterKey& operator&=(const QMessageAccountFilterKey &other);
    const QMessageAccountFilterKey& operator|=(const QMessageAccountFilterKey &other);

    bool operator==(const QMessageAccountFilterKey &other) const;
    const QMessageAccountFilterKey& operator=(const QMessageAccountFilterKey &other);

    static QMessageAccountFilterKey id(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp = QMessageDataComparator::Equal);
    static QMessageAccountFilterKey id(const QMessageAccountIdList &ids, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);
    static QMessageAccountFilterKey id(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageAccountFilterKey fromAddress(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageAccountFilterKey fromAddress(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

    static QMessageAccountFilterKey name(const QString &value, QMessageDataComparator::EqualityComparator cmp);
    static QMessageAccountFilterKey name(const QString &value, QMessageDataComparator::InclusionComparator cmp = QMessageDataComparator::Includes);

private:
    QMessageAccountFilterKeyPrivate *d_ptr;
};
#endif
