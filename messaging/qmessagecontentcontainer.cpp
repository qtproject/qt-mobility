/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagecontentcontainer.h"
#include "qmessagecontentcontainer_p.h"

/*!
    \class QMessageContentContainer
    \preliminary

    \brief The QMessageContentContainer class provides an interface for internet media (MIME) 
    type content that is possibly only partially retrieved.

    \ingroup messaging

    A QMessageContentContainer object can either directly contain media such as text, 
    image, audio, video, application or message data, or contain multiple parts of content, 
    but can not contain both media and multiple parts directly.
    
    Container objects can be constructed via their QMessageContentContainerId 
    identifier using the find() function of the parent QMessage, or constructed piece by 
    piece using setContentType(), setContent(), setHeaderField() and related functions.
    
    For textual content using a recognized charset encoding textContent() will 
    return the content as a unicode string.
    
    For non-multipart content content() will return the content after decoding any 
    transfer encoding used to represent binary data using 7-bit ASCII characters, such as 
    quoted-printable and base64.
    
    contentFileName() will return the suggested filename for an attachment, when 
    defined.

    The internet media (MIME) type of the container is returned by contentType(),
    the content subtype is returned by contentSubType(), the content type charset parameter 
    is returned by contentCharset(), and the content suggested filename by contentFileName(). The 
    type of a container that contains multiple parts of content must be "multipart" (case 
    insensitive).
    
    An indication of the size of the container and its contents on the originating server is 
    given by indicativeSize(). If the content is entirely available on the device 
    isContentAvailable() will return true.

    Non-multipart content can be serialized to a QDataStream using 
    writeContentTo(), and set using setContent() or setContentFromFile().
  
    A part of content can be appended to the existing content of a container using 
    appendContent(). Existing content may be replaced using replaceContent(). A list of 
    identifiers for directly contained parts of content is returned by contentIds().

    The default container contains an empty string, with type "text", subtype "plain", 
    and charset "US-ASCII".

    clearContents() will remove any existing parts, and set the content of the container 
    to be the default content.

    A container stores name value pairs known as header fields. Names are 
    ASCII strings, while values are charset encoded unicode strings. A header field may be 
    appended using appendHeaderField(), an existing header field may be 
    replaced using setHeaderField(). A list of header fields is returned by headerFields(). The 
    unicode string value of a header field is returned by headerField().

    If the container has been modified since it was last constructed containerDataModified() 
    returns true.

    \sa QMessage, QMessageContentContainerId
*/

/*!
    \fn QMessageContentContainer::QMessageContentContainer()
    
    Constructs an empty container object.
*/

/*!
    \fn QMessageContentContainer::QMessageContentContainer(const QMessageContentContainer &other)

    Constructs a copy of \a other.
    
    \sa operator=()
*/

/*!
    \fn QMessageContentContainer::operator=(const QMessageContentContainer& other)
  
    Assign the value of the QMessageContentContainer \a other to this.
*/

/*!
    \fn QMessageContentContainer::~QMessageContentContainer()
    
    Destroys the container object.
*/

/*!
    \fn QMessageContentContainer::containerId() const
    
    Returns the identifier of the container object.
*/

/*!
    \fn QMessageContentContainer::messageId() const

    Returns the identifier of the containing (parent) message if any; otherwise returns an invalid 
    message identifier.
*/

/*!
    \fn QMessageContentContainer::setContentType(const QByteArray &data)
    
    Clears all existing content, including all parts, using clearContents() and sets the content 
    type of the container to \a data.

    The content subtype, content charset and suggested content file name of the container 
    will be set to the default values for type \a data.

    \sa clearContents(), contentType(), contentSubType(), contentCharset(), contentFileName(), setHeaderField()
*/

/*!
    \fn QMessageContentContainer::contentType() const
    
    Returns the content type of the container. Common types are "text", "image", "audio", 
    "video", "application", "message" and "multipart".

    The internet media (MIME) type of the container is "multipart" iff the container directly 
    contains multiple parts rather than directly contains media.

    The default is "text".

    \sa setContentType()
*/

