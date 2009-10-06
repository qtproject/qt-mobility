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

#ifndef QCONTACTRELATIONSHIPFILTER_H
#define QCONTACTRELATIONSHIPFILTER_H

#include "qtcontactsglobal.h"
#include "qcontactfilter.h"

#include <QSharedDataPointer>
#include <QStringList>
#include <QList>
#include <QString>

class QContactRelationshipFilterPrivate;
class QTCONTACTS_EXPORT QContactRelationshipFilter : public QContactFilter
{
public:
    QContactRelationshipFilter();
    QContactRelationshipFilter(const QContactFilter& other);

    void setLeftIds(const QList<QUniqueId>& possibleIds);
    void setRightIds(const QList<QUniqueId>& possibleIds);
    void setLeftIds(const QUniqueId& possibleId) { setLeftIds(QList<QUniqueId>() << possibleId); }
    void setRightIds(const QUniqueId& possibleId) { setRightIds(QList<QUniqueId>() << possibleId); }

    void setLeftManagerUris(const QStringList& possibleManagerUris);
    void setRightManagerUris(const QStringList& possibleManagerUris);
    void setLeftManagerUris(const QString& possibleManagerUri) { setLeftManagerUris(QStringList(possibleManagerUri)); }
    void setRightManagerUris(const QString& possibleManagerUri) { setRightManagerUris(QStringList(possibleManagerUri)); }

    void setRelationshipTypes(const QStringList& relationshipTypes);
    void setRelationshipTypes(const QString& relationshipType) { setRelationshipTypes(QStringList(relationshipType)); }

    void setMinimumPriority(int min);
    void setMaximumPriority(int max);

    QList<QUniqueId> leftIds() const;
    QList<QUniqueId> rightIds() const;
    QStringList leftManagerUris() const;
    QStringList rightManagerUris() const;
    QStringList relationshipTypes() const;
    int minimumPriority() const;
    int maximumPriority() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactRelationshipFilter)
};

#endif
