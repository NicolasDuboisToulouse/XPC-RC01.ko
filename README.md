# XPC-RC01 - Linux kernel module

What is is device : https://www.google.com/search?q=xpc-rc01&source=lnms&tbm=isch

I don't have the remote, but only the receiver. Maybe you have it somewhere in a box ?\
Anyway, Logitech Harmony have it in its database... (I Hope Logitech reconize this ad for them...)


## Issue with this device
When you plug it, it is reconized as an HID keybord. Fine ! But most of the commands doesn't works.\
The device emmit invalid keyboard code for most of the keys. I Think there is a driver for Windows..

## What the driver is ?
This driver still an HID one. It's the better way. \
But all the strage code emmited by the device are converted to valid keyboard one. \
**It still a keyboard**, you have to bind keys fro your favorite software (aka. KODI).

The keys send by the driver are hard-coded. (A config file is too boring to handle...). \
If you want to change them, edit source code or send me a patch to support a config file !


