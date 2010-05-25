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
#include <QPair>
#include <QHash>

QT_BEGIN_NAMESPACE
class QBuffer;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

// The maximum number of bytes allowed to stay in memory after being read.  The smaller this is,
// the more time spent moving bytes around.  The larger it is, the more memory is wasted.
static const int MAX_OLD_BYTES_TO_KEEP = 8192;

/*
 * An LByteArray has a subset of QByteArray's interface, plus an efficient chopLeft function
 * 
 * It stores a QByteArray internally, plus a marker of where it starts and where it ends.
 */
class Q_AUTOTEST_EXPORT LByteArray
{
public:
    LByteArray() : mStart(0), mEnd(0) {}
    explicit LByteArray(const QByteArray& d) :mData(d), mStart(0), mEnd(d.size()) {}
    bool isEmpty() const {
        return mEnd <= mStart;
    }
    char at(int i) const {
        return mData.at(mStart + i);
    }
    QByteArray toByteArray() const {
        return mData.mid(mStart, mEnd-mStart);
    }
    /* Removes \a n bytes from the start of the QByteArray. */ 
    void chopLeft(int n) {
        Q_ASSERT(size() >= n && n >= 0);
        mStart += n;
    }
    QByteArray left(int n) {
        Q_ASSERT(size() >= n && n >= 0);
        return mData.mid(mStart, n);
    }
    int indexOf(const QByteArray& needle) {
        int index = mData.indexOf(needle, mStart) - mStart;
        if (index < size())
            return index;
        return -1;
    }
    int size() const {
        return mEnd - mStart;
    }
    const char* constData() const {
        return mData.constData() + mStart;
    }
    LByteArray& operator=(const QByteArray& ba) {
        mData = ba;
        mStart = 0;
        mEnd = mData.size();
        return *this;
    }
    bool operator==(const QByteArray& ba) {
        return toByteArray() == ba;
    }
    bool operator!=(const QByteArray& ba) {
        return toByteArray() != ba;
    }

private:
    /* Clears the memory of bytes before the start marker */
    void dropOldData() {
        if (mStart > MAX_OLD_BYTES_TO_KEEP && mEnd >= mStart) {
            mData.remove(0, mStart);
            mEnd -= mStart;
            mStart = 0;
        }
    }
    QByteArray mData;
    int mStart;
    int mEnd;
    friend class LineReader;
};

class Q_AUTOTEST_EXPORT LineReader
{
public:
    LineReader(QIODevice* device, QTextCodec* codec, int chunkSize = 1000);
    LByteArray readLine();
    void pushLine(const QByteArray& line);
    int odometer();
    bool atEnd();
    QTextCodec* codec();

private:
    bool tryReadLine(LByteArray& cursor, bool atEnd);

    QIODevice* mDevice;
    QTextCodec* mCodec;
    int mChunkSize; // How many bytes to read in one go.
    QList<QByteArrayMatcher> mCrlfList;
    QByteArray mFirstLine; // Stores a line that has been "pushed" in front by pushLine
    LByteArray mBuffer;
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

    bool parseVersitDocument(LineReader& device, QVersitDocument& document);
    bool parseVersitDocumentBody(LineReader& device, QVersitDocument& document);

    QVersitProperty parseNextVersitProperty(
        QVersitDocument::VersitType versitType,
        LineReader& lineReader);

    void parseVCard21Property(
        LByteArray& text,
        QVersitProperty& property,
        LineReader& lineReader);

    void parseVCard30Property(
        LByteArray& text,
        QVersitProperty& property,
        LineReader& lineReader);

    bool setVersionFromProperty(
        QVersitDocument& document,
        const QVersitProperty& property) const;

    bool unencode(
        QVariant& value,
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
    QPair<QStringList,QString> extractPropertyGroupsAndName(LByteArray& line, QTextCodec* codec)
            const;
    QMultiHash<QString,QString> extractVCard21PropertyParams(LByteArray& line, QTextCodec* codec)
            const;
    QMultiHash<QString,QString> extractVCard30PropertyParams(LByteArray& line, QTextCodec* codec)
            const;

    // "Private" functions
    QList<QByteArray> extractParams(LByteArray& line, QTextCodec *codec) const;
    QList<QByteArray> extractParts(const QByteArray& text, const QByteArray& separator,
                                   QTextCodec *codec) const;
    QByteArray extractPart(const QByteArray& text, int startPosition, int length=-1) const;
    QString paramName(const QByteArray& parameter, QTextCodec* codec) const;
    QString paramValue(const QByteArray& parameter, QTextCodec* codec) const;
    template <class T> static bool containsAt(const T& text, const QByteArray& ba, int index);
    bool splitStructuredValue(QVersitProperty& property,
                              bool hasEscapedBackslashes) const;
    static QStringList splitValue(const QString& string,
                                  const QChar& sep,
                                  QString::SplitBehavior behaviour,
                                  bool hasEscapedBackslashes);
    static void removeBackSlashEscaping(QString& text);

public: // Data
    /* key is the document type and property name, value is the type of property it is.
       If there is no entry, assume it is a PlainType */
    QHash<QPair<QVersitDocument::VersitType,QString>, QVersitProperty::ValueType> mValueTypeMap;
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
