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

#ifndef QCONTACTREQUESTS_P_H
#define QCONTACTREQUESTS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qcontactabstractrequest_p.h"
#include "qcontactfilter.h"
#include "qcontactsortorder.h"
#include "qcontact.h"
#include "qcontactgroup.h"
#include "qtcontactsglobal.h"

#include <QStringList>
#include <QList>

class QContactSaveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactSaveRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactSaveRequestPrivate()
    {
    }

    QContactAbstractRequest::Type type() const
    {
        return QContactAbstractRequest::ContactSave;
    }

    QList<QContact> m_contacts;
};

class QContactFetchRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactFetchRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactFetchRequestPrivate()
    {
    }

    QContactAbstractRequest::Type type() const
    {
        return QContactAbstractRequest::ContactFetch;
    }

    QContactFilter m_filter;
    QList<QContactSortOrder> m_sorting;
    QStringList m_definitionRestrictions;

    QList<QContact> m_contacts;
};

class QContactRemoveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactRemoveRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactRemoveRequestPrivate()
    {
    }

    QContactAbstractRequest::Type type() const
    {
        return QContactAbstractRequest::ContactRemove;
    }

    QContactFilter m_filter;
};

class QContactIdFetchRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactIdFetchRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactIdFetchRequestPrivate()
    {
    }

    QContactAbstractRequest::Type type() const
    {
        return QContactAbstractRequest::ContactIdFetch;
    }

    QContactFilter m_filter;
    QList<QContactSortOrder> m_sorting;

    QList<QUniqueId> m_ids;
};

class QContactGroupFetchRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactGroupFetchRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactGroupFetchRequestPrivate()
    {
    }

    QContactAbstractRequest::Type type() const
    {
        return QContactAbstractRequest::GroupFetch;
    }

    QList<QUniqueId> m_ids;
    QList<QContactGroup> m_groups;
};

class QContactGroupSaveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactGroupSaveRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactGroupSaveRequestPrivate()
    {
    }

    QContactAbstractRequest::Type type() const
    {
        return QContactAbstractRequest::GroupSave;
    }

    QList<QContactGroup> m_groups;

};

class QContactGroupRemoveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactGroupRemoveRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactGroupRemoveRequestPrivate()
    {
    }

    QContactAbstractRequest::Type type() const
    {
        return QContactAbstractRequest::GroupRemove;
    }

    QList<QUniqueId> m_ids;
};

class QContactDetailDefinitionFetchRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactDetailDefinitionFetchRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactDetailDefinitionFetchRequestPrivate()
    {
    }

    QContactAbstractRequest::Type type() const
    {
        return QContactAbstractRequest::DetailDefinitionFetch;
    }

    QStringList m_names;
    QMap<QString, QContactDetailDefinition> m_definitions;
};

class QContactDetailDefinitionSaveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactDetailDefinitionSaveRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactDetailDefinitionSaveRequestPrivate()
    {
    }

    QContactAbstractRequest::Type type() const
    {
        return QContactAbstractRequest::DetailDefinitionSave;
    }

    QList<QContactDetailDefinition> m_definitions;
};

class QContactDetailDefinitionRemoveRequestPrivate : public QContactAbstractRequestPrivate
{
public:
    QContactDetailDefinitionRemoveRequestPrivate()
        : QContactAbstractRequestPrivate()
    {
    }

    ~QContactDetailDefinitionRemoveRequestPrivate()
    {
    }

    QContactAbstractRequest::Type type() const
    {
        return QContactAbstractRequest::DetailDefinitionRemove;
    }

    QStringList m_names;
};

#endif
