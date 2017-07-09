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
  switch (raw_data[0])
  {
  //Field enum 1 : Standard keys.
  case 0x01:
    switch (raw_data[5]) {
    case 0x1E: return KEY_1;
    case 0x1F: return KEY_2;
    case 0x20: return KEY_3;
    case 0x21: return KEY_4;
    case 0x22: return KEY_5;
    case 0x23: return KEY_6;
    case 0x24: return KEY_7;
    case 0x25: return KEY_8;
    case 0x26: return KEY_9;
    case 0x27: return KEY_SPACE;
    case 0x28: return KEY_ENTER;
    case 0x2A: return KEY_M;
    case 0x4F: return KEY_RIGHT;
    case 0x50: return KEY_LEFT;
    case 0x51: return KEY_DOWN;
    case 0x52: return KEY_UP;
    }

    return KEY_RESERVED;


  //Field enum 2 : Control-keys. No repeat.
  case 0x02:
    switch(raw_data[1])
    {
    case 0x10: return KEY_F10;
    case 0x30: return KEY_SPACE;
    case 0x70: return KEY_I;
    }

    return KEY_RESERVED;


  //Field enum 3 : Radio-key. No repeat.
  case 0x03:
    if (raw_data[1] == 0x04) return KEY_F2;
    return KEY_RESERVED;


  //Field enum 4 : Extra keys. No repeat. Convert them to FX.
  case 0x04:
    switch(raw_data[1])
    {
    case 0x01: return KEY_I;
    case 0x02: return KEY_BACKSPACE;
    case 0x08: return KEY_A;
    case 0x10: return KEY_ESC;
    case 0x20: return KEY_Z;
    case 0x80: return KEY_SPACE;

    case 0x00:
      switch(raw_data[2])
      {
      case 0x04: return KEY_C;
      case 0x20: return KEY_SEMICOLON;
      }
    }

    return KEY_RESERVED;


  //Field enum 5: Multimedia keys.
  case 0x05:
    switch(raw_data[1])
    {
    case 0x01: return KEY_KPPLUS;    //VOL +
    case 0x02: return KEY_KPMINUS;   //VOL -
    case 0x04: return KEY_F8;        //MUTE
    case 0x08: return KEY_PAGEUP;    //CHANNEL +
    case 0x10: return KEY_PAGEDOWN;  //CHANNEL -

    case 0x00:
      switch(raw_data[2])
      {
      case 0x01: return KEY_P;   //PLAY
      case 0x02: return KEY_F9;  //RECORD
      case 0x04: return KEY_F;   //FORWARD
      case 0x08: return KEY_R;   //BACKWARD
      case 0x10: return KEY_DOT;
      case 0x20: return KEY_M;
      case 0x40: return KEY_X;
      }
    }
    return KEY_RESERVED;


  //Field enum 6 : Power-off key. No-repeat.
  case 0x06:
    if (raw_data[1] == 0x02) return KEY_S;
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

  //DEBUG  print_data(raw_data, size);


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
