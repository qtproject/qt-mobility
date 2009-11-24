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

#ifndef VERSITTEST_H
#define VERSITTEST_H

#include <QObject>
#include <QStringList>

class QFile;
class QIODevice;
class QContactManager;
class QVersitReader;
class QVersitWriter;
class QVersitContactImporter;
class QVersitContactExporter;


class VersitTest : public QObject
{
     Q_OBJECT

public:

    VersitTest();
    VersitTest(bool saveContacts,
               int scaledImageHeight,
               int scaledImageWidth);
     
public slots:
    void scale(const QString& imageFileName, QByteArray& imageData);

private slots:

    void initTestCase();    
    void init();
    void cleanup();
    void cleanupTestCase(); 

private slots:
    void test();
    void test_data();
    void exampleCode();

private:
    
    void executeTest(QFile& in, QIODevice& out);  

private:
 
    bool mSaveContacts;
    int mScaledImageHeight;
    int mScaledImageWidth;
    QContactManager* mContactManager;
    QStringList* mExcludedFields;    
    QStringList mFiles;
    QVersitReader* mReader;
    QVersitWriter* mWriter;
    QString mInputDirPath;
    QString mExcludeFieldsFileName;
    QString mOutputDirPath;
    QString mImageAndAudioClipPath;
    QVersitContactImporter* mImporter;
    QVersitContactExporter* mExporter;
};

#endif // VERSITTEST_H
