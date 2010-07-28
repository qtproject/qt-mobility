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
#include "qtcontacts.h"
#include "qcontacttag.h"
#include "qversitreader.h"
#include "qversitcontactimporter.h"
#include "qversitwriter.h"
#include "qversitcontactexporter.h"
#include "qversitdocument.h"
#include "qversitproperty.h"
#include "qversitresourcehandler.h"
#include <QCoreApplication>
#include <QBuffer>
#include <QList>
#include <QFile>

QTM_USE_NAMESPACE

void completeExample();
void exportExample();
void importExample();

//! [Resource handler]
class MyResourceHandler : public QVersitDefaultResourceHandler {
public:
   bool saveResource(const QByteArray& contents, const QVersitProperty& property,
                     QString* location) {
       Q_UNUSED(property)
       *location = QString::number(qrand());
       QFile file(*location);
       file.open(QIODevice::WriteOnly);
       file.write(contents); // In a real implementation, consider when this file will be deleted.
       return true;
   }
   bool loadResource(const QString& location, QByteArray* contents, QString* mimeType)
   {
       return QVersitDefaultResourceHandler::loadResource(location, contents, mimeType);
   }
};
//! [Resource handler]

#if 0
int main(int argc, char *argv[])
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)
    completeExample();
    exportExample();
    importExample();
}
#endif

void completeExample()
{
    // Create the input vCard
    //! [Complete example - create]
    QBuffer input;
    input.open(QBuffer::ReadWrite);
    QByteArray inputVCard =
        "BEGIN:VCARD\r\nVERSION:2.1\r\nFN:John Citizen\r\nN:Citizen;John;Q;;\r\nEND:VCARD\r\n";
    input.write(inputVCard);
    input.seek(0);
    //! [Complete example - create]

    // Parse the input into QVersitDocuments
    //! [Complete example - read]
    // Note: we could also use the more convenient QVersitReader(QByteArray) constructor.
    QVersitReader reader;
    reader.setDevice(&input);
    reader.startReading(); // Remember to check the return value
    reader.waitForFinished();
    QList<QVersitDocument> inputDocuments = reader.results();
    //! [Complete example - read]

    // Convert the QVersitDocuments to QContacts
    //! [Complete example - import]
    QVersitContactImporter importer;
    if (!importer.importDocuments(inputDocuments))
        return;
    QList<QContact> contacts = importer.contacts();
    // Note that the QContacts are not saved yet.
    // Use QContactManager::saveContacts() for saving if necessary
    //! [Complete example - import]

    // Export the QContacts back to QVersitDocuments
    //! [Complete example - export]
    QVersitContactExporter exporter;
    if (!exporter.exportContacts(contacts, QVersitDocument::VCard30Type))
        return;
    QList<QVersitDocument> outputDocuments = exporter.documents();
    //! [Complete example - export]

    // Encode the QVersitDocument back to a vCard
    //! [Complete example - write]
    // Note: we could also use the more convenient QVersitWriter(QByteArray*) constructor.
    QBuffer output;
    output.open(QBuffer::ReadWrite);
    QVersitWriter writer;
    writer.setDevice(&output);
    writer.startWriting(outputDocuments); // Remember to check the return value
    writer.waitForFinished();
    // output.buffer() now contains a vCard
    //! [Complete example - write]
}

void exportExample()
{
    //! [Export example]
    QVersitContactExporter contactExporter;

    QVersitContactExporterDetailHandlerV2* backupHandler =
        QVersitContactExporterDetailHandlerV2::createBackupHandler();
    contactExporter.setDetailHandler(backupHandler);

    QContact contact;
    // Create a name
    QContactName name;
    name.setFirstName(QString::fromAscii("John"));
    contact.saveDetail(&name);

    if (!contactExporter.exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type))
        return;
    QList<QVersitDocument> versitDocuments = contactExporter.documents();

    // detailHandler.mUnknownDetails now contains the list of unknown details

    delete backupHandler;
    //! [Export example]
}

void importExample()
{
    //! [Import example]
    QVersitContactImporter importer;

    QVersitContactImporterPropertyHandlerV2* backupHandler =
        QVersitContactImporterPropertyHandlerV2::createBackupHandler();
    importer.setPropertyHandler(backupHandler);

    QVersitDocument document;

    QVersitProperty property;
    property.setName(QString::fromAscii("N"));
    property.setValue("Citizen;John;Q;;");
    document.addProperty(property);

    property.setName(QString::fromAscii("X-UNKNOWN-PROPERTY"));
    property.setValue("some value");
    document.addProperty(property);

    if (importer.importDocuments(QList<QVersitDocument>() << document)) {
        QList<QContact> contactList = importer.contacts();
        // contactList.first() now contains the "N" property as a QContactName
        // propertyHandler.mUnknownProperties contains the list of unknown properties
    }

    delete backupHandler;
    //! [Import example]
}

