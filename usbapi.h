/*
 * ZenithOS SDK - Header File
 *
 * Copyright (C) 2025 ne5link
 *
 * Licensed under the GNU General Public License v3.0 (GPLv3).
 * See <https://www.gnu.org/licenses/> for details.
 *
 * Made by ne5link <3
 */

#ifndef USBAPI_H
#define USBAPI_H

#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int id;
    uint16_t vendor_id;
    uint16_t product_id;
    char manufacturer[256];
    char product[256];
} usb_device_info_t;

static libusb_context *usb_ctx = NULL;

static inline void usb_init() {
    if (libusb_init(&usb_ctx) < 0) {
        fprintf(stderr, "[usbapi] libusb init failed\n");
    } else {
        printf("[usbapi] libusb initialized\n");
    }
}

static inline void usb_exit() {
    libusb_exit(usb_ctx);
    printf("[usbapi] libusb exited\n");
}

static inline int usb_scan_devices(usb_device_info_t *devices, int max_devices) {
    libusb_device **devs;
    ssize_t count = libusb_get_device_list(usb_ctx, &devs);
    if (count < 0) return 0;

    int found = 0;
    for (ssize_t i = 0; i < count && found < max_devices; i++) {
        libusb_device *dev = devs[i];
        struct libusb_device_descriptor desc;

        if (libusb_get_device_descriptor(dev, &desc) == 0) {
            usb_device_info_t *info = &devices[found];
            info->id = found + 1;
            info->vendor_id = desc.idVendor;
            info->product_id = desc.idProduct;

            libusb_device_handle *handle;
            if (libusb_open(dev, &handle) == 0) {
                libusb_get_string_descriptor_ascii(handle, desc.iManufacturer,
                    (unsigned char *)info->manufacturer, sizeof(info->manufacturer));
                libusb_get_string_descriptor_ascii(handle, desc.iProduct,
                    (unsigned char *)info->product, sizeof(info->product));
                libusb_close(handle);
            } else {
                strcpy(info->manufacturer, "Unknown");
                strcpy(info->product, "Unknown");
            }

            found++;
        }
    }

    libusb_free_device_list(devs, 1);
    return found;
}

#endif // USBAPI_H

