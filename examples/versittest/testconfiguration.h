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

#ifndef TESTCONFIGURATION_H
#define TESTCONFIGURATION_H

#include <QXmlDefaultHandler>


class TestConfiguration : public QXmlDefaultHandler
{
public: // Constructors and destructor
    TestConfiguration();
    ~TestConfiguration();

public:
    int parse(const QString& fileName);
    bool saveContact(){return mSaveContact;}
    bool performanceTest(){return mPerformanceTest;}
    bool logToFile(){return mLogToFile;}
    QString testName(){return mTestName;}

protected: // From QXmlContentHandler
    bool startElement(
        const QString& namespaceURI,
        const QString& localName,
        const QString& qName,
        const QXmlAttributes& atts);

private: // Data
    QString mTestName;
    bool mSaveContact;
    bool mPerformanceTest;
    bool mLogToFile;
};


#endif // TESTCONFIGURATION_H





