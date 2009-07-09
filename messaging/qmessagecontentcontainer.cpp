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
** contact Nokia at http://www.qtsoftware.com/contact.
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
    identifier using the container() function of the parent QMessage, or constructed piece by 
    piece using setContentType(), setContent(), setHeaderField() and related functions.
    
    For textual content using a recognized charset encoding decodedTextContent() will 
    return the content as a unicode string.
    
    For non multipart content decodedContent() will return the content after decoding any 
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
    given by indicativeSize(). If the content is completely available on the device 
    contentAvailable() will return true.

    decodedContentFileName() will return the name of a local file containing the content, when 
    available. Non multipart content can be serialized to a QDataStream using 
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

    A list of charset names is returned by preferredCharsets(). When encoding unicode QString text 
    using setContent(), appendHeaderField() or setHeaderField() the first charset in the list 
    returned by preferredCharsets() that is capable of encoding  all characters in the given 
    unicode QString text will be used, or if none is capable UTF-8 will be used.
    
    If the container has been modified since it was last constructed containerDataModified() 
    returns true.

    \sa QMessage, QMessageContentContainerId
*/

/*!
    Constructs an empty container object.
*/
QMessageContentContainer::QMessageContentContainer()
{
}

/*!
    Destroys the container object.
*/
QMessageContentContainer::~QMessageContentContainer()
{
}

/*!
      Returns the identifier of the container object.
*/
QMessageContentContainerId QMessageContentContainer::containerId() const
{
    return QMessageContentContainerId();
}

/*!
      Sets the identifier of the container object to \a id.
*/
void QMessageContentContainer::setContainerId(const QMessageContentContainerId &id)
{
    Q_UNUSED(id)
}

/*!
    \fn QMessageId QMessageContentContainer::messageId() const

    Returns the identifier of the containing (parent) message if any; otherwise returns an invalid 
    message identifier.
*/
QMessageId QMessageContentContainer::messageId() const
{
    return QMessageId(); // stub
}

/*!
    Clears all existing content, including all parts, using clearContents() and sets the content 
    type of the container to \a data.

    The content subtype, content charset and suggested content file name of the container 
    will be set to the default values for type \a data.

    \sa clearContents(), contentType(), contentSubType(), contentCharset(), contentFileName(), setHeaderField()
*/
void QMessageContentContainer::setContentType(const QByteArray &data)
{
    Q_UNUSED(data)
}

/*!
    Returns the content type of the container. Common types are "text", "image", "audio", 
    "video", "application", "message" and "multipart".

    The internet media (MIME) type of the container is "multipart" iff the container directly 
    contains multiple parts rather than directly contains media.

    The default is "text".

    \sa setContentType()
*/
QByteArray QMessageContentContainer::contentType() const
{
    return QByteArray(); // stub
}

/*!
    Sets the internet media (MIME) content subtype of the content to \a data.

    \sa contentSubType()
*/
void QMessageContentContainer::setContentSubType(const QByteArray &data)
{
    Q_UNUSED(data);
}

/*!
    Returns the internet media (MIME) subtype of the content.

    The default is "plain" for "text" type media content, "mixed" for "multipart" type content, 
    and an empty array for other types of content.

    \sa setContentSubType()
*/
QByteArray QMessageContentContainer::contentSubType() const
{
    return QByteArray(); // stub
}

/*!
    Sets the internet media (MIME) content charset to \a data.

    \sa contentCharset()
*/
void QMessageContentContainer::setContentCharset(const QByteArray &data)
{
    Q_UNUSED(data)
}

/*!
    Returns the internet media (MIME) content charset, when defined; otherwise an empty array is 
    returned.

    The default is "US-ASCII" for "text" type content; and an empty array for other types.

    \sa setContentCharset()
*/
QByteArray QMessageContentContainer::contentCharset() const
{
    return QByteArray(); // stub
}

/*!
    Sets the suggested filename of the content to \a data.

    \sa contentFileName()
*/
void QMessageContentContainer::setContentFileName(const QByteArray &data)
{
    Q_UNUSED(data)
}

/*!
    Returns the suggested filename of the attachment, when defined;
    otherwise an empty array is returned.

    The default is an empty array.

    \sa setContentFileName()
*/
QByteArray QMessageContentContainer::contentFileName() const
{
    return QByteArray(); // stub
}

/*!
    Sets the content availability of the content contained to \a available.

    \sa contentAvailable()
*/
void QMessageContentContainer::setContentAvailable(bool available)
{
    Q_UNUSED(available)
}

