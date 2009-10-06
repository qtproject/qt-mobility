/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#ifndef VERSITTEST_H
#define VERSITTEST_H

#include <QObject>
#include <QStringList>

class QIODevice;


class VersitTest : public QObject
{
     Q_OBJECT
    
private slots:

    void initTestCase();    
    void init();
    void cleanup();
    void cleanupTestCase(); 

private slots:
    void test();
    void test_data();    

private:
    
    void executeTest(QIODevice& in, QIODevice& out);
    
private:
 
    QStringList* mExcludedFields;    
    QStringList mFiles;
};

#endif // VERSITTEST_H
