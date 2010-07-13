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
#include <QDebug>

static const char ENUM_THEME_EFFECT[] = "ThemeEffect";
static const char ENUM_ANIMATION_STATE[] = "State";

Dialog::Dialog()
{
    ui.setupUi(this);

    connect(ui.actuators, SIGNAL(currentIndexChanged(int)), SLOT(actuatorChanged()));
    connect(ui.enabled, SIGNAL(toggled(bool)), SLOT(enabledChanged(bool)));
    connect(ui.playPause, SIGNAL(pressed()), SLOT(playPauseClicked()));
    connect(ui.stop, SIGNAL(pressed()), &effect, SLOT(stop()));
    connect(ui.duration, SIGNAL(valueChanged(int)), SLOT(durationChanged(int)));
    connect(ui.intensity, SIGNAL(valueChanged(int)), SLOT(intensityChanged(int)));

    //for the envelope
    connect(ui.attackTime, SIGNAL(valueChanged(int)), SLOT(attackTimeChanged(int)));
    connect(ui.attackIntensity, SIGNAL(valueChanged(int)), SLOT(attackIntensityChanged(int)));
    connect(ui.fadeTime, SIGNAL(valueChanged(int)), SLOT(fadeTimeChanged(int)));
    connect(ui.fadeIntensity, SIGNAL(valueChanged(int)), SLOT(fadeIntensityChanged(int)));

    //for the period
    connect(ui.grpPeriod, SIGNAL(toggled(bool)), SLOT(periodToggled(bool)));
    connect(ui.period, SIGNAL(valueChanged(int)), SLOT(periodChanged(int)));

    connect(ui.instantPlay, SIGNAL(pressed()), SLOT(instantPlayClicked()));

    //file API
    connect(ui.browse, SIGNAL(pressed()), SLOT(browseClicked()));
    connect(ui.filePlayPause, SIGNAL(pressed()), SLOT(filePlayPauseClicked()));
    connect(ui.fileStop, SIGNAL(pressed()), &fileEffect, SLOT(stop()));

    foreach(const QFeedbackActuator &dev, QFeedbackActuator::actuators()) {
        ui.actuators->addItem(dev.name());
    }

    //adding the instant effects
    const QMetaObject &mo = QFeedbackEffect::staticMetaObject;
    const QMetaEnum &me = mo.enumerator(mo.indexOfEnumerator(ENUM_THEME_EFFECT));
    Q_ASSERT(me.keyCount());
    for (int i = 0 ; i < me.keyCount(); ++i) {
        ui.instantEffect->addItem(me.key(i));
    }

    //initialization
    durationChanged(effect.duration());
    intensityChanged(ui.intensity->value());
    attackTimeChanged(ui.attackTime->value());
    attackIntensityChanged(ui.attackIntensity->value());
    fadeTimeChanged(ui.fadeTime->value());
    fadeIntensityChanged(ui.fadeIntensity->value());

    ui.tabWidget->setTabEnabled(1, QFeedbackEffect::supportsThemeEffect());
    ui.tabWidget->setTabEnabled(2, !QFeedbackFileEffect::supportedMimeTypes().isEmpty());

    //that is a hackish way of updating the info concerning the effects
    startTimer(50);
}

QFeedbackActuator Dialog::currentActuator() const
{
    QList<QFeedbackActuator> devs = QFeedbackActuator::actuators();
    int index = ui.actuators->currentIndex();
    if (index == -1 || index > devs.count())
        return QFeedbackActuator();
    return devs.at(index);
}

void Dialog::actuatorChanged()
{
    QFeedbackActuator dev = currentActuator();
    enabledChanged(dev.isEnabled());
    effect.setActuator(dev);
}

void Dialog::enabledChanged(bool on)
{
    if (!on)
        effect.stop();
    QFeedbackActuator dev = currentActuator();
    dev.setEnabled(on);
    ui.enabled->setChecked(dev.isEnabled());
    ui.envelope->setEnabled(dev.isEnabled() && (dev.isCapabilitySupported(QFeedbackActuator::Envelope)));
    ui.grpPeriod->setEnabled(dev.isEnabled() && (dev.isCapabilitySupported(QFeedbackActuator::Period)));
}

void Dialog::playPauseClicked()
{
    if (effect.state() == QFeedbackEffect::Running) {
        effect.pause();
        if (effect.state() == QFeedbackEffect::Paused || effect.state() == QFeedbackEffect::Stopped) {
            ui.playPause->setText(tr("play"));
        }
    } else {
        effect.start();
        if (effect.state() == QFeedbackEffect::Running || effect.state() == QFeedbackEffect::Loading) {
            ui.playPause->setText(tr("pause"));
        }
    }
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

void Dialog::timerEvent(QTimerEvent *e)
{
    //update the display for effect
    {
        QFeedbackEffect::State newState = effect.state();
        const QMetaObject *mo = effect.metaObject();
        ui.effectState->setText(mo->enumerator(mo->indexOfEnumerator(ENUM_ANIMATION_STATE)).key(newState));
        ui.stop->setEnabled(newState != QFeedbackEffect::Stopped);
    }

    //update the display for effect
    {
        QFeedbackEffect::State newState = fileEffect.state();
        const QMetaObject *mo = fileEffect.metaObject();
        ui.fileEffectState->setText(mo->enumerator(mo->indexOfEnumerator(ENUM_ANIMATION_STATE)).key(newState));
        ui.fileStop->setEnabled(newState != QFeedbackEffect::Stopped);

        if (fileEffect.isLoaded()) {
            ui.fileStatus->setText( fileEffect.isLoaded() ? QString::fromLatin1("%1 : %2 ms").arg(tr("Loaded")).arg(fileEffect.duration()) : tr("Not Loaded") );
            ui.filePlayPause->setEnabled(fileEffect.isLoaded());
            ui.fileStop->setEnabled(false);
        } else {
            ui.fileStatus->setText( tr("No file loaded") );
        }
    }
    QDialog::timerEvent(e);
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
    const QMetaEnum &me = mo.enumerator(mo.indexOfEnumerator(ENUM_THEME_EFFECT));
    QFeedbackEffect::playThemeEffect(QFeedbackEffect::ThemeEffect(me.keyToValue(ui.instantEffect->currentText().toLatin1())));
}

void Dialog::browseClicked()
{
    fileEffect.stop();
    QString filename = QFileDialog::getOpenFileName(this, tr("feedback file"));
    ui.fileStop->setEnabled(false);
    ui.filePlayPause->setEnabled(fileEffect.isLoaded());
    ui.filename->setText(QDir::toNativeSeparators(filename));
    ui.fileStatus->setText( tr("Loading") );
    fileEffect.setFileName(filename);
}

void Dialog::filePlayPauseClicked()
{
    if (fileEffect.state() == QFeedbackEffect::Running)
        fileEffect.pause();
    else
        fileEffect.start();
}

#include "moc_dialog.cpp"
