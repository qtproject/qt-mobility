/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QUrl>
#include <QtCore/QDir>
#include "s60mediaplayerutils.h"
#include "s60mediarecognizer.h"
#include "s60mmtrace.h"
#include <e32def.h>
#include <e32cmn.h>
#include <apgcli.h>

static const TInt KMimeTypePrefixLength = 6; // "audio/" or "video/"

_LIT(KMimeTypePrefixAudio, "audio/");
_LIT(KMimeTypePrefixVideo, "video/");
_LIT(KMimeTypeRingingTone, "application/vnd.nokia.ringing-tone");

S60MediaRecognizer::S60MediaRecognizer(QObject *parent)
    : QObject(parent)
{
    TRACE("S60MediaRecognizer::S60MediaRecognizer" << qtThisPtr());
}

S60MediaRecognizer::~S60MediaRecognizer()
{
    TRACE("S60MediaRecognizer::~S60MediaRecognizer" << qtThisPtr());
    m_file.Close();
    m_fileServer.Close();
    m_recognizer.Close();
}

S60MediaRecognizer::MediaType S60MediaRecognizer::mediaType(const QUrl &url)
{
    S60MediaRecognizer::MediaType result;
    if (url.scheme() == "file")
        result = identifyMediaType(QDir::cleanPath(url.toLocalFile()));
    else
        result = Url;
    TRACE("S60MediaRecognizer::mediaType" << qtThisPtr()
          << "url" << url.toString() << "result" << result);
    return result;
}

S60MediaRecognizer::MediaType S60MediaRecognizer::identifyMediaType(const QString &fileName)
{
    S60MediaRecognizer::MediaType result = NotSupported;
    bool recognizerOpened = false;
    TInt err = m_recognizer.Connect();
    if (err == KErrNone)
        recognizerOpened = true;
    err = m_fileServer.Connect();
    if (err == KErrNone)
        recognizerOpened = true;
    // This is needed for sharing file handles for the recognizer
    err = m_fileServer.ShareProtected();
    if (err == KErrNone)
        recognizerOpened = true;
    if (recognizerOpened) {
        m_file.Close();
        err = m_file.Open(m_fileServer, QString2TPtrC(QDir::toNativeSeparators(fileName)),
                          EFileRead | EFileShareReadersOnly);
        if (err == KErrNone) {
            TDataRecognitionResult recognizerResult;
            err = m_recognizer.RecognizeData(m_file, recognizerResult);
            if (err == KErrNone) {
                const TPtrC mimeType = recognizerResult.iDataType.Des();
                if (mimeType.Left(KMimeTypePrefixLength).Compare(KMimeTypePrefixAudio) == 0
                    || mimeType.Compare(KMimeTypeRingingTone) == 0)
                    result = Audio;
                else if (mimeType.Left(KMimeTypePrefixLength).Compare(KMimeTypePrefixVideo) == 0)
                    result = Video;
            }
        }
    }
    return result;
}
