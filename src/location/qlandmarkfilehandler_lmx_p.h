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

#ifndef QLANDMARKFILEHANDLER_LMX_P_H
#define QLANDMARKFILEHANDLER_LMX_P_H

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

#include "qlandmark.h"

class QXmlStreamReader;
class QXmlStreamWriter;

class QIODevice;

QTM_BEGIN_NAMESPACE

class QLandmarkManagerEngine;

class Q_AUTOTEST_EXPORT QLandmarkFileHandlerLmx : public QObject
{
    Q_OBJECT

public:
    QLandmarkFileHandlerLmx();
    ~QLandmarkFileHandlerLmx();

    QList<QLandmark> landmarks() const;
    void setLandmarks(const QList<QLandmark> &landmarks);

    bool importData(QIODevice *device);
    bool exportData(QIODevice *device, const QString &nsPrefix = QString());

    QString errorString() const;

signals:
    void error(const QString &error);
    void finishedImport();
    void finishedExport();

private:
    bool readLmx();
    bool readLandmarkCollection(QList<QLandmark> &landmarkCollection);
    bool readLandmark(QLandmark &landmark);
    bool readCoordinates(QLandmark &landmark);
    bool readAddressInfo(QLandmark &landmark);
    bool readMediaLink(QLandmark &landmark);
    bool readCategory(QLandmarkCategoryId &categoryId);

    bool writeLmx();
    bool writeLandmarkCollection(const QList<QLandmark> &landmarkCollection);
    bool writeLandmark(const QLandmark &landmark);
    bool writeCoordinates(const QLandmark &landmark);
    bool writeAddressInfo(const QLandmark &landmark);
    bool writeMediaLink(const QLandmark &landmark);
    bool writeCategory(const QLandmarkCategoryId &categoryId);

    QString m_ns;
    QString m_nsPrefix;

    QList<QLandmark> m_landmarks;

    QXmlStreamReader *m_reader;
    QXmlStreamWriter *m_writer;

    QString m_error;
};

QTM_END_NAMESPACE

#endif // #ifndef QLANDMARKFILEHANDLER_LMX_P_H
