/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtDeclarative/qdeclarativeextensionplugin.h>
#include <QtDeclarative/qdeclarative.h>

#include "qdeclarativehapticseffect.h"
#include "qdeclarativefileeffect.h"
#include "qdeclarativethemeeffect.h"
#include "qdeclarativefeedback.h"
#include "qdeclarativefeedbackactuator.h"

/*!
    \qmlclass Feedback
    \brief The Feedback object defines a number of constants
    \ingroup qml-feedback-api

    This element is part of the \bold{QtMobility.feedback 1.1} module.

    There are several predefined enumerations and constants provided in this class:

    1. Duration
    This enum describes the possible effect predefined duration types. Generally a specific
    milliseconds value can be supplied instead of one of these values.
    \list
    \o Feedback.Infinite Infinite effect duration
    \endlist

    2. State
    This enum describes the state of the effect. An effect will be in one of these states.
    \list
    \o Feedback.Stopped The effect is not running. This is the initial state.
    \o Feedback.Paused Paused The effect is paused.
    \o Feedback.Running The effect is running.
    \o Feedback.Loading The effect is loading.
    \endlist

    3. ErrorType
    This enum describes the possible errors happening on the effect.
    \list
    \o Feedback.UnknownError An unknown error occurred.
    \o Feedback.DeviceBusy The feedback could not start because the device is busy,
       the device could be busy if a higher-priority client is using the haptics/actuator device.
    \endlist


    \sa FileEffect, ThemeEffect, HapticsEffect, {QFeedbackEffect}
*/

/*!
    \qmlclass FeedbackEffect QFeedbackEffect
    \brief The FeedbackEffect element is the base class for all feedback effects.
    \ingroup qml-feedback-api

    This element is part of the \bold{QtMobility.feedback 1.1} module.

    You can't create one of these elements directly, but several other elements
    inherit the methods and properties of these elements.

    \sa FileEffect, ThemeEffect, HapticsEffect, {QFeedbackEffect}
*/

/*!
  \qmlproperty int FeedbackEffect::duration

  The duration of the effect, in milliseconds.  This is 0 for effects of unknown
  duration, or Feedback.Infinite for effects that don't stop.
  \sa Feedback
  */

/*!
  \qmlproperty FeedbackEffect::State FeedbackEffect::state

  This is the current state of the effect.  It is one of:
  \list
  \o Feedback.Stopped - the effect is not playing
  \o Feedback.Loading - the effect is being loaded
  \o Feedback.Running - the effect is playing
  \o Feedback.Paused - the effect was being played, but is now paused.
  \endlist
  \sa Feedback
  */

/*!
  \qmlmethod FeedbackEffect::start()

  Start playback of this effect.
  */

/*!
  \qmlmethod FeedbackEffect::stop()

  Stops playback of this effect.
  */

/*!
  \qmlmethod FeedbackEffect::pause()

  Pause playback of this effect.  Not all devices or effects can pause feedback playback.
  */

/*!
  \qmlsignal FeedbackEffect::error(Feedback::ErrorType)

  This signal is emitted when an error occurs during playback of an effect.

  The error is one of the following values:
  \list
  \o Feedback.UnknownError - An unknown error occurred
  \o Feedback.DeviceBusy - The device resource is already being used.
  \endlist

  \sa Feedback QFeedbackEffect::ErrorType
  */

/*!
    \qmlclass FileEffect
    \brief The FileEffect element represents feedback data stored in a file.
    \ingroup qml-feedback-api
    \inherits FeedbackEffect

    This element is part of the \bold{QtMobility.feedback 1.1} module.

    \snippet doc/src/snippets/declarative/declarative-feedback.qml File Effect

    \sa HapticsEffect, {QFeedbackActuator}
*/


/*!
  \qmlproperty bool FileEffect::running

  This property is true if this feedback effect is running.
  */

/*!
  \qmlproperty bool FileEffect::paused

  This property is true if this feedback effect is paused.
  */

/*!
  \qmlproperty bool FileEffect::loaded

  This property is true if this feedback effect is loaded.
  */

/*!
  \qmlproperty url FileEffect::source

  This property stores the url for the feedback data.
  */

