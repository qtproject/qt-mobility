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


#ifndef QCONTACTACTION_H
#define QCONTACTACTION_H

#include "qtcontactsglobal.h"

#include "qcontactfilter.h"
#include "qcontactdetail.h"
#include "qcontact.h"

#include <QObject>

class QContactActionData;
class QTCONTACTS_EXPORT QContactAction : public QObject
{
    Q_OBJECT

public:
    virtual ~QContactAction() = 0;

    virtual QString actionName() const = 0;                                 // name of action this instance implements
    virtual QVariantMap metadata() const = 0;                               // label, icon etc
    virtual QString vendor() const = 0;                                     // vendor identification string
    virtual int implementationVersion() const = 0;                          // (minor) implementation version

    virtual QContactFilter contactFilter(const QVariant& value = QVariant()) const = 0; // use for matching
    virtual bool supportsDetail(const QContactDetail& detail) const = 0;    // whether this implementation supports the given detail
    virtual QList<QContactDetail> supportedDetails(const QContact& contact) const;
    virtual void performAction(const QContact& contact, const QContactDetail& detail = QContactDetail()) = 0;
};

#endif
