#include "qversitdocumentwriter_p.h"
#include "versitutils_p.h"
#include <QTextCodec>

QTM_USE_NAMESPACE

#define MAX_CHARS_FOR_LINE 76

/*!
 * \class QVersitDocumentWriter
 *
 * \brief The QVersitDocumentWriter class provides an interface for writing a
 * single versit document into a vCard text string.
 */

/*! Constructs a writer. */
QVersitDocumentWriter::QVersitDocumentWriter(
    const QByteArray& documentType,
    const QByteArray& version)
    : mDocumentType(documentType),
    mVersion(version)
{
}


/*!
* Encodes the \a document to text.
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
 * Encodes the groups and name in the \a property to text.
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
