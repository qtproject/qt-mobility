/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <stdio.h>

#include <QtCore>
#include <QTextStream>
#include <qservicemanager.h>
#include <QString>

QT_USE_NAMESPACE

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
    void browse(const QStringList &args);
    void search(const QStringList &args);
    void add(const QStringList &args);
    void remove(const QStringList &args);

private:
    bool setOptions(const QStringList &options);
    void showAllEntries();
    void showInterfaceInfo(const QServiceFilter &filter);
    void showServiceInfo(const QString &service);

    QServiceManager *serviceManager;
    QTextStream *stdoutStream;
};

CommandProcessor::CommandProcessor(QObject *parent)
    : QObject(parent),
      serviceManager(0),
      stdoutStream(new QTextStream(stdout))
{
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
    *stream << "Usage: servicefw [options] <command> [command parameters]\n\n"
            "Commands:\n"
            "\tbrowse     List all registered services\n"
            "\tsearch     Search for a service or interface\n"
            "\tadd        Register a service\n"
            "\tremove     Unregister a service\n"
            "\n"
            "Options:\n"
            "\t--system   Use the system-wide services database instead of the\n"
            "\t           user-specific database\n"
            "\n";
}

void CommandProcessor::browse(const QStringList &args)
{
    Q_UNUSED(args);
    showAllEntries();
}

void CommandProcessor::search(const QStringList &args)
{
    if (args.isEmpty()) {
        *stdoutStream << "Usage:\n\tsearch <service|interface [version]>\n\n"
                "Examples:\n"
                "\tsearch SomeService\n"
                "\tsearch com.nokia.SomeInterface\n"
                "\tsearch com.nokia.SomeInterface 3.5\n"
                "\tsearch com.nokia.SomeInterface 3.5+\n";
        return;
    }

    const QString &name = args[0];
    if (name.contains('.')) {
        QServiceFilter filter;
        if (args.count() > 1) {
            const QString &version = args[1];
            bool minimumMatch = version.endsWith('+');
            filter.setInterface(name,
                    minimumMatch ? (version.mid(0, version.length()-1)) : version,
                    minimumMatch ? QServiceFilter::MinimumVersionMatch : QServiceFilter::ExactVersionMatch);
            if (filter.interfaceName().isEmpty()) { // setInterface() failed
                *stdoutStream << "Error: invalid interface version: " << version << '\n';
                return;
            }
        } else {
            filter.setInterface(name);
        }
        showInterfaceInfo(filter);
    } else {
        showServiceInfo(name);
    }
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
        *stdoutStream << "Usage:\n\tadd <service-xml-file>\n";
        return;
    }

    const QString &xmlPath = args[0];
    if (!QFile::exists(xmlPath)) {
        *stdoutStream << "Error: cannot find file " << xmlPath << '\n';
        return;
    }

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

void CommandProcessor::remove(const QStringList &args)
{
    if (args.isEmpty()) {
        *stdoutStream << "Usage:\n\tremove <service-name>\n";
        return;
    }

    const QString &service = args[0];
    if (serviceManager->removeService(service))
        *stdoutStream << "Unregistered service " << service << '\n';
    else
        *stdoutStream << "Error: cannot unregister service " << service << '\n';
}

bool CommandProcessor::setOptions(const QStringList &options)
{
    if (serviceManager)
        delete serviceManager;

    QStringList opts = options;
    QMutableListIterator<QString> i(opts);
    while (i.hasNext()) {
        if (i.next() == "--system") {
            serviceManager = new QServiceManager(QService::SystemScope, this);
            i.remove();
        }
    }

    if (!opts.isEmpty()) {
        *stdoutStream << "Bad options: " << opts.join(" ") << "\n\n";
        showUsage();
        return false;
    }

    // other initialization, if not triggered by an option
    if (!serviceManager)
        serviceManager = new QServiceManager(this);

    return true;
}

void CommandProcessor::showAllEntries()
{
    QStringList services = serviceManager->findServices();
    if (services.isEmpty())
        *stdoutStream << "No services found.\n";
    else if (services.count() == 1)
        *stdoutStream << "Found 1 service.\n\n";
    else
        *stdoutStream << "Found " << services.count() << " services.\n\n";
    foreach (const QString &service, services)
        showServiceInfo(service);
}

void CommandProcessor::showInterfaceInfo(const QServiceFilter &filter)
{
    QString interface = filter.interfaceName();
    if (filter.majorVersion() >= 0 && filter.minorVersion() >= 0) {
        interface += QString(" %1.%2").arg(filter.majorVersion()).arg(filter.minorVersion());
        if (filter.versionMatchRule() == QServiceFilter::MinimumVersionMatch)
            interface += '+';
    }

    QList<QServiceInterfaceDescriptor> descriptors = serviceManager->findInterfaces(filter);
    if (descriptors.isEmpty()) {
        *stdoutStream << "Interface " << interface << " not found.\n";
        return;
    }

    *stdoutStream << interface << " is implemented by:\n";
    foreach (const QServiceInterfaceDescriptor &desc, descriptors)
        *stdoutStream << '\t' << desc.serviceName() << '\n';
}

void CommandProcessor::showServiceInfo(const QString &service)
{
    QList<QServiceInterfaceDescriptor> descriptors = serviceManager->findInterfaces(service);
    if (descriptors.isEmpty()) {
        *stdoutStream << "Service " << service << " not found.\n";
        return;
    }

    QString description = descriptors[0].attribute(
            QServiceInterfaceDescriptor::ServiceDescription).toString();
    QStringList capabilities = descriptors[0].attribute(
            QServiceInterfaceDescriptor::Capabilities).toStringList();

    *stdoutStream << service << ":\n";
    if (!description.isEmpty())
        *stdoutStream << '\t' << description << '\n';
    *stdoutStream << "\tLibrary: " << descriptors[0].attribute(
                    QServiceInterfaceDescriptor::Location).toString() << '\n'
            << "\tCapabilities: " << (capabilities.isEmpty() ? "" : capabilities.join(", ")) << '\n'
            << "\tImplements:\n";

    foreach (const QServiceInterfaceDescriptor &desc, descriptors) {
        *stdoutStream << "\t\t" << desc.interfaceName() << ' '
                << desc.majorVersion() << '.' << desc.minorVersion() << '\n';
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

#include "servicefw.moc"
