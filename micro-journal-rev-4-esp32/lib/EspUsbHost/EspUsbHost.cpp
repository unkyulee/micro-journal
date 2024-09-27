#include "EspUsbHost.h"

//
bool _capslock = false;
bool _isApple = false;

void EspUsbHost::_printPcapText(const char *title, uint16_t function, uint8_t direction, uint8_t endpoint, uint8_t type, uint8_t size, uint8_t stage, const uint8_t *data)
{
#if (ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO)
  uint8_t urbsize = 0x1c;
  if (stage == 0xff)
  {
    urbsize = 0x1b;
  }

  String data_str = "";
  for (int i = 0; i < size; i++)
  {
    if (data[i] < 16)
    {
      data_str += "0";
    }
    data_str += String(data[i], HEX) + " ";
  }

  printf("\n");
  printf("[PCAP TEXT]%s\n", title);
  printf("0000  %02x 00 00 00 00 00 00 00 00 00 00 00 00 00 %02x %02x\n", urbsize, (function & 0xff), ((function >> 8) & 0xff));
  printf("0010  %02x 01 00 01 00 %02x %02x %02x 00 00 00", direction, endpoint, type, size);
  if (stage != 0xff)
  {
    printf(" %02x\n", stage);
  }
  else
  {
    printf("\n");
  }
  printf("00%02x  %s\n", urbsize, data_str.c_str());
  printf("\n");
#endif
}

void EspUsbHost::begin(void)
{
  usbTransferSize = 0;

  const usb_host_config_t config = {
      .skip_phy_setup = false,
      .intr_flags = ESP_INTR_FLAG_LEVEL1,
  };
  esp_err_t err = usb_host_install(&config);
  if (err != ESP_OK)
  {
    ESP_LOGI("EspUsbHost", "usb_host_install() err=%x", err);
  }
  else
  {
    ESP_LOGI("EspUsbHost", "usb_host_install() ESP_OK");
  }

  const usb_host_client_config_t client_config = {
      .is_synchronous = true,
      .max_num_event_msg = 10,
      .async = {
          .client_event_callback = this->_clientEventCallback,
          .callback_arg = this,
      }};
  err = usb_host_client_register(&client_config, &this->clientHandle);
  if (err != ESP_OK)
  {
    ESP_LOGI("EspUsbHost", "usb_host_client_register() err=%x", err);
  }
  else
  {
    ESP_LOGI("EspUsbHost", "usb_host_client_register() ESP_OK");
  }
}

void EspUsbHost::_clientEventCallback(const usb_host_client_event_msg_t *eventMsg, void *arg)
{
  EspUsbHost *usbHost = (EspUsbHost *)arg;

  esp_err_t err;
  switch (eventMsg->event)
  {
  case USB_HOST_CLIENT_EVENT_NEW_DEV:
    ESP_LOGI("EspUsbHost", "USB_HOST_CLIENT_EVENT_NEW_DEV new_dev.address=%d", eventMsg->new_dev.address);
    err = usb_host_device_open(usbHost->clientHandle, eventMsg->new_dev.address, &usbHost->deviceHandle);
    if (err != ESP_OK)
    {
      ESP_LOGI("EspUsbHost", "usb_host_device_open() err=%x", err);
    }
    else
    {
      ESP_LOGI("EspUsbHost", "usb_host_device_open() ESP_OK");
    }

    usb_device_info_t dev_info;
    err = usb_host_device_info(usbHost->deviceHandle, &dev_info);
    if (err != ESP_OK)
    {
      ESP_LOGI("EspUsbHost", "usb_host_device_info() err=%x", err);
    }
    else
    {
      ESP_LOGI("EspUsbHost", "usb_host_device_info() ESP_OK\n"
                             "# speed                 = %d\n"
                             "# dev_addr              = %d\n"
                             "# vMaxPacketSize0       = %d\n"
                             "# bConfigurationValue   = %d\n"
                             "# str_desc_manufacturer = \"%s\"\n"
                             "# str_desc_product      = \"%s\"\n"
                             "# str_desc_serial_num   = \"%s\"",
               dev_info.speed,
               dev_info.dev_addr,
               dev_info.bMaxPacketSize0,
               dev_info.bConfigurationValue,
               getUsbDescString(dev_info.str_desc_manufacturer).c_str(),
               getUsbDescString(dev_info.str_desc_product).c_str(),
               getUsbDescString(dev_info.str_desc_serial_num).c_str());

      // check if Apple Inc.
      if (getUsbDescString(dev_info.str_desc_manufacturer) == "Apple Inc.")
      {
        _isApple = true;
      }
      else
      {
        _isApple = false;
      }
    }

    const usb_device_desc_t *dev_desc;
    err = usb_host_get_device_descriptor(usbHost->deviceHandle, &dev_desc);
    if (err != ESP_OK)
    {
      ESP_LOGI("EspUsbHost", "usb_host_get_device_descriptor() err=%x", err);
    }
    else
    {
      const uint8_t setup[8] = {0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x12, 0x00};
      _printPcapText("GET DESCRIPTOR Request DEVICE", 0x000b, 0x00, 0x80, 0x02, sizeof(setup), 0x00, setup);
      _printPcapText("GET DESCRIPTOR Response DEVICE", 0x0008, 0x01, 0x80, 0x02, sizeof(usb_device_desc_t), 0x03, (const uint8_t *)dev_desc);

      ESP_LOGI("EspUsbHost", "usb_host_get_device_descriptor() ESP_OK\n"
                             "#### DESCRIPTOR DEVICE ####\n"
                             "# bLength            = %d\n"
                             "# bDescriptorType    = %d\n"
                             "# bcdUSB             = 0x%x\n"
                             "# bDeviceClass       = 0x%x\n"
                             "# bDeviceSubClass    = 0x%x\n"
                             "# bDeviceProtocol    = 0x%x\n"
                             "# bMaxPacketSize0    = %d\n"
                             "# idVendor           = 0x%x\n"
                             "# idProduct          = 0x%x\n"
                             "# bcdDevice          = 0x%x\n"
                             "# iManufacturer      = %d\n"
                             "# iProduct           = %d\n"
                             "# iSerialNumber      = %d\n"
                             "# bNumConfigurations = %d",
               dev_desc->bLength,
               dev_desc->bDescriptorType,
               dev_desc->bcdUSB,
               dev_desc->bDeviceClass,
               dev_desc->bDeviceSubClass,
               dev_desc->bDeviceProtocol,
               dev_desc->bMaxPacketSize0,
               dev_desc->idVendor,
               dev_desc->idProduct,
               dev_desc->bcdDevice,
               dev_desc->iManufacturer,
               dev_desc->iProduct,
               dev_desc->iSerialNumber,
               dev_desc->bNumConfigurations);
    }

    const usb_config_desc_t *config_desc;
    err = usb_host_get_active_config_descriptor(usbHost->deviceHandle, &config_desc);
    if (err != ESP_OK)
    {
      ESP_LOGI("EspUsbHost", "usb_host_get_active_config_descriptor() err=%x", err);
    }
    else
    {
      const uint8_t setup[8] = {0x80, 0x06, 0x00, 0x02, 0x00, 0x00, 0x09, 0x00};
      _printPcapText("GET DESCRIPTOR Request CONFIGURATION", 0x000b, 0x00, 0x80, 0x02, sizeof(setup), 0x00, setup);
      _printPcapText("GET DESCRIPTOR Response CONFIGURATION", 0x0008, 0x01, 0x80, 0x02, sizeof(usb_config_desc_t), 0x03, (const uint8_t *)config_desc);

      ESP_LOGI("EspUsbHost", "usb_host_get_active_config_descriptor() ESP_OK\n"
                             "# bLength             = %d\n"
                             "# bDescriptorType     = %d\n"
                             "# wTotalLength        = %d\n"
                             "# bNumInterfaces      = %d\n"
                             "# bConfigurationValue = %d\n"
                             "# iConfiguration      = %d\n"
                             "# bmAttributes        = 0x%x\n"
                             "# bMaxPower           = %dmA",
               config_desc->bLength,
               config_desc->bDescriptorType,
               config_desc->wTotalLength,
               config_desc->bNumInterfaces,
               config_desc->bConfigurationValue,
               config_desc->iConfiguration,
               config_desc->bmAttributes,
               config_desc->bMaxPower * 2);
    }

    usbHost->_configCallback(config_desc);
    break;

  case USB_HOST_CLIENT_EVENT_DEV_GONE:
  {
    ESP_LOGI("EspUsbHost", "USB_HOST_CLIENT_EVENT_DEV_GONE dev_gone.dev_hdl=%x", eventMsg->dev_gone.dev_hdl);
    for (int i = 0; i < usbHost->usbTransferSize; i++)
    {
      if (usbHost->usbTransfer[i] == NULL)
      {
        continue;
      }

      err = usb_host_endpoint_clear(eventMsg->dev_gone.dev_hdl, usbHost->usbTransfer[i]->bEndpointAddress);
      if (err != ESP_OK)
      {
        ESP_LOGI("EspUsbHost", "usb_host_endpoint_clear() err=%x, dev_hdl=%x, bEndpointAddress=%x", err, eventMsg->dev_gone.dev_hdl, usbHost->usbTransfer[i]->bEndpointAddress);
      }
      else
      {
        ESP_LOGI("EspUsbHost", "usb_host_endpoint_clear() ESP_OK, dev_hdl=%x, bEndpointAddress=%x", eventMsg->dev_gone.dev_hdl, usbHost->usbTransfer[i]->bEndpointAddress);
      }

      err = usb_host_transfer_free(usbHost->usbTransfer[i]);
      if (err != ESP_OK)
      {
        ESP_LOGI("EspUsbHost", "usb_host_transfer_free() err=%x, err, usbTransfer=%x", err, usbHost->usbTransfer[i]);
      }
      else
      {
        ESP_LOGI("EspUsbHost", "usb_host_transfer_free() ESP_OK, usbTransfer=%x", usbHost->usbTransfer[i]);
      }

      usbHost->usbTransfer[i] = NULL;
    }
    usbHost->usbTransferSize = 0;

    for (int i = 0; i < usbHost->usbInterfaceSize; i++)
    {
      err = usb_host_interface_release(usbHost->clientHandle, usbHost->deviceHandle, usbHost->usbInterface[i]);
      if (err != ESP_OK)
      {
        ESP_LOGI("EspUsbHost", "usb_host_interface_release() err=%x, err, clientHandle=%x, deviceHandle=%x, Interface=%x", err, usbHost->clientHandle, usbHost->deviceHandle, usbHost->usbInterface[i]);
      }
      else
      {
        ESP_LOGI("EspUsbHost", "usb_host_interface_release() ESP_OK, clientHandle=%x, deviceHandle=%x, Interface=%x", usbHost->clientHandle, usbHost->deviceHandle, usbHost->usbInterface[i]);
      }

      usbHost->usbInterface[i] = 0;
    }
    usbHost->usbInterfaceSize = 0;

    usb_host_device_close(usbHost->clientHandle, usbHost->deviceHandle);

    usbHost->onGone(eventMsg);
  }
  break;

  default:
    ESP_LOGI("EspUsbHost", "clientEventCallback() default %d", eventMsg->event);
    break;
  }
}

