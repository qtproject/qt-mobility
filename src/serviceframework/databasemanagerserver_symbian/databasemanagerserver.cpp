/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "databasemanagerserver_p.h"
#include "clientservercommon.h"
#include "databasemanagersession_p.h"
#include "servicedatabase_p.h"

#include <QFileSystemWatcher>

#include <QCoreApplication>
//#include <QThread>

QTM_BEGIN_NAMESPACE

#define SEC_TOKEN 0x00000000

static TInt Timeout(TAny *aObject);

const TInt CDatabaseManagerServer::timeoutInterval = 30000000; // 30 seconds

TInt Timeout(TAny *aObject)
  {
    ((CDatabaseManagerServer *)aObject)->Shutdown();
    return 1;
  }

CDatabaseManagerServer::CDatabaseManagerServer()
    : CServer2(EPriorityNormal, ESharableSessions)
    , iSessionCount(0)
    {
    iPeriodic = CPeriodic::NewL(0);
    iPeriodic->Start(timeoutInterval, timeoutInterval, TCallBack(Timeout, this));
    iDb = new ServiceDatabase();
    initDbPath();

    iDatabaseManagerServerSignalHandler = new DatabaseManagerServerSignalHandler(this);
    iWatcher = new QFileSystemWatcher();
    QObject::connect(iWatcher, SIGNAL(directoryChanged(QString)),
            iDatabaseManagerServerSignalHandler, SLOT(importChanged(QString)));

    QString path = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    path += QDir::separator() + QString("import");

    // Make the directory incase no xml services are installed
    QDir dir;
    dir.mkdir(path);
    iWatcher->addPath(path);

    DiscoverServices();

    }

CSession2* CDatabaseManagerServer::NewSessionL(const TVersion& aVersion, const RMessage2& /*aMessage*/) const
    {
        if (!User::QueryVersionSupported(TVersion(KServerMajorVersionNumber, 
                KServerMinorVersionNumber, KServerBuildVersionNumber), aVersion))
            {
            User::Leave(KErrNotSupported);
            }
        
        return CDatabaseManagerServerSession::NewL(*const_cast<CDatabaseManagerServer*>(this), iDb->databasePath());
    }

void CDatabaseManagerServer::PanicServer(TDatabaseManagerSerververPanic aPanic)
    {
    _LIT(KTxtServerPanic,"Database manager server panic");
    User::Panic(KTxtServerPanic, aPanic);
    }

void CDatabaseManagerServer::IncreaseSessions()
    {
    iSessionCount++;
    iPeriodic->Cancel();
    }

void CDatabaseManagerServer::DecreaseSessions()
    {
    iSessionCount--;
    if (iSessionCount <= 0)
        {
        iPeriodic->Start(timeoutInterval, timeoutInterval, TCallBack(Timeout, this));        
        }
    }

void CDatabaseManagerServer::Shutdown()
  {
  QCoreApplication::exit(0);
  }

void CDatabaseManagerServer::initDbPath()
  {
  QString dbIdentifier = "_system";

  QDir dir(QDir::toNativeSeparators(QCoreApplication::applicationDirPath()));
  QString qtVersion(qVersion());
  qtVersion = qtVersion.left(qtVersion.size() -2); //strip off patch version
  QString dbName = QString("QtServiceFramework_") + qtVersion + dbIdentifier + QLatin1String(".db");
  iDb->setDatabasePath(dir.path() + QDir::separator() + dbName);

  // Logic for handling service db file:
  //   - If db file doesn't exist on C drive, copy the file from Z drive
  //      -> When database is copied from Z, save the database modification
  //         date to a setting file
  //   - If db file already exists on C drive, check the modification date
  //     of the database on Z drive. If the database on Z is newer than the
  //     stored value (or the setting doesn't exist), merge services from
  //     that database to the database on C drive and update the modification
  //     date to the setting file.
  //   - Check if there are any extra database files in the server's private
  //     folder on C drive. This can happen if the Qt version number changes
  //     with rom update. In this case merge the services from the old database
  //     and then delete the old database file.

  QFile dbFile(iDb->databasePath());
  QFileInfo dbFileInfo(dbFile);
  QFile romDb(QLatin1String("z:\\private\\2002ac7f\\") + dbFileInfo.fileName());
  QFileInfo romDbInfo(romDb);
  QSettings romDbSettings(dir.path() + QDir::separator() + "romdb.ini", QSettings::NativeFormat);
  
  if (!dbFileInfo.exists()) {
      // create folder first
      if (!dbFileInfo.dir().exists()) 
          QDir::root().mkpath(dbFileInfo.path());
      // copy file from ROM
      // why not use QFile::copy?
      if (romDb.open(QIODevice::ReadOnly) && dbFile.open(QFile::WriteOnly)) {
          QByteArray data = romDb.readAll();
          dbFile.write(data);
          dbFile.close();
          romDb.close();
          
          // save the rom db modification date
          romDbSettings.setValue(QLatin1String("modificationDate"), QVariant(romDbInfo.lastModified()));
      }
  } else {
      // rom db has already been copied to c. check if it has been modified
      // i.e. whether there has been rom update.
      QDateTime storedDate = romDbSettings.value(QLatin1String("modificationDate")).toDateTime();
      if (!storedDate.isValid() || romDbInfo.lastModified() > storedDate) {
          // Need to merge new services in rom db to the database on c-drive.
          // (If there is no stored date but C drive contains db file, that means
          // that the firmware has been updated and the old version didn't store
          // the rom db date.)
          
          // copy the rom db temporarily to C drive for merging
          QTemporaryFile tempDb;
          if (romDb.open(QIODevice::ReadOnly) && tempDb.open()) {
              QByteArray data = romDb.readAll();
              tempDb.write(data);
              tempDb.close();
              romDb.close();
              
              ServiceDatabase *dstDatabase = new ServiceDatabase();
              dstDatabase->setDatabasePath(iDb->databasePath());
              dstDatabase->mergeDatabase(tempDb.fileName());
              delete dstDatabase;
              romDbSettings.setValue(QLatin1String("modificationDate"), QVariant(romDbInfo.lastModified()));
          }
      }
  }
  
  //Merge the Db files,if there are multiple DB's.
  mergeDbFiles(dir.path(), dbName);
    
  iDb->open();
  }

