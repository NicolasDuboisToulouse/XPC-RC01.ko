XPC-RC01 - Linux kernel module
==============================

What is this device : https://www.google.com/search?q=xpc-rc01&source=lnms&tbm=isch

I don't have the remote, but only the receiver. Do you also have the receiver somewhere in a box ?\
Anyway, Logitech Harmony have it in its database, so the remote is useless... (I Hope Logitech will recognize my sponsoring...)

Issue with this device
----------------------
When you plug it, it is recognized as an HID keyboard. Fine ! But most of the commands doesn't work.\
The device emit invalid keyboard code for most of the keys. I Think there is a driver for Windows...

Originally, the device really doesn't work. But with recent kernel, the driver hid_zydacron is loaded and seems works, almost for some keys. I don't try it too much.

Remarks:
* The device send keydown event and immediately keyup event. No event are sent on real key up.
* Some keys support repeat (see table below). In this case the remote will repeat keydown and keyup events.


What the driver is ?
--------------------
This driver send keyboard keycodes for each known working remote. (see table below).
The keycodes are all standard ones, no *multimedia* keys. (i.e. all code are < 128).
You have to map your harmony remove regarding the devide you want to control.
If some keys are not fine for your usage, you have to modify the driver (or change the keyboard mapping). There is no configuration files.

Tested Devices
--------------
* Original : What send the original remove (I think, since I lost it). The key below it what I had recorded when I add it (maybe plus some keys from Harmony/MSI)
* Harmony, manufacturer MSI, remote xpc-rc01
* Harmony, manufacturer Packard Bell, remote Suttle XPC - DOES NO WORK
* Harmony, manufacturer Packard Bell, remote Suttle XPC G (work, see the 'G')
* Harmony, manufacturer Packard Bell, remote AOpen XPC - DOES NO WORK


Key table
---------
* The names of the key are the Harmony ones.
* I'm not sure of the name of the key for the original remote. In most case it's the MSI Harmony ones.
* Keyboard mapping is not in this table: it depend on what it wrote in the driver and I don't want to sychronize the files. So, look in driver code !


### Harmony commands that not working

| MSI            | Suttle XPC G   |
|----------------|----------------|
|                | Skip           |
|                | Ok             |
|                | FastForward    |


### Binding HID raw data to Harmony commands


| HID raw data            | REPEAT | Original       | MSI            | Suttle XPC G   |
|-------------------------|--------|----------------|----------------|----------------|
| 01 00 00 00 00 00 00 00 | NO     |                |                | Replay         |
| 01 00 00 00 00 1E 00 00 | YES    | 1              | 1              | 1              |
| 01 00 00 00 00 1F 00 00 | YES    | 2              | 2              | 2              |
| 01 00 00 00 00 20 00 00 | YES    | 3              | 3              | 3              |
| 01 00 00 00 00 21 00 00 | YES    | 4              | 4              | 4              |
| 01 00 00 00 00 22 00 00 | YES    | 5              | 5              | 5              |
| 01 00 00 00 00 23 00 00 | YES    | 6              | 6              | 6              |
| 01 00 00 00 00 24 00 00 | YES    | 7              | 7              | 7              |
| 01 00 00 00 00 25 00 00 | YES    | 8              | 8              | 8              |
| 01 00 00 00 00 26 00 00 | YES    | 9              | 9              | 9              |
| 01 00 00 00 00 27 00 00 | YES    | 0              | 0              | 0              |
| 01 00 00 00 00 28 00 00 | YES    | Select         | Select         | Play           |
| 01 00 00 00 00 2A 00 00 | YES    | Back           | Back           | Back           |
| 01 00 00 00 00 4F 00 00 | YES    | DirectionRight | DirectionRight | DirectionRight |
| 01 00 00 00 00 50 00 00 | YES    | DirectionLeft  | DirectionLeft  | DirectionLeft  |
| 01 00 00 00 00 51 00 00 | YES    | DirectionDown  | DirectionDown  | DirectionDown  |
| 01 00 00 00 00 52 00 00 | YES    | DirectionUp    | DirectionUp    | DirectionUp    |
| 02 10 00 00 00 00 00 00 | NO     | ORIG_TOGGLE    |                | Record         |
| 02 30 00 00 00 00 00 00 | YES    | ORIG_CAMERA    |                | Repeat         |
| 02 70 00 00 00 00 00 00 | YES    | ORIG_INFO      |                | Shuffle        |
| 03 04 00 00 00 00 00 00 | NO     | Radio          | Radio          |                |
| 04 00 04 00 00 00 00 00 | NO     | ORIG_MENU      |                | Info           |
| 04 00 20 00 00 00 00 00 | NO     | ORIG_TV_TEXT   | Close          |                |
| 04 01 00 00 00 00 00 00 | NO     | ORIG_HOME      |                | InputMusic     |
| 04 02 00 00 00 00 00 00 | NO     | TV             | TV             |                |
| 04 08 00 00 00 00 00 00 | NO     | ORIG_RED       | Music          | Eject          |
| 04 10 00 00 00 00 00 00 | NO     | ORIG_GREEN     | Picture        | Live           |
| 04 20 00 00 00 00 00 00 | NO     | ORIG_BLUE      | Video          | Guide          |
| 04 80 00 00 00 00 00 00 | NO     | ORIG_YELLOW    | DVDMenu        |                |
| 05 00 01 00 00 00 00 00 | NO     | Play & Pause   | Play & Pause   | Pause          |
| 05 00 02 00 00 00 00 00 | NO     | Record         | Record         | InputTuner     |
| 05 00 04 00 00 00 00 00 | YES    | FastForward    | FastForward    | ChannelUp      |
| 05 00 08 00 00 00 00 00 | YES    | Rewind         | Rewind         | VolumeUp       |
| 05 00 10 00 00 00 00 00 | NO     | Skip           | Skip           | Rewind         |
| 05 00 20 00 00 00 00 00 | NO     | ORIG_PREVIOUS  | Replay         | Stop           |
| 05 00 40 00 00 00 00 00 | NO     | Stop           | Stop           | InputTv        |
| 05 01 00 00 00 00 00 00 | YES    | VolumeUp       | VolumeUp       | VolumeDown     |
| 05 02 00 00 00 00 00 00 | YES    | VolumeDown     | VolumeDown     | InputVideo     |
| 05 04 00 00 00 00 00 00 | NO     | Mute           | Mute           | Mute           |
| 05 08 00 00 00 00 00 00 | YES    | ChannelUp      | ChannelUp      | ChannelDown    |
| 05 10 00 00 00 00 00 00 | YES    | ChannelDown    | ChannelDown    | InputPicture   |
| 06 02 00 00 00 00 00 00 | NO     | PowerToggle    | PowerToggle    | PowerToggle    |


### Total of working keys

|                         | Original       | MSI            | Suttle XPC G   |
|-------------------------|----------------|----------------|----------------|
| Total working keys      | 41             | 36             | 38             |