/*!
    \qmlclass HapticsEffect
    \brief The HapticsEffect element represents a custom haptic feedback effect.
    \ingroup qml-feedback-api
    \inherits FeedbackEffect

    This element is part of the \bold{QtMobility.feedback 1.1} module.

    This class closely corresponds to the C++ \l QFeedbackHapticsEffect class.

    \snippet doc/src/snippets/declarative/declarative-feedback.qml Haptics Effect

    \sa Actuator, {QFeedbackHapticsEffect}
*/

/*!
    \qmlproperty int HapticsEffect::duration

    The duration of the main part of the haptics effect, in milliseconds.
*/

/*!
    \qmlproperty double HapticsEffect::intensity

    The intensity of the main part of the haptics effect, from 0.0 to 1.0.
*/

/*!
    \qmlproperty int HapticsEffect::attackTime

    The duration of the attack (fade-in) part of the haptics effect.
*/

/*!
    \qmlproperty double HapticsEffect::attackIntensity

    The intensity of the attack (fade-in) part of the haptics effect, from 0.0 to 1.0.
*/


/*!
    \qmlproperty int HapticsEffect::fadeTime

    The duration of the fade-out part of the haptics effect.
*/

/*!
    \qmlproperty double HapticsEffect::fadeIntensity

    The intensity of the fade-out part of the haptics effect, from 0.0 to 1.0.
*/


/*!
    \qmlproperty int HapticsEffect::period

    The period of the haptics effect.  If the period is zero, the effect will
    not repeat.  If it is non-zero, the effect will repeat every period milliseconds.
*/


/*!
    \qmlproperty Actuator HapticsEffect::actuator

    The actuator that is used for playing this effect.
    \sa Actuator
*/

/*!
    \qmlclass Actuator QFeedbackActuator
    \brief The Actuator element represents a feedback actuator.
    \ingroup qml-feedback-api

    This element is part of the \bold{QtMobility.feedback 1.1} module.

    The Actuator class maps directly to the QFeedbackActuator C++ class, and
    can be used with HapticsEffect elements.

    There are several predefined enumerations and constants provided in this object:

    1. Capability
    \list
    \o Envelope Capability defining the wave type with attack/fade times and levels.
    \o Period   Capability defining that the device can play periodic effects.
    \endlist

    2. State
    \list
    \o Busy    The actuator is busy.
    \o Ready   The actuator is ready to play an effect.
    \o Unknown The actuator is in an unknown state.
    \endlist

    \sa HapticsEffect, {QFeedbackActuator}
*/

/*!
    \property Actuator::actuatorId
    This property hold the id of the feedback actuator.
*/

/*!
    \qmlproperty bool Actuator::valid

    This property is true if the actuator is valid.
*/

/*!
   \qmlproperty  Actuator::name
    This property holds the name of the feedback actuator.
*/
/*!
    \qmlproperty Actuator::state
    This property holds the state of the feedback actuator.
*/

/*!
    \qmlmethod bool Actuator::isCapabilitySupported(enumeration capability)
    Returns if the actuator supports the supplied \a capability, available capabilities are:
    \list
    \o Envelope Capability defining the wave type with attack/fade times and levels.
    \o Period   Capability defining that the device can play periodic effects.
    \endlist
*/
/*!
    \qmlproperty Actuator::enabled
    This property is true if the feedback actuator is enabled.
*/

/*!
    \qmlclass ThemeEffect
    \brief The ThemeEffect element represents a themed feedback effect.
    \ingroup qml-feedback-api

    This element is part of the \bold{QtMobility.feedback 1.1} module.

    This element is used for playing feedback effects that follow the
    system theme.  The actual feedback might be haptic, audio or some other
    method.

    \snippet doc/src/snippets/declarative/declarative-feedback.qml Theme
*/

/*!
    \qmlproperty bool ThemeEffect::supported

    This property is true if the system supports themed feedback effects.
*/

