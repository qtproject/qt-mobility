#include <stdio.h>
#include <QtCore>
#include <QtSql>
#include <servicedatabase_p.h>
#include <servicemetadata_p.h>
#include <databasemanager_p.h>

QTM_USE_NAMESPACE

#define WINSCW_DES_DEPLOY       "/epoc32/winscw/c/private/2002AC7F/Nokia/des"
#define ARMV5_DES_DEPLOY        "/epoc32/data/z/private/2002AC7F/Nokia/des"
#define WINSCW_DBPATH           "/epoc32/winscw/c/data/temp/QtServiceFW"
#define ARMV5_DBPATH            "/epoc32/data/c/private/2002AC7F/Nokia"

#define SEC_TOKEN               0x101FB657
#define TOOL_VERSION            "0.3"

#define TRPRINT(txt)  *stdoutStream << txt
#define TRPRINTL(lst) {foreach(const QString& _txt, lst) TRPRINT(_txt << '\n');}

QString dbName()
{
    QString qtVersion(qVersion());
    qtVersion = qtVersion.left(qtVersion.size() -2); //strip off patch version
    return QString("QtServiceFramework_") + qtVersion + "_system";
}

class CommandProcessor : public QObject
{
    Q_OBJECT

public:
    CommandProcessor(QObject *parent = 0);
    ~CommandProcessor();

    void execute(const QStringList &options, const QString &cmd, const QStringList &args);
    void showUsage();

public slots:
    void add(const QStringList &desList, const QString &onTarget);
    void remove(const QStringList &desList, const QString &onTarget);

private:
    bool setOptions(const QStringList &options);
    ServiceDatabase *initTargetDatabase(const QString &aTarget, bool deleteDbAllowed);
    QString getServiceFromXML(const QString &xmlFile);
    QStringList getServiceDescriptors(const QString &path);
    QStringList targetServiceDescriptors(const QString &specTarget, const QStringList &args);

    bool batchMode;
    bool requireInitialization;
    bool deleteDatabase;
    QString platform;
    QTextStream *stdoutStream;
    QMap<QString, QString> dbMap;
    QMap<QString, QString> deployMap;
};

CommandProcessor::CommandProcessor(QObject *parent)
    : QObject(parent),
      batchMode(false),
      requireInitialization(false),
      deleteDatabase(false),
      platform("all"),
      stdoutStream(new QTextStream(stdout))
{
    dbMap["winscw"] = WINSCW_DBPATH;
    deployMap["winscw"] = WINSCW_DES_DEPLOY;

    dbMap["armv5"] = ARMV5_DBPATH;
    deployMap["armv5"] = ARMV5_DES_DEPLOY;
}

CommandProcessor::~CommandProcessor()
{
    delete stdoutStream;
}

void CommandProcessor::execute(const QStringList &options, const QString &cmd, const QStringList &args)
{
    if (cmd.isEmpty()) {
        *stdoutStream << "Error: no command given\n\n";
        showUsage();
        return;
    }

    if (!setOptions(options)) {
        showUsage();
        return;
    }

    int methodIndex = metaObject()->indexOfMethod(cmd.toAscii() + "(QStringList,QString)");
    if (methodIndex < 0) {
        *stdoutStream << "Bad command: " << cmd << "\n\n";
        showUsage();
        return;
    }

    if (platform == "all") {
        // perform on all targets
        QMapIterator<QString, QString> i(dbMap);
        while (i.hasNext()) {
            i.next();
            const QString &tgt = i.key();
            if (batchMode) {
                // register services from the given (or default target) path
                metaObject()->method(methodIndex).invoke(this, Q_ARG(QStringList, targetServiceDescriptors(tgt, args)), Q_ARG(QString, tgt));
            }
            else {
                // register individual services
                metaObject()->method(methodIndex).invoke(this, Q_ARG(QStringList, args), Q_ARG(QString, tgt));
            }
        }
    }
    else {
        // perform on a single target
        if (batchMode) {
            metaObject()->method(methodIndex).invoke(this, Q_ARG(QStringList, targetServiceDescriptors(platform, args)), Q_ARG(QString, platform));
        }
        else
            metaObject()->method(methodIndex).invoke(this, Q_ARG(QStringList, args), Q_ARG(QString, platform));
    }
}

