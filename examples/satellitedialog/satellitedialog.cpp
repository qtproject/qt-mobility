/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "satellitedialog.h"

#include <QtAlgorithms>

#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QPainter>
#include <QSize>
#include <QRectF>
#include <QSizePolicy>
#include <QAction>
#include <QMenuBar>

#include "qgeopositioninfosource.h"
#include "qgeosatelliteinfosource.h"
#include "qgeopositioninfo.h"
#include "qgeosatelliteinfo.h"

class SatelliteWidget : public QWidget
{
public:
    SatelliteWidget(QWidget *parent,
                    SatelliteDialog::Ordering ordering,
                    SatelliteDialog::StrengthScaling scaling);

    void clearSatellites();
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &list);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &list);

    QSize sizeHint() const;

    SatelliteDialog::Ordering ordering() const;
    void setOrdering(SatelliteDialog::Ordering ordering);

    SatelliteDialog::StrengthScaling strengthScaling() const;
    void setStrengthScaling(SatelliteDialog::StrengthScaling scaling);

protected:
    void paintEvent(QPaintEvent *event);

private:
    static const int numBars = 32;
    static const int gapWidth = 1;
    static const int barWidth = 3;
    static const int spanWidth = gapWidth + barWidth;
    static const int borderOffset = 4;
    static const int legendTextOffset = 5;

    int textHeight;
    int legendHeight;

    QColor inUseColor;
    QColor inViewColor;

    SatelliteDialog::Ordering m_ordering;
    SatelliteDialog::StrengthScaling m_scaling;
    int m_maximumSignalStrength;

    void updateSatelliteList();
    void paintSatellite(QPainter &painter, const QRect &bounds, int index);
    void paintLegend(QPainter &painter, const QRect &bounds);

    QList<QGeoSatelliteInfo> satellitesInView;
    QList<QGeoSatelliteInfo> satellitesInUse;
    QList<QPair<QGeoSatelliteInfo, bool> > satellites;
};

SatelliteWidget::SatelliteWidget(QWidget *parent,
                                 SatelliteDialog::Ordering ordering,
                                 SatelliteDialog::StrengthScaling scaling) : QWidget(parent),
        m_ordering(ordering),
        m_scaling(scaling)
{
    QPainter painter(this);
    textHeight = painter.fontMetrics().height();
    legendHeight = borderOffset + textHeight + 2;

    inViewColor = QColor(192, 192, 255);
    inUseColor = QColor(64, 64, 255);
}

SatelliteDialog::Ordering SatelliteWidget::ordering() const
{
    return m_ordering;
}

void SatelliteWidget::setOrdering(SatelliteDialog::Ordering ordering)
{
    if (ordering != m_ordering) {
        m_ordering = ordering;
        updateSatelliteList();
    }
}

SatelliteDialog::StrengthScaling SatelliteWidget::strengthScaling() const
{
    return m_scaling;
}

void SatelliteWidget::setStrengthScaling(SatelliteDialog::StrengthScaling scaling)
{
    if (scaling != m_scaling) {
        m_scaling = scaling;
        updateSatelliteList();
    }
}

bool sortByPrn(const QGeoSatelliteInfo &s1, const QGeoSatelliteInfo &s2)
{
    return s1.prnNumber() < s2.prnNumber();
}

bool sortBySignalStrength(const QGeoSatelliteInfo &s1, const QGeoSatelliteInfo &s2)
{
    return s1.signalStrength() < s2.signalStrength();
}

void SatelliteWidget::clearSatellites()
{
    satellitesInView.clear();
    satellitesInUse.clear();
    updateSatelliteList();
}

void SatelliteWidget::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &list)
{
    satellitesInView = list;
    qSort(satellitesInView.begin(), satellitesInView.end(), sortByPrn);
    updateSatelliteList();
}

void SatelliteWidget::satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &list)
{
    satellitesInUse = list;
    qSort(satellitesInUse.begin(), satellitesInUse.end(), sortByPrn);
    updateSatelliteList();
}

bool sortPairsByPrn(const QPair<QGeoSatelliteInfo, bool> &p1, const QPair<QGeoSatelliteInfo, bool> &p2)
{
    return sortByPrn(p1.first, p2.first);
}

bool sortPairsBySignalStrength(const QPair<QGeoSatelliteInfo, bool> &p1, const QPair<QGeoSatelliteInfo, bool> &p2)
{
    return sortBySignalStrength(p1.first, p2.first);
}

