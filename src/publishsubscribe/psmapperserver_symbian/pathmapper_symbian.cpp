/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include "pathmapper_symbian_p.h"
#include "xqsettingsmanager.h"
#include <QDir>
#include <QFileSystemWatcher>
#include <QTimer>

#include <QDebug>

QTM_BEGIN_NAMESPACE

const QString CrPathPrefix("cr");
const QString PsPathPrefix("ps");
const QString FmPathPrefix("fm");
const QString KeyParameterRaw("?raw");
const QString KeyParameterString("?string");

// check if the path is for numeric access to central repository, publish&subscribe or featuremanager
// (i.e. the path starts with "/cr/", "/ps/" or "/fm/")
static bool isNumericPath(const QString &path)
{
    QStringList pathParts = path.split('/', QString::SkipEmptyParts);
    if (pathParts.size() == 0)
        return false;

    if (pathParts[0] == CrPathPrefix || pathParts[0] == PsPathPrefix || pathParts[0] == FmPathPrefix)
        return true;
    else
        return false;
}

// parse following cases:
//  - numerical access to central repository:
//      * "/cr/<category>/<key>" (f.ex: "/cr/0x100000ab/0x0000000d")
//  - numerical access to publish&subscribe
//      * "/ps/<category>/<key>" (f.ex: "/ps/0x00000edc/0xab000005")
//  - numerical access to featuremanager flags
//      * "/fm/<flag>" (f.ex: "/fm/0x0000001f")

static bool parseNumericPath(const QString &path, PathMapper::Target &target, quint32 &category, quint32 &key)
{
    QStringList pathParts = path.split('/', QString::SkipEmptyParts);
    bool success = false;

    if (pathParts.size() == 3 && (pathParts[0] == CrPathPrefix || pathParts[0] == PsPathPrefix)) {
        target = (pathParts[0] == CrPathPrefix) ? PathMapper::TargetCRepository : PathMapper::TargetRPropery;
        category = pathParts[1].toUInt(&success, 0);
        if (success) {
            key = pathParts[2].toUInt(&success, 0);
        }
    } else if (pathParts.size() == 2 && pathParts[0] == FmPathPrefix) {
        target = PathMapper::TargetFeatureManager;
        category = 0;
        key = pathParts[1].toUInt(&success, 0);
    }

    return success;
}

// Removes url-style parameter from path (only know parameters are removed).
// Example: Path "/cr/0x100012ab/0x1?raw" returns "/cr/0x100012ab/0x1"
static QString removeUrlParameter(const QString &path)
{
    QString returnString = path;
    if (returnString.endsWith(KeyParameterRaw))
        returnString.chop(KeyParameterRaw.size());
    else if (returnString.endsWith(KeyParameterString))
        returnString.chop(KeyParameterString.size());
    
    return returnString;
}

CCRMLDirectoryMonitor::CCRMLDirectoryMonitor() : CActive(EPriorityStandard)
{
    if (m_fs.Connect() == KErrNone) {
        CActiveScheduler::Add(this);
        IssueNotifyChange();
    }
}

CCRMLDirectoryMonitor::~CCRMLDirectoryMonitor()
{
    Cancel();
    m_fs.Close();
}

void CCRMLDirectoryMonitor::IssueNotifyChange()
{
    _LIT(KCRMLDirectory, "?:\\resource\\qt\\crml");
    m_fs.NotifyChange(ENotifyFile, iStatus, KCRMLDirectory);
    SetActive();
}

void CCRMLDirectoryMonitor::RunL()
{
    emit directoryChanged();
    IssueNotifyChange();
}


void CCRMLDirectoryMonitor::DoCancel()
{
    m_fs.NotifyChangeCancel();
}

PathMapper::PathMapper()
{
    m_CRMLDirectoryMonitor = new CCRMLDirectoryMonitor;

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->setInterval(1000);

    connect(m_CRMLDirectoryMonitor, SIGNAL(directoryChanged()), timer, SLOT(start()));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateMappings()));

    updateMappings();
}

PathMapper::~PathMapper()
{
    delete m_CRMLDirectoryMonitor;
}

void PathMapper::updateMappings()
{
    QStringList filters;
    filters << "*.qcrml" << "*.confml";
    m_paths.clear();

    foreach (const QFileInfo &info, QDir::drives()) {
        const QDir crmlDir(info.path() + "resource/qt/crml");

        foreach (const QString &fileName, crmlDir.entryList(filters, QDir::Files)) {
            QList<KeyData> keyDatas = m_crmlParser.parseQCrml(crmlDir.filePath(fileName));
            if (m_crmlParser.error() != QCrmlParser::NoError)
                qDebug() << "error:" << m_crmlParser.errorString();

            foreach (const KeyData &keyData, keyDatas) {
                if (!m_paths.contains(keyData.path())) {
                    m_paths.insert(keyData.path(), PathData(PathMapper::Target(keyData.target()),
                                                            keyData.repoId(), keyData.keyId()));
                }
            }
        }
    }
}

bool PathMapper::getChildren(const QString &path, QSet<QString> &children) const
{
    QString basePath = path;
    if (basePath.right(1) != QString(QLatin1Char('/')))
        basePath += QLatin1Char('/');
    foreach (const QString &foundPath, childPaths(basePath)) {
        QString value = foundPath.mid(basePath.size());
        int index = value.indexOf(QLatin1Char('/'));
        if (index != -1)
            value = value.mid(0, index);
        children.insert(value);
    }
    return children.count() > 0;
}

QStringList PathMapper::childPaths(const QString &path) const
{
    QString basePath = removeUrlParameter(path);
    QStringList children;
    XQSettingsManager settingsManager;

    // In case of numeric cenrep, pubsub and featuremanager access, there can be no childpaths.
    // Just return the original path, if it is valid.
    if (isNumericPath(basePath)) {
        Target target;
        quint32 category;
        quint32 key;
        if (parseNumericPath(basePath, target, category, key)) {
            XQSettingsKey settingsKey(XQSettingsKey::Target(target), (long)category, (unsigned long)key);
            settingsManager.readItemValue(settingsKey);
            if (settingsManager.error() != XQSettingsManager::NotFoundError) {
                children << basePath;
            }
        }
        return children;
    }

    QHashIterator<QString, PathData> i(m_paths);
    while (i.hasNext()) {
        i.next();
        if (i.key().startsWith(basePath)) {
            const PathData &data = i.value();
            PathMapper::Target target = data.m_target;
            quint32 category = data.m_category;
            quint32 key = data.m_key;
            XQSettingsKey settingsKey(XQSettingsKey::Target(target), (long)category, (unsigned long)key);
            settingsManager.readItemValue(settingsKey);
            if (settingsManager.error() != XQSettingsManager::NotFoundError) {
                children << i.key();
            }
        }
    }
    return children;
}

bool PathMapper::resolvePath(const QString &path, Target &target, quint32 &category, quint32 &key) const
{
    QString pathWithoutParameter = removeUrlParameter(path);
    
    if (isNumericPath(pathWithoutParameter))
        return parseNumericPath(pathWithoutParameter, target, category, key);

    if (m_paths.contains(pathWithoutParameter)) {
        const PathData &data = m_paths.value(pathWithoutParameter);
        target = data.m_target;
        category = data.m_category;
        key = data.m_key;
        return true;
    }
    return false;
}

#include "moc_pathmapper_symbian_p.cpp"

QTM_END_NAMESPACE
