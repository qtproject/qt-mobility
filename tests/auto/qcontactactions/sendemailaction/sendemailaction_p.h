/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QCONTACTSENDEMAILACTION_P_H
#define QCONTACTSENDEMAILACTION_P_H

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

#include "qcontactaction.h"
#include "qcontactactionfactory.h"

#include <QSharedData>
#include <QString>
#include <QVariantMap>

QTM_USE_NAMESPACE

class QContactSendEmailAction : public QContactAction
{
    Q_OBJECT

public:
    QContactSendEmailAction();
    ~QContactSendEmailAction();

    QContactActionDescriptor actionDescriptor() const;
    QVariantMap metaData() const;

    QContactFilter contactFilter(const QVariant& value) const;
    bool isDetailSupported(const QContactDetail& detail, const QContact& contact = QContact()) const;
    QList<QContactDetail> supportedDetails(const QContact& contact) const;
    bool invokeAction(const QContact& contact, const QContactDetail& detail = QContactDetail(), const QVariantMap& params = QVariantMap());
    QVariantMap results() const;
    State state() const {return QContactAction::FinishedState;}

private slots:
    void performAction();
};

class QContactSendEmailActionFactory : public QContactActionFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QContactActionFactory)

public:
    QContactSendEmailActionFactory();
    ~QContactSendEmailActionFactory();

    QString name() const;
    QList<QContactActionDescriptor> actionDescriptors() const;
    QContactAction* instance(const QContactActionDescriptor& descriptor) const;
    QVariantMap actionMetadata(const QContactActionDescriptor& descriptor) const;
};

#endif