/*!
    \fn QMessageContentContainer::setContentSubType(const QByteArray &data)
    
    Sets the internet media (MIME) content subtype of the content to \a data.

    \sa contentSubType()
*/

/*!
    \fn QMessageContentContainer::contentSubType() const
    
    Returns the internet media (MIME) subtype of the content.

    The default is "plain" for "text" type media content, "mixed" for "multipart" type content, 
    and an empty array for other types of content.

    \sa setContentSubType()
*/

/*!
    \fn QMessageContentContainer::setContentCharset(const QByteArray &data)
    
    Sets the internet media (MIME) content charset to \a data.

    \sa contentCharset()
*/

/*!
    \fn QMessageContentContainer::contentCharset() const
    
    Returns the internet media (MIME) content charset, when defined; otherwise an empty array is 
    returned.

    The default is "US-ASCII" for "text" type content; and an empty array for other types.

    \sa setContentCharset()
*/

/*!
    \fn QMessageContentContainer::setContentFileName(const QByteArray &data)
    
    Sets the suggested filename of the content to \a data.

    \sa contentFileName()
*/

/*!
    \fn QMessageContentContainer::contentFileName() const
    
    Returns the suggested filename of the attachment, when defined;
    otherwise an empty array is returned.

    The default is an empty array.

    \sa setContentFileName()
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
    \fn QMessageContentContainer::writeTextContentTo(QTextStream& out) const
    
    For a non-multipart container, writes the content as returned by textContent() 
    to the stream \a out; otherwise does nothing.
*/

/*!
    \fn QMessageContentContainer::writeContentTo(QDataStream& out) const
    
    For a non-multipart container, writes the content as returned by content() 
    to the stream \a out; otherwise does nothing.
*/

/*!
    \fn QMessageContentContainer::clearContents()
    
    Clears existing content either media or parts, and resets the content type, subtype and 
    charset to default values for text content. 

    Does not modify header fields other than "Content-Type".

    \sa contentType(), contentSubType(), contentCharset()
*/

/*!
    \fn QMessageContentContainer::setContent(const QString &text)
    
    Sets the content to \a text, content type to "text", and charset to the first charset 
    returned by preferredCharsets() that can encode \a text if any; otherwise sets the charset 
    to "UTF-8".

    Does not modify the content subtype, the subtype should be set separately.

    \sa setContentSubType()
*/

/*!
    \fn QMessageContentContainer::setContent(const QByteArray &data)
    
    Sets the content to \a data.

    Does not modify the content type, subtype or charset, they should be set separately.

    \sa setContentType(), setContentSubType(), setContentCharset()

*/

/*!
    \fn QMessageContentContainer::setContentFromFile(const QString &fileName)
    
    For a multipart container does nothing; otherwise sets the content of the container to be the 
    content of the file \a fileName.

    Does not modify the content type, subtype or charset, they should be set separately.

    \sa setContentType(), setContentSubType(), setContentCharset()
*/

/*!
    \fn QMessageContentContainer::readContentFrom(QDataStream &in)
    
    For a multipart container does nothing; otherwise sets the content of the container by 
    reading from the stream \a in.

    Does not modify the content type, subtype or charset, they should be set separately.

    \sa writeContentTo(), setContentType(), setContentSubType(), setContentCharset()
*/

/*!
    \fn QMessageContentContainer::appendContent(const QMessageContentContainer & content)
    
    Appends \a content to the end of the list of content contained.

    For a non-multipart container, before a part is appended the content type of the 
    container is set to "multipart" and the contents of the container cleared with clearContents().

    Returns an identifier for the appended content.

    \sa find(), clearContents(), replaceContent(), contentIds()
*/

/*!
    \fn QMessageContentContainer::replaceContent(const QMessageContentContainerId &id, const QMessageContentContainer & content)
    
    If the container contains content with the identifier \a id, either directly or recursively 
    then replaces that content with \a content; otherwise does nothing.

    \sa find(), clearContents(), appendContent(), contentIds()
*/

