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

extern QStringList getQTIncludePath();

int main(int argc, char *argv[])
{
    int ret = 0;
    if(argc == 3){
        QString err = "";
        //Extract the file name and path from the arguments
        QString interfaceHeaderFile = argv[1];
        QString checkHeaderFile = argv[2];
        QString curpath = QDir::currentPath();
        //Create FileInfos for the header files
        QFileInfo iFileInfo(interfaceHeaderFile);
        if (iFileInfo.isDir() || !iFileInfo.exists()){
            if(iFileInfo.isDir())
                err = "Cannot compare a directory: " + interfaceHeaderFile;
            else
                err = "File does not exist: " + interfaceHeaderFile;
        }
        if(err == ""){
            QFileInfo chFileInfo(checkHeaderFile);
            if (chFileInfo.isDir() || !chFileInfo.exists()){
                if(chFileInfo.isDir())
                    err = "Cannot compare a directory: " + checkHeaderFile;
                else
                    err = "File does not exist: " + checkHeaderFile;
            }
            if(err == ""){
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

                if(!ichecklib.check(i_ichecklib)){
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

        if(err != "") //prit out error
        {
            cout << endl << "Error:" << endl;
            cout << (const char *)err.toLatin1();
            ret = -2;
        }
    }
    else{
        cout << "icheck ";
        cout << "<Interface header>";
        cout << " <headerfile to check>";
    }
    cout << endl;
    return ret;
}
