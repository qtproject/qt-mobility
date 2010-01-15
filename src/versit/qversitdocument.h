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

#ifndef QVERSITDOCUMENT_H
#define QVERSITDOCUMENT_H

#include "qmobilityglobal.h"

#include <QList>
#include <QSharedDataPointer>
#include <QVariant>

class QTextCodec;

QTM_BEGIN_NAMESPACE

class QVersitDocumentPrivate;
class QVersitProperty;

class Q_VERSIT_EXPORT QVersitDocument
{
public:
    QVersitDocument();
    QVersitDocument(const QVersitDocument& other);
    ~QVersitDocument();

    QVersitDocument& operator=(const QVersitDocument& other);
    bool operator==(const QVersitDocument& other);
    bool operator!=(const QVersitDocument& other);
    
    /*! Versit document type */
    enum VersitType {
        InvalidType,
        VCard21Type,   // vCard version 2.1
        VCard30Type    // vCard version 3.0 (RFC 2426)
    };

    // XXX We use setVersitType but not addVersitProperty.. maybe just setType?
    // metadata about the versit document itself.
    void setVersitType(VersitType type);
    VersitType versitType() const;

    void addProperty(const QVersitProperty& property);
    void removeProperty(const QVersitProperty& property);
    void removeAllProperties(const QString& name);
    QList<QVersitProperty> properties() const;

    // XXX this is setDefaultCodec in QVersitProperty
    void setCodec(QTextCodec* codec);
    QTextCodec* codec(); // XXX const

    bool isEmpty() const;

private:
    
    QSharedDataPointer<QVersitDocumentPrivate> d;
};

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_NAMESPACE::QVersitDocument);

#endif // QVERSITDOCUMENT_H
