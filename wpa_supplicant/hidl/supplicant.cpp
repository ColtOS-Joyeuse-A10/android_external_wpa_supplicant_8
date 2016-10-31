/*
 * hidl interface for wpa_supplicant daemon
 * Copyright (c) 2004-2016, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2004-2016, Roshan Pius <rpius@google.com>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "supplicant.h"
#include "hidl_manager.h"
#include "../src/utils/wpa_debug.h"

namespace wpa_supplicant_hidl {

Supplicant::Supplicant(struct wpa_global *global) : wpa_global_(global) {}
android::hidl::Status Supplicant::CreateInterface(
    const fi::w1::wpa_supplicant::ParcelableIfaceParams &params,
    android::sp<fi::w1::wpa_supplicant::IIface> *iface_object_out)
{
	/* Check if required Ifname argument is missing */
	if (params.ifname_.isEmpty()) {
		return android::hidl::Status::fromExceptionCode(
		    android::hidl::Status::EX_ILLEGAL_ARGUMENT,
		    "Ifname missing in params.");
	}
	/*
	 * Try to get the wpa_supplicant record for this iface, return
	 * an error if we already control it.
	 */
	if (wpa_supplicant_get_iface(wpa_global_, params.ifname_.string()) !=
	    NULL) {
		return android::hidl::Status::fromServiceSpecificError(
		    ERROR_IFACE_EXISTS,
		    "wpa_supplicant already controls this interface.");
	}

	android::hidl::Status status;
	struct wpa_supplicant *wpa_s = NULL;
	struct wpa_interface iface;

	os_memset(&iface, 0, sizeof(iface));
	iface.driver = os_strdup(params.driver_.string());
	iface.ifname = os_strdup(params.ifname_.string());
	iface.confname = os_strdup(params.config_file_.string());
	iface.bridge_ifname = os_strdup(params.bridge_ifname_.string());
	/* Otherwise, have wpa_supplicant attach to it. */
	wpa_s = wpa_supplicant_add_iface(wpa_global_, &iface, NULL);
	/* The supplicant core creates a corresponding hidl object via
	 * HidlManager when |wpa_supplicant_add_iface| is called. */
	if (!wpa_s) {
		status = android::hidl::Status::fromServiceSpecificError(
		    ERROR_GENERIC,
		    "wpa_supplicant couldn't grab this interface.");
	} else {
		HidlManager *hidl_manager = HidlManager::getInstance();

		if (!hidl_manager ||
		    hidl_manager->getIfaceHidlObjectByIfname(
			wpa_s->ifname, iface_object_out)) {
			status =
			    android::hidl::Status::fromServiceSpecificError(
				ERROR_GENERIC,
				"wpa_supplicant encountered a hidl error.");
		} else {
			status = android::hidl::Status::ok();
		}
	}
	os_free((void *)iface.driver);
	os_free((void *)iface.ifname);
	os_free((void *)iface.confname);
	os_free((void *)iface.bridge_ifname);
	return status;
}

android::hidl::Status Supplicant::RemoveInterface(const std::string &ifname)
{
	struct wpa_supplicant *wpa_s;

	wpa_s = wpa_supplicant_get_iface(wpa_global_, ifname.c_str());
	if (!wpa_s) {
		return android::hidl::Status::fromServiceSpecificError(
		    ERROR_IFACE_UNKNOWN,
		    "wpa_supplicant does not control this interface.");
	}
	if (wpa_supplicant_remove_iface(wpa_global_, wpa_s, 0)) {
		return android::hidl::Status::fromServiceSpecificError(
		    ERROR_GENERIC,
		    "wpa_supplicant couldn't remove this interface.");
	}
	return android::hidl::Status::ok();
}

android::hidl::Status Supplicant::GetInterface(
    const std::string &ifname,
    android::sp<fi::w1::wpa_supplicant::IIface> *iface_object_out)
{
	struct wpa_supplicant *wpa_s;

	wpa_s = wpa_supplicant_get_iface(wpa_global_, ifname.c_str());
	if (!wpa_s) {
		return android::hidl::Status::fromServiceSpecificError(
		    ERROR_IFACE_UNKNOWN,
		    "wpa_supplicant does not control this interface.");
	}

	HidlManager *hidl_manager = HidlManager::getInstance();
	if (!hidl_manager ||
	    hidl_manager->getIfaceHidlObjectByIfname(
		wpa_s->ifname, iface_object_out)) {
		return android::hidl::Status::fromServiceSpecificError(
		    ERROR_GENERIC,
		    "wpa_supplicant encountered a hidl error.");
	}

	return android::hidl::Status::ok();
}

