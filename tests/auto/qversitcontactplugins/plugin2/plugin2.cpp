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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtPlugin>
#include "plugin2.h"

QTM_USE_NAMESPACE

class ContactHandler2 : public QVersitContactHandler
{
public:
    void propertyProcessed(const QVersitDocument& document,
                           const QVersitProperty& property,
                           const QContact& contact,
                           bool* alreadyProcessed,
                           QList<QContactDetail>* updatedDetails)
    {
        Q_UNUSED(document)
        Q_UNUSED(property)
        Q_UNUSED(contact)
        Q_UNUSED(alreadyProcessed)
        Q_UNUSED(updatedDetails)
    }
    void documentProcessed(const QVersitDocument& document,
                           QContact* contact)
    {
        Q_UNUSED(document)
        QContactDetail detail("TestDetail");
        detail.setValue("Plugin", 2);
        contact->saveDetail(&detail);
    }
    void detailProcessed(const QContact& contact,
                         const QContactDetail& detail,
                         const QVersitDocument& document,
                         QSet<QString>* processedFields,
                         QList<QVersitProperty>* toBeRemoved,
                         QList<QVersitProperty>* toBeAdded)
    {
        Q_UNUSED(contact)
        Q_UNUSED(detail)
        Q_UNUSED(document)
        Q_UNUSED(processedFields)
        Q_UNUSED(toBeRemoved)
        Q_UNUSED(toBeAdded)
    }
    void contactProcessed(const QContact& contact,
                          QVersitDocument* document)
    {
        Q_UNUSED(contact)
        QVersitProperty property;
        property.setName("TEST-PROPERTY");
        property.setValue("2");
        document->addProperty(property);
    }
};

QString Plugin2::name() const
{
    return "com.nokia.qt.mobility.versit.plugin2";
}

int Plugin2::index() const
{
    return 4;
}

QVersitContactHandler* Plugin2::createHandler() const
{
    return new ContactHandler2;
}

QSet<QString> Plugin2::profiles() const
{
    return QSet<QString>() << "Test";
}

Q_EXPORT_PLUGIN2(versit_plugin2, Plugin2);