void EspUsbHost::_configCallback(const usb_config_desc_t *config_desc)
{
  const uint8_t *p = &config_desc->val[0];
  uint8_t bLength;

  const uint8_t setup[8] = {0x80, 0x06, 0x00, 0x02, 0x00, 0x00, (uint8_t)config_desc->wTotalLength, 0x00};
  _printPcapText("GET DESCRIPTOR Request CONFIGURATION", 0x000b, 0x00, 0x80, 0x02, sizeof(setup), 0x00, setup);
  _printPcapText("GET DESCRIPTOR Response CONFIGURATION", 0x0008, 0x01, 0x80, 0x02, config_desc->wTotalLength, 0x03, (const uint8_t *)config_desc);

  for (int i = 0; i < config_desc->wTotalLength; i += bLength, p += bLength)
  {
    bLength = *p;
    if ((i + bLength) <= config_desc->wTotalLength)
    {
      const uint8_t bDescriptorType = *(p + 1);
      this->onConfig(bDescriptorType, p);
    }
    else
    {
      return;
    }
  }
}

void EspUsbHost::task(void)
{
  esp_err_t err = usb_host_lib_handle_events(1, &this->eventFlags);
  if (err != ESP_OK && err != ESP_ERR_TIMEOUT)
  {
    ESP_LOGI("EspUsbHost", "usb_host_lib_handle_events() err=%x eventFlags=%x", err, this->eventFlags);
  }

  err = usb_host_client_handle_events(this->clientHandle, 1);
  if (err != ESP_OK && err != ESP_ERR_TIMEOUT)
  {
    ESP_LOGI("EspUsbHost", "usb_host_client_handle_events() err=%x", err);
  }

  if (this->isReady)
  {
    unsigned long now = millis();
    if ((now - this->lastCheck) > this->interval)
    {
      this->lastCheck = now;

      for (int i = 0; i < this->usbTransferSize; i++)
      {
        if (this->usbTransfer[i] == NULL)
        {
          continue;
        }

        esp_err_t err = usb_host_transfer_submit(this->usbTransfer[i]);
        if (err != ESP_OK && err != ESP_ERR_NOT_FINISHED && err != ESP_ERR_INVALID_STATE)
        {
          // ESP_LOGI("EspUsbHost", "usb_host_transfer_submit() err=%x", err);
        }
      }
    }
  }
}

String EspUsbHost::getUsbDescString(const usb_str_desc_t *str_desc)
{
  String str = "";
  if (str_desc == NULL)
  {
    return str;
  }

  for (int i = 0; i < str_desc->bLength / 2; i++)
  {
    if (str_desc->wData[i] > 0xFF)
    {
      continue;
    }
    str += char(str_desc->wData[i]);
  }
  return str;
}

