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


#ifndef QCONTACTREQUEST_H
#define QCONTACTREQUEST_H

#include "qcontactabstractrequest.h"
#include "qcontactabstractrequestresult.h"

#include "qcontactsortorder.h"
#include "qcontactfilter.h"
#include "qcontact.h"
#include "qcontactmanager.h"

#include <QSharedData>
#include <QString>
#include <QList>
#include <QStringList>

class QContactRequestData;
class QTCONTACTS_EXPORT QContactRequest : public QContactAbstractRequest
{
    Q_OBJECT

public:
    QContactRequest();
    ~QContactRequest();

    /* First, the base class functionality */
    bool isFinished() const;
    QContactManager::Error error() const;
    QContactAbstractRequest::Status status() const;

    QContactAbstractRequest::Type type() const {return QContactAbstractRequest::Contact;}

    /* Filtering */
    enum SelectionType {
        SelectAll,
        SelectByIds,
        SelectByObject,
        SelectByFilter
    };
    void clearSelection();
    void selectById(const QList<QUniqueId>& contactIds);
    void selectByObject(const QList<QContact>& contacts);
    void selectByFilter(const QContactFilter& filter);

    SelectionType selectionType() const;
    QList<QUniqueId> idSelection() const;
    QList<QContact> contactSelection() const;
    QContactFilter filterSelection() const;

    /* Sorting */
    void setSortOrder(const QContactSortOrder& order);
    QContactSortOrder sortOrder() const;

    /* What to get/store */
    enum Restriction {
        NoRestriction,
        RestrictToIds,
        RestrictToDetails
    };

    void clearRestrictions();                                // full contact
    void restrictToIds();                                    // just ids
    void restrictToDetails(const QStringList& detailDefinitionIds); // partial contact
    Restriction restriction();
    QStringList detailRestrictions() const;

public slots:
    /* Base class slots */
    bool waitForFinished(int msecs = 0);
    bool waitForProgress(int msecs = 0);
    void start(QContactManager *manager, QContactAbstractRequest::Operation operation);
    void cancel();

public:
    /* Results */
    QList<QUniqueId> ids();                                  // if restrictToIds is set
    QList<QContact> contacts();                              // otherwise (full and partial contacts)
    QList<QContactManager::Error> errors();

signals:
    void progress(QContactRequest* self, bool finished, bool appendOnly);   // appendOnly true if ids/contacts order has not changed, just stuff added

private:
    Q_DISABLE_COPY(QContactRequest);
    friend class QContactAbstractRequestResult;
    QContactRequestData* d;
};

#endif
