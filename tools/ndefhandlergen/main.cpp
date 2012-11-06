/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QCoreApplication>
#include <QtCore/QResource>
#include <QtCore/QDir>

#include <QtCore/QDebug>

static const QLatin1String applicationNameTag("%APPNAME%");
static const QLatin1String applicationPathTag("%APPPATH%");
static const QLatin1String dataTypeTag("%DATATYPE%");

struct Data
{
    QString templateName;
    QString applicationName;
    QString applicationPath;
    QString dataType;
    QString matchString;
};

void showHelp()
{
    qWarning("Generate platform specific NFC message handler registration files.\n"
             "Usage: ndefhandlergen [options]\n"
             "\n"
             "    -template TEMPLATE    Template to use.\n"
             "    -appname APPNAME      Name of the application.\n"
             "    -apppath APPPATH      Path to installed application binary.\n"
             "    -datatype DATATYPE    URN of the NDEF message type to match.\n"
             "    -match MATCHSTRING    Platform specific match string.\n"
             "\n"
             "The -datatype and -match options are mutually exclusive.\n");

    QDir templates(QLatin1String(":/templates"));
    qWarning("Available templates: %s\n", qPrintable(templates.entryList().join(QLatin1String(", "))));
}

bool processTemplateFile(const QString &templateFile, const QString &outputFile, const Data &data)
{
    QResource resource(templateFile);
    if (!resource.isValid())
        return false;

    QString templateData =
        QString::fromUtf8(reinterpret_cast<const char *>(resource.data()), resource.size());

    templateData.replace(applicationNameTag, data.applicationName);
    templateData.replace(applicationPathTag, data.applicationPath);
    templateData.replace(dataTypeTag, data.matchString);

    QFile output(outputFile);
    if (!output.open(QIODevice::WriteOnly))
        return false;

    output.write(templateData.toUtf8());
    output.close();

    return true;
}

bool generateSymbian(Data data)
{
    const QString outputFile = QLatin1String("ndefhandler_") + data.applicationName +
                               QLatin1String(".xml");

    if (data.matchString.isEmpty())
        data.matchString = data.dataType;

    return processTemplateFile(QLatin1String("/templates/symbian/symbian.xml"), outputFile, data);
}



bool generateHarmattan(Data data)
{
    const QString outputFile = QLatin1String("ndefhandler_") + data.applicationName;

    if (data.matchString.isEmpty())
        data.matchString = QLatin1Char('\'') + data.dataType + QLatin1String("'[1:*];");

    bool success = false;

    success |= processTemplateFile(QLatin1String("/templates/harmattan/harmattan.conf"),
                                   outputFile + QLatin1String(".conf"), data);
    success |= processTemplateFile(QLatin1String("/templates/harmattan/harmattan.service"),
                                   outputFile + QLatin1String(".service"), data);
    success |= processTemplateFile(QLatin1String("/templates/harmattan/harmattan.postinst"),
                                   outputFile + QLatin1String(".postinst"), data);
    success |= processTemplateFile(QLatin1String("/templates/harmattan/harmattan.prerm"),
                                   outputFile + QLatin1String(".prerm"), data);

    return success;
}

bool checkInput(const Data &data)
{
    if (data.templateName.isEmpty()) {
        qWarning("Error: -template option must be provided.\n");
        return false;
    }
    if (data.templateName != QLatin1String("symbian") &&
        data.templateName != QLatin1String("harmattan")) {
        qWarning("Error: Invalid template name specified, %s\n", qPrintable(data.templateName));
        return false;
    }
    if (data.applicationName.isEmpty()) {
        qWarning("Error: -appname option must be provided.\n");
        return false;
    }
    if (data.templateName == QLatin1String("harmattan") && data.applicationPath.isEmpty()) {
        qWarning("Error: -apppath option must be provided.\n");
        return false;
    }
    if (!data.dataType.isEmpty() && !data.matchString.isEmpty()) {
        qWarning("Error: -datatype and -match are mutually exclusive.\n");
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Data data;

    QListIterator<QString> i(a.arguments());
    if (i.hasNext())
        i.next();   // skip over first argument

    if (!i.hasNext()) {
        showHelp();
        return 0;
    }

    while (i.hasNext()) {
        const QString &arg = i.next();

        if (arg == QLatin1String("-template") && i.hasNext()) {
            data.templateName = i.next();
        } else if (arg == QLatin1String("-appname") && i.hasNext()) {
            data.applicationName = i.next();
        } else if (arg == QLatin1String("-apppath") && i.hasNext()) {
            data.applicationPath = i.next();
        } else if (arg == QLatin1String("-datatype") && i.hasNext()) {
            data.dataType = i.next();
        } else if (arg == QLatin1String("-match") && i.hasNext()) {
            data.matchString = i.next();
        } else if (arg == QLatin1String("-help")) {
            showHelp();
            return 0;
        } else {
            qWarning("Unknown Option %s\n", qPrintable(arg));
        }
    }

    // for compatibility
    if (data.templateName == QLatin1String("maemo6"))
        data.templateName = QLatin1String("harmattan");

    if (!checkInput(data)) {
        showHelp();
        return 1;
    }

    bool success = false;

    if (data.templateName == QLatin1String("symbian"))
        success = generateSymbian(data);
    else if (data.templateName == QLatin1String("harmattan"))
        success = generateHarmattan(data);

    return success ? 0 : 1;
}
