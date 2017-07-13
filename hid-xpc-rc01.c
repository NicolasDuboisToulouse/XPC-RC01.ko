/*
 *  HID driver for XPC-RC01
 *
 *  Copyright (c) 2017 Dubois Nicolas
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/device.h>
#include <linux/input.h>
#include <linux/hid.h>
#include <linux/module.h>
#include <asm/unaligned.h>
#include <asm/byteorder.h>


#define USB_XPC_VENDOR 0x13ec
#define USB_XPC_DEVICE 0x0006


struct xpc_sc {
  unsigned long quirks;
  s32           last_key;
};



static s32 raw_data_to_key(u8 *raw_data)
{

  //                                              | REPEAT | Original       | MSI            | Suttle XPC G   |

  switch (raw_data[0])
  {
  //Field enum 1
  case 0x01:
    switch (raw_data[5]) {
    case 0x00: return KEY_F4;                  // | NO     |                |                | Replay         |
    case 0x1E: return KEY_1;                   // | YES    | 1              | 1              | 1              |
    case 0x1F: return KEY_2;                   // | YES    | 2              | 2              | 2              |
    case 0x20: return KEY_3;                   // | YES    | 3              | 3              | 3              |
    case 0x21: return KEY_4;                   // | YES    | 4              | 4              | 4              |
    case 0x22: return KEY_5;                   // | YES    | 5              | 5              | 5              |
    case 0x23: return KEY_6;                   // | YES    | 6              | 6              | 6              |
    case 0x24: return KEY_7;                   // | YES    | 7              | 7              | 7              |
    case 0x25: return KEY_8;                   // | YES    | 8              | 8              | 8              |
    case 0x26: return KEY_9;                   // | YES    | 9              | 9              | 9              |
    case 0x27: return KEY_0;                   // | YES    | 0              | 0              | 0              |
    case 0x28: return KEY_ENTER;               // | YES    | Select         | Select         | Play           |
    case 0x2A: return KEY_BACKSPACE;           // | YES    | Back           | Back           | Back           |
    case 0x4F: return KEY_RIGHT;               // | YES    | DirectionRight | DirectionRight | DirectionRight |
    case 0x50: return KEY_LEFT;                // | YES    | DirectionLeft  | DirectionLeft  | DirectionLeft  |
    case 0x51: return KEY_DOWN;                // | YES    | DirectionDown  | DirectionDown  | DirectionDown  |
    case 0x52: return KEY_UP;                  // | YES    | DirectionUp    | DirectionUp    | DirectionUp    |
    }

    return KEY_RESERVED;


  //Field enum 2
  case 0x02:
    switch(raw_data[1])
    {
    case 0x10: return KEY_F1;                  // | NO     | ORIG_TOGGLE    |                | Record         |
    case 0x30: return KEY_F2;                  // | YES    | ORIG_CAMERA    |                | Repeat         |
    case 0x70: return KEY_F3;                  // | YES    | ORIG_INFO      |                | Shuffle        |
    }

    return KEY_RESERVED;


  //Field enum 3
  case 0x03:
    switch(raw_data[1])
    {
    case 0x04: return KEY_F10;                 // | NO     | Radio          | Radio          |                |
    }

    return KEY_RESERVED;


  //Field enum 4
  case 0x04:
    switch(raw_data[1])
    {
    case 0x00:
      switch(raw_data[2])
      {
      case 0x04: return KEY_F5;                // | NO     | ORIG_MENU      |                | Info           |
      case 0x20: return KEY_F11;               // | NO     | ORIG_TV_TEXT   | Close          |                |
      }
    case 0x01: return KEY_F12;                 // | NO     | ORIG_HOME      |                | InputMusic     |
    case 0x02: return KEY_T;                   // | NO     | TV             | TV             |                |
    case 0x08: return KEY_E;                   // | NO     | ORIG_RED       | Music          | Eject          |
    case 0x10: return KEY_F9;                  // | NO     | ORIG_GREEN     | Picture        | Live           |
    case 0x20: return KEY_V;                   // | NO     | ORIG_BLUE      | Video          | Guide          |
    case 0x80: return KEY_D;                   // | NO     | ORIG_YELLOW    | DVDMenu        |                |
    }

    return KEY_RESERVED;


  //Field enum 5
  case 0x05:
    switch(raw_data[1])
    {
    case 0x00:
      switch(raw_data[2])
      {
      case 0x01: return KEY_P;                 // | NO     | Play & Pause   | Play & Pause   | Pause          |
      case 0x02: return KEY_F6;                // | NO     | Record         | Record         | InputTuner     |
      case 0x04: return KEY_F;                 // | YES    | FastForward    | FastForward    | ChannelUp      |
      case 0x08: return KEY_R;                 // | YES    | Rewind         | Rewind         | VolumeUp       |
      case 0x10: return KEY_F7;                // | NO     | Skip           | Skip           | Rewind         |
      case 0x20: return KEY_F8;                // | NO     | ORIG_PREVIOUS  | Replay         | Stop           |
      case 0x40: return KEY_S;                 // | NO     | Stop           | Stop           | InputTv        |
      }
    case 0x01: return KEY_KPPLUS;              // | YES    | VolumeUp       | VolumeUp       | VolumeDown     |
    case 0x02: return KEY_KPMINUS;             // | YES    | VolumeDown     | VolumeDown     | InputVideo     |
    case 0x04: return KEY_M;                   // | NO     | Mute           | Mute           | Mute           |
    case 0x08: return KEY_PAGEUP;              // | YES    | ChannelUp      | ChannelUp      | ChannelDown    |
    case 0x10: return KEY_PAGEDOWN;            // | YES    | ChannelDown    | ChannelDown    | InputPicture   |
    }

    return KEY_RESERVED;


  //Field enum 6
  case 0x06:
    switch(raw_data[1])
    {
    case 0x02: return KEY_END;                 // | NO     | PowerToggle    | PowerToggle    | PowerToggle    |
    }

    return KEY_RESERVED;


  default:
    return KEY_RESERVED;
  }

  return KEY_RESERVED;
}


void print_data(u8* data, int size)
{
  static char data_string[256];
  char* pos = data_string;

  int i = 0;
  for (i = 0; i < 8; i++) {
    sprintf(pos, "%02hhX ", data[i]);
    pos += 3;
  }
  *pos = 0;
  printk(data_string);
}


static int xpc_raw_event(struct hid_device *hdev, struct hid_report *report,
			 u8 *raw_data, int size)
{
  struct xpc_sc *xpc = hid_get_drvdata(hdev);
  struct hid_input *hidinput = list_entry(hdev->inputs.next, struct hid_input, list);
  s32 key = raw_data_to_key(raw_data);

  //DEBUG print_data(raw_data, size);


  if (!(hdev->claimed & HID_CLAIMED_INPUT))
    return 0;

  //Send key-up event
  if (xpc->last_key) {
    input_report_key(hidinput->input, xpc->last_key, 0);
    xpc->last_key = 0;
  }

  //Send key-down event
  else if (key) {
    input_report_key(hidinput->input, key, 1);
    xpc->last_key = key;
  }

  input_sync(hidinput->input);

  return 1;
}

int xpc_event(struct hid_device *hdev, struct hid_field *field,
	      struct hid_usage *usage, __s32 value)
{
  //All done in raw_event
  return 1;
}


static int xpc_probe(struct hid_device *hdev,
		     const struct hid_device_id *id)
{
  struct xpc_sc *xpc;
  int ret;

  xpc = kzalloc(sizeof(*xpc), GFP_KERNEL);
  if (xpc == NULL) {
    dev_err(&hdev->dev, "can't alloc device descriptor\n");
    ret = -ENOMEM;
    goto err_free;
  }

  xpc->quirks = id->driver_data;
  xpc->last_key = 0;

  hid_set_drvdata(hdev, xpc);

  ret = hid_parse(hdev);
  if (ret) {
    dev_err(&hdev->dev, "parse failed\n");
    goto err_free;
  }

  ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
  if (ret) {
    dev_err(&hdev->dev, "hw start failed\n");
    goto err_free;
  }

  printk("xpc-rc01: driver succefully started.\n");
  return 0;

 err_free:
  kfree(xpc);
  return ret;
}

static void xpc_remove(struct hid_device *hdev)
{
  struct xpc_sc *xpc = hid_get_drvdata(hdev);

  printk("xpc-rc01: remove driver.\n");
  hid_hw_stop(hdev);
  kfree(xpc);
}


static const struct hid_device_id xpc_devices[] = {
  { HID_USB_DEVICE(USB_XPC_VENDOR, USB_XPC_DEVICE) },
  { }
};
MODULE_DEVICE_TABLE(hid, xpc_devices);


static struct hid_driver xpc_driver = {
  .name          = "xpc-rc01",
  .id_table      = xpc_devices,
  .probe         = xpc_probe,
  .remove        = xpc_remove,
  .event         = xpc_event,
  .raw_event     = xpc_raw_event,
};

static int __init xpc_init(void)
{
  int ret;
  ret = hid_register_driver(&xpc_driver);
  if (ret) {
    printk("xpc-rc01: driver registration fail ! (code: %d)\n", ret);
  } else {
    printk("xpc-rc01: driver registered.\n");
  }
  return ret;
}

static void __exit xpc_exit(void)
{
  printk("xpc-rc01: unregister driver.\n");
  hid_unregister_driver(&xpc_driver);
}


module_init(xpc_init);
module_exit(xpc_exit);
MODULE_LICENSE("GPL");
