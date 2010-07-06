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
#include "qmessagecontentcontainer.h"
#ifdef Q_OS_SYMBIAN
#include "qmessagecontentcontainer_symbian_p.h"
#elif defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#include "qmessagecontentcontainer_maemo_p.h"
#else
#include "qmessagecontentcontainer_p.h"
#endif

QTM_BEGIN_NAMESPACE

/*!
    \class QMessageContentContainer

    \brief The QMessageContentContainer class provides an interface for internet media (MIME) 
    and Transport Neutral Encapsulation Format (TNEF) content that is possibly only 
    partially retrieved.

    
    \inmodule QtMessaging
    
    \ingroup messaging

    A QMessageContentContainer object can either directly contain media such as text, 
    image, audio, video, application or message data, or contain multiple parts of content, 
    but can not contain both media and multiple parts directly.
    
    Container objects can be obtained via their QMessageContentContainerId 
    identifier, using the find() function of the containing QMessage object.
    
    For textual content using a recognized charset encoding textContent() will 
    return the content as a unicode string.
    
    For non-multipart content content() will return the content data after decoding any 
    transfer encoding used to represent binary data using 7-bit ASCII characters, such as 
    quoted-printable and base64.
    
    The internet media (MIME) type of the container is returned by contentType(),
    the content subtype is returned by contentSubType(), the content type charset parameter 
    is returned by contentCharset(), and the content suggested filename by suggestedFileName(). 

    The type of a container that contains multiple parts of content must be "multipart" (case 
    insensitive).  A list of identifiers for directly contained parts of content is returned 
    by contentIds().
    
    An indication of the size of the container and its contents on the originating server is 
    given by size(). If the content is entirely available on the device 
    isContentAvailable() will return true.

    Non-multipart content can be serialized to a QDataStream using 
    writeContentTo(), or to a QTextStream using writeTextContentTo().
  
    A container also stores name-value pairs known as header fields. Names are ASCII strings, 
    while values are charset encoded unicode strings.  A list of the header fields present 
    in a container is returned by headerFields(). The unicode string values associated 
    with a particular header field name are returned by headerFieldValues().

    \sa QMessage, QMessageContentContainerId
*/

/*!
    \fn QMessageContentContainer::QMessageContentContainer()
    
    Constructs an empty container object.
*/

/*!
    \fn QMessageContentContainer::QMessageContentContainer(const QMessageContentContainer &other)

    Constructs a copy of \a other.
*/

/*!
    \internal
    \fn QMessageContentContainer::operator=(const QMessageContentContainer& other)
*/

/*!
    \fn QMessageContentContainer::~QMessageContentContainer()
    
    Destroys the container object.
*/

/*!
    \fn QMessageContentContainer::contentType() const
    
    Returns the content type of the container. Common types are "text", "image", "audio", 
    "video", "application", "message" and "multipart".

    The internet media (MIME) type of the container is "multipart" if the container directly 
    contains multiple parts rather than directly contains media.

    \sa contentSubType(), contentCharset()
*/

/*!
    \fn QMessageContentContainer::contentSubType() const
    
    Returns the internet media (MIME) subtype of the content.

    \sa contentType(), contentCharset()
*/

/*!
    \fn QMessageContentContainer::contentCharset() const
    
    Returns the internet media (MIME) content charset, when defined; 
    otherwise an empty array is returned.

    \sa contentType(), contentSubType()
*/

/*!
    \fn QMessageContentContainer::suggestedFileName() const
    
    Returns the suggested filename for the attachment, when defined;
    otherwise an empty array is returned.

    The default is an empty array.
*/

/*!
    \fn QMessageContentContainer::isContentAvailable() const
    
    Returns true if the entirety of the content contained is available on the device; 
    otherwise returns false.
*/

/*!
    \fn QMessageContentContainer::size() const
    
    If the size of the container is known then returns an indication of the size of the container 
    on the originating server, including contents; otherwise returns 0.
*/

/*!
    \fn QMessageContentContainer::textContent() const
    
    For textual content encoded with a recognized charset, returns the content as a unicode string; 
    otherwise a null string is returned.
*/

/*!
    \fn QMessageContentContainer::content() const
    
    Return the content after decoding any transfer encoding used to represent binary data 
    using 7-bit ASCII characters, such as quoted-printable and base64.
  
    For textual content any text charset encoding such as Shift-JIS, ISO 2022-JP, KOI8-R, 
    Windows-1251 etc will not be decoded.

    \sa textContent()
*/

/*!
    \fn QMessageContentContainer::writeTextContent(QTextStream& out) const
    
    For a non-multipart container, writes the content as returned by textContent() 
    to the stream \a out; otherwise does nothing.
*/

/*!
    \fn QMessageContentContainer::writeContent(QDataStream& out) const
    
    For a non-multipart container, writes the content as returned by content() 
    to the stream \a out; otherwise does nothing.
*/

/*!
    \fn QMessageContentContainer::contentIds() const
    
    For a multipart container returns a list of identifiers for all content directly contained by 
    the container; otherwise returns an empty list.

    \sa find(), contains()
*/

/*!
    \fn QMessageContentContainer::find(const QMessageContentContainerId &id) const
    
    If the container contains another container with identifier \a id either directly or 
    recursively, then returns the value of that other container; otherwise returns an 
    empty container constructed with the default constructor.

    \sa contains(), contentIds()
*/

/*!
    \fn bool QMessageContentContainer::contains(const QMessageContentContainerId &id) const;
    
    If the container contains content with the identifier \a id, either directly or recursively 
    then returns true; otherwise returns false.

    \sa find(), contentIds()
*/

/*!
    \fn QMessageContentContainer::headerFieldValue(const QByteArray &name) const
    
    Returns the value of the first header field of the container with the name \a name, if any;
    otherwise returns a null string.

    \sa headerFields(), headerFieldValues()
*/

/*!
    \fn QMessageContentContainer::headerFieldValues(const QByteArray &name) const
    
    Returns a list of values of header fields with the name \a name, if any;
    otherwise returns an empty list.

    \sa headerFields(), headerFieldValue()
*/

/*!
    \fn QMessageContentContainer::headerFields() const
    
    Returns a list of names of header fields of the container.

    \sa headerFieldValue(), headerFieldValues()
*/

QTM_END_NAMESPACE