void EspUsbHost::onConfig(const uint8_t bDescriptorType, const uint8_t *p)
{
  switch (bDescriptorType)
  {
  case USB_DEVICE_DESC:
  {
    ESP_LOGI("EspUsbHost", "USB_DEVICE_DESC(0x01)");
  }
  break;

  case USB_CONFIGURATION_DESC:
  {
#if (ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO)
    const usb_config_desc_t *config_desc = (const usb_config_desc_t *)p;
    ESP_LOGI("EspUsbHost", "USB_CONFIGURATION_DESC(0x02)\n"
                           "# bLength             = %d\n"
                           "# bDescriptorType     = %d\n"
                           "# wTotalLength        = %d\n"
                           "# bNumInterfaces      = %d\n"
                           "# bConfigurationValue = %d\n"
                           "# iConfiguration      = %d\n"
                           "# bmAttributes        = 0x%x\n"
                           "# bMaxPower           = %dmA",
             config_desc->bLength,
             config_desc->bDescriptorType,
             config_desc->wTotalLength,
             config_desc->bNumInterfaces,
             config_desc->bConfigurationValue,
             config_desc->iConfiguration,
             config_desc->bmAttributes,
             config_desc->bMaxPower * 2);
#endif
  }
  break;

  case USB_STRING_DESC:
  {
#if (ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO)
    const usb_standard_desc_t *desc = (const usb_standard_desc_t *)p;
    String data_str = "";
    for (int i = 0; i < (desc->bLength - 2); i++)
    {
      if (desc->val[i] < 16)
      {
        data_str += "0";
      }
      data_str += String(desc->val[i], HEX) + " ";
    }
    ESP_LOGI("EspUsbHost", "USB_STRING_DESC(0x03) bLength=%d, bDescriptorType=0x%x, data=[%s]",
             desc->bLength,
             desc->bDescriptorType,
             data_str);
#endif
  }
  break;

  case USB_INTERFACE_DESC:
  {
    const usb_intf_desc_t *intf = (const usb_intf_desc_t *)p;
    ESP_LOGI("EspUsbHost", "USB_INTERFACE_DESC(0x04)\n"
                           "# bLength            = %d\n"
                           "# bDescriptorType    = %d\n"
                           "# bInterfaceNumber   = %d\n"
                           "# bAlternateSetting  = %d\n"
                           "# bNumEndpoints      = %d\n"
                           "# bInterfaceClass    = 0x%x\n"
                           "# bInterfaceSubClass = 0x%x\n"
                           "# bInterfaceProtocol = 0x%x\n"
                           "# iInterface         = %d",
             intf->bLength,
             intf->bDescriptorType,
             intf->bInterfaceNumber,
             intf->bAlternateSetting,
             intf->bNumEndpoints,
             intf->bInterfaceClass,
             intf->bInterfaceSubClass,
             intf->bInterfaceProtocol,
             intf->iInterface);

    this->claim_err = usb_host_interface_claim(this->clientHandle, this->deviceHandle, intf->bInterfaceNumber, intf->bAlternateSetting);
    if (this->claim_err != ESP_OK)
    {
      ESP_LOGI("EspUsbHost", "usb_host_interface_claim() err=%x", claim_err);
    }
    else
    {
      ESP_LOGI("EspUsbHost", "usb_host_interface_claim() ESP_OK");
      this->usbInterface[this->usbInterfaceSize] = intf->bInterfaceNumber;
      this->usbInterfaceSize++;
      _bInterfaceNumber = intf->bInterfaceNumber;
      _bInterfaceClass = intf->bInterfaceClass;
      _bInterfaceSubClass = intf->bInterfaceSubClass;
      _bInterfaceProtocol = intf->bInterfaceProtocol;
    }
  }
  break;

  case USB_ENDPOINT_DESC:
  {
    const usb_ep_desc_t *ep_desc = (const usb_ep_desc_t *)p;
    ESP_LOGI("EspUsbHost", "USB_ENDPOINT_DESC(0x05)\n"
                           "# bLength          = %d\n"
                           "# bDescriptorType  = %d\n"
                           "# bEndpointAddress = 0x%x(EndpointID=%d, Direction=%s)\n"
                           "# bmAttributes     = 0x%x(%s)\n"
                           "# wMaxPacketSize   = %d\n"
                           "# bInterval        = %d",
             ep_desc->bLength,
             ep_desc->bDescriptorType,
             ep_desc->bEndpointAddress, USB_EP_DESC_GET_EP_NUM(ep_desc), USB_EP_DESC_GET_EP_DIR(ep_desc) ? "IN" : "OUT",
             ep_desc->bmAttributes,
             (ep_desc->bmAttributes & USB_BM_ATTRIBUTES_XFERTYPE_MASK) == USB_BM_ATTRIBUTES_XFER_CONTROL ? "CTRL" : (ep_desc->bmAttributes & USB_BM_ATTRIBUTES_XFERTYPE_MASK) == USB_BM_ATTRIBUTES_XFER_ISOC ? "ISOC"
                                                                                                                : (ep_desc->bmAttributes & USB_BM_ATTRIBUTES_XFERTYPE_MASK) == USB_BM_ATTRIBUTES_XFER_BULK   ? "BULK"
                                                                                                                : (ep_desc->bmAttributes & USB_BM_ATTRIBUTES_XFERTYPE_MASK) == USB_BM_ATTRIBUTES_XFER_INT    ? "Interrupt"
                                                                                                                                                                                                             : "",
             ep_desc->wMaxPacketSize,
             ep_desc->bInterval);

    if (this->claim_err != ESP_OK)
    {
      ESP_LOGI("EspUsbHost", "claim_err skip");
      return;
    }

    this->endpoint_data_list[USB_EP_DESC_GET_EP_NUM(ep_desc)].bInterfaceNumber = _bInterfaceNumber;
    this->endpoint_data_list[USB_EP_DESC_GET_EP_NUM(ep_desc)].bInterfaceClass = _bInterfaceClass;
    this->endpoint_data_list[USB_EP_DESC_GET_EP_NUM(ep_desc)].bInterfaceSubClass = _bInterfaceSubClass;
    this->endpoint_data_list[USB_EP_DESC_GET_EP_NUM(ep_desc)].bInterfaceProtocol = _bInterfaceProtocol;
    this->endpoint_data_list[USB_EP_DESC_GET_EP_NUM(ep_desc)].bCountryCode = _bCountryCode;

    if ((ep_desc->bmAttributes & USB_BM_ATTRIBUTES_XFERTYPE_MASK) != USB_BM_ATTRIBUTES_XFER_INT)
    {
      ESP_LOGI("EspUsbHost", "err ep_desc->bmAttributes=%x", ep_desc->bmAttributes);
      return;
    }

    if (ep_desc->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK)
    {
      esp_err_t err = usb_host_transfer_alloc(ep_desc->wMaxPacketSize + 1, 0, &this->usbTransfer[this->usbTransferSize]);
      if (err != ESP_OK)
      {
        this->usbTransfer[this->usbTransferSize] = NULL;
        ESP_LOGI("EspUsbHost", "usb_host_transfer_alloc() err=%x", err);
        return;
      }
      else
      {
        ESP_LOGI("EspUsbHost", "usb_host_transfer_alloc() ESP_OK data_buffer_size=%d", ep_desc->wMaxPacketSize + 1);
      }

      this->usbTransfer[this->usbTransferSize]->device_handle = this->deviceHandle;
      this->usbTransfer[this->usbTransferSize]->bEndpointAddress = ep_desc->bEndpointAddress;
      this->usbTransfer[this->usbTransferSize]->callback = this->_onReceive;
      this->usbTransfer[this->usbTransferSize]->context = this;
      this->usbTransfer[this->usbTransferSize]->num_bytes = ep_desc->wMaxPacketSize;
      interval = ep_desc->bInterval;
      isReady = true;
      this->usbTransferSize++;
    }
  }
  break;

  case USB_INTERFACE_ASSOC_DESC:
  {
#if (ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO)
    const usb_iad_desc_t *iad_desc = (const usb_iad_desc_t *)p;
    ESP_LOGI("EspUsbHost", "USB_INTERFACE_ASSOC_DESC(0x0b)\n"
                           "# bLength           = %d\n"
                           "# bDescriptorType   = %d\n"
                           "# bFirstInterface   = %d\n"
                           "# bInterfaceCount   = %d\n"
                           "# bFunctionClass    = 0x%x\n"
                           "# bFunctionSubClass = 0x%x\n"
                           "# bFunctionProtocol = 0x%x\n"
                           "# iFunction         = %d",
             iad_desc->bLength,
             iad_desc->bDescriptorType,
             iad_desc->bFirstInterface,
             iad_desc->bInterfaceCount,
             iad_desc->bFunctionClass,
             iad_desc->bFunctionSubClass,
             iad_desc->bFunctionProtocol,
             iad_desc->iFunction);
#endif
  }
  break;

  case USB_HID_DESC:
  {
    const tusb_hid_descriptor_hid_t *hid_desc = (const tusb_hid_descriptor_hid_t *)p;
    ESP_LOGI("EspUsbHost", "USB_HID_DESC(0x21)\n"
                           "# bLength         = %d\n"
                           "# bDescriptorType = 0x%x\n"
                           "# bcdHID          = 0x%x\n"
                           "# bCountryCode    = 0x%x\n"
                           "# bNumDescriptors = %d\n"
                           "# bReportType     = 0x%x\n"
                           "# wReportLength   = %d",
             hid_desc->bLength,
             hid_desc->bDescriptorType,
             hid_desc->bcdHID,
             hid_desc->bCountryCode,
             hid_desc->bNumDescriptors,
             hid_desc->bReportType,
             hid_desc->wReportLength);
    _bCountryCode = hid_desc->bCountryCode;

    submitControl(0x81, 0x00, 0x22, _bInterfaceNumber, hid_desc->wReportLength);
  }
  break;

  default:
  {
#if (ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO)
    const usb_standard_desc_t *desc = (const usb_standard_desc_t *)p;
    String data_str = "";
    for (int i = 0; i < (desc->bLength - 2); i++)
    {
      if (desc->val[i] < 16)
      {
        data_str += "0";
      }
      data_str += String(desc->val[i], HEX) + " ";
    }
    ESP_LOGI("EspUsbHost", "USB_???_DESC(%02x) bLength=%d, bDescriptorType=0x%x, data=[%s]",
             bDescriptorType,
             desc->bLength,
             desc->bDescriptorType,
             data_str);
#endif
  }
  }
}

