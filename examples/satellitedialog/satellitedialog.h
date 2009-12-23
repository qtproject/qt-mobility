
#ifndef QGEOSATELLITEDIALOG_H
#define QGEOSATELLITEDIALOG_H

#include <QObject>
#include <QDialog>
#include <QTimer>

#include <qmobilityglobal.h>

#include "qgeopositioninfosource.h"
#include "qgeosatelliteinfosource.h"
#include "qgeopositioninfo.h"
#include "qgeosatelliteinfo.h"

class SatelliteWidget;
class QPushButton;

QTM_USE_NAMESPACE

class SatelliteDialog : public QDialog
{
    Q_OBJECT

public:
    enum ExitBehaviour {
        ExitOnFixOrCancel,
        ExitOnCancel
    };

    enum Ordering {
        OrderBySignalStrength,
        OrderByPrnNumber
    };

    enum StrengthScaling {
        ScaleToMaxAvailable,
        ScaleToMaxPossible
    };

    SatelliteDialog(QWidget *parent = 0, int noSatelliteTimeoutSeconds = 30, ExitBehaviour exitBehaviour = ExitOnFixOrCancel, Ordering ordering = OrderBySignalStrength, StrengthScaling scaling = ScaleToMaxPossible);

    void connectSources(QGeoPositionInfoSource *posSource, QGeoSatelliteInfoSource *satSource);

    ExitBehaviour exitBehaviour() const;
    void setExitBehaviour(ExitBehaviour exitBehaviour);

    Ordering ordering() const;
    void setOrdering(Ordering ordering);

    StrengthScaling strengthScaling() const;
    void setStrengthScaling(StrengthScaling scaling);

public slots:
    void satellitesInViewUpdated(const QList<QGeoSatelliteInfo> &list);
    void satellitesInUseUpdated(const QList<QGeoSatelliteInfo> &list);
    void positionUpdated(const QGeoPositionInfo &pos);

private slots:
    void switchButtonClicked();
    void noSatelliteTimeout();
#if defined(Q_OS_SYMBIAN)
    // workaround for QTBUG-4771
    void restoreWindowTitle();
#endif

private:
    int m_noSatelliteTimeoutSeconds;
    ExitBehaviour m_exitBehaviour;
    QTimer *noSatelliteTimer;

    SatelliteWidget *satelliteWidget;
    QPushButton *switchButton;
    QPushButton *cancelButton;
#if defined(Q_OS_SYMBIAN)
    // workaround for QTBUG-4771
    QString oldTitle;
#endif
};

#endif // #ifndef QGEOSATELLITEDIALOG_H
