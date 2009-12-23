
#include "qgeosatellitedialog.h"

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

class QGeoSatelliteWidget : public QWidget
{
public:
    QGeoSatelliteWidget(QWidget *parent, 
            QGeoSatelliteDialog::Ordering ordering, 
            QGeoSatelliteDialog::StrengthScaling scaling);

    void noSatelliteTimeout();
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &list);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &list);    

    QSize sizeHint() const;

    QGeoSatelliteDialog::Ordering ordering() const;
    void setOrdering(QGeoSatelliteDialog::Ordering ordering);

    QGeoSatelliteDialog::StrengthScaling strengthScaling() const;
    void setStrengthScaling(QGeoSatelliteDialog::StrengthScaling scaling);

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

    QGeoSatelliteDialog::Ordering m_ordering;
    QGeoSatelliteDialog::StrengthScaling m_scaling;    
    int m_maximumSignalStrength;

    void updateSatelliteList();
    void paintSatellite(QPainter &painter, const QRect &bounds, int index);
    void paintLegend(QPainter &painter, const QRect &bounds);

    QList<QGeoSatelliteInfo> satellitesInView;
    QList<QGeoSatelliteInfo> satellitesInUse;
    QList<QPair<QGeoSatelliteInfo, bool> > satellites;
};

QGeoSatelliteWidget::QGeoSatelliteWidget(QWidget *parent, 
            QGeoSatelliteDialog::Ordering ordering, 
            QGeoSatelliteDialog::StrengthScaling scaling) : QWidget(parent),
                        m_ordering(ordering),
                        m_scaling(scaling)
{    
    QPainter painter(this);
    textHeight = painter.fontMetrics().height();
    legendHeight = borderOffset + textHeight + 2;    
}

QGeoSatelliteDialog::Ordering QGeoSatelliteWidget::ordering() const
{
    return m_ordering;
}

void QGeoSatelliteWidget::setOrdering(QGeoSatelliteDialog::Ordering ordering)
{
    if (ordering != m_ordering) {
        m_ordering = ordering;
        updateSatelliteList();
    }
}

QGeoSatelliteDialog::StrengthScaling QGeoSatelliteWidget::strengthScaling() const
{
    return m_scaling;
}

void QGeoSatelliteWidget::setStrengthScaling(QGeoSatelliteDialog::StrengthScaling scaling)
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

void QGeoSatelliteWidget::noSatelliteTimeout()
{
    satellitesInView.clear();
    satellitesInUse.clear();
    updateSatelliteList();
}

void QGeoSatelliteWidget::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &list)
{
    satellitesInView = list;
    qSort(satellitesInView.begin(), satellitesInView.end(), sortByPrn);
    updateSatelliteList();
}

void QGeoSatelliteWidget::satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &list)
{
    satellitesInUse = list;
    qSort(satellitesInUse.begin(), satellitesInUse.end(), sortByPrn);
    updateSatelliteList();
}

bool sortPairsByPrn(const QPair<QGeoSatelliteInfo, bool> &p1, const QPair<QGeoSatelliteInfo, bool> &p2 )
{
    return sortByPrn(p1.first, p2.first);
}

bool sortPairsBySignalStrength(const QPair<QGeoSatelliteInfo, bool> &p1, const QPair<QGeoSatelliteInfo, bool> &p2 )
{
    return sortBySignalStrength(p1.first, p2.first);
}

