#include "qversitdocumentwriter_p.h"
#include "versitutils_p.h"

QTM_BEGIN_NAMESPACE

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
QByteArray QVersitDocumentWriter::encodeVersitDocument(const QVersitDocument& document)
{
    QList<QVersitProperty> properties = document.properties();
    QByteArray encodedDocument;

    encodedDocument += "BEGIN:" + mDocumentType + "\r\n";
    encodedDocument += "VERSION:" + mVersion + "\r\n";
    foreach (QVersitProperty property, properties) {
        encodedDocument.append(encodeVersitProperty(property));
    }
    encodedDocument += "END:" + mDocumentType + "\r\n";

    VersitUtils::fold(encodedDocument,MAX_CHARS_FOR_LINE);
    return encodedDocument;
}

/*!
 * Encodes the groups and name in the \a property to text.
 */
QByteArray QVersitDocumentWriter::encodeGroupsAndName(
    const QVersitProperty& property) const
{
    QByteArray encodedGroupAndName;
    QStringList groups = property.groups();
    if (!groups.isEmpty()) {
        QString groupAsString = groups.join(QString::fromAscii("."));
        encodedGroupAndName.append(groupAsString.toAscii());
        encodedGroupAndName.append(".");
    }
    encodedGroupAndName.append(property.name().toAscii());
    return encodedGroupAndName;
}

QTM_END_NAMESPACE
