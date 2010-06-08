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

#include <stdio.h>
#include <QtCore>
#include <QtSql>
#include <servicedatabase_p.h>
#include <servicemetadata_p.h>
#include <databasemanager_p.h>

QT_USE_NAMESPACE

QTM_USE_NAMESPACE

/*
 * If the emulator supports per process writeable static
 * data then we can use a separate server process and
 * put the db in the same location as on a real
 * device
 */
#define WINSCW_DES_DEPLOY_WSD   "epoc32/winscw/c/private/2002AC7F/des"
#define WINSCW_DBPATH_WSD       "epoc32/winscw/c/private/2002AC7F"

#define WINSCW_DES_DEPLOY       "epoc32/winscw/c/private/2002AC7F/des"
#define WINSCW_DBPATH           "epoc32/winscw/c/data/temp/QtServiceFW"

#define HW_DES_DEPLOY           "epoc32/data/z/private/2002AC7F/des"
#define HW_DBPATH               "epoc32/data/z/private/2002AC7F"

#define SEC_TOKEN               0x101FB657
#define TOOL_VERSION            "0.4"

#define MESSAGE(msg) \
{ \
   *stdoutStream << msg; \
   stdoutStream->flush(); \
}

QString dbName()
{
    QString qtVersion(qVersion());
    qtVersion = qtVersion.left(qtVersion.size() -2); //strip off patch version
    return QString("QtServiceFramework_") + qtVersion + "_system";
};

class GeneratorPaths
{
public:
    GeneratorPaths(){};
    GeneratorPaths(const QString &dbPath, const QString &desPath) : databasePath(dbPath), descriptorsPath(desPath){};
    ~GeneratorPaths(){};
    GeneratorPaths(const GeneratorPaths &copy){databasePath = copy.databasePath; descriptorsPath = copy.descriptorsPath;};
public:
    QString databasePath;
    QString descriptorsPath;
};

class CommandProcessor : public QObject
{
    Q_OBJECT

public:
    CommandProcessor(QObject *parent = 0);
    ~CommandProcessor();

    bool initialize();
    void execute(const QStringList &options, const QString &cmd, const QStringList &args);
    void showUsage();

public slots:
    void add(const QStringList &desList, ServiceDatabase *db);
    void remove(const QStringList &desList, ServiceDatabase *db);

private:
    bool setOptions(const QStringList &options, QMap<QString, GeneratorPaths> &targets);
    ServiceDatabase *initTargetDatabase(const QString &dbPath, bool deleteDbAllowed);
    QString getServiceFromXML(const QString &xmlFile);
    QStringList getServiceDescriptors(const QString &path);
    QStringList targetServiceDescriptors(const QString &desPath, const QStringList &args);

    bool batchMode;
    bool requireInitialization;
    bool deleteDatabase;
    QTextStream *stdoutStream;
    QMap<QString, QString> targetMap; // (target, platform)
    QMap<QString, GeneratorPaths> pathMap; // (platform, (databasePath, descriptorsPath))
    QMap<QString, GeneratorPaths> wsdMap; // (platform, (databasePath, descriptorsPath))
};

CommandProcessor::CommandProcessor(QObject *parent)
    : QObject(parent),
      batchMode(false),
      requireInitialization(false),
      deleteDatabase(false),
      stdoutStream(new QTextStream(stdout))
{
}

CommandProcessor::~CommandProcessor()
{
    delete stdoutStream;
}

bool CommandProcessor::initialize()
{
    QString epocRoot(qgetenv("EPOCROOT").data());
    if (epocRoot.isEmpty()) {
        *stdoutStream << "ERROR: EPOCROOT not set\n";
        return false;
    }

    targetMap["winscw"] = "emulator";
    targetMap["armv5"] = "hw";
    targetMap["armv6"] = "hw";
    targetMap["gcce"] = "hw";

    if (epocRoot.right(1) != QDir::separator())
        epocRoot += QDir::separator();

    wsdMap["nowsd"] = GeneratorPaths(QDir::toNativeSeparators(epocRoot + WINSCW_DBPATH),
                                     QDir::toNativeSeparators(epocRoot + WINSCW_DES_DEPLOY));

    wsdMap["wsd"] = GeneratorPaths(QDir::toNativeSeparators(epocRoot + WINSCW_DBPATH_WSD),
                                   QDir::toNativeSeparators(epocRoot + WINSCW_DES_DEPLOY_WSD));

#ifdef QT_SYMBIAN_EMULATOR_SUPPORTS_PERPROCESS_WSD
    pathMap["emulator"] = wsdMap["wsd"];
#else
    pathMap["emulator"] = wsdMap["nowsd"];
#endif
    pathMap["hw"] = GeneratorPaths(QDir::toNativeSeparators(epocRoot + HW_DBPATH),
                                   QDir::toNativeSeparators(epocRoot + HW_DES_DEPLOY));
    return true;
}