void QGeoSatelliteWidget::updateSatelliteList()
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

    if (m_ordering == QGeoSatelliteDialog::OrderByPrnNumber) {
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

void QGeoSatelliteWidget::paintEvent(QPaintEvent * /*event*/)
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

void QGeoSatelliteWidget::paintSatellite(QPainter &painter, const QRect &bounds, int index)
{    
    int bars = numBars;
    if (m_ordering == QGeoSatelliteDialog::OrderBySignalStrength)
        bars = satellites.size();

    double pixelsPerUnit = (double) bounds.width() / (double) (bars * spanWidth + gapWidth);
    double spanPixels = pixelsPerUnit * spanWidth;
    double gapPixels = pixelsPerUnit * gapWidth;
    double barPixels = pixelsPerUnit * barWidth;

    painter.setPen(QApplication::palette().color(QPalette::WindowText));
    if (!satellites.at(index).second) {
        painter.setBrush(QColor(192, 192, 255));
    } else {
        painter.setBrush(QColor(64, 64, 255));
    }    

    int maximum = 100;
    if (m_scaling == QGeoSatelliteDialog::ScaleToMaxAvailable) {
        maximum = m_maximumSignalStrength;
    }

    int i = index;
    if (m_ordering == QGeoSatelliteDialog::OrderByPrnNumber)
        i = satellites.at(index).first.prnNumber() - 1;

    double height = ((double) satellites.at(index).first.signalStrength() / (double) maximum ) * bounds.height();

    QRectF r(bounds.x() + gapPixels + i * spanPixels, bounds.y() + bounds.height() - 1 - height, barPixels, height);
   
    painter.drawRect(r);
}

void QGeoSatelliteWidget::paintLegend(QPainter &painter, const QRect &bounds)
{
    double halfWidth = (double) bounds.width() / 2.0;

    double keyX = bounds.x() + 1;
    double textX = keyX + legendHeight + 2 + legendTextOffset;
    double y = bounds.y()+ 1;
    double keyWidth = legendHeight - 2 - borderOffset;
    double textWidth = halfWidth  - legendHeight - 3 - legendTextOffset;
    double height = legendHeight - 2 - borderOffset;

    QRectF viewKeyRect(keyX, y, keyWidth, height);    
    QRectF viewTextRect(textX, y, textWidth, height);
    QRectF useKeyRect(keyX + halfWidth, y, keyWidth, height);    
    QRectF useTextRect(textX + halfWidth, y, textWidth, height);

    painter.setPen(QApplication::palette().color(QPalette::WindowText));

    painter.setBrush(QColor(192, 192, 255));    
    painter.drawRect(viewKeyRect);

    painter.setBrush(QColor(64, 64, 255));        
    painter.drawRect(useKeyRect);

    painter.setPen(QApplication::palette().color(QPalette::Text));
    painter.drawText(viewTextRect, Qt::AlignLeft, tr("In View"));
    painter.drawText(useTextRect, Qt::AlignLeft, tr("In Use"));
}


QSize QGeoSatelliteWidget::sizeHint() const
{
    return QSize(parentWidget()->width(), parentWidget()->width()/2 + legendHeight);
}

QGeoSatelliteDialog::QGeoSatelliteDialog(QWidget *parent,
        int noSatelliteTimeoutSeconds,
        ExitBehaviour exitBehaviour, 
        Ordering ordering, 
        StrengthScaling scaling) : QDialog(parent), 
                                    m_noSatelliteTimeoutSeconds(noSatelliteTimeoutSeconds),
                                    m_exitBehaviour(exitBehaviour)
{   
    noSatelliteTimer = new QTimer(this);
    noSatelliteTimer->setInterval(m_noSatelliteTimeoutSeconds * 1000);
    noSatelliteTimer->setSingleShot(true);

    connect(noSatelliteTimer, SIGNAL(timeout()), this, SLOT(noSatelliteTimeout()));

    satelliteWidget = new QGeoSatelliteWidget(this, ordering, scaling);

    QLabel *label = new QLabel(tr("Satellite Signal Strength"));
    label->setAlignment(Qt::AlignCenter);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
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

#if defined(Q_OS_SYMBIAN)
    // workaround for QTBUG-4771
    oldTitle = windowTitle();
    connect(this, SIGNAL(finished(int)), this, SLOT(restoreWindowTitle()));
#endif

    setWindowTitle(tr("Waiting for GPS fix"));

    setModal(true);
}

#if defined(Q_OS_SYMBIAN)
// workaround for QTBUG-4771
void QGeoSatelliteDialog::restoreWindowTitle()
{
    setWindowTitle(oldTitle);
}
#endif

void QGeoSatelliteDialog::connectSources(QGeoPositionInfoSource *posSource, QGeoSatelliteInfoSource *satSource)
{
    connect(posSource, SIGNAL(positionUpdated(const QGeoPositionInfo &)),
            this, SLOT(positionUpdated(const QGeoPositionInfo &)));
    connect(satSource, SIGNAL(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)),
            this, SLOT(satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &)));
    connect(satSource, SIGNAL(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)),
            this, SLOT(satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &)));
}

void QGeoSatelliteDialog::switchButtonClicked()
{
    QGeoSatelliteDialog::Ordering o = ordering();
    if (o == QGeoSatelliteDialog::OrderByPrnNumber)
        setOrdering(QGeoSatelliteDialog::OrderBySignalStrength);
    else if (o == QGeoSatelliteDialog::OrderBySignalStrength)
        setOrdering(QGeoSatelliteDialog::OrderByPrnNumber);
}

QGeoSatelliteDialog::ExitBehaviour QGeoSatelliteDialog::exitBehaviour() const 
{
    return m_exitBehaviour;
}

void QGeoSatelliteDialog::setExitBehaviour(QGeoSatelliteDialog::ExitBehaviour exitBehaviour)
{
    m_exitBehaviour = exitBehaviour;
}

QGeoSatelliteDialog::Ordering QGeoSatelliteDialog::ordering() const
{
    return satelliteWidget->ordering();
}

void QGeoSatelliteDialog::setOrdering(QGeoSatelliteDialog::Ordering ordering)
{
    satelliteWidget->setOrdering(ordering);
}

QGeoSatelliteDialog::StrengthScaling QGeoSatelliteDialog::strengthScaling() const
{
    return satelliteWidget->strengthScaling();
}

void QGeoSatelliteDialog::setStrengthScaling(QGeoSatelliteDialog::StrengthScaling scaling)
{
    satelliteWidget->setStrengthScaling(scaling);
}

void QGeoSatelliteDialog::noSatelliteTimeout()
{
    satelliteWidget->noSatelliteTimeout();
}

void QGeoSatelliteDialog::satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &list)
{
    noSatelliteTimer->stop();
    satelliteWidget->satellitesInViewUpdated(list);        
    noSatelliteTimer->start();
}

void QGeoSatelliteDialog::satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &list)
{
    noSatelliteTimer->stop();
    satelliteWidget->satellitesInUseUpdated(list);
    noSatelliteTimer->start();
}

void QGeoSatelliteDialog::positionUpdated(const QGeoPositionInfo &pos)
{
    if (m_exitBehaviour == ExitOnFixOrCancel) {
        accept();
    }
}

