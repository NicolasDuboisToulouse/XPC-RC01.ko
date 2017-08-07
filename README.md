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


### Harmony commands that not working

| MSI            | Suttle XPC G   |
|----------------|----------------|
|                | Skip           |
|                | Ok             |
|                | FastForward    |

### Detailled commands

See bindings.txt in kodi folder.