/*!
    Returns true if the entirety of the content contained is available on the device; 
    otherwise returns false.
*/
bool QMessageContentContainer::contentAvailable() const
{
    return false;
}

/*!
    Set the size indication of the container including contents to \a size.

    \sa indicativeSize()
*/
void QMessageContentContainer::setIndicativeSize(uint size)
{
    Q_UNUSED(size)
}

/*!
    Returns an indication of the size of the container including contents.
*/
uint QMessageContentContainer::indicativeSize() const
{
    return 0;
}

/*!
    For textual content encoded with a recognized charset decodedTextContent() will return 
    the content as a unicode string; otherwise a null string is returned.
    
    \sa preferredCharsets()
*/
QString QMessageContentContainer::decodedTextContent() const
{
    return QString::null;
}

/*!
    Return the content after decoding any transfer encoding used to represent binary data 
    using 7-bit ASCII characters, such as quoted-printable and base64.
  
    For textual content any text charset encoding such as Shift-JIS, ISO 2022-JP, KOI8-R, 
    Windows-1251 etc will not be decoded.

    \sa decodedTextContent()
*/
QByteArray QMessageContentContainer::decodedContent() const
{
    return QByteArray(); // stub
}

/*!
    Returns the name of a local file containing the content, when available.
*/
QString QMessageContentContainer::decodedContentFileName() const
{
    return QString(); // stub
}

/*!
    For a non multipart container writes the content as would be returned by decodedContent() 
    into the stream \a out; otherwise for a multipart container does nothing.

    \sa readContentFrom()
*/
void QMessageContentContainer::writeContentTo(QDataStream& out) const
{
    Q_UNUSED(out)
}

/*!
    Clears existing content either media or parts, and resets the content type, subtype and 
    charset to default values for text content. 

    Does not modify header fields other than "Content-Type".

    \sa contentType(), contentSubType(), contentCharset()
*/
void QMessageContentContainer::clearContents()
{
}

/*!
    Sets the content to \a text, content type to "text", and charset to the first charset 
    returned by preferredCharsets() that can encode \a text if any; otherwise sets the charset 
    to "UTF-8".

    Does not modify the content subtype, the subtype should be set separately.

    \sa preferredCharsets(), setContentSubType()
*/
void QMessageContentContainer::setContent(const QString &text)
{
    Q_UNUSED(text)
}

/*!
    Sets the content to \a data.

    Does not modify the content type, subtype or charset, they should be set separately.

    \sa setContentType(), setContentSubType(), setContentCharset()

*/
void QMessageContentContainer::setContent(const QByteArray &data)
{
    Q_UNUSED(data)
}

/*!
    For a multipart container does nothing; otherwise sets the content of the container to be the 
    content of the file \a fileName.

    Does not modify the content type, subtype or charset, they should be set separately.

    \sa setContentType(), setContentSubType(), setContentCharset()
*/
void QMessageContentContainer::setContentFromFile(const QString &fileName)
{
    Q_UNUSED(fileName)
}

/*!
    For a multipart container does nothing; otherwise sets the content of the container by 
    reading from the stream \a in.

    Does not modify the content type, subtype or charset, they should be set separately.

    \sa writeContentTo(), setContentType(), setContentSubType(), setContentCharset()
*/
void QMessageContentContainer::readContentFrom(QDataStream &in)
{
    Q_UNUSED(in)
}

/*!
    Appends \a content to the end of the list of content contained.

    For a non multipart container, before a part is appended the content type of the 
    container is set to "multipart" and the contents of the container cleared with clearContents().

    Returns an identifier for the appended content.

    \sa container(), clearContents(), replaceContent(), contentIds()
*/
QMessageContentContainerId QMessageContentContainer::appendContent(const QMessageContentContainer & content)
{
    Q_UNUSED(content)
    return QMessageContentContainerId(); // stub
}

/*!
    If the container contains content with the identifier \a id, either directly or recursively 
    then replaces that content with \a content; otherwise does nothing.

    \sa container(), clearContents(), appendContent(), contentIds()
*/
void QMessageContentContainer::replaceContent(const QMessageContentContainerId &id, const QMessageContentContainer & content)
{
    Q_UNUSED(id)
    Q_UNUSED(content)
}

/*!
    For a multipart container returns a list of identifiers for all content directly contained by 
    the container; otherwise returns an empty list.

    \sa container(), clearContents(), appendContent(), replaceContent()
*/
QMessageContentContainerIdList QMessageContentContainer::contentIds() const
{
    return QMessageContentContainerIdList(); // stub
}

