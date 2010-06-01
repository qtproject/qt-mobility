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
#include <QApplication>
#include <QStringList>

#if defined(Q_OS_SYMBIAN)
# include <f32file.h>
#endif

QTM_BEGIN_NAMESPACE

class DirChecker
{
public:
    DirChecker();
    ~DirChecker();
    bool checkDir(const QDir& dir);

private:
#if defined(Q_OS_SYMBIAN)
    RFs rfs;
#endif
};

#if defined(Q_OS_SYMBIAN)
DirChecker::DirChecker()
{
    qt_symbian_throwIfError(rfs.Connect());
}

bool DirChecker::checkDir(const QDir& dir)
{
    bool pathFound = false;
    // In Symbian, going cdUp() in a c:/private/<uid3>/ will result in *platsec* error at fileserver (requires AllFiles capability)
    // Also, trying to cd() to a nonexistent directory causes *platsec* error. This does not cause functional harm, but should
    // nevertheless be changed to use native Symbian methods to avoid unnecessary platsec warnings (as per qpluginloader.cpp).
    // Use native Symbian code to check for directory existence, because checking
    // for files from under non-existent protected dir like E:/private/<uid> using
    // QDir::exists causes platform security violations on most apps.
    QString nativePath = QDir::toNativeSeparators(dir.absolutePath());
    TPtrC ptr = TPtrC16(static_cast<const TUint16*>(nativePath.utf16()), nativePath.length());
    TUint attributes;
    TInt err = rfs.Att(ptr, attributes);
    if (err == KErrNone) {
        // yes, the directory exists.
        pathFound = true;
    }
    return pathFound;
}

DirChecker::~DirChecker()
{
    rfs.Close();
}
#else
DirChecker::DirChecker()
{
}

DirChecker::~DirChecker()
{
}

bool DirChecker::checkDir(const QDir &dir)
{
    return dir.exists();
}
#endif

inline QStringList mobilityPlugins(const QString plugintype)
{
#if !defined QT_NO_DEBUG
    const bool showDebug = qgetenv("QT_DEBUG_PLUGINS").toInt() > 0;
#endif

    QStringList paths = QApplication::libraryPaths();
#ifdef QTM_PLUGIN_PATH
    paths << QLatin1String(QTM_PLUGIN_PATH);
#endif
#if !defined QT_NO_DEBUG
    if (showDebug)
        qDebug() << "Plugin paths:" << paths;
#endif

    DirChecker dirChecker;

    //temp variable to avoid multiple identic path
    QSet<QString> processed;

    /* Discover a bunch o plugins */
    QStringList plugins;

    /* Enumerate our plugin paths */
    for (int i=0; i < paths.count(); i++) {
        if (processed.contains(paths.at(i)))
            continue;
        processed.insert(paths.at(i));
        QDir pluginsDir(paths.at(i));
        if (!dirChecker.checkDir(pluginsDir))
            continue;

#if defined(Q_OS_WIN)
        if (pluginsDir.dirName().toLower() == QLatin1String("debug") || pluginsDir.dirName().toLower() == QLatin1String("release"))
            pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
        if (pluginsDir.dirName() == QLatin1String("MacOS")) {
            pluginsDir.cdUp();
            pluginsDir.cdUp();
            pluginsDir.cdUp();
        }
#endif

        QString subdir(QLatin1String("plugins/"));
        subdir += plugintype;
        if (pluginsDir.path().endsWith(QLatin1String("/plugins"))
            || pluginsDir.path().endsWith(QLatin1String("/plugins/")))
            subdir = plugintype;

        if (dirChecker.checkDir(QDir(pluginsDir.path() + QLatin1Char('/') + subdir))) {
            pluginsDir.cd(subdir);
            QStringList files = pluginsDir.entryList(QDir::Files);

#if !defined QT_NO_DEBUG
            if (showDebug)
                qDebug() << "Looking for " << plugintype << " plugins in" << pluginsDir.path() << files;
#endif

            for (int j=0; j < files.count(); j++) {
                plugins <<  pluginsDir.absoluteFilePath(files.at(j));
            }
        }
    }
    return  plugins;
}

QTM_END_NAMESPACE