/*!
    \qmlproperty ThemeEffect ThemeEffect::effect

    This property holds the specific themed effect type to play.  It is one of:

  \list
    \o ThemeEffect.Basic - Generic feedback.
    \o ThemeEffect.Sensitive - Generic sensitive feedback.
    \o ThemeEffect.BasicButton - Feedback for interacting with a button (e.g. pressing).
    \o ThemeEffect.SensitiveButton - Sensitive feedback for interacting with a button (e.g. auto repeat).
    \o ThemeEffect.BasicKeypad - Feedback for interacting with a keypad button.
    \o ThemeEffect.SensitiveKeypad - Sensitive feedback for interacting with a keypad button.
    \o ThemeEffect.BasicSlider - Feedback for moving a slider.
    \o ThemeEffect.SensitiveSlider - Sensitive feedback for moving a slider.
    \o ThemeEffect.BasicItem - Feedback when interacting with a list or grid item.
    \o ThemeEffect.SensitiveItem - Sensitive feedback when interacting with a list or grid item.
    \o ThemeEffect.ItemScroll - Feedback when scrolling a list or grid item view.
    \o ThemeEffect.ItemPick - Feedback when selecting an item to move in a list or grid view.
    \o ThemeEffect.ItemDrop - Feedback when dropping an item in a list or grid view.
    \o ThemeEffect.ItemMoveOver - Feedback when moving an item in a list or grid view.
    \o ThemeEffect.BounceEffect - Feedback for a bounce effect.
    \o ThemeEffect.CheckBox - Feedback for selecting a checkbox.
    \o ThemeEffect.MultipleCheckBox - Feedback for selecting checkboxes of multiple items.
    \o ThemeEffect.Editor - Feedback for interacting with an editor.
    \o ThemeEffect.TextSelection - Feedback for selecting text.
    \o ThemeEffect.BlankSelection - Feedback for a blank selection.
    \o ThemeEffect.LineSelection - Feedback for selecting a line.
    \o ThemeEffect.EmptyLineSelection - Feedback for selecting an empty line.
    \o ThemeEffect.PopUp - Generic feedback for interacting with a popup.
    \o ThemeEffect.PopupOpen - Generic feedback when a popup opens.
    \o ThemeEffect.PopupClose - Generic feedback when a popup closes.
    \o ThemeEffect.Flick - Generic feedback when starting a flick gesture.
    \o ThemeEffect.StopFlick - Generic feedback when stopping a flick.
    \o ThemeEffect.MultiPointTouchActivate - Generic feedback when a touch gesture with more than one point is started.
    \o ThemeEffect.RotateStep - Feedback when rotating using a gesture.
    \o ThemeEffect.LongPress - Feedback for a long press (or tap and hold) gesture.
    \o ThemeEffect.PositiveTacticon - Generic feedback for notification of a successful operation.
    \o ThemeEffect.NeutralTacticon - Generic feedback for notification.
    \o ThemeEffect.NegativeTacticon - Generic feedback for notification of a failed operation.
  \endlist

  \sa QFeedbackEffect::ThemeEffect
*/

/*!
    \qmlmethod ThemeEffect::play()

    Call this to play the themed effect.
*/

class QFeedbackDeclarativeModule : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    virtual void registerTypes(const char *uri)
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("QtMobility.feedback"));
        qmlRegisterUncreatableType<QDeclarativeFeedback>("QtMobility.feedback", 1, 1, "Feedback", "Feedback is a namespace"); // It's not really
        qmlRegisterType<QDeclarativeFeedbackActuator>("QtMobility.feedback", 1, 1, "Actuator");
        qmlRegisterType<QDeclarativeFileEffect>("QtMobility.feedback", 1, 1, "FileEffect");
        qmlRegisterType<QDeclarativeHapticsEffect>("QtMobility.feedback", 1, 1, "HapticsEffect");
        qmlRegisterType<QDeclarativeThemeEffect>("QtMobility.feedback", 1, 1, "ThemeEffect");
    }
};

#include "moc_qdeclarativethemeeffect.cpp"
#include "moc_qdeclarativefeedback.cpp"
#include "feedback.moc"

Q_EXPORT_PLUGIN2(declarative_feedback, QT_PREPEND_NAMESPACE(QFeedbackDeclarativeModule));