void CommandProcessor::execute(const QStringList &options, const QString &cmd, const QStringList &args)
{
    if(cmd.isEmpty())
        MESSAGE("Error: no command given\n\n");

    // setup options and collect target(s)
    // parse options, and fails if there's no cmd
    // usage output includes paths and targets, so make
    // sure to parse options that might change them
    QMap<QString, GeneratorPaths> targets;
    if (!setOptions(options, targets) || cmd.isEmpty()) {
        showUsage();
        return;
    }

    int methodIndex = metaObject()->indexOfMethod(cmd.toAscii() + "(QStringList,ServiceDatabase*)");
    if (methodIndex < 0) {
        MESSAGE("Bad command: " << cmd << "\n\n");
        showUsage();
        return;
    }

    QMapIterator<QString, GeneratorPaths> i(targets);
    while (i.hasNext()) {
        i.next();
        const QString &target = i.key();
        const GeneratorPaths &paths = i.value();
        ServiceDatabase *db = initTargetDatabase(paths.databasePath, true);
        if (!db) {
            MESSAGE("ERROR: database for " << target << " cannot be opened/created.\n");
            continue;
        }
        MESSAGE("Database[" << i.key() << "]: " << db->databasePath() << '\n');
        QStringList desList = (batchMode) ? targetServiceDescriptors(paths.descriptorsPath, args) : args;
        // register services
        metaObject()->method(methodIndex).invoke(this, Q_ARG(QStringList, desList), Q_ARG(ServiceDatabase*, db));
        // remove database so other targets can be handled
        db->close();
        QSqlDatabase::removeDatabase(db->m_connectionName);
        delete db;
    }
}

void CommandProcessor::showUsage()
{
    *stdoutStream << "Service framework database management tool, version " << QString(TOOL_VERSION) << "\n"
            "Usage: servicedbgen [options] <command> [command parameters]\n\n"
            "Commands:\n"
            "\tadd     Register or update a service\n"
            "\tremove  Unregister a service\n"
            "\nOptions:\n"
            "\t-t<tgt> Specifies the target platform to be used (e.g. armv5, all).\n"
            "\t-b      Batch mode (take service descriptors from a path). If specified, \n"
            "\t        the command parameters are specifying the path to the folder\n"
            "\t        where the service descriptors reside. In case command parameters\n"
            "\t        are not given, the target specific default path will be taken\n"
            "\t        (see paths below).\n"
            "\t-c      Delete the service database, has effect only in batch mode. \n"
            "\t-i      The services require initialization upon first load.\n"
            "\t-w<1|0> Override the default support for WSD in the emulator\n"
            "\n"
            "Supported targets and their database paths:";
    QMapIterator<QString, QString> i(targetMap);
    while (i.hasNext()) {
        i.next();
        GeneratorPaths out(pathMap[i.value()]);
        *stdoutStream << "\nTarget: " << i.key() << "\n\tpath: " << out.databasePath << "\n\tdeployment: " << out.descriptorsPath << '\n';
    }
    *stdoutStream << "\nExamples:\n"
        " - registering service1 and service2 for winscw target:\n"
        "\tservicedbgen -twinscw add service1.xml service2.xml\n\n"
        " - registering service1 for all targets, service requesting initialization:\n"
        "\tservicedbgen -tall -i add service1.xml\n\n"
        " - unregistering service1 from all targets:\n"
        "\tservicedbgen -tall remove service1.xml\n\n"
        " - registering all services for armv5 from default descriptor path:\n"
        "\tservicedbgen -tall -b -c add\n\n"
        " - registering services for all targets from specific descriptor path:\n"
        "\tservicedbgen -tall -b add ./mypath/services\n\n"
        " - removing services for all targets from default descriptor path:\n"
        "\tservicedbgen -tall -b remove\n\n";
    stdoutStream->flush();
}

bool CommandProcessor::setOptions(const QStringList &options, QMap<QString, GeneratorPaths> &targets)
{
    QStringList opts = options;
    QMutableListIterator<QString> i(opts);

    while (i.hasNext()) {
        QString option = i.next();
        if (option == "-b") {
            batchMode = true;
            i.remove();
        }
        else if (option == "-c") {
            deleteDatabase = true;
            i.remove();
        }
        else if (option == "-i") {
            requireInitialization = true;
            i.remove();
        }
        else if (option.startsWith("-t")) {
            QString target = option.right(option.size() - 2).toLower();
            if (target == "all") {
                // copy all target paths
                targets.clear();
                targets = pathMap;
            }
            else if (targetMap[target].isEmpty()) {
                MESSAGE("ERROR: unknown target " << target << '\n');
                return false;
            }
            else if (!targets.contains(targetMap[target])){
                targets[targetMap[target]] = pathMap[targetMap[target]];
            }
            i.remove();
        }
        else if (option.startsWith("-w")) {
            QString wsd = option.right(option.size() - 2).toLower();
            if (wsd == "0") {
                pathMap["emulator"] = wsdMap["nowsd"];
            }
            else if (wsd == "1"){
                pathMap["emulator"] = wsdMap["wsd"];
            }
            else {
                MESSAGE("ERROR: unknown parameter " << wsd << '\n');
                return false;
            }
            if(targets.contains("emulator"))
                targets["emulator"] = pathMap["emulator"];
            i.remove();
        }
    }
    if (!opts.isEmpty()) {
        MESSAGE("Bad options: " << opts.join(" ") << "\n\n");
        return false;
    }
    if (targets.isEmpty()) {
        MESSAGE("No options\n");
        targets = pathMap;
    }

    return true;
}

