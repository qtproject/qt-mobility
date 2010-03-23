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
#include "qversitreader.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "versitutils_p.h"

#include <QObject>
#include <QThread>
#include <QByteArray>
#include <QIODevice>
#include <QList>
#include <QPointer>
#include <QScopedPointer>
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>

class QBuffer;

QTM_BEGIN_NAMESPACE

// The maximum number of bytes allowed to stay in memory after being read.  The smaller this is,
// the more time spent moving bytes around.  The larger it is, the more memory is wasted.
static const int MAX_OLD_BYTES_TO_KEEP = 8192;

class Q_AUTOTEST_EXPORT VersitCursor
{
public:
    VersitCursor() : position(-1), selection(-1) {}
    explicit VersitCursor(const QByteArray& d) :data(d), position(0), selection(0) {}
    QByteArray data;
    int position;
    int selection;

    void setData(const QByteArray& d) {data = d; position = selection = 0;}
    void setPosition(int pos) {position = pos; selection = qMax(pos, selection);}
    void setSelection(int pos) {selection = qMax(pos, position);}
    void dropOldData()
    {
        if (position > MAX_OLD_BYTES_TO_KEEP && selection >= position) {
            data.remove(0, position);
            selection -= position;
            position = 0;
        }
    }
};

class Q_AUTOTEST_EXPORT LineReader
{
public:
    LineReader(QIODevice* device, QTextCodec* codec, int chunkSize = 1000);
    VersitCursor readLine();
    int odometer();
    bool atEnd();
    QTextCodec* codec();

private:
    bool tryReadLine(VersitCursor& cursor, bool atEnd);

    QIODevice* mDevice;
    QTextCodec* mCodec;
    int mChunkSize; // How many bytes to read in one go.
    QList<QByteArrayMatcher> mCrlfList;
    VersitCursor mBuffer;
    int mOdometer;
    int mSearchFrom;
};

class Q_AUTOTEST_EXPORT QVersitReaderPrivate : public QThread
{
    Q_OBJECT

public: // Constructors and destructor
    QVersitReaderPrivate();
    ~QVersitReaderPrivate();
    void init(QVersitReader* reader);

signals:
    void stateChanged(QVersitReader::State state);
    void resultsAvailable();

protected: // From QThread
     void run();

public: // New functions
    void read();

    // mutexed getters and setters.
    void setState(QVersitReader::State);
    QVersitReader::State state() const;
    void setError(QVersitReader::Error);
    QVersitReader::Error error() const;
    void setCanceling(bool cancelling);
    bool isCanceling();

    bool parseVersitDocument(LineReader& device,
                             QVersitDocument& document,
                             bool foundBegin = false);

    QVersitProperty parseNextVersitProperty(
        QVersitDocument::VersitType versitType,
        LineReader& lineReader);

    void parseVCard21Property(
        VersitCursor& text,
        QVersitProperty& property,
        LineReader& lineReader);

    void parseVCard30Property(
        VersitCursor& text,
        QVersitProperty& property,
        LineReader& lineReader);

    bool setVersionFromProperty(
        QVersitDocument& document,
        const QVersitProperty& property) const;

    void unencode(
        QVariant& value,
        VersitCursor& cursor,
        QVersitProperty& property,
        QTextCodec* codec,
        LineReader& lineReader) const;

    QString decodeCharset(
        const QByteArray& value,
        QVersitProperty& property,
        QTextCodec* defaultCodec,
        QTextCodec** codec) const;

    void decodeQuotedPrintable(QString& text) const;

    /* These functions operate on a cursor describing a single line */
    QPair<QStringList,QString> extractPropertyGroupsAndName(VersitCursor& line, QTextCodec* codec)
            const;
    QByteArray extractPropertyValue(VersitCursor& line) const;
    QMultiHash<QString,QString> extractVCard21PropertyParams(VersitCursor& line, QTextCodec* codec)
            const;
    QMultiHash<QString,QString> extractVCard30PropertyParams(VersitCursor& line, QTextCodec* codec)
            const;

    // "Private" functions
    QList<QByteArray> extractParams(VersitCursor& line, QTextCodec *codec) const;
    QList<QByteArray> extractParts(const QByteArray& text, const QByteArray& separator,
                                   QTextCodec *codec) const;
    QByteArray extractPart(const QByteArray& text, int startPosition, int length=-1) const;
    QString paramName(const QByteArray& parameter, QTextCodec* codec) const;
    QString paramValue(const QByteArray& parameter, QTextCodec* codec) const;
    static bool containsAt(const QByteArray& text, const QByteArray& ba, int index);

public: // Data
    QPointer<QIODevice> mIoDevice;
    QScopedPointer<QBuffer> mInputBytes; // Holds the data set by setData()
    QList<QVersitDocument> mVersitDocuments;
    int mDocumentNestingLevel; // Depth in parsing nested Versit documents
    QTextCodec* mDefaultCodec;
    QVersitReader::State mState;
    QVersitReader::Error mError;
    bool mIsCanceling;
    mutable QMutex mMutex;
};

QTM_END_NAMESPACE

#endif // QVERSITREADER_P_H
