/*******************************************************

********************************************************/

#ifndef __STC_ISP_H__
#define __STC_ISP_H__

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>

#include <hidapi.h>

// Headers needed for sleeping.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

// Fallback/example
#ifndef HID_API_MAKE_VERSION
#define HID_API_MAKE_VERSION(mj, mn, p) (((mj) << 24) | ((mn) << 8) | (p))
#endif
#ifndef HID_API_VERSION
#define HID_API_VERSION HID_API_MAKE_VERSION(HID_API_VERSION_MAJOR, HID_API_VERSION_MINOR, HID_API_VERSION_PATCH)
#endif

//
// Sample using platform-specific headers
#if defined(__APPLE__) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_darwin.h>
#endif

#if defined(_WIN32) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_winapi.h>
#endif

#if defined(USING_HIDAPI_LIBUSB) && HID_API_VERSION >= HID_API_MAKE_VERSION(0, 12, 0)
#include <hidapi_libusb.h>
#endif
//

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

	const char* hid_bus_name(hid_bus_type bus_type);

	void print_device(struct hid_device_info* cur_dev);
	void print_hid_report_descriptor_from_device(hid_device* device);

	void print_hid_report_descriptor_from_path(const char* path);
	void print_devices(struct hid_device_info* cur_dev);
	void print_devices_with_descriptor(struct hid_device_info* cur_dev);

	void hid_usage();

	void hid_read_dev_info(hid_device* handle);
	void send_feature(hid_device* handle);

	void read_feature(hid_device* handle);
	void write_requested_state(hid_device* handle);
	void read_requested_state(hid_device* handle);

	void linked_version(int argc, char** argv, const char* my_func);

	void test_all_dev();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