//! [Import relationship example]
/*! Adds contacts in  \a newContacts into \a manager, converting categories specified
  with tags into group membership relationships.  Note that this implementation uses the
  synchronous API of QContactManager for clarity.  It is recommended that the asynchronous
  API is used in practice.

  Relationships are added so that if a contact, A, has a tag "b", then a HasMember relationship is
  created between a group contact in the manager, B with display label "b", and contact A.  If there
  does not exist a group contact with display label "b", one is created.
  */
void insertWithGroups(const QList<QContact>& newContacts, QContactManager* manager)
{
    // Cache map from group names to QContactIds
    QMap<QString, QContactId> groupMap;

    foreach (QContact contact, newContacts) {
        if (!manager->saveContact(&contact))
            continue; // In practice, better error handling may be required
        foreach (const QContactTag& tag, contact.details<QContactTag>()) {
            QString groupName = tag.tag();
            QContactId groupId;
            if (groupMap.contains(groupName)) {
                // We've already seen a group with the right name
                groupId = groupMap.value(groupName);
            } else {
                QContactDetailFilter groupFilter;
                groupFilter.setDetailDefinitionName(QContactType::DefinitionName);
                groupFilter.setValue(QLatin1String(QContactType::TypeGroup));
                groupFilter.setMatchFlags(QContactFilter::MatchExactly);
                // In practice, some detail other than the display label could be used
                QContactDetailFilter nameFilter = QContactDisplayLabel::match(groupName);
                QList<QContactLocalId> matchingGroups = manager->contactIds(groupFilter & nameFilter);
                if (!matchingGroups.isEmpty()) {
                    // Found an existing group in the manager
                    QContactId groupId;
                    groupId.setManagerUri(manager->managerUri());
                    groupId.setLocalId(matchingGroups.first());
                    groupMap.insert(groupName, groupId);
                }
                else {
                    // Make a new group
                    QContact groupContact;
                    QContactName name;
                    name.setCustomLabel(groupName);
                    // Beware that not all managers support custom label
                    groupContact.saveDetail(&name);
                    if (!manager->saveContact(&groupContact))
                        continue; // In practice, better error handling may be required
                    groupId = groupContact.id();
                    groupMap.insert(groupName, groupId);
                }
            }
            // Add the relationship
            QContactRelationship rel;
            rel.setFirst(groupId);
            rel.setRelationshipType(QContactRelationship::HasMember);
            rel.setSecond(contact.id());
            manager->saveRelationship(&rel);
        }
    }
}
//! [Import relationship example]

//! [Export relationship example]
/*! Adds QContactTag details to the \a contacts, based on the \a relationships.

  Tags are created such that if a group contact, B with display label "b", has a HasMember
  relationship with a contact, A, then a QContactTag, "b", is added to A.

  Group contacts can be passed in with the \a contacts list.  If a contact is part of a group which
  is not in \a contacts, the \a manager is queried to find them.
  */
void createTagsFromGroups(QList<QContact>* contacts,
                          const QList<QContactRelationship>& relationships,
                          const QContactManager* manager)
{
    // Map from QContactIds to group names
    QMap<QContactId, QString> groupMap;

    // Map from QContactIds to indices into the contacts list
    QMap<QContactId, int> indexMap;
    // Build up groupMap and indexMap
    for (int i = 0; i < contacts->size(); ++i) {
        QContact contact = contacts->at(i);
        if (contact.type() == QContactType::TypeGroup) {
            // In practice, you may want to check that there aren't two distinct groups with the
            // same name, and you may want to use a field other than display label.
            groupMap.insert(contact.id(), contact.displayLabel());
        }
        indexMap.insert(contact.id(), i);
    }

    // Now add all the tags specified by the group relationships
    foreach (const QContactRelationship& rel, relationships) {
        if (rel.relationshipType() == QContactRelationship::HasMember
            && indexMap.contains(rel.second())) {
            QString groupName = groupMap.value(rel.first()); // Have we seen the group before?
            if (groupName.isEmpty()) {
                // Try and find the group in the manager
                QContactId groupId = rel.second();
                QContactFetchHint fetchHint;
                fetchHint.setDetailDefinitionsHint(QStringList(QContactDisplayLabel::DefinitionName));
                QContact contact = manager->contact(groupId.localId(), fetchHint);
                if (!contact.isEmpty()) {
                    groupName = contact.displayLabel();
                    groupMap.insert(groupId, groupName); // Cache the group id/name
                }
            }
            if (!groupName.isEmpty()) {
                // Add the tag
                QContactTag tag;
                tag.setTag(groupName);
                (*contacts)[indexMap.value(rel.second())].saveDetail(&tag);
            }
        }
    }
}
//! [Export relationship example]