QStringList CDatabaseManagerServer::getExistingDbfiles(const QString &dirPath)
{
    QDir dir(dirPath);
    QStringList filters;
    dir.setSorting(QDir::Time);
    
    filters << "*.db";
    dir.setNameFilters(filters);
    
    return (dir.entryList(QDir::Files));
}

void CDatabaseManagerServer::mergeDbFiles(const QString &dbDirPath, const QString &dbFileName)
{
    QStringList dbList = getExistingDbfiles(dbDirPath);
    if(dbList.contains(dbFileName)) {
        //Remove dstDb
        dbList.removeAll(dbFileName);
        int dbListCount = dbList.count();
        if(dbListCount > 0) {
            QString dstDbFileName = dbDirPath + QDir::separator() + dbFileName;          
            ServiceDatabase *dstDatabase = new ServiceDatabase();
            dstDatabase->setDatabasePath(dstDbFileName);
            //Merge the DB's
            QString srcDbFileName;
            for(int i = 0; i < dbListCount; i++) {
                srcDbFileName = dbDirPath + QDir::separator() + dbList[i];
                //Merge the src DB with dst DB.
                if(dstDatabase->mergeDatabase(srcDbFileName)) {
  #ifdef QT_SFW_SERVICEDATABASE_DEBUG
                  qDebug() << "CDatabaseManagerServer::initDbPath():-"
                      << "dstDatabase->mergeDatabase Success";
  #endif             
                QFile::remove(srcDbFileName);
                }
            }
            delete dstDatabase;            
        }
    }

}
        
void CDatabaseManagerServer::DiscoverServices()
{
  QString path = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
  QSettings settings(path + QDir::separator() + "autoimport.ini", 
      QSettings::NativeFormat);  
 
  QString imports = path + QDir::separator() + "import";
    
  QDir dir(imports);
  dir.setFilter(QDir::Files);
  QStringList filters;
  filters << "*.xml";
  dir.setNameFilters(filters);
  QSet<QString> seen;
  
  QString tok = QString::number(SEC_TOKEN);
  
  QStringList files = dir.entryList();
  while(!files.isEmpty()){
      QString file = files.takeFirst();
      seen << file;
      
      QFileInfo fileinfo(imports + QDir::separator() + file);
      
      if(settings.contains(file)){ 
         if(fileinfo.lastModified() == settings.value(file).toDateTime()) {
             continue;
         }           
      }
      QFile f(imports + QDir::separator() + file);
      // read contents, register
      ServiceMetaData parser(&f);
      if (!parser.extractMetadata()) {
          f.remove();
          f.close();
          continue;
      }
      const ServiceMetaDataResults data = parser.parseResults();
      ServiceMetaDataResults results = parser.parseResults();
      QString servicename = results.name;
      
      if(iDb->registerService(results, tok)){
          iDb->serviceInitialized(results.name, tok);
      }
      f.close();
      settings.setValue(file, fileinfo.lastModified());
      settings.setValue(file + "/service_name", servicename);
  }
  
  QSet<QString> oldfiles = settings.allKeys().toSet();
  oldfiles -= seen;
  foreach(QString old, oldfiles){       
      if(old.contains('/'))
        continue;
      QString servicename = settings.value(old + "/service_name").toString();
      iDb->unregisterService(servicename, tok);
      settings.remove(old);
  }
}

void DatabaseManagerServerSignalHandler::importChanged(const QString& path)
{
  iDatabaseManagerServerSession->DiscoverServices();
}


QTM_END_NAMESPACE


#include "moc_databasemanagerserver_p.cpp"
// End of File
