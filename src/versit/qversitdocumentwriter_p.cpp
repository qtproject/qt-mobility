#include "qversitdocumentwriter_p.h"
#include "versitutils_p.h"
#include <QTextCodec>

QTM_USE_NAMESPACE

#define MAX_CHARS_FOR_LINE 76

/*!
 * \class QVersitDocumentWriter
 * \internal
 * \brief The QVersitDocumentWriter class provides an interface for writing a
 * single versit document into a vCard text string.
 */

/*! Constructs a writer.
 * \a documentType is the type of Versit document, as printed on the BEGIN line of output
 * eg. "VCARD"
 * \a version is the version of the Versit format, as printed on the VERSION line of output.
 * eg. "2.1"
 */
QVersitDocumentWriter::QVersitDocumentWriter(
    const QByteArray& documentType,
    const QByteArray& version)
    : mDocumentType(documentType),
    mVersion(version)
{
}


/*!
* Encodes the \a document to text using the given \a codec.
*/
QByteArray QVersitDocumentWriter::encodeVersitDocument(const QVersitDocument& document,
                                                       QTextCodec* codec)
{
    QList<QVersitProperty> properties = document.properties();
    QByteArray encodedDocument;

    encodedDocument += codec->fromUnicode(QLatin1String("BEGIN:" + mDocumentType + "\r\n"));
    encodedDocument += codec->fromUnicode(QLatin1String("VERSION:" + mVersion + "\r\n"));
    foreach (QVersitProperty property, properties) {
        encodedDocument.append(encodeVersitProperty(property, codec));
    }
    encodedDocument += codec->fromUnicode(QLatin1String("END:" + mDocumentType + "\r\n"));

    VersitUtils::fold(encodedDocument, MAX_CHARS_FOR_LINE);
    return encodedDocument;
}

/*!
 * Encodes the groups and name in the \a property to text using the given \a codec.
 */
QByteArray QVersitDocumentWriter::encodeGroupsAndName(const QVersitProperty& property,
                                                      QTextCodec* codec) const
{
    QByteArray encodedGroupAndName;
    QStringList groups = property.groups();
    if (!groups.isEmpty()) {
        QString groupAsString = groups.join(QLatin1String("."));
        encodedGroupAndName.append(codec->fromUnicode(groupAsString));
        encodedGroupAndName.append(codec->fromUnicode(QLatin1String(".")));
    }
    encodedGroupAndName.append(codec->fromUnicode(property.name()));
    return encodedGroupAndName;
}