void EspUsbHost::_onReceive(usb_transfer_t *transfer)
{
  EspUsbHost *usbHost = (EspUsbHost *)transfer->context;
  endpoint_data_t *endpoint_data = &usbHost->endpoint_data_list[(transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_NUM_MASK)];

#if (ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_VERBOSE)
  _printPcapText("URB_INTERRUPT in", 0x0009, 0x01, transfer->bEndpointAddress, 0x01, transfer->actual_num_bytes, 0xff, (const uint8_t *)transfer->data_buffer);

  String buffer_str = "";
  for (int i = 0; i < transfer->actual_num_bytes; i++)
  {
    if (transfer->data_buffer[i] < 16)
    {
      buffer_str += "0";
    }
    buffer_str += String(transfer->data_buffer[i], HEX) + " ";
  }
  buffer_str.trim();
  ESP_LOGV("EspUsbHost", "transfer\n"
                         "# bInterfaceClass    = 0x%x\n"
                         "# bInterfaceSubClass = 0x%x\n"
                         "# bInterfaceProtocol = 0x%x\n"
                         "# bCountryCode       = 0x%x > usb_transfer_t data_buffer=[%s]\n"
                         "# data_buffer_size   = %d\n"
                         "# num_bytes          = %d\n"
                         "# actual_num_bytes   = %d\n"
                         "# flags              = 0x%x\n"
                         "# bEndpointAddress   = 0x%x\n"
                         "# timeout_ms         = %d\n"
                         "# num_isoc_packets   = %d",
           endpoint_data->bInterfaceClass,
           endpoint_data->bInterfaceSubClass,
           endpoint_data->bInterfaceProtocol,
           endpoint_data->bCountryCode,
           buffer_str.c_str(),
           transfer->data_buffer_size,
           transfer->num_bytes,
           transfer->actual_num_bytes,
           transfer->flags,
           transfer->bEndpointAddress,
           transfer->timeout_ms,
           transfer->num_isoc_packets);
#endif

  if (endpoint_data->bInterfaceClass == USB_CLASS_HID)
  {
    if (endpoint_data->bInterfaceSubClass == HID_SUBCLASS_BOOT)
    {
      if (endpoint_data->bInterfaceProtocol == HID_ITF_PROTOCOL_KEYBOARD)
      {
        static hid_keyboard_report_t last_report = {};

        if (transfer->data_buffer[2] == HID_KEY_NUM_LOCK)
        {
          // HID_KEY_NUM_LOCK TODO!
        }

        if (transfer->data_buffer[2] == HID_KEY_CAPS_LOCK)
        {
          // Mark caps lock status
          _capslock = !_capslock;
        }

        else if (memcmp(&last_report, transfer->data_buffer, sizeof(last_report)))
        {
          // chenge
          hid_keyboard_report_t report = {};
          report.modifier = transfer->data_buffer[0];
          report.reserved = transfer->data_buffer[1];
          report.keycode[0] = transfer->data_buffer[2];
          report.keycode[1] = transfer->data_buffer[3];
          report.keycode[2] = transfer->data_buffer[4];
          report.keycode[3] = transfer->data_buffer[5];
          report.keycode[4] = transfer->data_buffer[6];
          report.keycode[5] = transfer->data_buffer[7];

          if (_isApple)
          {
            // at the reserved position sends the modifier
            //
            report.modifier = transfer->data_buffer[1];
            //report.reserved = transfer->data_buffer[0];
            report.keycode[0] = transfer->data_buffer[2];
            report.keycode[1] = transfer->data_buffer[3];
            report.keycode[2] = transfer->data_buffer[4];
            report.keycode[3] = transfer->data_buffer[5];
            report.keycode[4] = transfer->data_buffer[6];
            report.keycode[5] = transfer->data_buffer[7];

            // caps lock at 4th byte
            if (transfer->data_buffer[3] == HID_KEY_CAPS_LOCK)
            {
              // Mark caps lock status
              _capslock = !_capslock;
            }
          }

          usbHost->onKeyboard(report, last_report);

          // control shift key pressed
          bool shift = (report.modifier & KEYBOARD_MODIFIER_LEFTSHIFT) || (report.modifier & KEYBOARD_MODIFIER_RIGHTSHIFT);
          // apple keyboard has different SHIFT code

          // when capslock is on then shift key is always pressed
          if (_capslock)
            shift = true;

          // control if altgr key is pressed
          bool altgr = (report.modifier & KEYBOARD_MODIFIER_RIGHTALT) || (report.modifier & KEYBOARD_MODIFIER_RIGHTALT);

          // Check for released keys
          for (int i = 0; i < 6; i++)
          {
            if (last_report.keycode[i] != 0)
            {
              bool key_released = true;
              for (int j = 0; j < 6; j++)
              {
                if (last_report.keycode[i] == report.keycode[j])
                {
                  key_released = false;
                  break;
                }
              }
              if (key_released)
              {
                usbHost->onKeyboardKeyReleased(usbHost->getKeycodeToAscii(last_report.keycode[i], shift, altgr, true), last_report.keycode[i], shift);
              }
            }
          }

          for (int i = 0; i < 6; i++)
          {
            // a key is pressed or kept pressed
            if (report.keycode[i] != 0)
            {
              // check if the same key appear in the previous report
              bool newkey = true;
              for (int j = 0; j < 6; j++)
              {
                if (last_report.keycode[j] == report.keycode[i])
                {
                  newkey = false;
                  break;
                }
              }

              // otherwise register new key press
              if (newkey)
                usbHost->onKeyboardKey(usbHost->getKeycodeToAscii(report.keycode[i], shift, altgr, false), report.keycode[i], shift);
            }
          }

          memcpy(&last_report, &report, sizeof(last_report));
        }
      }
      else if (endpoint_data->bInterfaceProtocol == HID_ITF_PROTOCOL_MOUSE)
      {
        static uint8_t last_buttons = 0;
        hid_mouse_report_t report = {};
        report.buttons = transfer->data_buffer[1];
        report.x = (uint8_t)transfer->data_buffer[2];
        report.y = (uint8_t)transfer->data_buffer[4];
        report.wheel = (uint8_t)transfer->data_buffer[6];
        usbHost->onMouse(report, last_buttons);
        if (report.buttons != last_buttons)
        {
          usbHost->onMouseButtons(report, last_buttons);
          last_buttons = report.buttons;
        }
        if (report.x != 0 || report.y != 0 || report.wheel != 0)
        {
          usbHost->onMouseMove(report);
        }
      }
    }
    else
    {
      // usbHost->_onReceiveGamepad();
    }
  }

  usbHost->onReceive(transfer);
}

