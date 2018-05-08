#ifndef SAITEK_JOYSTICK_H
#define SAITEK_JOYSTICK_H

#ifdef _WIN32
	//This was tested on Saitek X7-38
	#include <dinput.h>

	//how to use...
	/*
	#define SAITEK_X7_38_ENABLE
	#include "saitek_joystick.h"

	if (!FAILED(joystick_setup()))
	{
		DIJOYSTATE2 js;
		while (true)
			joystick_poll(&js);
	}

	joystick_destroy();*/

	static LPDIRECTINPUT8 di;
	static LPDIRECTINPUTDEVICE8 joystick;

	BOOL CALLBACK joystick_enum_callback(const DIDEVICEINSTANCE* instance, VOID* context);
	BOOL CALLBACK joystick_enum_axis_callback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context);
	HRESULT joystick_setup();
	bool joystick_poll(DIJOYSTATE2* js);
	void joystick_destroy();

	#ifdef SAITEK_X7_38_ENABLE
		LONG joystick_get_throttle(DIJOYSTATE2* js);
		LONG joystick_get_x(DIJOYSTATE2* js);
		LONG joystick_get_y(DIJOYSTATE2* js);
	#endif
#endif

#endif