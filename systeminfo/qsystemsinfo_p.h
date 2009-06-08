#ifndef QSYSTEMSINFO_P_H
#define QSYSTEMSINFO_P_H

#include <QObject>
#include <QSize>

#include "qsystemsinfo.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QStringList;

class QSystemsInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemsInfoPrivate(QObject *parent = 0);

//    QSystemsInfo::Error error() const;

// general
    QString currentLanguage() const; // 2 letter ISO 639-1
    QStringList availableLanguages() const;	 // 2 letter ISO 639-1

    QString getVersion(QSystemsInfo::Version,  const QString &parameter) const;

    QString countryCode() const; //2 letter ISO 3166-1

    qint32 getCellNetworkStatus();
    qint32 networkSignalStrength();
    qint32 cellId();
    qint32 lac();

    qint32 currentMCC(); // Mobile Country Code
    qint32 currentMNC(); // Mobile Network Code

    qint32 homeMCC();
    qint32 homeMNC();

    bool isLocationEnabled() const;
    bool isWLANAccessible() const;
    QString operatorName();

//features
    bool hasFeatureSupported(QSystemsInfo::Feature feature);
    QString getDetailOfFeature(QSystemsInfo::Feature feature);

// display
    qint32 displayBrightness();
    qint32 colorDepth(qint32 screen);
    void setScreenSaverEnabled(bool b);
    void setScreenBlankingEnabled(bool b);
    bool isScreenLockOn();

// memory
    bool hasRamMemoryLevel();
    qint64 freeMemoryLevel() const;
    qint64 availableDiskSpace(const QString &driveVolume);
    qint64 totalDiskSpace(const QString &driveVolume);
    QStringList listOfVolumes();
    int getVolumeType(const QString &driveVolume); //returns enum


// device


    QString imei() const;
    QString imsi() const;
    QString manufacturer() const;
    QString model() const;

    qint32 getInputMethodType();

    qint32 batteryLevel() const;

    qint32 getSimStatus();
    bool isDeviceLocked();

Q_SIGNALS:
    void memoryCritical(qint32);
    void diskSpaceCritical(const QString &driveVolume, qint32);

    void profileChanged(QSystemsInfo::Profile);

    void networkStatusChanged(QSystemsInfo::NetworkMode, QSystemsInfo::CellNetworkStatus);
 
    void batteryLevelChanged(QSystemsInfo::BatteryLevel);
    void batteryLevelCritical(qint32);
    void powerStateChanged(QSystemsInfo::PowerState);
    void newCardInserted(const QString &driveVolume);

 private:

#if defined(Q_OS_LINUX) ||  defined(Q_OS_WIN32)
    virtual bool isCriticalMemory() const;
    virtual bool isBatteryCharging();
    virtual bool isDiskSpaceCritical(const QString &driveVolume);
#endif

};

QT_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_P_H*/

// End of file

