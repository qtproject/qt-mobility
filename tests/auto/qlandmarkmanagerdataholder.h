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

#ifndef QLANDMARKMANAGERDATAHOLDER_H
#define QLANDMARKMANAGERDATAHOLDER_H

#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QString>
#include <QDebug>

#include "qlandmarkmanager.h"

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QTM_BEGIN_NAMESPACE
class QLandmark;
class QLandmarkManagerDataHolder
{
public:
    QLandmarkManagerDataHolder()
    {
        QLandmarkManager *lm = new QLandmarkManager();
        if (lm) {
            savedCategories = lm->categories();
            savedLandmarks = lm->landmarks();

            for (int i=0; i < savedLandmarks.count(); ++i) {
                QList<int> indexes;
                for (int j=0; j < savedLandmarks.at(i).categoryIds().count(); ++j) {
                    for (int k=0; k < savedCategories.count(); ++k) {
                        if (savedCategories.at(k).categoryId() == savedLandmarks.at(i).categoryIds().at(j)) {
                            indexes.append(k);
                        }
                    }
                }
                landmarkCategoryIndex.append(indexes);
            }

            for (int i=0; i < savedCategories.count(); ++i) {
                lm->removeCategory(savedCategories.at(i));
            }

            lm->removeLandmarks(lm->landmarkIds());
            delete lm;
        }
    }

    ~QLandmarkManagerDataHolder()
    {
        QLandmarkManager* lm = new QLandmarkManager();
        if (lm) {
            qDebug() << "Restoring landmarks for" << lm->managerName();
            for (int i=0; i < savedCategories.count(); ++i) {
                if (!lm->isReadOnly(savedCategories[i].categoryId())) {
                    savedCategories[i].setCategoryId(QLandmarkCategoryId());
                    lm->saveCategory(&(savedCategories[i]));
                }
            }

            for (int i=0; i < savedLandmarks.count(); ++i) {
                savedLandmarks[i].setLandmarkId(QLandmarkId());
                savedLandmarks[i].setCategoryIds(QList<QLandmarkCategoryId>());
                for (int j=0; j < landmarkCategoryIndex.at(i).count(); ++j) {
                    savedLandmarks[i].addCategoryId(savedCategories.at(landmarkCategoryIndex.at(i).at(j)).categoryId());
                }
                lm->saveLandmark(&(savedLandmarks[i]));

            }
            delete lm;
        }
    }
private:
    QList<QLandmark> savedLandmarks;
    QList<QLandmarkCategory> savedCategories;
    QList<QList<int> > landmarkCategoryIndex;
};

QTM_END_NAMESPACE

#endif

