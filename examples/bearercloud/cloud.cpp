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

#include "cloud.h"

#include <qnetworksession.h>

#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QPixmapCache>

#include <QDebug>

#include <math.h>

Cloud::Cloud(const QNetworkConfiguration &config, QGraphicsItem *parent)
:   QGraphicsItem(parent), configuration(config)
{
    session = new QNetworkSession(configuration);
    connect(session, SIGNAL(newConfigurationActivated()),
            this, SLOT(newConfigurationActivated()));
    connect(session, SIGNAL(stateChanged(QNetworkSession::State)),
            this, SLOT(stateChanged(QNetworkSession::State)));

    icon = new QGraphicsPixmapItem(this);
    text = new QGraphicsTextItem(this);

    currentScale = 0;
    finalScale = 1;
    setTransform(QTransform::fromScale(currentScale, currentScale), false);
    setOpacity(0);

    newConfigurationActivated();
}

Cloud::~Cloud()
{
    delete session;
}

void Cloud::setFinalScale(qreal factor)
{
    finalScale = factor;
}

void Cloud::setOrbit(qreal radius, qreal angle)
{
    finalPos = QPointF(radius * cos(angle), radius * sin(angle));
}

void Cloud::advance(int phase)
{
    if (phase == 0)
        return;

    if (currentScale < finalScale) {
        currentScale = qMin<qreal>(currentScale + 0.01, finalScale);
        setTransform(QTransform::fromScale(currentScale, currentScale), false);
    } else if (currentScale > finalScale) {
        currentScale = qMax<qreal>(currentScale - 0.01, finalScale);
        setTransform(QTransform::fromScale(currentScale, currentScale), false);
    } else if (qFuzzyCompare(currentScale + 1.0, 1.0)) {
        deleteLater();
    }

    QPointF direction = finalPos - pos();
    qreal length = sqrt(direction.rx() * direction.rx() + direction.ry() * direction.ry());
    if (pos() != finalPos) {
        if (length < 1) {
            setPos(finalPos);
        } else {
            direction = direction / length;
            setPos(pos() + direction);
        }
    }

    if (opacity() != finalOpacity) {
        if (length > 1) {
            // use translation as reference
            setOpacity(opacity() + (finalOpacity - opacity()) / length);
        } else if (qAbs(finalScale - currentScale) > 0.0) {
            // use scale as reference
            setOpacity(opacity() + (finalOpacity - opacity()) / qAbs(finalScale - currentScale));
        } else {
            setOpacity(finalOpacity);
        }
    }
}

QRectF Cloud::boundingRect() const
{
    return childrenBoundingRect();
}

void Cloud::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
}

void Cloud::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (session->isActive())
            session->close();
        else
            session->open();

        event->accept();
    }
}

void Cloud::stateChanged(QNetworkSession::State state)
{
    if (configuration.name().isEmpty())
        finalOpacity = qreal(0.1);
    else if (session->state() == QNetworkSession::NotAvailable)
        finalOpacity = 0.5;
    else
        finalOpacity = 1.0;

    QString tooltip;

    if (configuration.name().isEmpty())
        tooltip += tr("<b>HIDDEN NETWORK</b><br>");
    else
        tooltip += tr("<b>%1</b><br>").arg(configuration.name());

    const QNetworkInterface interface = session->interface();
    if (interface.isValid())
        tooltip += tr("<br>Interface: %1").arg(interface.humanReadableName());
    tooltip += tr("<br>Id: %1").arg(configuration.identifier());

    const QString bearerName = session->bearerName();
    if (!bearerName.isEmpty())
        tooltip += tr("<br>Bearer: %1").arg(bearerName);

    QString s = tr("<br>State: %1 (%2)");
    switch (state) {
    case QNetworkSession::Invalid:
        s = s.arg(tr("Invalid"));
        break;
    case QNetworkSession::NotAvailable:
        s = s.arg(tr("Not Available"));
        break;
    case QNetworkSession::Connecting:
        s = s.arg(tr("Connecting"));
        break;
    case QNetworkSession::Connected:
        s = s.arg(tr("Connected"));
        break;
    case QNetworkSession::Closing:
        s = s.arg(tr("Closing"));
        break;
    case QNetworkSession::Disconnected:
        s = s.arg(tr("Disconnected"));
        break;
    case QNetworkSession::Roaming:
        s = s.arg(tr("Roaming"));
        break;
    default:
        s = s.arg(tr("Unknown"));
    }

    if (session->isActive())
        s = s.arg(tr("Active"));
    else
        s = s.arg(tr("Inactive"));

    tooltip += s;

    tooltip += tr("<br><br>Active time: %1 seconds").arg(session->activeTime());
    tooltip += tr("<br>Received data: %1 bytes").arg(session->receivedData());
    tooltip += tr("<br>Sent data: %1 bytes").arg(session->sentData());

    setToolTip(tooltip);
}

void Cloud::newConfigurationActivated()
{
    QPixmap pixmap;
    const QString bearerName = session->bearerName();
    if (!bearerName.isEmpty() && !QPixmapCache::find(bearerName, pixmap)) {
        if (bearerName == QLatin1String("WLAN"))
            pixmap.load(":/wlan.png");
        else if (bearerName == QLatin1String("Ethernet"))
            pixmap.load(":/lan.png");

        QPixmapCache::insert(bearerName, pixmap);
    }

    icon->setPixmap(pixmap);

    if (configuration.name().isEmpty()) {
        text->setPlainText(tr("HIDDEN NETWORK"));
    } else {
        if (configuration.type() == QNetworkConfiguration::ServiceNetwork)
            text->setHtml("<b>" + configuration.name() + "</b>");
        else
            text->setPlainText(configuration.name());
    }

    const qreal height = icon->boundingRect().height() + text->boundingRect().height();

    icon->setPos(icon->boundingRect().width() / -2, height / -2);

    text->setPos(text->boundingRect().width() / -2,
                 height / 2 - text->boundingRect().height());

    stateChanged(session->state());
}

