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

#include "dialog.h"

#include <QtCore/qmetaobject.h>

#include <QtGui/QFileDialog>

static const char ENUM_INSTANT_EFFECT[] = "InstantEffect";
static const char ENUM_DEVICE_TYPE[] = "Type";
static const char ENUM_ANIMATION_STATE[] = "State";

Dialog::Dialog()
{
    ui.setupUi(this);

    connect(ui.devices, SIGNAL(currentIndexChanged(int)), SLOT(deviceChanged()));
    connect(ui.enabled, SIGNAL(toggled(bool)), SLOT(enabledChanged(bool)));
    connect(ui.playPause, SIGNAL(clicked()), SLOT(playPauseClicked()));
    connect(ui.stop, SIGNAL(clicked()), &effect, SLOT(stop()));
    connect(ui.duration, SIGNAL(valueChanged(int)), SLOT(durationChanged(int)));
    connect(ui.intensity, SIGNAL(valueChanged(int)), SLOT(intensityChanged(int)));
    connect(&effect, SIGNAL(stateChanged(QAbstractAnimation::State, QAbstractAnimation::State)), SLOT(effectStateChanged(QAbstractAnimation::State)));

    //for the envelope
    connect(ui.attackTime, SIGNAL(valueChanged(int)), SLOT(attackTimeChanged(int)));
    connect(ui.attackIntensity, SIGNAL(valueChanged(int)), SLOT(attackIntensityChanged(int)));
    connect(ui.fadeTime, SIGNAL(valueChanged(int)), SLOT(fadeTimeChanged(int)));
    connect(ui.fadeIntensity, SIGNAL(valueChanged(int)), SLOT(fadeIntensityChanged(int)));

    //for the period
    connect(ui.grpPeriod, SIGNAL(toggled(bool)), SLOT(periodToggled(bool)));
    connect(ui.period, SIGNAL(valueChanged(int)), SLOT(periodChanged(int)));

    connect(ui.instantPlay, SIGNAL(clicked()), SLOT(instantPlayClicked()));

    //file API
    connect(ui.browse, SIGNAL(clicked()), SLOT(browseClicked()));
    connect(ui.filePlayPause, SIGNAL(clicked()), SLOT(filePlayPauseClicked()));
    connect(ui.fileStop, SIGNAL(clicked()), SLOT(fileStopClicked()));
    connect(&fileEffect, SIGNAL(stateChanged(QAbstractAnimation::State, QAbstractAnimation::State)), SLOT(fileEffectStateChanged(QAbstractAnimation::State)));

    foreach(const QFeedbackDevice &dev, QFeedbackDevice::devices()) {
        ui.devices->addItem(dev.name());
    }
    ui.devices->addItem("test device");

    //adding the instant effects
    const QMetaObject &mo = QFeedbackEffect::staticMetaObject;
    const QMetaEnum &me = mo.enumerator(mo.indexOfEnumerator(ENUM_INSTANT_EFFECT));
    for (int i = 0 ; i < me.keyCount(); ++i) {
        ui.instantEffect->addItem(me.key(i));
    }

    //initialization
    durationChanged(effect.duration());
    effectStateChanged(effect.state());
    intensityChanged(ui.intensity->value());
    attackTimeChanged(ui.attackTime->value());
    attackIntensityChanged(ui.attackIntensity->value());
    fadeTimeChanged(ui.fadeTime->value());
    fadeIntensityChanged(ui.fadeIntensity->value());
    fileEffectStateChanged(fileEffect.state());

    ui.tabWidget->setTabEnabled(1, QFeedbackEffect::supportsThemeEffect());
    ui.tabWidget->setTabEnabled(2, !QFileFeedbackEffect::supportedFileSuffixes().isEmpty());
}

QFeedbackDevice Dialog::currentDevice() const
{
    QList<QFeedbackDevice> devs = QFeedbackDevice::devices();
    int index = ui.devices->currentIndex();
    return index < devs.count() ? devs.at(index) : QFeedbackDevice();
}

void Dialog::deviceChanged()
{
    QFeedbackDevice dev = currentDevice();
    const QMetaObject &mo = QFeedbackDevice::staticMetaObject;
    ui.deviceType->setText(mo.enumerator(mo.indexOfEnumerator(ENUM_DEVICE_TYPE)).key(dev.type()));
    enabledChanged(dev.isEnabled());
    effect.setDevice(dev);
}