void SatelliteWidget::updateSatelliteList()
{
    satellites.clear();

    int useSize = satellitesInUse.size();
    int viewSize = satellitesInView.size();

    if ((useSize == 0) && (viewSize == 0)) {
        update();
        return;
    }

    for (int i = 0; i < useSize; ++i) {
        if (satellitesInUse.at(i).signalStrength() != 0)
            satellites << QPair<QGeoSatelliteInfo, bool>(satellitesInUse.at(i), true);
    }

    QList<QGeoSatelliteInfo>::iterator end = satellitesInUse.end();

    for (int i = 0; i < viewSize; ++i) {
        if (satellitesInView.at(i).signalStrength() == 0)
            continue;

        QList<QGeoSatelliteInfo>::iterator j =
            qBinaryFind(
                satellitesInUse.begin(),
                end,
                satellitesInView.at(i),
                sortByPrn
            );

        if (j == end) {
            satellites << QPair<QGeoSatelliteInfo, bool>(satellitesInView.at(i), false);
        }
    }

    int satSize = satellites.size();

    if (m_ordering == SatelliteDialog::OrderByPrnNumber) {
        qSort(satellites.begin(), satellites.end(), sortPairsByPrn);
        m_maximumSignalStrength = 0;
        for (int i = 0; i < satSize; ++i) {
            if (satellites.at(i).first.signalStrength() > m_maximumSignalStrength)
                m_maximumSignalStrength = satellites.at(i).first.signalStrength();
        }

    } else {
        qSort(satellites.begin(), satellites.end(), sortPairsBySignalStrength);
        m_maximumSignalStrength = satellites.at(satSize - 1).first.signalStrength();
    }

    update();
}

void SatelliteWidget::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect satBounds = QRect(rect().x() + borderOffset,
                            rect().y() + borderOffset,
                            rect().width() - 2 * borderOffset,
                            rect().height() - legendHeight - 2 * borderOffset);


    painter.setPen(QApplication::palette().color(QPalette::WindowText));
    painter.setBrush(QApplication::palette().color(QPalette::Base));
    painter.drawRect(satBounds);

    int size = satellites.size();
    for (int i = 0; i < size; ++i) {
        paintSatellite(painter, satBounds, i);
    }

    QRect legendBounds = QRect(rect().x() + borderOffset,
                               rect().height() - legendHeight,
                               rect().width() - 2 * borderOffset,
                               legendHeight);

    paintLegend(painter, legendBounds);
}

void SatelliteWidget::paintSatellite(QPainter &painter, const QRect &bounds, int index)
{
    int bars = numBars;
    if (m_ordering == SatelliteDialog::OrderBySignalStrength)
        bars = satellites.size();

    double pixelsPerUnit = (double) bounds.width() / (double)(bars * spanWidth + gapWidth);
    double spanPixels = pixelsPerUnit * spanWidth;
    double gapPixels = pixelsPerUnit * gapWidth;
    double barPixels = pixelsPerUnit * barWidth;

    painter.setPen(QApplication::palette().color(QPalette::WindowText));
    if (!satellites.at(index).second) {
        painter.setBrush(inViewColor);
    } else {
        painter.setBrush(inUseColor);
    }

    int maximum = 100;
    if (m_scaling == SatelliteDialog::ScaleToMaxAvailable) {
        maximum = m_maximumSignalStrength;
    }

    int i = index;
    if (m_ordering == SatelliteDialog::OrderByPrnNumber)
        i = satellites.at(index).first.prnNumber() - 1;

    double height = ((double) satellites.at(index).first.signalStrength() / (double) maximum) * bounds.height();

    QRectF r(bounds.x() + gapPixels + i * spanPixels, bounds.y() + bounds.height() - 1 - height, barPixels, height);

    painter.drawRect(r);
}

void SatelliteWidget::paintLegend(QPainter &painter, const QRect &bounds)
{
    double halfWidth = (double) bounds.width() / 2.0;

    double keyX = bounds.x() + 1;
    double textX = keyX + legendHeight + 2 + legendTextOffset;
    double y = bounds.y() + 1;
    double keyWidth = legendHeight - 2 - borderOffset;
    double textWidth = halfWidth  - legendHeight - 3 - legendTextOffset;
    double height = legendHeight - 2 - borderOffset;

    QRectF viewKeyRect(keyX, y, keyWidth, height);
    QRectF viewTextRect(textX, y, textWidth, height);
    QRectF useKeyRect(keyX + halfWidth, y, keyWidth, height);
    QRectF useTextRect(textX + halfWidth, y, textWidth, height);

    painter.setPen(QApplication::palette().color(QPalette::WindowText));

    painter.setBrush(inViewColor);
    painter.drawRect(viewKeyRect);

    painter.setBrush(inUseColor);
    painter.drawRect(useKeyRect);

    painter.setPen(QApplication::palette().color(QPalette::Text));
    painter.drawText(viewTextRect, Qt::AlignLeft, tr("In View"));
    painter.drawText(useTextRect, Qt::AlignLeft, tr("In Use"));
}


QSize SatelliteWidget::sizeHint() const
{
    return QSize(parentWidget()->width(), parentWidget()->width() / 2 + legendHeight);
}

