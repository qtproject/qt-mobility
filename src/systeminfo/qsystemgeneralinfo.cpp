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

#include "qmobilityglobal.h"
#include "qsystemgeneralinfo.h"
#include "qsysteminfocommon_p.h"

#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QFileSystemWatcher>
#include <QMetaType>

#include <locale.h>

QTM_BEGIN_NAMESPACE

  /*!
    \class QSystemInfo

    \ingroup systeminfo
   \inmodule QtSystemInfo

    \brief The QSystemInfo class provides access to various general information from the system.

   \bold NOTE: In the future, parts of the QtSystemInfo API may be moved and renamed into an existing Qt class that provides similiar functionality.

\table
\header
    \o Class
\row
    \o QSystemInfo::currentLanguage
\row
    \o QSystemInfo::availableLanguages
\row
    \o QSystemInfo::currentCountryCode
\row
    \o QSystemDisplayInfo::displayBrightness
\row
    \o QSystemDisplayInfo::colorDepth
\row
    \o QSystemScreenSaver::screenSaverInhibited
\row
    \o QSystemScreenSaver::setScreenSaverInhibit
\endtable

    Platform notes
    Some functionality may or may not be supported on various platforms. Depending on if there
is a reliable way to gather such information.
*/

/*!
    \enum QSystemInfo::Version
    This enum describes the version component.

    \value Os                    Operating system version / platform ID.
    \value QtCore                Qt library version.
    \value Firmware              Version of (flashable) system as a whole.
    \value QtMobility            QtMobility library version.

  */
/*!
    \enum QSystemInfo::Feature
    This enum describes the features of the device or computer.

    \value BluetoothFeature       Bluetooth feature available.
    \value CameraFeature          Camera feature available.
    \value FmradioFeature         FM Radio feature available.
    \value IrFeature              Infrared feature available.
    \value LedFeature             LED's feature available.
    \value MemcardFeature         Memory card feature available.
    \value UsbFeature             Universal System Bus (USB) feature available.
    \value VibFeature             Vibration feature available.
    \value WlanFeature            Wireless Local Area Network (WLAN) feature available.
    \value SimFeature             Subscriber Identity Module (SIM) available.
    \value LocationFeature        Global Positioning System (GPS) and/or other location feature available.
    \value VideoOutFeature        Video out feature available.
    \value HapticsFeature         Haptics feature available.
    \value FmTransmitterFeature   FM Radio transmitter available.
  */




/*!
  \fn void QSystemInfo::currentLanguageChanged(const QString &lang)

  This signal is emitted whenever the current language changes, specified by \a lang,
  which is in 2 letter, ISO 639-1 specification form.
  */



Q_GLOBAL_STATIC(QSystemInfoPrivate, sysinfoPrivate)

#ifdef QT_SIMULATOR
QSystemInfoPrivate *getSystemInfoPrivate() { return sysinfoPrivate(); }
#endif

/*!
\fn QSystemInfo::QSystemInfo(QObject *parent)
   Constructs a QSystemInfo object with the given \a parent.
 */

QSystemInfo::QSystemInfo(QObject *parent)
    : QObject(parent), d(sysinfoPrivate())
{
    qRegisterMetaType<QSystemInfo::Version>("QSystemInfo::Version");
    qRegisterMetaType<QSystemInfo::Feature>("QSystemInfo::Feature");
#ifdef TESTR
    priv = sysinfoPrivate();
#endif
}

/*!
  Destroys the QSystemInfo object
*/
QSystemInfo::~QSystemInfo()
{
}

/*!
    \internal

    This function is called when the client connects to signals.

    \sa connectNotify()
*/

void QSystemInfo::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentLanguageChanged(QString))))) {
        connect(d,SIGNAL(currentLanguageChanged(QString)),
                this,SIGNAL(currentLanguageChanged(QString)));
    }
}

/*!
    \internal

    This function is called when the client disconnects from the signals.

    \sa connectNotify()
*/
void QSystemInfo::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentLanguageChanged(QString))))) {
        disconnect(d,SIGNAL(currentLanguageChanged(QString)),
                   this,SIGNAL(currentLanguageChanged(QString)));
    }
}

/*!
  \property QSystemInfo::currentLanguage
  \brief The current Language
    Returns the current language in 2 letter ISO 639-1 format.
 */
QString QSystemInfo::currentLanguage()
{
    return sysinfoPrivate()->currentLanguage();
}
/*!
  \property  QSystemInfo::availableLanguages
  \brief List of available languages.

    Returns a QStringList of available Qt language translations in 2 letter ISO 639-1 format.
    If the Qt translations cannot be found, returns the current system language.
  */
QStringList QSystemInfo::availableLanguages()
{
    return sysinfoPrivate()->availableLanguages();
}

/*!
  Returns the version of QSystemInfo::Version \a type,
  with optional platform dependent \a parameter as a string.

  Version will be returned in "major.minor.build" form.

  In case a particular version does not use the "build" part, it is set to 0.
  If a particular element is not available at all, an error "Not Installed" will be returned by
  the API.
*/
QString QSystemInfo::version(QSystemInfo::Version type, const QString &parameter)
{
    switch(type) {
        case QSystemInfo::QtMobility:
            return QLatin1String(QTM_VERSION_STR);
        default:
            return sysinfoPrivate()->version(type, parameter);
    }
}

/*!
  \property  QSystemInfo::currentCountryCode
  \brief The current locale country code.

    Returns the 2 letter ISO 3166-1 for the current country code.
*/
QString QSystemInfo::currentCountryCode()
{
    return sysinfoPrivate()->currentCountryCode();
}

/*!
    Returns true if the QSystemInfo::Feature \a feature is supported, otherwise false.
*/

bool QSystemInfo::hasFeatureSupported(QSystemInfo::Feature feature)
{
    return sysinfoPrivate()->hasFeatureSupported(feature);
}



#include "moc_qsystemgeneralinfo.cpp"

QTM_END_NAMESPACE
