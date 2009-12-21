
#ifndef QGEOSATELLITEDIALOG_H
#define QGEOSATELLITEDIALOG_H

#include <QDialog>
#include <QTimer>

#include <qmobilityglobal.h>

#include "qgeopositioninfo.h"
#include "qgeosatelliteinfo.h"

class QGeoSatelliteWidget;
class QPushButton;

QTM_USE_NAMESPACE

class QGeoSatelliteDialog : public QDialog
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

    QGeoSatelliteDialog(QWidget *parent = 0, int noSatelliteTimeoutSeconds = 30, ExitBehaviour exitBehaviour = ExitOnFixOrCancel, Ordering ordering = OrderBySignalStrength, StrengthScaling scaling = ScaleToMaxPossible);

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

private:
    int m_noSatelliteTimeoutSeconds;
    ExitBehaviour m_exitBehaviour;
    QTimer *noSatelliteTimer;

    QGeoSatelliteWidget *satelliteWidget;
    QPushButton *switchButton;
    QPushButton *cancelButton;
};

#endif // #ifndef QGEOSATELLITEDIALOG_H