void Dialog::enabledChanged(bool on)
{
    if (!on)
        effect.stop();
    QFeedbackDevice dev = currentDevice();
    dev.setEnabled(on);
    ui.enabled->setChecked(dev.isEnabled());
    ui.envelope->setEnabled(dev.isEnabled() && (dev.supportedCapabilities() & QFeedbackDevice::Envelope));
    ui.grpPeriod->setEnabled(dev.isEnabled() && (dev.supportedCapabilities() & QFeedbackDevice::Period));
}

void Dialog::playPauseClicked()
{
    if (effect.state() == QAbstractAnimation::Running)
        effect.pause();
    else
        effect.start();
}

void Dialog::durationChanged(int duration)
{
    effect.setDuration(duration);
    ui.attackTime->setMaximum(duration);
    ui.fadeTime->setMaximum(duration);
    attackTimeChanged(ui.attackTime->value());
}

void Dialog::intensityChanged(int value)
{
    effect.setIntensity(qreal(value) / ui.intensity->maximum());
    ui.lblIntensity->setText(QString::number(effect.intensity()));
}

void Dialog::effectStateChanged(QAbstractAnimation::State newState)
{
    const QMetaObject *mo = effect.metaObject();
    ui.effectState->setText(mo->enumerator(mo->indexOfEnumerator(ENUM_ANIMATION_STATE)).key(newState));
    ui.stop->setEnabled(newState != QAbstractAnimation::Stopped);
}

void Dialog::attackTimeChanged(int attackTime)
{
    effect.setAttackTime(attackTime);
    //let's check the boundaries
    if (attackTime + ui.fadeTime->value() > ui.duration->value())
        ui.fadeTime->setValue(ui.duration->value() - attackTime);
}

void Dialog::attackIntensityChanged(int attackIntensity)
{
    effect.setAttackIntensity(qreal(attackIntensity) / ui.attackIntensity->maximum());
    ui.lblAttackIntensity->setText(QString::number(effect.attackIntensity()));
}

void Dialog::fadeTimeChanged(int fadeTime)
{
    effect.setFadeTime(fadeTime);
    //let's check the boundaries
    if (fadeTime + ui.attackTime->value() > ui.duration->value())
        ui.attackTime->setValue(ui.duration->value() - fadeTime);
}

void Dialog::fadeIntensityChanged(int fadeIntensity)
{
    effect.setFadeIntensity(qreal(fadeIntensity) / ui.fadeIntensity->maximum());
    ui.lblFadeIntensity->setText(QString::number(effect.fadeIntensity()));
}

void Dialog::periodToggled(bool on)
{
    effect.setPeriod(on ? ui.period->value() : 0);
}

void Dialog::periodChanged(int value)
{
    effect.setPeriod(value);
}

void Dialog::instantPlayClicked()
{
    const QMetaObject &mo = QFeedbackEffect::staticMetaObject;
    const QMetaEnum &me = mo.enumerator(mo.indexOfEnumerator(ENUM_INSTANT_EFFECT));
    QFeedbackEffect::playThemeEffect(QFeedbackEffect::InstantEffect(me.keyToValue(ui.instantEffect->currentText().toLatin1())));
}

void Dialog::browseClicked()
{
    fileEffect.stop();
    QString filename = QFileDialog::getOpenFileName(this, tr("feedback file"));
    QFileInfo info(filename);
    bool ok = info.isReadable();
    if (ok) {
        ui.filename->setText(QDir::toNativeSeparators(filename));
        fileEffect.setFile(filename);
        fileEffect.setLoaded(true);
    }
    ui.filePlayPause->setEnabled(ok);
    ui.fileStop->setEnabled(false);
}

void Dialog::filePlayPauseClicked()
{
    if (fileEffect.state() == QAbstractAnimation::Running)
        fileEffect.pause();
    else
        fileEffect.start();
}

void Dialog::fileStopClicked()
{
    fileEffect.stop();
}

void Dialog::fileEffectStateChanged(QAbstractAnimation::State newState)
{
    const QMetaObject *mo = fileEffect.metaObject();
    ui.fileEffectState->setText(mo->enumerator(mo->indexOfEnumerator(ENUM_ANIMATION_STATE)).key(newState));
    ui.fileStop->setEnabled(newState != QAbstractAnimation::Stopped);
}

#include "moc_dialog.cpp"