void CommandProcessor::showUsage()
{
    *stdoutStream << "Service framework database management tool, version " << QString(TOOL_VERSION) << "\n"
            "Usage: servicedbgen [options] <command> [command parameters]\n\n"
            "Commands:\n"
            "\tadd        Register or update a service\n"
            "\tremove     Unregister a service\n"
            "\nOptions:\n"
            "\t-t<target> Specify the target platform to be used (e.g. winscw, armv5, all).\n"
            "\t-b         Batch mode (take service descriptors from a path). If specified, the \n"
            "\t           command parameters are specifying the path to teh folder where the.\n"
            "\t           service descriptors reside. In case command parameters are not given,\n"
            "\t           the target specific default path will be taken (see paths below).\n"
            "\t-c         Delete the service database, has effect only in batch mode. \n"
            "\t-i         The services require initialization upon first load.\n"
            "\n"
            "Supported targets and their database paths:";
    QMapIterator<QString, QString> i(dbMap);
    while (i.hasNext()) {
        i.next();
        *stdoutStream << "\nTarget: " << i.key() << "\n\tpath: " << i.value() << "\n\tdeployment: " << deployMap[i.key()] << '\n';
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
}

bool CommandProcessor::setOptions(const QStringList &options)
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
            platform = option.right(option.size() - 2).toLower();
            *stdoutStream << "Target: " << platform << '\n';
            i.remove();
        }
    }
    if (!opts.isEmpty()) {
        *stdoutStream << "Bad options: " << opts.join(" ") << "\n\n";
        return false;
    }

    // check the target string and initialize
    if ((platform != "all") && dbMap[platform].isEmpty()) {
        *stdoutStream << "ERROR: unknown target" << platform << '\n';
        return false;
    }

    return true;
}

ServiceDatabase *CommandProcessor::initTargetDatabase(const QString &aTarget, bool deleteDbAllowed)
{
    QString tgtPath = dbMap[aTarget];
    if (!tgtPath.isEmpty()) {

        ServiceDatabase *db = new ServiceDatabase;
        db->setDatabasePath(tgtPath + QDir::separator() + dbName() + QLatin1String(".db"));

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

QStringList CommandProcessor::targetServiceDescriptors(const QString &specTarget, const QStringList &args)
{
    if (batchMode) {
        // arguments are paths or empty!
        if (args.isEmpty()) {
            // default path for target
            QDir::root().mkpath(deployMap[specTarget]);
            return getServiceDescriptors(deployMap[specTarget]);
        }
        else {
            // get files from arguments
            QStringList files;
            foreach (const QString &folder, args) {
                files << getServiceDescriptors(folder);
            }
            return files;
        }
    }

    // individual mode
    return args;
}

void CommandProcessor::add(const QStringList &desList, const QString &onTarget)
{
    if (desList.isEmpty()) {
        *stdoutStream << "ERROR: No descriptor files/path specified, or the path does not contain service descriptors!\n";
        *stdoutStream << "Usage:\n\tadd <service-xml-file(s)>[|path-to-service-xmls]\n";
        showUsage();
        return;
    }

    ServiceDatabase *db = initTargetDatabase(onTarget, true);
    if (!db) {
        *stdoutStream << "ERROR: database cannot be opened/created.\n";
        return;
    }
    QString securityToken = QString::number(SEC_TOKEN);

    foreach (const QString &serviceXml, desList) {
        QFile f(serviceXml);
        ServiceMetaData parser(&f);
        if (!parser.extractMetadata()) {
            *stdoutStream << "Parsing error: " << serviceXml;
            continue;
        }

        const ServiceMetaDataResults results = parser.parseResults();
        db->unregisterService(results.name, securityToken);
        if (db->registerService(results, securityToken)) {
            if (!requireInitialization) {
                // remove the previous installation of the service from the database
                db->serviceInitialized(results.name, securityToken);
            }
            *stdoutStream << "Service " << results.name << " registered to " << onTarget << '\n';
        }
        else
            *stdoutStream << "ERROR: Service " << results.name << " registration to " << onTarget << " failed\n";
    }
    // remove database so other targets can be handled
    QSqlDatabase::removeDatabase(db->m_connectionName);
    delete db;
}

void CommandProcessor::remove(const QStringList &desList, const QString &onTarget)
{
    if (desList.isEmpty()) {
        *stdoutStream << "ERROR: No descriptor files/path specified, or the path does not contain service descriptors!\n";
        *stdoutStream << "Usage:\n\tremove <service-xml-file(s)|path-to-service-xmls>\n";
        return;
    }

    ServiceDatabase *db = initTargetDatabase(onTarget, false);
    if (!db) {
        *stdoutStream << "ERROR: database cannot be opened.\n";
        return;
    }
    QString securityToken = QString::number(SEC_TOKEN);

    foreach (const QString &serviceXml, desList) {
        const QString &service = getServiceFromXML(serviceXml);
        if (db->unregisterService(service, securityToken))
            *stdoutStream << "Service " << service << " unregistered from " << onTarget << '\n';
        else
            *stdoutStream << "ERROR: Service " << service << " unregistration from " << onTarget << " failed\n";
    }
    // remove database so other targets can be handled
    QSqlDatabase::removeDatabase(db->m_connectionName);
    delete db;
}


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = QCoreApplication::arguments();
    CommandProcessor processor;

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