ServiceDatabase *CommandProcessor::initTargetDatabase(const QString &databasePath, bool deleteDbAllowed)
{
    if (!databasePath.isEmpty()) {

        ServiceDatabase *db = new ServiceDatabase;
        db->setDatabasePath(databasePath + QDir::separator() + dbName() + QLatin1String(".db"));

        if (batchMode && deleteDatabase && deleteDbAllowed) {
            // delete file
            QFile dbFile(db->databasePath());
            dbFile.remove();
        }
        if (db->open())
            return db;
        else
            delete db;
    }
    return NULL;
}

QString CommandProcessor::getServiceFromXML(const QString &xmlFile)
{
    QFile file(xmlFile);
    ServiceMetaData data(0);
    if (file.open(QIODevice::ReadOnly)) {
        data.setDevice(&file);
        if (data.extractMetadata()) {
            return data.parseResults().name;
        }
    }

    return QString();
}

QStringList CommandProcessor::getServiceDescriptors(const QString &path)
{
    QDir dir(QDir::toNativeSeparators(path));
    if (!dir.exists()) {
        return QStringList();
    }
    QStringList fileExt;
    fileExt << "*.xml";
    QStringList files = dir.entryList(fileExt);
    QStringList ret;
    foreach (const QString &f, files) {
        ret << (QDir::toNativeSeparators(path) + QDir::separator() + f);
    }
    return ret;
}

QStringList CommandProcessor::targetServiceDescriptors(const QString &desPath, const QStringList &args)
{
    if (batchMode) {
        // arguments are paths or empty!
        if (args.isEmpty()) {
            // default path for target
            QDir::root().mkpath(desPath);
            return getServiceDescriptors(desPath);
        }
        else {
            // get files from arguments
            QStringList files;
            foreach (const QString &folder, args)
                files << getServiceDescriptors(folder);
            return files;
        }
    }

    // individual mode
    return args;
}

void CommandProcessor::add(const QStringList &desList, ServiceDatabase *db)
{
    if (desList.isEmpty()) {
        MESSAGE("ERROR: No descriptor files/path specified, or the path does not contain service descriptors!\n");
        MESSAGE("Usage:\n\tadd <service-xml-file(s)>[|path-to-service-xmls]\n");
        showUsage();
        return;
    }

    QString securityToken = QString::number(SEC_TOKEN);

    foreach (const QString &serviceXml, desList) {
        QFile f(serviceXml);
        ServiceMetaData parser(&f);
        if (!parser.extractMetadata()) {
            MESSAGE("Parsing error: " << serviceXml << '\n');
            continue;
        }

        const ServiceMetaDataResults results = parser.parseResults();
        db->unregisterService(results.name, securityToken);
        if (db->registerService(results, securityToken)) {
            if (!requireInitialization) {
                // remove the previous installation of the service from the database
                db->serviceInitialized(results.name, securityToken);
            }
            MESSAGE("Service " << results.name << " registered\n");
        }
        else
            MESSAGE("ERROR: Service " << results.name << " registration to " << db->databasePath() << " failed\n");
    }
}

void CommandProcessor::remove(const QStringList &desList, ServiceDatabase *db)
{
    if (desList.isEmpty()) {
        MESSAGE("ERROR: No descriptor files/path specified, or the path does not contain service descriptors!\n");
        MESSAGE("Usage:\n\tremove <service-xml-file(s)|path-to-service-xmls>\n");
        return;
    }

    QString securityToken = QString::number(SEC_TOKEN);

    foreach (const QString &serviceXml, desList) {
        const QString &service = getServiceFromXML(serviceXml);
        if (service.isEmpty()) {
            MESSAGE("ERROR: empty service descriptor or wrong parameter given.\n");
            continue;
        }
        if (db->unregisterService(service, securityToken))
            MESSAGE("Service " << service << " unregistered\n")
        else
            MESSAGE("ERROR: Service " << service << " unregistration from " << db->databasePath() << " failed\n");
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = QCoreApplication::arguments();
    CommandProcessor processor;

    if (!processor.initialize())
        return 1;

    if (args.count() == 1 || args.value(1) == "--help" || args.value(1) == "-h") {
        processor.showUsage();
        return 0;
    }

    QStringList options;
    for (int i=1; i<args.count(); i++) {
        if (args[i].startsWith("-"))
            options += args[i];
    }

    processor.execute(options, args.value(options.count() + 1), args.mid(options.count() + 2));
    return 0;
}

#include "servicedbgen.moc"
