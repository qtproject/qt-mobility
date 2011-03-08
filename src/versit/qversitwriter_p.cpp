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

#include "qmobilityglobal.h"
#include "qversitwriter_p.h"
#include "qversitdocumentwriter_p.h"
#include "qvcard21writer_p.h"
#include "qvcard30writer_p.h"
#include "versitutils_p.h"

#include <QStringList>
#include <QMutexLocker>
#include <QScopedPointer>
#include <QTextCodec>
#include <QBuffer>

QTM_USE_NAMESPACE

/*! Constructs a writer. */
QVersitWriterPrivate::QVersitWriterPrivate()
    : mIoDevice(0),
    mState(QVersitWriter::InactiveState),
    mError(QVersitWriter::NoError),
    mIsCanceling(false),
    mDefaultCodec(0)
{
}

/*! Destroys a writer. */
QVersitWriterPrivate::~QVersitWriterPrivate()
{
}

/*! Links the signals from this to the signals of \a writer. */
void QVersitWriterPrivate::init(QVersitWriter* writer)
{
    qRegisterMetaType<QVersitWriter::State>("QVersitWriter::State");
    connect(this, SIGNAL(stateChanged(QVersitWriter::State)),
            writer, SIGNAL(stateChanged(QVersitWriter::State)), Qt::DirectConnection);
}

/*!
 * Do the actual writing and set the error and state appropriately.
 */
void QVersitWriterPrivate::write()
{
    bool canceled = false;

    // Try to get the type from the parameter to startWriting...
    QVersitDocument::VersitType type = documentType();

    foreach (const QVersitDocument& document, mInput) {
        if (isCanceling()) {
            canceled = true;
            break;
        }

        // Get type from the document if not specified in startWriting
        if (type == QVersitDocument::InvalidType)
            type = document.type();

        QScopedPointer<QVersitDocumentWriter> writer(writerForType(type, document));
        QTextCodec* codec = mDefaultCodec;
        if (codec == NULL) {
            if (type == QVersitDocument::VCard21Type) {
                codec = QTextCodec::codecForName("ISO-8859-1");
                writer->setAsciiCodec();
            } else {
                codec = QTextCodec::codecForName("UTF-8");
            }
        }
        writer->setCodec(codec);
        writer->setDevice(mIoDevice);
        if (!writer->encodeVersitDocument(document)) {
            setError(QVersitWriter::IOError);
            break;
        }
    }
    if (canceled)
        setState(QVersitWriter::CanceledState);
    else
        setState(QVersitWriter::FinishedState);
}

/*!
 * Inherited from QThread, called by QThread when the thread has been started.
 */
void QVersitWriterPrivate::run()
{
    write();
}

void QVersitWriterPrivate::setState(QVersitWriter::State state)
{
    mMutex.lock();
    mState = state;
    mMutex.unlock();
    emit stateChanged(state);
}

QVersitWriter::State QVersitWriterPrivate::state() const
{
    QMutexLocker locker(&mMutex);
    return mState;
}

void QVersitWriterPrivate::setError(QVersitWriter::Error error)
{
    QMutexLocker locker(&mMutex);
    mError = error;
}

QVersitWriter::Error QVersitWriterPrivate::error() const
{
    QMutexLocker locker(&mMutex);
    return mError;
}

/*!
 * Returns a QVersitDocumentWriter that can encode a QVersitDocument of type \a type.
 * The caller is responsible for deleting the object.
 */
QVersitDocumentWriter* QVersitWriterPrivate::writerForType(QVersitDocument::VersitType type, const QVersitDocument& document)
{
    switch (type) {
        case QVersitDocument::InvalidType:
        {
            // Neither startWriting or the document provided the type.
            // Need to infer the type from the document's componentType
            QString componentType(document.componentType());
            if (componentType == QLatin1String("VCARD")) {
                return new QVCard30Writer(QVersitDocument::VCard30Type);
            } else if (componentType == QLatin1String("VCALENDAR")
                    || componentType == QLatin1String("VEVENT")
                    || componentType == QLatin1String("VTODO")
                    || componentType == QLatin1String("VJOURNAL")
                    || componentType == QLatin1String("VTIMEZONE")
                    || componentType == QLatin1String("VALARM")) {
                return new QVCard30Writer(QVersitDocument::ICalendar20Type);
            } else {
                return new QVCard30Writer(QVersitDocument::VCard30Type);
            }
        }
        case QVersitDocument::VCard21Type:
            return new QVCard21Writer(type);
        case QVersitDocument::VCard30Type:
            return new QVCard30Writer(type);
        default:
            return new QVCard30Writer(type);
    }
}

void QVersitWriterPrivate::setCanceling(bool canceling)
{
    QMutexLocker locker(&mMutex);
    mIsCanceling = canceling;
}

bool QVersitWriterPrivate::isCanceling() const
{
    QMutexLocker locker(&mMutex);
    return mIsCanceling;
}

void QVersitWriterPrivate::setDocumentType(QVersitDocument::VersitType type)
{
    QMutexLocker locker(&mMutex);
    mType = type;
}

QVersitDocument::VersitType QVersitWriterPrivate::documentType() const
{
    QMutexLocker locker(&mMutex);
    return mType;
}

#include "moc_qversitwriter_p.cpp"