SatelliteDialog::SatelliteDialog(QWidget *parent,
                                 int noSatelliteTimeoutSeconds,
                                 ExitBehaviour exitBehaviour,
                                 Ordering ordering,
                                 StrengthScaling scaling) : QDialog(parent),
        m_noSatelliteTimeoutSeconds(noSatelliteTimeoutSeconds),
        m_exitBehaviour(exitBehaviour),
        m_ordering(ordering),
        m_scaling(scaling)
{
    noSatelliteTimer = new QTimer(this);
    noSatelliteTimer->setInterval(m_noSatelliteTimeoutSeconds * 1000);
    noSatelliteTimer->setSingleShot(true);

    connect(noSatelliteTimer, SIGNAL(timeout()), this, SLOT(noSatelliteTimeout()));

    satelliteWidget = new SatelliteWidget(this, ordering, scaling);

    QLabel *titleLabel = new QLabel(tr("Satellite Signal Strength"));
    titleLabel->setAlignment(Qt::AlignCenter | Qt::AlignBottom);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(satelliteWidget);

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE)
    QAction *switchAction = new QAction(tr("Switch"), this);
    switchAction->setSoftKeyRole(QAction::PositiveSoftKey);

    connect(switchAction, SIGNAL(triggered()), this, SLOT(switchButtonClicked()));
    addAction(switchAction);

    QAction *cancelAction = new QAction(tr("Cancel"), this);
    cancelAction->setSoftKeyRole(QAction::NegativeSoftKey);

    connect(cancelAction, SIGNAL(triggered()), this, SLOT(reject()));
    addAction(cancelAction);

    QMenuBar *menuBar = new QMenuBar(this);
    menuBar->addAction(switchAction);
    menuBar->addAction(cancelAction);

#if defined(Q_OS_WINCE)
    menuBar->setDefaultAction(cancelAction);
#endif

#else
    cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    switchButton = new QPushButton(tr("Switch"));
    connect(switchButton, SIGNAL(clicked()), this, SLOT(switchButtonClicked()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(switchButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);
#endif

    setLayout(mainLayout);

/*#if defined(Q_OS_SYMBIAN)
    // workaround for QTBUG-4771
    oldTitle = windowTitle();
    connect(this, SIGNAL(finished(int)), this, SLOT(restoreWindowTitle()));
#endif*/

    setWindowTitle(tr("Waiting for GPS fix"));

    setModal(true);
}

/*#if defined(Q_OS_SYMBIAN)
// workaround for QTBUG-4771
void SatelliteDialog::restoreWindowTitle()
{
    setWindowTitle(oldTitle);
}
#endif*/

void SatelliteDialog::connectSources(QGeoPositionInfoSource *posSource, QGeoSatelliteInfoSource *satSource)
{
    connect(posSource, SIGNAL(positionUpdated(const QGeoPositionInfo &)),
            this, SLOT(positionUpdated(const QGeoPositionInfo &)));

    connect(satSource, SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)),
            this, SLOT(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    connect(satSource, SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)),
            this, SLOT(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));
}

void SatelliteDialog::switchButtonClicked()
{
    SatelliteDialog::Ordering o = ordering();
    if (o == SatelliteDialog::OrderByPrnNumber)
        setOrdering(SatelliteDialog::OrderBySignalStrength);
    else if (o == SatelliteDialog::OrderBySignalStrength)
        setOrdering(SatelliteDialog::OrderByPrnNumber);
}

SatelliteDialog::ExitBehaviour SatelliteDialog::exitBehaviour() const
{
    return m_exitBehaviour;
}

void SatelliteDialog::setExitBehaviour(SatelliteDialog::ExitBehaviour exitBehaviour)
{
    m_exitBehaviour = exitBehaviour;
}

SatelliteDialog::Ordering SatelliteDialog::ordering() const
{
    return satelliteWidget->ordering();
}

void SatelliteDialog::setOrdering(SatelliteDialog::Ordering ordering)
{
    satelliteWidget->setOrdering(ordering);
}

SatelliteDialog::StrengthScaling SatelliteDialog::strengthScaling() const
{
    return satelliteWidget->strengthScaling();
}

void SatelliteDialog::setStrengthScaling(SatelliteDialog::StrengthScaling scaling)
{
    satelliteWidget->setStrengthScaling(scaling);
}

void SatelliteDialog::noSatelliteTimeout()
{
    satelliteWidget->clearSatellites();
}

void SatelliteDialog::positionUpdated(const QGeoPositionInfo &/*pos*/)
{
    if (m_exitBehaviour == ExitOnFixOrCancel) {
        accept();
    }
}

void SatelliteDialog::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &list)
{
    noSatelliteTimer->stop();
    satelliteWidget->satellitesInViewUpdated(list);
    noSatelliteTimer->start();
}

void SatelliteDialog::satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &list)
{
    noSatelliteTimer->stop();
    satelliteWidget->satellitesInUseUpdated(list);
    noSatelliteTimer->start();
}