void EspUsbHost::onMouse(hid_mouse_report_t report, uint8_t last_buttons)
{
  ESP_LOGD("EspUsbHost", "last_buttons=0x%02x(%c%c%c%c%c), buttons=0x%02x(%c%c%c%c%c), x=%d, y=%d, wheel=%d",
           last_buttons,
           (last_buttons & MOUSE_BUTTON_LEFT) ? 'L' : ' ',
           (last_buttons & MOUSE_BUTTON_RIGHT) ? 'R' : ' ',
           (last_buttons & MOUSE_BUTTON_MIDDLE) ? 'M' : ' ',
           (last_buttons & MOUSE_BUTTON_BACKWARD) ? 'B' : ' ',
           (last_buttons & MOUSE_BUTTON_FORWARD) ? 'F' : ' ',
           report.buttons,
           (report.buttons & MOUSE_BUTTON_LEFT) ? 'L' : ' ',
           (report.buttons & MOUSE_BUTTON_RIGHT) ? 'R' : ' ',
           (report.buttons & MOUSE_BUTTON_MIDDLE) ? 'M' : ' ',
           (report.buttons & MOUSE_BUTTON_BACKWARD) ? 'B' : ' ',
           (report.buttons & MOUSE_BUTTON_FORWARD) ? 'F' : ' ',
           report.x,
           report.y,
           report.wheel);
}

void EspUsbHost::onMouseButtons(hid_mouse_report_t report, uint8_t last_buttons)
{
  ESP_LOGD("EspUsbHost", "last_buttons=0x%02x(%c%c%c%c%c), buttons=0x%02x(%c%c%c%c%c), x=%d, y=%d, wheel=%d",
           last_buttons,
           (last_buttons & MOUSE_BUTTON_LEFT) ? 'L' : ' ',
           (last_buttons & MOUSE_BUTTON_RIGHT) ? 'R' : ' ',
           (last_buttons & MOUSE_BUTTON_MIDDLE) ? 'M' : ' ',
           (last_buttons & MOUSE_BUTTON_BACKWARD) ? 'B' : ' ',
           (last_buttons & MOUSE_BUTTON_FORWARD) ? 'F' : ' ',
           report.buttons,
           (report.buttons & MOUSE_BUTTON_LEFT) ? 'L' : ' ',
           (report.buttons & MOUSE_BUTTON_RIGHT) ? 'R' : ' ',
           (report.buttons & MOUSE_BUTTON_MIDDLE) ? 'M' : ' ',
           (report.buttons & MOUSE_BUTTON_BACKWARD) ? 'B' : ' ',
           (report.buttons & MOUSE_BUTTON_FORWARD) ? 'F' : ' ',
           report.x,
           report.y,
           report.wheel);

  // LEFT
  if (!(last_buttons & MOUSE_BUTTON_LEFT) && (report.buttons & MOUSE_BUTTON_LEFT))
  {
    ESP_LOGI("EspUsbHost", "Mouse LEFT Click");
  }
  if ((last_buttons & MOUSE_BUTTON_LEFT) && !(report.buttons & MOUSE_BUTTON_LEFT))
  {
    ESP_LOGI("EspUsbHost", "Mouse LEFT Release");
  }

  // RIGHT
  if (!(last_buttons & MOUSE_BUTTON_RIGHT) && (report.buttons & MOUSE_BUTTON_RIGHT))
  {
    ESP_LOGI("EspUsbHost", "Mouse RIGHT Click");
  }
  if ((last_buttons & MOUSE_BUTTON_RIGHT) && !(report.buttons & MOUSE_BUTTON_RIGHT))
  {
    ESP_LOGI("EspUsbHost", "Mouse RIGHT Release");
  }

  // MIDDLE
  if (!(last_buttons & MOUSE_BUTTON_MIDDLE) && (report.buttons & MOUSE_BUTTON_MIDDLE))
  {
    ESP_LOGI("EspUsbHost", "Mouse MIDDLE Click");
  }
  if ((last_buttons & MOUSE_BUTTON_MIDDLE) && !(report.buttons & MOUSE_BUTTON_MIDDLE))
  {
    ESP_LOGI("EspUsbHost", "Mouse MIDDLE Release");
  }

  // BACKWARD
  if (!(last_buttons & MOUSE_BUTTON_BACKWARD) && (report.buttons & MOUSE_BUTTON_BACKWARD))
  {
    ESP_LOGI("EspUsbHost", "Mouse BACKWARD Click");
  }
  if ((last_buttons & MOUSE_BUTTON_BACKWARD) && !(report.buttons & MOUSE_BUTTON_BACKWARD))
  {
    ESP_LOGI("EspUsbHost", "Mouse BACKWARD Release");
  }

  // FORWARD
  if (!(last_buttons & MOUSE_BUTTON_FORWARD) && (report.buttons & MOUSE_BUTTON_FORWARD))
  {
    ESP_LOGI("EspUsbHost", "Mouse FORWARD Click");
  }
  if ((last_buttons & MOUSE_BUTTON_FORWARD) && !(report.buttons & MOUSE_BUTTON_FORWARD))
  {
    ESP_LOGI("EspUsbHost", "Mouse FORWARD Release");
  }
}

