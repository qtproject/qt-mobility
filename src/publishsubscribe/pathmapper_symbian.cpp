/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "pathmapper_symbian.h"
#include "xqsettingsmanager.h"
#include <QDir>

#include <QDebug>

QTM_BEGIN_NAMESPACE

PathMapper::PathMapper()
{
    const QDir crmlDir("c:\\resource\\qt\\crml");
    QStringList filters;
    filters << "*.qcrml" << "*.confml";
    QStringList files = crmlDir.entryList(filters, QDir::Files);
    foreach (QString fileName, files) {
        QList<KeyData> keyDatas = m_crmlParser.parseQCrml(QDir::toNativeSeparators(crmlDir.filePath(fileName)));
        if (m_crmlParser.error() != QCrmlParser::NoError) {
            qDebug() << "error:" << m_crmlParser.errorString();
        }
        foreach (KeyData keyData, keyDatas) {
            m_paths.insert(keyData.path(), PathData(PathMapper::Target(keyData.target()), keyData.repoId(), keyData.keyId()));
        }
    }
}

PathMapper::~PathMapper()
{
}

bool PathMapper::getChildren(QString path, QSet<QString> &children) const
{
    bool found = false;
    if (path.right(1) != QString(QLatin1Char('/')))
        path += QLatin1Char('/');
    XQSettingsManager settingsManager;
    QHashIterator<QString, PathData> i(m_paths);
    while (i.hasNext()) {
        i.next();
        if (i.key().startsWith(path)) {
            QString value = i.key().mid(path.size());
            int index = value.indexOf(QLatin1Char('/'));
            if (index != -1) {  //Subdirectory
                value = value.mid(0, index);
                children.insert(value);
                found = true;
            } else {    //Leaf
                const PathData &data = i.value();
                PathMapper::Target target = data.m_target;;
                quint32 category = data.m_category;
                quint32 key = data.m_key;
                XQSettingsKey settingsKey(XQSettingsKey::Target(target), (long)category, (unsigned long)key);
                settingsManager.readItemValue(settingsKey);

                if (settingsManager.error() != XQSettingsManager::NotFoundError) {
                    //QString value = i.key().mid(path.size());
                    children.insert(value);
                    found = true;
                }
            }
        }
    }
    return found;
}

bool PathMapper::resolvePath(QString path, Target &target, quint32 &category, quint32 &key) const
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

#include "moc_pathmapper_symbian.cpp"

QTM_END_NAMESPACE
