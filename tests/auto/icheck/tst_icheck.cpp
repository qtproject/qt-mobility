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

#include <QtTest/QtTest>
#include "../../../tools/icheck/ichecklib.h"
#include <qmobilityglobal.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QDomDocument>
#include <QDebug>

#if defined(Q_OS_SYMBIAN)
# define TESTDATA_DIR "."
#endif

//TESTED_CLASS=
//TESTED_FILES=
extern QStringList getQTIncludePath();

QString getTestFileFolder()
{
    QString ret;
    ret = TESTDATA_DIR;
    ret += "/testdata";
    QDir qdir(ret);
    if(!qdir.exists())
        ret = "";
    return ret;
}

//---- Test case class ----
class TestCase
{
public:
    TestCase(QDomNode& xmlnode);
    bool run();
    QString getErrorMsg() { return errorMsg; }
    QString getTestName() { return testName; }
private:
    bool loadXmlInformation();
private:
    QDomNode xmlNode;
    QString errorMsg;
    QString testName;
    QString iheader;
    QString header;
    QStringList expectedResult;
};

TestCase::TestCase(QDomNode& xmlnode)
: xmlNode(xmlnode)
  , errorMsg("")
  , testName("")
  , iheader("")
  , header("")
{
    expectedResult.clear();
}

bool TestCase::loadXmlInformation()
{
    QDomNodeList nodeList = xmlNode.childNodes();
    for(int i = 0; i < nodeList.count(); i++){
        QDomNode nd = nodeList.at(i);
        if(nd.childNodes().count() > 0){
            if(nd.nodeName() == "Name")
                testName = nd.firstChild().nodeValue();
            else if(nd.nodeName() == "InterfaceHeader")
                iheader = getTestFileFolder() + "/" + nd.firstChild().nodeValue();
            else if(nd.nodeName() == "CompareHeader")
                header = getTestFileFolder() + "/" + nd.firstChild().nodeValue();
            else if(nd.nodeName() == "ExpectedResult"){
                QDomNodeList chnodeList = nd.childNodes();
                for(int b = 0; b < chnodeList.count(); b++){
                    QDomNode chnd = chnodeList.at(b);
                    if(chnd.childNodes().size() > 0)
                        expectedResult << chnd.firstChild().nodeValue();
                }
            }
        }
    }
    
    // check if we have all informations
    QTextStream out(&errorMsg);
    if(testName.size() <= 0)
        out << "No test name defined. Error in Test.xml file!" << endl;
    if(iheader.size() <= 0)
        out << "No interface header defined. Error in Test.xml file!" << endl;
    if(header.size() <= 0)
        out << "No compare header defined. Error in Test.xml file!" << endl;
    if(errorMsg.size() > 0)
        return false;

    //check if the headers are available
    QFile ichfile(iheader);
    if (!ichfile.exists()){
        out << iheader << " file not found on drive." << endl;
    }
    QFile chfile(header);
    if (!chfile.exists()){
        out << header << " file not found on drive." << endl;
    }
    if(errorMsg.size() > 0)
        return false;

    return true;
}

bool TestCase::run()
{
    QTextStream out(&errorMsg);
    if(loadXmlInformation()){
        qDebug() << "Start: " << testName;
        //Start the test
        QString curpath = getTestFileFolder();
        //Create FileInfos for the header files
        QFileInfo iFileInfo(iheader);
        QFileInfo chFileInfo(header);

        //Now create a list of the include path 
        QString chIncludepath = chFileInfo.absolutePath();
        QStringList chIncludepathlist;
        chIncludepathlist << chIncludepath;
        chIncludepathlist << getQTIncludePath();

        QString iIncludepath = iFileInfo.absolutePath();
        QStringList iIncludepathlist;
        iIncludepathlist << iIncludepath;

        //Create a list of all the soucre files they need to be parsed.
        //In our case it is just the header file
        QStringList chFilelist;
        chFilelist << chFileInfo.filePath();

        QStringList iFilelist;
        iFilelist << iFileInfo.filePath();

        ICheckLib i_ichecklib;
        i_ichecklib.ParseHeader(iIncludepathlist, iFilelist);

        ICheckLib ichecklib;
        ichecklib.ParseHeader(chIncludepathlist, chFilelist);

        ichecklib.check(i_ichecklib, "");
        QStringList result = ichecklib.getErrorMsg();
        if(result.count() != expectedResult.count()){
            out << testName << " failed. Expected result line count doesn't equals to the result line count." << endl;
            return false;
        }
        bool ret = true;
        for(int i = 0; i < result.count(); i++){
            QString leftcp = result[i].replace(" ","");
            QString rightcp = expectedResult[i].replace(" ","");
            if(leftcp != rightcp){
                out << endl << "--- " << testName << " ---" << endl << " failed. Expected result line[" << (i + 1) << "] count doesn't equals to the result line count." << endl;
                ret = false;
            }
        }
        return ret;
    }
    return false;
}
//---- end Test case class ----

class tst_ICheck: public QObject
{
Q_OBJECT

public:
    tst_ICheck();
    virtual ~tst_ICheck();

private slots:
    void doTests();
    void initTestCase();
};

tst_ICheck::tst_ICheck()
{
}

tst_ICheck::~tst_ICheck()
{
}

void tst_ICheck::initTestCase()
{
}

void tst_ICheck::doTests()
{
    QString msg;
    QString xmltestfile = getTestFileFolder();
    xmltestfile += "/Test.xml";
    QFile xmlfile(xmltestfile);
    bool failed = false;
    if (xmlfile.exists()){
        QDomDocument document;
        if (document.setContent(&xmlfile)) {
            QDomElement rootnd = document.documentElement();
            if(rootnd.isElement()){
                QDomNodeList nodeList = rootnd.childNodes();
                for(int i = 0; i < nodeList.count(); i++){
                    QDomNode nd = nodeList.at(i);
                    TestCase test(nd);
                    if(!test.run()){
                        QWARN(test.getErrorMsg().toLatin1());
                        failed = true;
                    }
                }
            }
        }
    }
    else {
        QFAIL ( QString(xmltestfile + " file not found").toLatin1() );
    }
    if(failed)
        QFAIL ( "Test failed, please read warnings!" );
}

QTEST_MAIN(tst_ICheck)
#include "tst_icheck.moc"
