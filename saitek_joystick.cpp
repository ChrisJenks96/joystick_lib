#ifdef _WIN32
	#define SAITEK_X7_38_ENABLE
	#include "saitek_joystick.h"

	BOOL CALLBACK joystick_enum_callback(const DIDEVICEINSTANCE* instance, VOID* context)
	{
		HRESULT hr = di->CreateDevice(instance->guidInstance, &joystick, NULL);
		if (FAILED(hr))
			return DIENUM_CONTINUE;
		return DIENUM_STOP;
	}

	BOOL CALLBACK joystick_enum_axis_callback(const DIDEVICEOBJECTINSTANCE* instance, VOID* context)
	{
		HWND hdlg = (HWND)context;
		DIPROPRANGE prop_range;
		prop_range.diph.dwSize = sizeof(DIPROPRANGE); 
		prop_range.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
		prop_range.diph.dwHow = DIPH_BYID; 
		prop_range.diph.dwObj = instance->dwType;
		prop_range.lMin = -1000; 
		prop_range.lMax = 1000; 

		if (FAILED(joystick->SetProperty(DIPROP_RANGE, &prop_range.diph)))
			return DIENUM_STOP;

		return DIENUM_CONTINUE;
	}

	bool joystick_poll(DIJOYSTATE2* js)
	{
		HRESULT hr;
		if (joystick == NULL)
			return false;
		hr = joystick->Poll();
		if (FAILED(hr))
		{
			hr = joystick->Acquire();
			while (hr == DIERR_INPUTLOST)
				hr = joystick->Acquire();

			if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED))
				return false;

			if (hr == DIERR_OTHERAPPHASPRIO)
				return false;
		}

		if (FAILED(hr = joystick->GetDeviceState(sizeof(DIJOYSTATE2), js)))
			return hr;

		return true;
	}

	HRESULT joystick_setup()
	{
		HRESULT hr;
		if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&di, NULL)))
			return hr;
		else
		{
			if (FAILED(hr = di->EnumDevices(DI8DEVCLASS_GAMECTRL, joystick_enum_callback, NULL, DIEDFL_ATTACHEDONLY)))
				return hr;
			else
			{
				if (joystick == NULL){
					return E_FAIL;
				}

				else
				{
					DIDEVCAPS capabilities;
					if (FAILED(hr = joystick->SetDataFormat(&c_dfDIJoystick2)))
						return hr;
					if (FAILED(hr = joystick->SetCooperativeLevel(GetConsoleWindow(), DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
						return hr;
					capabilities.dwSize = sizeof(DIDEVCAPS);
					if (FAILED(hr = joystick->GetCapabilities(&capabilities)))
						return hr;
					if (FAILED(hr = joystick->EnumObjects(joystick_enum_axis_callback, NULL, DIDFT_AXIS)))
						return hr;
				}
			}
		}
	}

	void joystick_destroy()
	{
		if (joystick != NULL)
			joystick->Unacquire();
	}

	#ifdef SAITEK_X7_38_ENABLE
		//invert the throttle (-1000 is on max 100 (wrong way round))
		//convert to 0 - 100 like the unit
		LONG joystick_get_throttle(DIJOYSTATE2* js){
			return (-js->rglSlider[0] + 1000) / 20;
		}

		LONG joystick_get_x(DIJOYSTATE2* js){
			return js->lX;
		}

		LONG joystick_get_y(DIJOYSTATE2* js){
			return js->lY;
		}
	#endif
#endif