/*!
    \fn QMessageContentContainer::contentIds() const
    
    For a multipart container returns a list of identifiers for all content directly contained by 
    the container; otherwise returns an empty list.

    \sa find(), clearContents(), appendContent(), replaceContent()
*/

/*!
    \fn QMessageContentContainer::find(const QMessageContentContainerId &id) const
    
    If the container contains another container with identifier \a id either directly or 
    recursively, then returns the value of that other container; otherwise returns an 
    empty container constructed with the default constructor.

    \sa contains(), contentIds(), clearContents(), appendContent(), replaceContent()
*/

/*!
    \fn bool QMessageContentContainer::contains(const QMessageContentContainerId &id) const;
    
    If the container contains content with the identifier \a id, either directly or recursively 
    then returns true; otherwise returns false.
*/

/*!
    \fn QMessageContentContainer::appendHeaderField(const QByteArray &name, const QString &value)
    
    Append a header field with name \a name and value \a value to the end of the list of 
    header fields for the container. Any existing header field with the same name is not 
    modified.

    If \a value is not ASCII text then it will be encoded by the first charset returned by 
    preferredCharsets() that can encode \a value if any; otherwise \a value will be encoded using 
    "UTF-8".

    \sa setHeaderField(), headerField(), headerFields()
*/

/*!
    \fn QMessageContentContainer::setHeaderField(const QByteArray &name, const QString &value)
    
    Sets the value of the first header field of the container with name \a name to \a value if it 
    already exists; otherwise appends a header with the supplied name and value.

    If \a value is not ASCII text then it will be encoded by the first charset returned by 
    preferredCharsets() that can encode \a value if any; otherwise \a value will be encoded using 
    "UTF-8".

    The type, subtype and charset of the container are stored in the 'Content-Type' header
    field, the transfer encoding, such as quoted-printable, is stored in the 
    'Content-Transfer-Encoding' header field, the filename of the container is stored in
    the 'Content-Disposition' header field. These header fields should not be modified using
    the setHeaderField() function and doing so may result in undefined behavior.

    \sa appendHeaderField(), headerField(), headerFields()
*/

/*!
    \fn QMessageContentContainer::headerField(const QByteArray &name) const
    
    Returns the value of the first header field of the container with the name \a name, if any;
    otherwise returns a null string.

    \sa headerFieldValues(), appendHeaderField(), setHeaderField(), headerFields()
*/

/*!
    \fn QMessageContentContainer::headerFieldValues(const QByteArray &name) const
    
    Returns a list of values of header fields with the name \a name, if any;
    otherwise returns an empty list.

    \sa appendHeaderField(), setHeaderField(), headerField(), headerFields()
*/

/*!
    \fn QMessageContentContainer::headerFields() const
    
    Returns a list of names of header fields of the container.

    \sa appendHeaderField(), setHeaderField(), headerField()
*/

/*!
    \fn QMessageContentContainer::appendHeaderField(const QByteArray &name, const QByteArray &value)
    
    Append a header field with name \a name and value \a value to the end of the list of 
    header fields for the container. Any existing header field with the same name is not 
    modified.

    \sa setHeaderField(), headerField(), headerFields()
*/

/*!
    \fn QMessageContentContainer::setHeaderField(const QByteArray &name, const QByteArray &value)
    
    Sets the value of the first header field of the container with name \a name to \a value if it 
    already exists; otherwise appends a header with the supplied name and value.

    \sa appendHeaderField(), headerField(), headerFields()
*/

/*!
    \fn QMessageContentContainer::containerDataModified() const
    
    Returns true if the container has been modified since it was constructed; 
    otherwise returns false.
*/

/*!
    \fn QMessageContentContainer::prependContent(const QMessageContentContainer & content)
    
    Prepend \a content to the start of the list of content contained.

    For a non-multipart container, before a part is prepended the content type of the 
    container is set to "multipart" and the contents of the container cleared with clearContents().

    Returns an identifier for the prepended content.
*/

/*!
    \fn QMessageContentContainer::removeContent(const QMessageContentContainerId &id)
    
    If the container contains content with the identifier \a id, either directly or recursively 
    then removes that content element.

    \sa find(), clearContents(), contentIds()
*/