void EspUsbHost::onMouseMove(hid_mouse_report_t report)
{
  ESP_LOGD("EspUsbHost", "buttons=0x%02x(%c%c%c%c%c), x=%d, y=%d, wheel=%d",
           report.buttons,
           (report.buttons & MOUSE_BUTTON_LEFT) ? 'L' : ' ',
           (report.buttons & MOUSE_BUTTON_RIGHT) ? 'R' : ' ',
           (report.buttons & MOUSE_BUTTON_MIDDLE) ? 'M' : ' ',
           (report.buttons & MOUSE_BUTTON_BACKWARD) ? 'B' : ' ',
           (report.buttons & MOUSE_BUTTON_FORWARD) ? 'F' : ' ',
           report.x,
           report.y,
           report.wheel);
}

void EspUsbHost::setHIDLocal(hid_local_enum_t code)
{
  hidLocal = code;
}

esp_err_t EspUsbHost::submitControl(const uint8_t bmRequestType, const uint8_t bDescriptorIndex, const uint8_t bDescriptorType, const uint16_t wInterfaceNumber, const uint16_t wDescriptorLength)
{
  usb_transfer_t *transfer;
  usb_host_transfer_alloc(wDescriptorLength + 8 + 1, 0, &transfer);

  transfer->num_bytes = wDescriptorLength + 8;
  transfer->data_buffer[0] = bmRequestType;
  transfer->data_buffer[1] = 0x06;
  transfer->data_buffer[2] = bDescriptorIndex;
  transfer->data_buffer[3] = bDescriptorType;
  transfer->data_buffer[4] = wInterfaceNumber & 0xff;
  transfer->data_buffer[5] = wInterfaceNumber >> 8;
  transfer->data_buffer[6] = wDescriptorLength & 0xff;
  transfer->data_buffer[7] = wDescriptorLength >> 8;

  transfer->device_handle = deviceHandle;
  transfer->bEndpointAddress = 0x00;
  transfer->callback = _onReceiveControl;
  transfer->context = this;

  // submitControl(0x81, 0x00, 0x22, 0x0000, 136);
  if (bmRequestType == 0x81 && bDescriptorIndex == 0x00 && bDescriptorType == 0x22)
  {
    _printPcapText("GET DESCRIPTOR Request HID Report", 0x0028, 0x00, 0x80, 0x02, 8, 0, transfer->data_buffer);
  }

  esp_err_t err = usb_host_transfer_submit_control(clientHandle, transfer);
  if (err != ESP_OK)
  {
    ESP_LOGI("EspUsbHost", "usb_host_transfer_submit_control() err=%x", err);
  }
  return err;
}

