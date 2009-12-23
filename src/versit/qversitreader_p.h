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


#ifndef QVERSITREADER_P_H
#define QVERSITREADER_P_H

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

#include "qmobilityglobal.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "versitutils_p.h"

#include <QObject>
#include <QThread>
#include <QByteArray>
#include <QIODevice>
#include <QList>
#include <QTimer>
#include <QPointer>
#include <QByteArray>

QTM_BEGIN_NAMESPACE


class Q_AUTOTEST_EXPORT QVersitReaderPrivate : public QThread
{
    Q_OBJECT

public: // Constructors and destructor
    QVersitReaderPrivate(); 
    ~QVersitReaderPrivate();

public: // New functions
    bool isReady() const;
    bool read();

    bool parseVersitDocument(VersitCursor& cursor, QVersitDocument& document, bool foundBegin = false);

    QVersitProperty parseNextVersitProperty(
        QVersitDocument::VersitType versitType,
        VersitCursor& text);

    void parseVCard21Property(
        VersitCursor& text,
        QVersitProperty& property);

    void parseVCard30Property(
        VersitCursor& text,
        QVersitProperty& property);

    void parseAgentProperty(
        VersitCursor& text,
        QVersitProperty& property);

    bool setVersionFromProperty(
        QVersitDocument& document,
        const QVersitProperty& property) const;

    void unencode(
        QByteArray& value,
        VersitCursor& cursor,
        QVersitProperty& property) const;

    QString decodeCharset(
        const QByteArray& value,
        QVersitProperty& property) const;

protected: // From QThread
     void run();

public: // Data
    QPointer<QIODevice> mIoDevice;
    QList<QVersitDocument> mVersitDocuments;
    int mDocumentNestingLevel; // Depth in parsing nested Versit documents
    QByteArray mDefaultCharset;
};

QTM_END_NAMESPACE

#endif // QVERSITREADER_P_H
