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

#include <qnetworkconfiguration.h>
#include <qnetworksession.h>

#include <QGraphicsItem>

class QGraphicsTextItem;
class QGraphicsPixmapItem;

class Cloud : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    Cloud(const QNetworkConfiguration &config, QGraphicsItem *parent = 0);
    ~Cloud();

    void setFinalScale(qreal factor);
    void setOrbit(qreal radius, qreal angle);
    void setDeleteAfterAnimation(bool deleteAfter);

    void advance(int phase);
    QRectF boundingRect() const;
    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private Q_SLOTS:
    void stateChanged(QNetworkSession::State state);
    void newConfigurationActivated();
    QString configurationStateToString(QNetworkConfiguration::StateFlags state);
    QString sessionStateToString(QNetworkSession::State state);

private:
    QNetworkConfiguration configuration;
    QNetworkSession *session;

    QGraphicsTextItem *text;
    QGraphicsPixmapItem *icon;

    QPointF finalPos;
    qreal finalOpacity;
    qreal finalScale;
    qreal currentScale;

    bool deleteAfterAnimation;
};

