#include <stdio.h>
#include <QtCore>
#include <qservicemanager.h>
#include <servicemetadata_p.h>

QTM_USE_NAMESPACE

class CommandProcessor : public QObject
{
    Q_OBJECT

public:
    CommandProcessor(QObject *parent = 0);
    ~CommandProcessor();

    void execute(const QStringList &options, const QString &cmd, const QStringList &args);
    void showUsage();
    static void showUsage(QTextStream *stream);

public slots:
    void add(const QStringList &args);
    void remove(const QStringList &args);
    void update(const QStringList &args);
    void load(const QStringList &args);

private:
    bool setOptions(const QStringList &options);
    QString getServiceFromXML(const QString &xmlFile);

    QTextStream *stdoutStream;
    bool paramIsPath;
    QServiceManager *serviceManager;
};

CommandProcessor::CommandProcessor(QObject *parent)
    : QObject(parent),
      stdoutStream(new QTextStream(stdout)),
      paramIsPath(false)
{
    serviceManager = new QServiceManager(this);
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

    if (!setOptions(options))
        return;

    int methodIndex = metaObject()->indexOfMethod(cmd.toAscii() + "(QStringList)");
    if (methodIndex < 0) {
        *stdoutStream << "Bad command: " << cmd << "\n\n";
        showUsage();
        return;
    }

    if (!metaObject()->method(methodIndex).invoke(this, Q_ARG(QStringList, args)))
        *stdoutStream << "Cannot invoke method for command:" << cmd << '\n';
}

void CommandProcessor::showUsage()
{
    showUsage(stdoutStream);
}

void CommandProcessor::showUsage(QTextStream *stream)
{
    *stream << "Usage: servicedbgen [options] <command> [command parameters]\n\n"
            "Commands:\n"
            "\tadd        Register a service\n"
            "\tremove     Unregister a service\n"
            "\tupdate     Updates a service (developement time)\n"
            "\nOptions:\n"
            "\t-p|--path  The command parameters are folders"
            "\n";
}

bool CommandProcessor::setOptions(const QStringList &options)
{
    QStringList opts = options;
    QMutableListIterator<QString> i(opts);
    while (i.hasNext()) {
        if ((i.next() == "--path") || (i.next() == "-p")) {
            paramIsPath = true;
            i.remove();
        }
    }

    if (!opts.isEmpty()) {
        *stdoutStream << "Bad options: " << opts.join(" ") << "\n\n";
        showUsage();
        return false;
    }

    return true;
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




static const char * const errorTable[] = {
    "No error", //0
    "Storage read error",
    "Invalid service location",
    "Invalid service xml",
    "Invalid service interface descriptor",
    "Service already exists",
    "Interface implementation already exists",
    "Loading of plug-in failed",
    "Service or interface not found",
    "Insufficient capabilities to access service",
    "Unknown error"
};

void CommandProcessor::add(const QStringList &args)
{
    if (args.isEmpty()) {
        *stdoutStream << "Usage:\n\tadd <service-xml-file|path-to-service-xmls>\n";
        return;
    }

    const QString &xmlPath = args[0];
    if (!QFile::exists(xmlPath)) {
        *stdoutStream << "Error: cannot find file/path " << xmlPath << '\n';
        return;
    }

    if (paramIsPath)
    {
        // load xmls from the folder and register them one-by-one
        // TODO
    }
    else
    {
        // Service Manager based code
        if (serviceManager->addService(xmlPath)) {
            *stdoutStream << "Registered service at " << xmlPath << '\n';
        } else {
            int error = serviceManager->error();
            if (error > 11) //map anything larger than 11 to 11
                error = 11;
            *stdoutStream << "Error: cannot register service at " << xmlPath
                    << " (" << errorTable[error] << ")" << '\n';
        }
    }
}

void CommandProcessor::remove(const QStringList &args)
{
    if (args.isEmpty()) {
        *stdoutStream << "Usage:\n\tremove <service-xml-file|path-to-service-xmls>\n";
        return;
    }

    if (paramIsPath)
    {

    }
    else
    {
        QString service = getServiceFromXML(args[0]);
        if (serviceManager->removeService(service))
            *stdoutStream << "Unregistered service " << service << '\n';
        else
            *stdoutStream << "Error: cannot unregister service " << service << '\n';
    }
}

void CommandProcessor::update(const QStringList &args)
{
    if (args.isEmpty()) {
        *stdoutStream << "Usage:\n\tupdate <service-xml-file|path-to-service-xmls>\n";
        return;
    }

    remove(args);
    add(args);
}

void CommandProcessor::load(const QStringList &args)
{
    if (args.isEmpty() || args.size() < 2) {
        *stdoutStream << "Usage:\\n\tload <service-name> <interface_uri> [interface-version]\n";
    } else {
        QServiceFilter filter;
        filter.setServiceName(args[0]);
        if (args.size() > 2)
            filter.setInterface(args[1], args[2]);
        else
            filter.setInterface(args[1]);
        QList<QServiceInterfaceDescriptor> list = serviceManager->findInterfaces(filter);
        QObject *obj = serviceManager->loadInterface(list[0]);
        if (obj) {
            obj->setParent(this);
            *stdoutStream << "Interface " << args[1] << " loaded from service " << args[0];
        } else {
            int error = serviceManager->error();
            if (error > 11) //map anything larger than 11 to 11
                error = 11;
            *stdoutStream << "Error: cannot load interface " << args[1] << " from service " << args[0]
                    << " (" << errorTable[error] << ")" << '\n';
        }
    }
}



int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = QCoreApplication::arguments();

    if (args.count() == 1 || args.value(1) == "--help" || args.value(1) == "-h") {
        QTextStream stream(stdout);
        CommandProcessor::showUsage(&stream);
        return 0;
    }

    QStringList options;
    for (int i=1; i<args.count(); i++) {
        if (args[i].startsWith("--"))
            options += args[i];
    }

    CommandProcessor processor;
    processor.execute(options, args.value(options.count() + 1), args.mid(options.count() + 2));
    return 0;
}

#include "servicedbgen.moc"
