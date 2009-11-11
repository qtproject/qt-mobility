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

#include <QDir>
#include <QFile>
#include <QStringList>
#include <QBuffer>
#include <QList>
#include <QtCore>
#include <qcontact.h>
#include <qcontactmanager.h>
#include <qversitreader.h>
#include <qversitwriter.h>
#include <qversitdocument.h>
#include <qversitcontactimporter.h>
#include <qversitcontactexporter.h>

#include "qversitdocsample.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    VersitDocSample versitSample;
    // Generate QContact from VCard file
    QFile in("testvcard_2.1-vcf");
    QList<QContact> contacts = versitSample.generateContact(in);

    // Generate VCard file from QContact
    QContact contact;
    versitSample.generateVCard(contact);
    
    // Generate QContact from VCard file and save to contact database
    versitSample.generateAndSaveContact(in);

    return 0;
}

//! [Generate QContact from VCard file]
QList<QContact> VersitDocSample::generateContact(QIODevice& in)
{
   // Seek beginning of the file
   in.seek(0);
   // Create versit reader and read the vcard file content
   QVersitReader* reader = new QVersitReader;
   reader->setDevice(&in);
   reader->readAll();   
   // Get list of QVersitDocument from reader
   QList<QVersitDocument> documents = reader->result();   
   // Create QVersitContactImporter to convert QVersitDocument into QContact
   QVersitContactImporter importer;
   // Generate contact from each document
   QList<QContact> contacts;
   foreach (QVersitDocument document, documents) {
   	    // VCard may have image and audio ( Like LOGO,RINGTONE etc)
   	    // Configure the storage location of this content
        importer.setImagePath(QString::fromAscii("/images/"));
        importer.setAudioClipPath(QString::fromAscii("/audio/"));
        // Converts QVersitDocument into QContact
        QContact contact = importer.importContact(document);
        // Append contact to contact List
        contacts.append(contact);
    }  
    delete reader;
    return contacts;
}
//! [Generate QContact from VCard file]

//! [Generate VCard file from QContact]
void VersitDocSample::generateVCard(QContact& contact)
{
    // Create versit writer
    QVersitWriter* writer = new QVersitWriter;
    // Create file and set to writer
    QFile out("testvcard_2.1-vcf");
    out.open(QIODevice::ReadWrite);
    writer->setDevice(&out);
    // Create QVersitContactExporter to convert QContact into QVersitDocument
    QVersitContactExporter exporter;        
    connect(&exporter, SIGNAL(scale(const QString&,QByteArray&)),
            0, SLOT(scale(const QString&,QByteArray&)));
    // Create versit document
    QVersitDocument document = exporter.exportContact(contact,QVersitDocument::VCard21);
    // Encode and write to output file
    writer->setVersitDocument(document);
    writer->writeAll();
}
//! [Generate VCard file from QContact]

// Callback for image scaling
void VersitDocSample::scale(const QString& imageFileName, QByteArray& imageData)
{
    int scaledImageHeight = 100;
    int scaledImageWidth = 100;
    if (scaledImageHeight > 0 && scaledImageWidth > 0) {
        QImage image(imageFileName);
        image = image.scaled(scaledImageHeight,scaledImageWidth);
        QBuffer imageBuffer(&imageData);
        QByteArray fileExtension = imageFileName.section('.',-1).toUpper().toAscii();
        image.save(&imageBuffer,fileExtension.constData());
    }
}

//! [Generate QContact from VCard file and save to contact database]
void VersitDocSample::generateAndSaveContact(QFile& in)
{
   // Seek beginning of the file
   in.seek(0);
   // Create versit reader and read the vcard file content
   QVersitReader* reader = new QVersitReader;
   reader->setDevice(&in);
   reader->readAll();   
   // Get list of QVersitDocument from reader
   QList<QVersitDocument> documents = reader->result();
   // Create QVersitContactImporter to convert QVersitDocument into QContact
   QVersitContactImporter importer;
   // Generate contact from each document
   foreach (QVersitDocument document, documents) {
        // VCard may have image and audio ( Like LOGO,RINGTONE etc)
        // Configure the storage location of this content
        importer.setImagePath(QString::fromAscii("/images/"));
        importer.setAudioClipPath(QString::fromAscii("/audio/"));
        // Converts QVersitDocument into QContact
        QContact contact = importer.importContact(document);
        // Save contact
        QContactManager* contactManager = new QContactManager(QString::fromAscii("symbian"));
        contactManager->saveContact(&contact);
        delete contactManager;
    }  
}
//! [Generate QContact from VCard file and save to contact database]




