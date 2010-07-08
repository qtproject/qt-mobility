/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of QtUiTest.
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

//TESTED_COMPONENT=src/multimedia

testcase = {

  preconditions: function()
  {

      prompt(twiki('*Preconditions to the Multimedia Player tests :*

   1. Have 2 test .mp4 file available on device (Such as the file available from here: http://camendesign.com/code/video_for_everybody/test.html) 
   1. Have 2 test .mp3 and .vaw file available on device 
   1. Have a number of .jpg, .gif and .png files available on device 
   1. Have a valid m3u file for testing sideshow functionality (a test file is available on crud: crud-playlist.m3u at http://crud-nokia.trolltech.com.au/crud-playlist.m3u). A test collection of images and a link to an image on the net is linked here: playlist.tar (Please note, the paths in the .m3u file are to images in the /opt/media/pix/ , if your test environment differs, please edit accordingly) 
   1. Make sure device can ping destinations on the network (if applicable) 
   1. Have an audio stream url available (http://202.6.74.107:8060/triplej.mp3 for example) 
   1. Have a video stream available(Such as: http://clips.vorwaerts-gmbh.de/big_buck_bunny.mp4) 
   1. Access to Internet available to device 
   1. Have radio reception available in testing area (required for testing of radio) 
   1. "Click" means using devices keypad for selection and navigation 
   1. "Tap" means using devices touchscreen for selection and navigation
'));
  },

  player1: function()
  {
      prompt(twiki('*Player Test 1 :* Listen to an audio file on device

   1. Launch player application
   1.  Click [Open]
   1.  In "Open File" window navigate and highlight test .mp3 file
   1.  Click on selected file to return to player UI
   1.  Verify selected file has populated playlist window
   1.  Highlight file in playlist window by clicking on it
   1.  Click [Play]
   1.  Verify that .mp3 file is audible on device'));
  },

  player2: function()
  {
      prompt(twiki('*Player Test 2:* All player control features of player application function properly with audio file

   1. Continuation of Test 1

   1. Click on [Pause]
   1. Verify that file has paused
   1. Verify that [Pause] controll has now changed to [Play] controll
   1. Click on [Play]
   1. Verify that file continues to play from where previously paused.
   1. Verify that [Play] icon is now a [Pause] icon
   1. Click on [Next File] control
   1. Verify that next file in playlist is playing from start
   1. After some time (10-20 sec) click on [Beginning of File] control
   1. Verify that current file is being played from start
   1. Let some arbitrary time pass, then click on [Beginning of File] control twice in quick succession.
   1. Verify that previous file in playlist is playing from start
   1. Click on [Stop] control
   1. Verify that file has stopped playing
   1. Click on [Play]
   1. Verify that file continues to play from start of file.
   1. Let file play till end
   1. Verify player continues playing the next file in the playlist window
   1. After some arbitrary time, click on [Stop]
   1. Click on [Next File] control
   1. Verify that next file in playlist is highlighted
   1. Verify that file is not automatically playing
   1. Click on [Beginning of File] control
   1. Verify that previous file in playlist is highlighted
   1. Tap on file in playlist
   1. Verify that file plays in player.
   1. Select number of files greater than can be displayed without scrolling in to playlist
   1. Verify that list can be scrolled through
   1. With file playing, verify that hardware keys can change volume up and down
   1. With file playing, verify that UI volume control slider can change volume up and down
   1. Select volume at 50% (or thereabout)
   1. Click or Tap on [Mute] control 
   1. Verify that sound is muted while file is playing
   1. Verify that [Mute] control is displaying the Muted icon
   1. Click or Tap on [Mute] control again
   1. Verify that sound has reverted back to level selected prior to muting
   1. Tap and drag positional slider to right, then release
   1. Verify that player continues to play file at later part of file
   1. Tap and drag positional slider to left, then release
   1. Verify that player continues to play file at earlier part of file
   1. *Note* [Color Option] is not implemented so far in player
   1. *Note* [Full Screen] opens a blank screen with no content
'));
  },

  player3: function()
  {
      prompt(twiki('*Player Test 3:* Fast forward trough a playing audio file

   1. *Note* This test is not applicable to the Symbian platform

   1. Continuation of previous test ...

   1. Click or Tap on [1.0] (Multiplier) control. 
   1. Select [2.0] 
   1. Verify that file is playing at double speed
   1. With multiplier control set on [2.0], select a new file from the pla list window
   1. Verify that the Multiplier control has reverted back to [1.0]
   1. Verify that the audio is playing at normal speed 
   1. Click or Tap on [Multiplier] control. 
   1. Select [0.5] 
   1. Verify that file is playing at half speed 
   1. With multiplier control set on [0.5], select a new file from the pla
list window
   1. Verify that the Multiplier control has reverted back to [1.0]
   1. Verify that the audio is playing at normal speed'));
  },

  player4: function()
  {
      prompt(twiki('*Player Test 4 :* Watch a video file on device

   1. Launch player application
   1. Click [Open]
   1. In "Open File" navigate and highlight test .MP4 file
   1. Click on selected file to return to player UI
   1. Verify selected file has populated playlist
   1. Highlight file in playlist by clicking on it
   1. Click Play [|>]
   1. Verify that .MP4 file is playing on device
'));
  },

  player5: function()
  {
      prompt(twiki('*Player Test 5:* All features of player application function properly with video file

   1. Continuation of Test 4

   1. Click on [Pause]
   1. Verify that file has paused
   1. Verify that the [Pause] icon has changed to a [Play] icon
   1. Click on [Play]
   1. Verify that the [Play] icon has changed to a [Pause] icon
   1. Verify that file continues to play from where previously paused.
   1. Click on [Next File]
   1. Verify that next file in playlist is playing from start
   1. After some time (10-20 sec) click on [Beginning of File] control
   1. Verify that current file is being played from start
   1. Let arbitrary time pass, then click on [Beginning of File] control twice in quick succession.
   1. Verify that previous file in playlist is playing from start
   1. Click on [Stop]
   1. Verify that file has stopped playing
   1. Click on [Play]
   1. Verify that file continues to play from start of file.
   1. Let file play till end
   1. Verify player stops at end of file.
   1. Start playing file.
   1. After arbitrary time, click on [Stop]
   1. Click on [Next File]
   1. Verify that next file in playlist is highlighted
   1. Verify that file is not automatically playing
   1. Click on [Beginning of File]
   1. Verify that previous file in playlist is highlighted
   1. Tap on file in playlist
   1. Verify that file plays in player.
   1. Select number of files greater than can be displayed without scrolling in to playlist
   1. Verify that list can be scrolled through
   1. With file playing, verify that hardware keys can change volume up and down
   1. With file playing, verify that UI volume control slider can change volume up and down
   1. Select volume at 50% (or thereabout)
   1. Click or Tap on [Mute] control 
   1. Verify that sound is muted while file is playing
   1. Verify that [Mute] control is displaying the Muted icon
   1. Click or Tap on [Mute] control again
   1. Verify that sound has reverted back to level selected prior to muting
   1. Click or Tap on [1.0] (Multiplier) control.
   1. Select [2.0]
   1. Verify that file is playing at double speed
   1. With multiplier control set on [2.0], select a new file from the playlist window
   1. Verify that the Multiplier control has reverted back to [1.0]
   1. Verify that the video is playing at normal speed
   1. Click or Tap on [Multiplier] control.
   1. Select [0.5]
   1. Verify that file is playing at half speed
   1. Tap and drag positional slider to right, then release
   1. Verify that player continues to play file at later part of file
   1. Tap and drag positional slider to left, then release
   1. Verify that player continues to play file at earlier part of file
   1. *Note* [Color Option] is not implemented so far in player
   1. Start playing file
   1. Click or Tap on [FullScreen] controll
   1. Verify that video is now playing in Fullscreen mode
   1. When at end of current file, verify that next file in playlist is being played
   1. When at last file in playlist and at end of file, verify that fullscreen mode is exited and user returned to player interface and that [FullScreen] is no longer selected.
'));
  },

  player6: function()
  {
      prompt(twiki('*Player Test 6 :* Fast Forward through a playing video file

   1. *Note* This test is not applicable to the Symbian platform

   1. While playing a video file, Click or Tap on [1.0] (Multiplier) control 	  
   1. Select [2.0] 	
   1. Verify that file is playing at double speed 
   1. Click or Tap on [Multiplier] control 	  
   1. Select [0.5] 	
   1. Verify that file is playing at half speed'));
  },

  player7: function()
  {
    prompt(twiki('*Player Test 7:* All features of player application function properly with streaming

   1. *Note* Functionality for this feature is being implemented due to bugs in .m3u playback on Maemo. Once resolved, this test will revert back to its original form
   1. *Note* In order to play stream on Maemo, invoke player with following command from console:
            run-standalone.sh ./player http://202.6.74.107:8060/triplej.mp3
   1. *Note* For this test you need a playlist of streaming media urls. Suggest: http://202.6.74.107:8060/triplej.mp3 ; http://www.abc.net.au/streaming/triplej.asx ; rtsp://media1.abc.net.au/broadcast/triplej.rm ; mms://media3.abc.net.au/triplej ; depending on your platform and backend support.

   1. In player application, Click or Tap [Open]
   1. In "Open File" navigate and highlight test playlist file
   1. Click or Tap on selected file to return to player UI
   1. Verify selected file has populated playlist
   1. Highlight file in playlist by selecting or clicking it
   1. Click [Play]
   1. Verify that stream is playing on device
   1. Click [Pause] 	
   1. Verify that stream is paused on device
   1. Verify that the [Pause] button has changed to a [Play] button
   1. Click [Play] again
   1. Verify that stream is playing on device
   1. Click [Stop]
   1. Open local mp3 file in to play list and select it to play 	
   1. Verify that the mp3 plays while the stream name is still present in the playlist 
   1. Re-select the stream in the playlist 	
   1. Verify that the stream starts playing again 
   1. Click [Next File] 	
   1. Verify that the player started playing the next file in the playlist 
   1. Click [Previous File] 	
   1. Verify that the original stream is playing again 
   1. During stream playback, have headphones connected to device 	
   1. Verify that the sound is now playing through the headphones 
   1. Invoke a system notification on the device (such as an incoming SMS) 	
   1. Verify that the device correctly transmits the audible system notification through the devices speakers and then returns to playing the stream through the headphones
'));
    },
    
  radio1: function()
  {
      prompt(twiki('*Radio Test 1 :* Listen to an radio transmition on the device

   1. *Note* This test requires radio reception and headphones to be connected to device 
   1. *Note* Audible system notifications should be enabled for this test

   1. Start Radio application 	
   1. Verify that 0 KHz and No Signal is displayed 
   1. Click on [Scan Up] 	
   1. Audible "white noise" should be heard as the radio searches for a frequency lock 
   1. Frequency should be increasing in 100 KHz steps until a strong signal is found 	
   1. Verify that a radio signal can be heard. It is possible for the radio to lock on a silent station signal, in that case progress further up the frequencies 
   1. Once a strong audible signal is found 	
   1. Verify that the stations frequency and Got Signal message is displayed 
   1. Click on [Scan Up] 	
   1. Verify that the radio has moved off the previous station and in progressing through the frequencies 
   1. Once on a radio station, Click on [Freq Up] 	
   1. Verify that the radio frequency has moved off the station by 100KHz and is stationary 
   1. Click on [Freq Up] again 	
   1. Verify that the radio frequency has moved up another 100KHz and is stationary 
   1. Click on [Freq Down] twice 	
   1. Verify that you are now at the frequency of the previous radio station and that the signal is audible 
   1. Click on [Scan Down] 	
   1. Verify that the radio has moved off the previous station, progressing through the frequencies, and finds the original station from this test 
   1. While on a strong and audible radio signal, evoke a system notification on device (eg: incoming SMS, alarm notification) 	
   1. Verify that after the system notification has been delivered, the radio reverts to outputting the audio signal back through the headphones 

'));
  },

  camera1: function()
  {
      prompt(twiki(readLocalFile(baseDataPath() + "testtext/" + currentTestFunction())));
  },

  recorder1: function()
  {
      prompt(twiki('*Recorder Test 1 :* Record audio on the device

On Maemo5, need to select following in order to create wav file: 1) Start audiorecorder app 2) Select "audio/PCM" for Audio codec 3) Select wav for File Container 4) Press "Output File" button 5) Type a file name, f.ex test.wav 6) Press "Open" 7) Press "Record" button 8) Record for some time and then press "Stop" button 9) test.wav file is in the same folder than where application was started from 

If the file container is selected before the compatible audio codec has been selected then the error message is shown: "Not compatible codecs and container format". 

If I then select "audio/PCM" as the audio codec I\'m able to record .wav file despite the error message. So, the reason is that error message isn\'t wiped out even the selected container and codec are compatible.i

   1. Open audiorecorder application (On Maemo5 apply these settings)
   1. Select Audio Device 	     (pulseaudio)
   1. Select File Container 	     (wav)
   1. Select Audio Codec 	     (audio/PCM)
   1. Select Audio Quality 	     (low/medium/high)
   1. Select Output File 	     (ie: test.wav)
   1. Click Record 	  
   1. Verify that [Record] button has changed in to [Stop]
   1. Verify that Duration is counting seconds
   1. Verify that Recording is displayed
   1. Click Stop 	  
   1. Verify that Duration is now 0
   1. Select Output File in file browser (file should be located in same folder as application)	
   1. Verify that an audio file has been generated
   1. Play output file in media player
   1. Verify that audible audio is recorded in file
'));
  },

  slideshow1: function()
  {
      prompt(twiki('*Slideshow Test 1 :* Play a slideshow of pictures on the device from various sources

   1. Click on [Open] Icon 	  
   1. Select Directory from the list 	  
   1. Navigate to desired directory 	  
   1. Select desired directory containing pictures 	  
   1. Click [Play] 	
   1. Verify that the slideshow starts 
   1. Click on [Pause] 	
   1. Verify that slideshow stops on given image 
   1. Click [Play] again 	
   1. Verify that slideshow resumes 
   1. Click [Next File] 	
   1. Verify that slideshow jumps to next image in directory 
   1. Click [Previous File] 	
   1. Verify that slideshow jumps to previous image in directory 
   1. Click [Stop] 	
   1. Verify that slideshow has stopped 
   1. Click on [Open] Icon 	  
   1. Select Playlist from the list 	  
   1. Click [Play] 	
   1. Verify that slideshow starts 
   1. Click on [Pause] 	
   1. Verify that slideshow stops on given image 
   1. Click [Play] again 	
   1. Verify that slideshow resumes 
   1. Click [Next File] 	
   1. Verify that slideshow jumps to next image in directory 
   1. Click [Previous File] 	
   1. Verify that slideshow jumps to previous image in directory 
   1. Click [Stop] 	
   1. Verify that slideshow has stopped
'));
  }

}
