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

#include "S60mediarecognizer.h"
#if !defined(USE_CMEDIARECOGNIZER)
#include <MPMediaRecognizer.h>
#else
#include <CMediaRecognizer.h>
#endif
#include <e32def.h>
#include <e32cmn.h>
#include <QtCore/qurl.h>
#include <QtCore/qdir.h>
#include <QtCore/qdebug.h>

S60MediaRecognizer::S60MediaRecognizer(QObject *parent) : QObject(parent)
{
	TRAP_IGNORE(m_recognizer = MediaRecognizer::NewL());
}


S60MediaRecognizer::~S60MediaRecognizer()
{
	delete m_recognizer;
	m_recognizer = NULL;
}

bool S60MediaRecognizer::checkUrl(const QUrl &url)
{
	TBool isValidUrl = false;
	TPtrC validUrlPtr (static_cast<const TUint16*>(url.toString().utf16()), url.toString().length());
#if !defined(USE_CMEDIARECOGNIZER)
	isValidUrl = m_recognizer->ValidUrl(validUrlPtr);
#else
    isValidUrl = m_recogizer->IsValidUrlPrefix(validUrlPtr);
#endif
	return isValidUrl;
}

S60MediaRecognizer::MediaType S60MediaRecognizer::IdentifyMediaType(const QUrl &url)
{
#if !defined(USE_CMEDIARECOGNIZER)
	MediaRecognizer::TMPMediaType type = MediaRecognizer::EUnidentified;
#else
	MediaRecognizer::TMediaType type = MediaRecognizer::EUnidentified;
#endif
	QString filePath = QDir::toNativeSeparators(url.toLocalFile());
	if (filePath.isNull()) {
		filePath = url.toString();
	}
	TPtrC16 urlPtr(reinterpret_cast<const TUint16*>(filePath.utf16()));

	TRAP_IGNORE(type = m_recognizer->IdentifyMediaTypeL(urlPtr, ETrue);)
	m_recognizer->FreeFilehandle();

	switch (type) {
	   case MediaRecognizer::LocalAudioFile:
		   return Audio;
	   case MediaRecognizer::ELocalVideoFile:
		   return Video;
	   case MediaRecognizer::EUrl:
		   return Url;
	   case MediaRecognizer::ELocalAudioPlaylist:
	   // TODO: Must be considered when streams will be implemented
	   case MediaRecognizer::ELocalRamFile:
	   case MediaRecognizer::ELocalSdpFile:
	   // case CMPMediaRecognizer::EProgressiveDownload:
	   case MediaRecognizer::EUnidentified:
	   default:
		   break;
	}

    return NotSupported;
}
