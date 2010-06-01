/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qsfwtestutil.h"

#include <QDir>
#include <QFileInfoList>
#include <QFile>
#include <QSettings>
#include <QDebug>
#include <QCoreApplication>

void QSfwTestUtil::setupTempUserDb()
{
    QSettings::setUserIniPath(tempUserDbDir());
}

void QSfwTestUtil::setupTempSystemDb()
{
    QSettings::setSystemIniPath(tempSystemDbDir());
}

void QSfwTestUtil::removeTempUserDb()
{
    removeDirectory(tempUserDbDir());
}

void QSfwTestUtil::removeTempSystemDb()
{
    removeDirectory(tempSystemDbDir());
}

QString QSfwTestUtil::tempUserDbDir()
{
    return tempSettingsPath("__user__");
}

QString QSfwTestUtil::tempSystemDbDir()
{
    return tempSettingsPath("__system__");
}

QString QSfwTestUtil::userDirectory()
{
    return tempSettingsPath("__user__/");
}

QString QSfwTestUtil::systemDirectory()
{
    return tempSettingsPath("__system__/");
}

QString QSfwTestUtil::tempSettingsPath(const char *path)
{
#if defined(Q_OS_SYMBIAN) && defined(__WINS__)
    // On emulator, use hardcoded path instead of private directories to
    // enable a shared database.
    Q_UNUSED(path);
    return QDir::toNativeSeparators("C:/Data/temp/QtServiceFW");
#else
    // Temporary path for files that are specified explictly in the constructor.
    //QString tempPath = QDir::tempPath();
    QString tempPath = QCoreApplication::applicationDirPath();
    if (tempPath.endsWith("/"))
        tempPath.truncate(tempPath.size() - 1);
    return QDir::toNativeSeparators(tempPath + "/QtServiceFramework_tests/" + QLatin1String(path));
#endif
}

void QSfwTestUtil::removeDirectory(const QString &path)
{
    QDir dir(path);
    QFileInfoList fileList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);
    foreach(QFileInfo file, fileList) {
        if(file.isFile()) {
            QFile::remove (file.canonicalFilePath());
        }
        if(file.isDir()) {
            QFile::Permissions perms = QFile::permissions(file.canonicalFilePath());
            perms = perms | QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner;
            QFile::setPermissions(file.canonicalFilePath(), perms);
            removeDirectory(file.canonicalFilePath());
        }
    }
    dir.rmpath(path);
}

#if defined(Q_OS_SYMBIAN) && !defined(__WINS__)
#include <e32base.h>
#include <f32file.h>
void QSfwTestUtil::removeDatabases()
{
    TFindServer findServer(_L("!qsfwdatabasemanagerserver"));
    TFullName name;
    if (findServer.Next(name) == KErrNone)
    {
        RProcess dbServer;
        if (dbServer.Open(_L("qsfwdatabasemanagerserver")) == KErrNone)
        {
            dbServer.Kill(KErrNone);
            dbServer.Close();    
        }
    }    

    RFs fs;
    fs.Connect();
    CleanupClosePushL(fs);
    CFileMan* fileMan=CFileMan::NewL(fs);
    CleanupStack::PushL(fileMan);
    fileMan->Delete(_L("c:\\private\\2002AC7F\\QtServiceFramework_4.6_system.db")); //Server's fixed UID3
    CleanupStack::PopAndDestroy(2, &fs);    
}
#endif
