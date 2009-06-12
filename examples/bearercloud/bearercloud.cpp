/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "bearercloud.h"
#include "cloud.h"

#include <QGraphicsTextItem>
#include <QTimer>
#include <QDateTime>
#include <QHostInfo>
#include <QDebug>

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static qreal getRadiusFromState(QNetworkConfiguration::StateFlags state)
{
    switch (state) {
    case QNetworkConfiguration::Active:
        return 100;
        break;
    case QNetworkConfiguration::Discovered:
        return 150;
        break;
    case QNetworkConfiguration::Defined:
        return 200;
        break;
    case QNetworkConfiguration::Undefined:
        return 250;
        break;
    default:
        return 300;
    }
}

BearerCloud::BearerCloud(QObject *parent)
:   QGraphicsScene(parent), updateTriggered(false)
{
    setSceneRect(-300, -300, 600, 600);

    qsrand(QDateTime::currentDateTime().toTime_t());

    offset[QNetworkConfiguration::Active] = 2 * M_PI * qrand() / RAND_MAX;
    offset[QNetworkConfiguration::Discovered] = offset[QNetworkConfiguration::Active] + M_PI / 6;
    offset[QNetworkConfiguration::Defined] = offset[QNetworkConfiguration::Discovered] - M_PI / 6;
    offset[QNetworkConfiguration::Undefined] = offset[QNetworkConfiguration::Undefined] + M_PI / 6;

    thisDevice = new QGraphicsTextItem(QHostInfo::localHostName());
    thisDevice->setPos(thisDevice->boundingRect().width() / -2,
                       thisDevice->boundingRect().height() / -2);
    addItem(thisDevice);

    qreal radius = getRadiusFromState(QNetworkConfiguration::Active);
    QGraphicsEllipseItem *orbit = new QGraphicsEllipseItem(-radius, -radius, 2*radius, 2*radius);
    orbit->setPen(QColor(Qt::green));
    addItem(orbit);
    radius = getRadiusFromState(QNetworkConfiguration::Discovered);
    orbit = new QGraphicsEllipseItem(-radius, -radius, 2*radius, 2*radius);
    orbit->setPen(QColor(Qt::blue));
    addItem(orbit);
    radius = getRadiusFromState(QNetworkConfiguration::Defined);
    orbit = new QGraphicsEllipseItem(-radius, -radius, 2*radius, 2*radius);
    orbit->setPen(QColor(Qt::darkGray));
    addItem(orbit);
    radius = getRadiusFromState(QNetworkConfiguration::Undefined);
    orbit = new QGraphicsEllipseItem(-radius, -radius, 2*radius, 2*radius);
    orbit->setPen(QColor(Qt::lightGray));
    addItem(orbit);

    triggerUpdate();

    connect(&manager, SIGNAL(updateCompleted()), this, SLOT(updateConfigurations()));
    connect(&manager, SIGNAL(configurationAdded(QNetworkConfiguration)),
            this, SLOT(triggerUpdate()));
    connect(&manager, SIGNAL(configurationRemoved(QNetworkConfiguration)),
            this, SLOT(triggerUpdate()));
    connect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
            this, SLOT(triggerUpdate()));

    QTimer *animationTimer = new QTimer(this);
    connect(animationTimer, SIGNAL(timeout()), this, SLOT(advance()));
    animationTimer->start(1000 / 33);
}

BearerCloud::~BearerCloud()
{
}

void BearerCloud::updateConfigurations()
{
    // find and delete removed configurations
    QList<QNetworkConfiguration> allConfigurations = manager.allConfigurations();

    QMultiMap<QNetworkConfiguration::StateFlags, QString> configStates;

    QList<QString> previousIds = configurations.keys();
    for (int i = 0; i < allConfigurations.count(); ++i) {
        const QNetworkConfiguration &config = allConfigurations.at(i);

        if (configurations.contains(config.identifier())) {
            // configurations updated
            previousIds.removeOne(config.identifier());

            configStates.insert(config.state(), config.identifier());
        } else {
            // new configuration
            configStates.insert(config.state(), config.identifier());
        }
    }
    while (!previousIds.isEmpty()) {
        // delete configuration
        Cloud *item = configurations.take(previousIds.takeFirst());
        item->setFinalScale(0.0);
        item->setDeleteAfterAnimation(true);
    }

    foreach (const QNetworkConfiguration::StateFlags &state, configStates.uniqueKeys()) {
        const qreal radius = getRadiusFromState(state);
        const qreal angle = 2 * M_PI / configStates.count(state);

        QList<QString> identifiers = configStates.values(state);
        for (int i = 0; i < identifiers.count(); ++i) {
            if (configurations.contains(identifiers.at(i))) {
                // update animation
                Cloud *item = configurations.value(identifiers.at(i));

                item->setOrbit(radius, i * angle + offset[state]);
            } else {
                // new animation
                QNetworkConfiguration config =
                    manager.configurationFromIdentifier(identifiers.at(i));
                Cloud *item = new Cloud(config);

                configurations.insert(identifiers.at(i), item);

                item->setPos(radius * cos(i * angle + offset[state]),
                             radius * sin(i * angle + offset[state]));
                item->setOrbit(radius, i * angle + offset[state]);

                addItem(item);
            }
        }
    }

    updateTriggered = false;
}

void BearerCloud::triggerUpdate()
{
    if (!updateTriggered) {
        QTimer::singleShot(0, this, SLOT(updateConfigurations()));
        updateTriggered = true;
    }
}

