/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QVERSITDOCUMENTWRITER_H
#define QVERSITDOCUMENTWRITER_H

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

#include "qversitproperty.h"
#include <QByteArray>
#include <QMultiHash>

class QIODevice;
class QTextCodec;
class QTextEncoder;

QTM_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QVersitDocumentWriter
{
public:
    QVersitDocumentWriter(const QByteArray& documentType, const QByteArray& version);
    virtual ~QVersitDocumentWriter();

    void setCodec(QTextCodec* codec);
    void setDevice(QIODevice* device);

    virtual void encodeVersitProperty(const QVersitProperty& property) = 0;
    virtual void encodeParameters(const QMultiHash<QString,QString>& parameters) = 0;
    void encodeVersitDocument(const QVersitDocument& document);
    void encodeGroupsAndName(const QVersitProperty& property);

    void writeString(const QString& string, bool useUtf8 = false);
    void writeCrlf();

    QByteArray mDocumentType;
    QByteArray mVersion;
    QIODevice* mDevice;
    QTextCodec* mCodec;
    QTextEncoder* mEncoder;
    QTextEncoder* mUtf8Encoder;
    bool mSuccessful;
    int mCurrentLineLength;
};

QTM_END_NAMESPACE

#endif // QVERSITDOCUMENTWRITER_H