/*!
    If the container contains another container with identifier \a id either directly or 
    recursively then returns that other container; otherwise returns an empty container 
    constructed with the default constructor.

    \sa contentIds(), clearContents(), appendContent(), replaceContent()
*/
const QMessageContentContainer QMessageContentContainer::container(const QMessageContentContainerId id) const
{
    Q_UNUSED(id)
    return QMessageContentContainer(); // stub
}

/*!
    If the container contains another container with identifier \a id either directly or 
    recursively then returns that other container; otherwise returns an empty container 
    constructed with the default constructor.

    \sa contentIds(), clearContents(), appendContent(), replaceContent()
*/
QMessageContentContainer QMessageContentContainer::container(const QMessageContentContainerId id)
{
    Q_UNUSED(id)
    return QMessageContentContainer(); // stub
}

/*!
    Append a header field with name \a name and value \a value to the end of the list of 
    header fields for the container. Any existing header field with the same name is not 
    modified.

    If \a value is not ASCII text then it will be encoded by the first charset returned by 
    preferredCharsets() that can encode \a value if any; otherwise \a value will be encoded using 
    "UTF-8".

    \sa setHeaderField(), headerField(), headerFields()
*/
void QMessageContentContainer::appendHeaderField(const QByteArray &name, const QString &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}

/*!
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

    \sa appendHeaderField(), headerField(), headerFields(), preferredCharsets()
*/
void QMessageContentContainer::setHeaderField(const QByteArray &name, const QString &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}

/*!
    Returns the value of the first header field of the container with the name \a name, if any;
    otherwise returns a null string.

    \sa headerFieldValues(), appendHeaderField(), setHeaderField(), headerFields(), preferredCharsets()
*/
QString QMessageContentContainer::headerField(const QByteArray &name) const
{
    Q_UNUSED(name)
    return QString(); // stub
}

/*!
    Returns a list of values of header fields with the name \a name, if any;
    otherwise returns an empty list.

    \sa appendHeaderField(), setHeaderField(), headerField(), headerFields(), preferredCharsets()
*/
QList<QString> QMessageContentContainer::headerFieldValues(const QByteArray &name) const
{
    Q_UNUSED(name)
    return QList<QString>(); // stub
}

/*!
    Returns a list of names of header fields of the container.

    \sa appendHeaderField(), setHeaderField(), headerField(), preferredCharsets()
*/
QList<QByteArray> QMessageContentContainer::headerFields() const
{
    return QList<QByteArray>(); // stub
}

/*!
    Append a header field with name \a name and value \a value to the end of the list of 
    header fields for the container. Any existing header field with the same name is not 
    modified.

    \sa setHeaderField(), headerField(), headerFields(), preferredCharsets()
*/
void QMessageContentContainer::appendHeaderField(const QByteArray &name, const QByteArray &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}

/*!
    Sets the value of the first header field of the container with name \a name to \a value if it 
    already exists; otherwise appends a header with the supplied name and value.

    \sa appendHeaderField(), headerField(), headerFields(), preferredCharsets()
*/
void QMessageContentContainer::setHeaderField(const QByteArray &name, const QByteArray &value)
{
    Q_UNUSED(name)
    Q_UNUSED(value)    
}

/*!
    Sets the ordered by preference list of names of charsets to use when encoding unicode QString 
    text with setContent(), appendHeaderField() or setHeaderField() to \a charsetNames.

    \sa preferredCharsets()
*/
void QMessageContentContainer::setPreferredCharsets(const QList<QByteArray> &charsetNames)
{
    Q_UNUSED(charsetNames);
}

/*!
    Returns an ordered by preference list of charset names to use when encoding unicode QString 
    text with setContent(), appendHeaderField() or setHeaderField().

    \sa setPreferredCharsets()
*/
QList<QByteArray> QMessageContentContainer::preferredCharsets()
{
    return QList<QByteArray>(); // stub;
}

/*!
    Returns true if the container has been modified since it was constructed; 
    otherwise returns false.
*/
bool QMessageContentContainer::containerDataModified() const
{
    return false; // stub
}

/*!
    Sets the modified data state of the container to \a modified.
*/
void QMessageContentContainer::setContainerDataModified(bool modified)
{
    Q_UNUSED(modified);
}

/*!
    Prepend \a content to the start of the list of content contained.

    For a non multipart container, before a part is prepended the content type of the 
    container is set to "multipart" and the contents of the container cleared with clearContents().

    Returns an identifier for the prepended content.
*/
QMessageContentContainerId QMessageContentContainer::prependContent(const QMessageContentContainer & content)
{
    Q_UNUSED(content);
    return QMessageContentContainerId(); // stub
}