void EspUsbHost::_onReceiveControl(usb_transfer_t *transfer)
{
  _printPcapText("GET DESCRIPTOR Response HID Report", 0x0008, 0x01, 0x80, 0x02, transfer->actual_num_bytes - 8, 0x03, &transfer->data_buffer[8]);

  ESP_LOGV("EspUsbHost", "_onReceiveControl()\n"
                         "# data_buffer_size   = %d\n"
                         "# num_bytes          = %d\n"
                         "# actual_num_bytes   = %d\n"
                         "# flags              = 0x%x\n"
                         "# bEndpointAddress   = 0x%x\n"
                         "# timeout_ms         = %d\n"
                         "# num_isoc_packets   = %d",
           transfer->data_buffer_size,
           transfer->num_bytes,
           transfer->actual_num_bytes,
           transfer->flags,
           transfer->bEndpointAddress,
           transfer->timeout_ms,
           transfer->num_isoc_packets);

#if (ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO)
  printf("=====================================================\n");
  uint16_t page = 0;
  uint8_t level = 0;
  uint8_t size = 0;
  uint8_t reportId = 0;
  uint8_t collection = 0;
  uint8_t usage = 0;
  uint8_t *p = &transfer->data_buffer[8];
  for (int i = 0; i < (transfer->actual_num_bytes - 8); i += size)
  {
    size = (p[i] & 3) + 1;

    // RAW data
    printf("[");
    for (int j = 0; j < size; j++)
    {
      // Hex
      printf(" %02x", p[i + j]);
    }
    for (int j = 0; j < (3 - size); j++)
    {
      // Padding
      printf("   ");
    }
    printf(" ] ");

    // pre
    if (p[i] == 0xc0)
    {
      // END_COLLECTION
      level--;
    }

    // level padding
    for (int j = 0; j < level; j++)
    {
      printf("  ");
    }

    // item
    uint8_t item = getItem(p[i]);
    int16_t val = (int8_t)p[i + 1];
    if (size == 3)
    {
      val = *(int16_t *)&p[i + 1];
    }
    if (item == getItem(0x05))
    {
      printf("USAGE_PAGE ");
      page = p[i + 1];
      if (page == 0x01)
      {
        printf("(Generic Desktop)");
      }
      else if (page == 0x02)
      {
        printf("(Simulation Controls)");
      }
      else if (page == 0x03)
      {
        printf("(VR Controls)");
      }
      else if (page == 0x04)
      {
        printf("(Sport Controls)");
      }
      else if (page == 0x05)
      {
        printf("(Game Controls)");
      }
      else if (page == 0x06)
      {
        printf("(Generic Device Controls)");
      }
      else if (page == 0x07)
      {
        printf("(Keyboard/Keypad)");
      }
      else if (page == 0x08)
      {
        printf("(LED)");
      }
      else if (page == 0x09)
      {
        printf("(Button)");
      }
      else if (page == 0x0a)
      {
        printf("(Ordinal)");
      }
      else if (page == 0x0b)
      {
        printf("(Telephony Device)");
      }
      else if (page == 0x0c)
      {
        printf("(Consumer)");
      }
      else if (page == 0x0d)
      {
        printf("(Digitizers)");
      }
      else if (page == 0x0e)
      {
        printf("(Haptics)");
      }
      else if (page == 0x0f)
      {
        printf("(Physical Input Device)");
      }
      else if (page == 0x10)
      {
        printf("(Unicode)");
      }
      else if (page == 0x11)
      {
        printf("(SoC)");
      }
      else if (page == 0x12)
      {
        printf("(Eye and Head Trackers)");
      }
      else if (page == 0x14)
      {
        printf("(Auxiliary Display)");
      }
      else if (page == 0x20)
      {
        printf("(Sensors)");
      }
      else if (page == 0x40)
      {
        printf("(Medical Instrument)");
      }
      else if (page == 0x41)
      {
        printf("(Braille Display)");
      }
      else if (page == 0x59)
      {
        printf("(Lighting And Illumination)");
      }
      else if (page == 0x80)
      {
        printf("(Monitor)");
      }
      else if (page == 0x81)
      {
        printf("(Monitor Enumerated)");
      }
      else if (page == 0x82)
      {
        printf("(VESA Virtual Controls)");
      }
      else if (page == 0x84)
      {
        printf("(Power)");
      }
      else if (page == 0x85)
      {
        printf("(Battery System)");
      }
      else if (page == 0x8c)
      {
        printf("(Barcode Scanner)");
      }
      else if (page == 0x8d)
      {
        printf("(Scales)");
      }
      else if (page == 0x8e)
      {
        printf("(Magnetic Stripe Reader)");
      }
      else if (page == 0x90)
      {
        printf("(Camera Control)");
      }
      else if (page == 0x91)
      {
        printf("(Arcade)");
      }
      else if (page == 0x92)
      {
        printf("(Gaming Device)");
      }
      else
      {
        if (size == 2)
        {
          printf("(Vendor 0x%02x)", p[i + 1]);
        }
        else
        {
          printf("(Vendor 0x%02x%02x)", p[i + 2], p[i + 1]);
        }
      }
    }
    else if (item == getItem(0x09))
    {
      printf("USAGE ");
      usage = p[i + 1];
      if (page == 0x01)
      {
        // Generic Desktop Page (0x01)
        if (usage == 0x00)
        {
          printf("(Undefined)");
        }
        else if (usage == 0x01)
        {
          printf("(Pointer)");
        }
        else if (usage == 0x02)
        {
          printf("(Mouse)");
        }
        else if (usage == 0x04)
        {
          printf("(Joystick)");
        }
        else if (usage == 0x05)
        {
          printf("(Gamepad)");
        }
        else if (usage == 0x06)
        {
          printf("(Keyboard)");
        }
        else if (usage == 0x07)
        {
          printf("(Keypad)");
        }
        else if (usage == 0x30)
        {
          printf("(X)");
        }
        else if (usage == 0x31)
        {
          printf("(Y)");
        }
        else if (usage == 0x32)
        {
          printf("(Z)");
        }
        else if (usage == 0x33)
        {
          printf("(Rx)");
        }
        else if (usage == 0x34)
        {
          printf("(Ry)");
        }
        else if (usage == 0x35)
        {
          printf("(Rz)");
        }
        else if (usage == 0x36)
        {
          printf("(Slider)");
        }
        else if (usage == 0x37)
        {
          printf("(Dial)");
        }
        else if (usage == 0x38)
        {
          printf("(Wheel)");
        }
        else if (usage == 0x39)
        {
          printf("(Hat Switch)");
        }
        else if (usage == 0x39)
        {
          printf("(Hat Switch)");
        }
        else if (usage == 0x3A)
        {
          printf("(Counted Buffer)");
        }
        else if (usage == 0x3B)
        {
          printf("(Byte Count)");
        }
        else if (usage == 0x3C)
        {
          printf("(Motion Wakeup)");
        }
        else if (usage == 0x3D)
        {
          printf("(Start)");
        }
        else if (usage == 0x3E)
        {
          printf("(Select)");
        }
        else if (usage == 0x40)
        {
          printf("(Vx)");
        }
        else if (usage == 0x41)
        {
          printf("(Vy)");
        }
        else if (usage == 0x42)
        {
          printf("(Vz)");
        }
        else if (usage == 0x43)
        {
          printf("(Vbrx)");
        }
        else if (usage == 0x44)
        {
          printf("(Vbry)");
        }
        else if (usage == 0x45)
        {
          printf("(Vbrz)");
        }
        else if (usage == 0x46)
        {
          printf("(Vno)");
        }
        else if (usage == 0x47)
        {
          printf("(Feature Notification)");
        }
        else if (usage == 0x48)
        {
          printf("(Resolution Multiplier)");
        }
        else if (usage == 0x49)
        {
          printf("(Qx)");
        }
        else if (usage == 0x4A)
        {
          printf("(Qy)");
        }
        else if (usage == 0x4B)
        {
          printf("(Qz)");
        }
        else if (usage == 0x4C)
        {
          printf("(Qw)");
        }
        else if (usage == 0x80)
        {
          printf("(System Control)");
        }
        else if (usage == 0x81)
        {
          printf("(System Power Down)");
        }
        else if (usage == 0x82)
        {
          printf("(System Sleep)");
        }
        else if (usage == 0x83)
        {
          printf("(System Wake Up)");
        }
        else if (usage == 0x84)
        {
          printf("(System Context Menu)");
        }
        else if (usage == 0x85)
        {
          printf("(System Main Menu)");
        }
        else if (usage == 0x86)
        {
          printf("(System App Menu)");
        }
        else if (usage == 0x87)
        {
          printf("(System Menu Help)");
        }
        else if (usage == 0x88)
        {
          printf("(System Menu Exit)");
        }
        else if (usage == 0x89)
        {
          printf("(System Menu Select)");
        }
        else if (usage == 0x8A)
        {
          printf("(System Menu Right)");
        }
        else if (usage == 0x8B)
        {
          printf("(System Menu Left)");
        }
        else if (usage == 0x8C)
        {
          printf("(System Menu Up)");
        }
        else if (usage == 0x8D)
        {
          printf("(System Menu Down)");
        }
        else if (usage == 0x8E)
        {
          printf("(System Cold Restart)");
        }
        else if (usage == 0x8F)
        {
          printf("(System Warm Restart)");
        }
        else if (usage == 0x90)
        {
          printf("(D-pad Up)");
        }
        else if (usage == 0x91)
        {
          printf("(D-pad Down)");
        }
        else if (usage == 0x92)
        {
          printf("(D-pad Right)");
        }
        else if (usage == 0x93)
        {
          printf("(D-pad Left)");
        }
        else if (usage == 0x94)
        {
          printf("(Index Trigger)");
        }
        else if (usage == 0x95)
        {
          printf("(Palm Trigger)");
        }
        else if (usage == 0x96)
        {
          printf("(Thumbstick)");
        }
        else if (usage == 0x97)
        {
          printf("(System Function Shift)");
        }
        else if (usage == 0x98)
        {
          printf("(System Function Shift Lock)");
        }
        else if (usage == 0x99)
        {
          printf("(System Function Shift Lock Indicator)");
        }
        else if (usage == 0x9A)
        {
          printf("(System Dismiss Notification)");
        }
        else if (usage == 0x9B)
        {
          printf("(System Do Not Disturb)");
        }
        else if (usage == 0xA0)
        {
          printf("(System Dock)");
        }
        else if (usage == 0xA1)
        {
          printf("(System Undock)");
        }
        else if (usage == 0xA2)
        {
          printf("(System Setup)");
        }
        else if (usage == 0xA3)
        {
          printf("(System Break)");
        }
        else if (usage == 0xA4)
        {
          printf("(System Debugger Break)");
        }
        else if (usage == 0xA5)
        {
          printf("(Application Break)");
        }
        else if (usage == 0xA6)
        {
          printf("(Application Debugger Break)");
        }
        else if (usage == 0xA7)
        {
          printf("(System Speaker Mute)");
        }
        else if (usage == 0xA8)
        {
          printf("(System Hibernate)");
        }
        else if (usage == 0xA9)
        {
          printf("(System Microphone Mute)");
        }
        else if (usage == 0xB0)
        {
          printf("(System Display Invert)");
        }
        else if (usage == 0xB1)
        {
          printf("(System Display Internal)");
        }
        else if (usage == 0xB2)
        {
          printf("(System Display External)");
        }
        else if (usage == 0xB3)
        {
          printf("(System Display Both)");
        }
        else if (usage == 0xB4)
        {
          printf("(System Display Dual)");
        }
        else if (usage == 0xB5)
        {
          printf("(System Display Toggle Int/Ext Mode)");
        }
        else if (usage == 0xB6)
        {
          printf("(System Display Swap Primary/Secondary)");
        }
        else if (usage == 0xB7)
        {
          printf("(System Display Toggle LCD Autoscale)");
        }
        else if (usage == 0xC0)
        {
          printf("(Sensor Zone)");
        }
        else if (usage == 0xC1)
        {
          printf("(RPM)");
        }
        else if (usage == 0xC2)
        {
          printf("(Coolant Level)");
        }
        else if (usage == 0xC3)
        {
          printf("(Coolant Critical Level)");
        }
        else if (usage == 0xC4)
        {
          printf("(Coolant Pump)");
        }
        else if (usage == 0xC5)
        {
          printf("(Chassis Enclosure)");
        }
        else if (usage == 0xC6)
        {
          printf("(Wireless Radio Button)");
        }
        else if (usage == 0xC7)
        {
          printf("(Wireless Radio LED)");
        }
        else if (usage == 0xC8)
        {
          printf("(Wireless Radio Slider Switch)");
        }
        else if (usage == 0xC9)
        {
          printf("(System Display Rotation Lock Button)");
        }
        else if (usage == 0xCA)
        {
          printf("(System Display Rotation Lock Slider Switch)");
        }
        else if (usage == 0xCB)
        {
          printf("(Control Enable)");
        }
        else if (usage == 0xD0)
        {
          printf("(Dockable Device Unique ID)");
        }
        else if (usage == 0xD1)
        {
          printf("(Dockable Device Vendor ID)");
        }
        else if (usage == 0xD2)
        {
          printf("(Dockable Device Primary Usage Page)");
        }
        else if (usage == 0xD3)
        {
          printf("(Dockable Device Primary Usage ID)");
        }
        else if (usage == 0xD4)
        {
          printf("(Dockable Device Docking State)");
        }
        else if (usage == 0xD5)
        {
          printf("(Dockable Device Display Occlusion)");
        }
        else if (usage == 0xD6)
        {
          printf("(Dockable Device Object Type)");
        }
        else if (usage == 0xE0)
        {
          printf("(Call Active LED)");
        }
        else if (usage == 0xE1)
        {
          printf("(Call Mute Toggle)");
        }
        else if (usage == 0xE2)
        {
          printf("(Call Mute LED)");
        }
        else
        {
          printf("(? ? ? ?)");
        }
      }
      else
      {
        printf("(0x%02x)", usage);
      }
    }
    else if (item == getItem(0x15))
    {
      printf("LOGICAL_MINIMUM ");
      printf("(%d)", val);
    }
    else if (item == getItem(0x19))
    {
      printf("USAGE_MINIMUM ");
      if (size == 2)
      {
        printf("(0x%02x)", p[i + 1]);
      }
      else
      {
        printf("(0x%02x%02x)", p[i + 2], p[i + 1]);
      }
    }
    else if (item == getItem(0x25))
    {
      printf("LOGICAL_MAXIMUM ");
      printf("(%d)", val);
    }
    else if (item == getItem(0x29))
    {
      printf("USAGE_MAXIMUM ");
      if (size == 2)
      {
        printf("(0x%02x)", p[i + 1]);
      }
      else
      {
        printf("(0x%02x%02x)", p[i + 2], p[i + 1]);
      }
    }
    else if (item == getItem(0x35))
    {
      printf("PHYSIAL_MINIMUM ");
      printf("(%d)", val);
    }
    else if (item == getItem(0x45))
    {
      printf("PHYSIAL_MAXIMUM ");
      printf("(%d)", val);
    }
    else if (item == getItem(0x55))
    {
      printf("UNIT_EXPONENT ");
      if (size == 2)
      {
        printf("(0x%02x)", p[i + 1]);
      }
      else
      {
        printf("(0x%02x%02x)", p[i + 2], p[i + 1]);
      }
    }
    else if (item == getItem(0x65))
    {
      printf("UNIT ");
      if (size == 2)
      {
        printf("(0x%02x)", p[i + 1]);
      }
      else
      {
        printf("(0x%02x%02x)", p[i + 2], p[i + 1]);
      }
    }
    else if (item == getItem(0x75))
    {
      printf("REPORT_SIZE ");
      printf("(%d)", val);
    }
    else if (item == getItem(0x81))
    {
      printf("INPUT ");
      uint8_t val = p[i + 1];
      printf("(");
      if (val & (1 << 0))
      {
        // 1
        printf("Cnst,");
      }
      else
      {
        // 0
        printf("Data,");
      }
      if (val & (1 << 1))
      {
        // 1
        printf("Var,");
      }
      else
      {
        // 0
        printf("Ary,");
      }
      if (val & (1 << 2))
      {
        // 1
        printf("Rel");
      }
      else
      {
        // 0
        printf("Abs");
      }
      printf(")");
    }
    else if (item == getItem(0x85))
    {
      printf("REPORT_ID ");
      reportId = p[i + 1];
      printf("(%d)", reportId);
    }
    else if (item == getItem(0x91))
    {
      printf("OUTPUT ");
      uint8_t val = p[i + 1];
      printf("(");
      if (val & (1 << 0))
      {
        // 1
        printf("Cnst,");
      }
      else
      {
        // 0
        printf("Data,");
      }
      if (val & (1 << 1))
      {
        // 1
        printf("Var,");
      }
      else
      {
        // 0
        printf("Ary,");
      }
      if (val & (1 << 2))
      {
        // 1
        printf("Rel");
      }
      else
      {
        // 0
        printf("Abs");
      }
      printf(")");
    }
    else if (item == getItem(0x95))
    {
      printf("REPORT_COUNT ");
      printf("(%d)", val);
    }
    else if (item == getItem(0xa1))
    {
      printf("COLLECTION ");
      level++;
      collection = p[i + 1];
      if (collection == 0x00)
      {
        printf("(Physical)");
      }
      else if (collection == 0x01)
      {
        printf("(Application)");
      }
      else if (collection == 0x02)
      {
        printf("(Logical)");
      }
      else
      {
        printf("(? ? ? ?)");
      }
    }
    else if (item == getItem(0xa4))
    {
      printf("PUSH");
    }
    else if (item == getItem(0xa9))
    {
      printf("DELIMITER ");
      if (p[i + 1] == 0x01)
      {
        printf("(Open)");
      }
      else
      {
        printf("(Close)");
      }
    }
    else if (item == getItem(0xb1))
    {
      printf("FEATURE ");
      uint8_t val = p[i + 1];
      printf("(");
      if (val & (1 << 0))
      {
        // 1
        printf("Cnst,");
      }
      else
      {
        // 0
        printf("Data,");
      }
      if (val & (1 << 1))
      {
        // 1
        printf("Var,");
      }
      else
      {
        // 0
        printf("Ary,");
      }
      if (val & (1 << 2))
      {
        // 1
        printf("Rel");
      }
      else
      {
        // 0
        printf("Abs");
      }
      printf(")");
    }
    else if (item == getItem(0xb4))
    {
      printf("POP");
    }
    else if (item == getItem(0xc0))
    {
      printf("END_COLLECTION");
    }
    else
    {
      printf("? ? ? ?");
    }

    printf("\n");
  }

  printf("-----------------------------------------------------\n");
#endif

  usb_host_transfer_free(transfer);
}