android::hidl::Status Supplicant::SetDebugParams(
    int level, bool show_timestamp, bool show_keys)
{
	int internal_level;
	if (convertDebugLevelToInternalLevel(level, &internal_level)) {
		const std::string error_msg =
		    "invalid debug level: " + std::to_string(level);
		return android::hidl::Status::fromExceptionCode(
		    android::hidl::Status::EX_ILLEGAL_ARGUMENT,
		    error_msg.c_str());
	}
	if (wpa_supplicant_set_debug_params(
<<<<<<< 390ba2881ef621db480848b7e50b93d61542206a:wpa_supplicant/binder/supplicant.cpp
		wpa_global_, internal_level, show_timestamp, show_keys)) {
		return android::binder::Status::fromServiceSpecificError(
=======
		wpa_global_, level, show_timestamp, show_keys)) {
		return android::hidl::Status::fromServiceSpecificError(
>>>>>>> wpa_supplicant: HIDL implementation (1/2):wpa_supplicant/hidl/supplicant.cpp
		    ERROR_GENERIC,
		    "wpa_supplicant could not set debug params.");
	}
	return android::hidl::Status::ok();
}

android::hidl::Status Supplicant::GetDebugLevel(int *level_out)
{
<<<<<<< 390ba2881ef621db480848b7e50b93d61542206a:wpa_supplicant/binder/supplicant.cpp
	if (convertDebugLevelToExternalLevel(wpa_debug_level, level_out)) {
		const std::string error_msg =
		    "invalid debug level: " + std::to_string(wpa_debug_level);
		return android::binder::Status::fromExceptionCode(
		    android::binder::Status::EX_ILLEGAL_ARGUMENT,
		    error_msg.c_str());
	}
	return android::binder::Status::ok();
=======
	*level_out = wpa_debug_level;
	return android::hidl::Status::ok();
>>>>>>> wpa_supplicant: HIDL implementation (1/2):wpa_supplicant/hidl/supplicant.cpp
}

android::hidl::Status Supplicant::GetDebugShowTimestamp(
    bool *show_timestamp_out)
{
	*show_timestamp_out = wpa_debug_timestamp ? true : false;
	return android::hidl::Status::ok();
}

android::hidl::Status Supplicant::GetDebugShowKeys(bool *show_keys_out)
{
	*show_keys_out = wpa_debug_show_keys ? true : false;
	return android::hidl::Status::ok();
}

android::hidl::Status Supplicant::RegisterCallback(
    const android::sp<fi::w1::wpa_supplicant::ISupplicantCallback> &callback)
{
	HidlManager *hidl_manager = HidlManager::getInstance();
	if (!hidl_manager ||
	    hidl_manager->addSupplicantCallbackHidlObject(callback)) {
		return android::hidl::Status::fromServiceSpecificError(
		    ERROR_GENERIC,
		    "wpa_supplicant encountered a hidl error.");
	}
	return android::hidl::Status::ok();
}
<<<<<<< 390ba2881ef621db480848b7e50b93d61542206a:wpa_supplicant/binder/supplicant.cpp

/**
 * Helper function to convert the debug level parameter from the binder
 * interface values to internal values.
 */
int Supplicant::convertDebugLevelToInternalLevel(
    int external_level, int *internal_level)
{
	switch (external_level) {
	case DEBUG_LEVEL_EXCESSIVE:
		*internal_level = MSG_EXCESSIVE;
		return 0;
	case DEBUG_LEVEL_MSGDUMP:
		*internal_level = MSG_MSGDUMP;
		return 0;
	case DEBUG_LEVEL_DEBUG:
		*internal_level = MSG_DEBUG;
		return 0;
	case DEBUG_LEVEL_INFO:
		*internal_level = MSG_INFO;
		return 0;
	case DEBUG_LEVEL_WARNING:
		*internal_level = MSG_WARNING;
		return 0;
	case DEBUG_LEVEL_ERROR:
		*internal_level = MSG_ERROR;
		return 0;
	default:
		wpa_printf(
		    MSG_ERROR, "Invalid external log level: %d",
		    external_level);
		return 1;
	}
}

/**
 * Helper function to convert the debug level parameter from the internal values
 * to binder interface values.
 */
int Supplicant::convertDebugLevelToExternalLevel(
    int internal_level, int *external_level)
{
	switch (internal_level) {
	case MSG_EXCESSIVE:
		*external_level = DEBUG_LEVEL_EXCESSIVE;
		return 0;
	case MSG_MSGDUMP:
		*external_level = DEBUG_LEVEL_MSGDUMP;
		return 0;
	case MSG_DEBUG:
		*external_level = DEBUG_LEVEL_DEBUG;
		return 0;
	case MSG_INFO:
		*external_level = DEBUG_LEVEL_INFO;
		return 0;
	case MSG_WARNING:
		*external_level = DEBUG_LEVEL_WARNING;
		return 0;
	case MSG_ERROR:
		*external_level = DEBUG_LEVEL_ERROR;
		return 0;
	default:
		wpa_printf(
		    MSG_ERROR, "Invalid internal log level: %d",
		    internal_level);
		return 1;
	}
}
} /* namespace wpa_supplicant_binder */
=======
} /* namespace wpa_supplicant_hidl */
>>>>>>> wpa_supplicant: HIDL implementation (1/2):wpa_supplicant/hidl/supplicant.cpp