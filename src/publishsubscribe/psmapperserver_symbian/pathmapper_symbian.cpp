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

#include "pathmapper_symbian_p.h"
#include "xqsettingsmanager.h"
#include <QDir>
#include <QFileSystemWatcher>
#include <QTimer>

#include <QDebug>

QTM_BEGIN_NAMESPACE

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
    QString basePath = path;
    QStringList children;
    if (basePath.right(1) == QString(QLatin1Char('/')))
        basePath.chop(1);
    QHashIterator<QString, PathData> i(m_paths);
    XQSettingsManager settingsManager;
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
    if (m_paths.contains(path)) {
        const PathData &data = m_paths.value(path);
        target = data.m_target;
        category = data.m_category;
        key = data.m_key;
        return true;
    }
    return false;
}

#include "moc_pathmapper_symbian_p.cpp"

QTM_END_NAMESPACE
