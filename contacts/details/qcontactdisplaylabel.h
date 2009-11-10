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


#ifndef QCONTACTDISPLAYLABEL_H
#define QCONTACTDISPLAYLABEL_H

#include <QtDebug>
#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"

/* Leaf class */
class QTCONTACTS_EXPORT QContactDisplayLabel : public QContactDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldLabel;
    const char* FieldSynthesized;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactDisplayLabel, "DisplayLabel")
    Q_DECLARE_LATIN1_LITERAL(FieldLabel, "Label");
    Q_DECLARE_LATIN1_LITERAL(FieldSynthesized, "Synthesized");
#endif

    void setLabel(const QString& label)
    {
        qWarning("setLabel(): This function has been deprecated!  Display label is supposed to be READ-ONLY.  Support for mutable display labels will be removed in week 47!");
        setValue(FieldLabel, label);
        setValue(FieldSynthesized, label.isEmpty() ? true : false);
    }
    QString label() const {return value(FieldLabel);}
    void setSynthesized(bool synthesized) {qWarning("setSynthesized(): This function has been deprecated!  Display label is supposed to be READ-ONLY.  Support for mutable display labels will be removed in week 47!");setValue(FieldSynthesized, synthesized);}
    bool isSynthesized() const {qWarning("synthesized(): This function has been deprecated!  Display label is supposed to be READ-ONLY.  Support for mutable display labels will be removed in week 47!");return value<bool>(FieldSynthesized);}
};

#endif
