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

#include "ichecklib.h"
#include <QtCore/QCoreApplication>
#include <QString>
#include <QStringList>
#include <iostream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>

using namespace std;

QT_USE_NAMESPACE

extern QStringList getQTIncludePath();

int main(int argc, char *argv[])
{
    int ret = 0;
    if(argc >= 3){
        QString err = "";
        //Extract the file name and path from the arguments
        QString interfaceHeaderFile = argv[1];
        QString checkHeaderFile = argv[2];

        const QString curpath = QDir::currentPath();
        //Create FileInfos for the header files
        QFileInfo iFileInfo(interfaceHeaderFile);
        if (iFileInfo.isDir() || !iFileInfo.exists()){
            if(iFileInfo.isDir())
                err = "Cannot compare a directory: " + interfaceHeaderFile;
            else
                err = "File does not exist: " + interfaceHeaderFile;
        }
        if(err.isEmpty()){
            QFileInfo chFileInfo(checkHeaderFile);
            if (chFileInfo.isDir() || !chFileInfo.exists()){
                if(chFileInfo.isDir())
                    err = "Cannot compare a directory: " + checkHeaderFile;
                else
                    err = "File does not exist: " + checkHeaderFile;
            }
            if(err.isEmpty()){
                //Read other parameters
                QString outputfile;
                for(int i = 3; i < argc; i++)
                {
                    //Read output file
                    int size = strlen(argv[i]);
                    if(strncmp(argv[i], "-output", size) == 0){
                        if( (i + 1) < argc){
                            outputfile = argv[i + 1];
                            i++;
                        }
                    }
                }

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
                chFilelist << chFileInfo.absoluteFilePath();

                QStringList iFilelist;
                iFilelist << iFileInfo.absoluteFilePath();

                ICheckLib i_ichecklib;
                i_ichecklib.ParseHeader(iIncludepathlist, iFilelist);

                ICheckLib ichecklib;
                ichecklib.ParseHeader(chIncludepathlist, chFilelist);

                if(!ichecklib.check(i_ichecklib, outputfile)){
                    cout << "Folowing interface items are missing:" << endl;
                    QStringList errorlist = ichecklib.getErrorMsg();
                    foreach(QString msg, errorlist){
                        cout << (const char *)msg.toLatin1() << endl;
                    }
                    ret = -1;
                }
                else
                    cout << "Interface is full defined.";
            }
        }

        if(!err.isEmpty()) //prit out error
        {
            cout << endl << "Error:" << endl;
            cout << (const char *)err.toLatin1();
            ret = -2;
        }
    }
    else{
        cout << "Usage: icheck <Interface header> <headerfile to check>" << endl;
        cout << "-output ...... Textfile name containing the output result." << endl;
    }
    cout << endl;
    return ret;
}
