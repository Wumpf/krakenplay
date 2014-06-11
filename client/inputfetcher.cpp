#include "inputfetcher.h"

#include <iostream>
#include <sstream>

#include <OISInputManager.h>
#include <OISException.h>

namespace Krakenplay
{

#if defined OIS_WIN32_PLATFORM
#include <windows.h>

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

#endif

	const char* InputFetcher::inputDeviceNames[6] = { "OISUnknown", "OISKeyboard", "OISMouse", "OISJoyStick", "OISTablet", "OISOther" };


	InputFetcher::InputFetcher()
	{
		OIS::ParamList pl;

#if defined OIS_WIN32_PLATFORM
		// Need to create a capture window for input grabbing.
		MSG msg = { 0 };
		WNDCLASS wc = { 0 };
		wc.lpfnWndProc = &WndProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
		wc.lpszClassName = L"krakenplay";
		if(!RegisterClass(&wc))
			OIS_EXCEPT(OIS::E_General, "Failed to create Win32 window class!");
		HWND hWnd = CreateWindow(wc.lpszClassName, L"Krakenplay Client", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 100, 100, 0, 0, wc.hInstance, NULL);
		if(hWnd == NULL)
			OIS_EXCEPT(OIS::E_General, "Failed to create Win32 Window Dialog!");

		// Needed for input?
		ShowWindow(hWnd, SW_SHOW);

		std::ostringstream wnd;
		wnd << (size_t)hWnd;

		pl.insert(std::make_pair(std::string("WINDOW"), wnd.str()));

		// Default mode is foreground exclusive..but, we want to transmit mouse - so nonexclusive
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));

#elif defined OIS_LINUX_PLATFORM // NOT TESTED YET
		//Connects to default X window
		if(!(xDisp = XOpenDisplay(0)))
			OIS_EXCEPT(E_General, "Error opening X!");
		//Create a window
		xWin = XCreateSimpleWindow(xDisp, DefaultRootWindow(xDisp), 0, 0, 100, 100, 0, 0, 0);
		//bind our connection to that window
		XMapWindow(xDisp, xWin);
		//Select what events we want to listen to locally
		XSelectInput(xDisp, xWin, StructureNotifyMask);
		XEvent evtent;
		do
		{
			XNextEvent(xDisp, &evtent);
		} while(evtent.type != MapNotify);

		std::ostringstream wnd;
		wnd << xWin;

		pl.insert(std::make_pair(std::string("WINDOW"), wnd.str()));

		// For this demo, show mouse and do not grab (confine to window)
		pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
#elif defined OIS_APPLE_PLATFORM // NOT TESTED YET
		// create the window rect in global coords
		::Rect windowRect;
		windowRect.left = 0;
		windowRect.top = 0;
		windowRect.right = 300;
		windowRect.bottom = 300;

		// set the default attributes for the window
		WindowAttributes windowAttrs = kWindowStandardDocumentAttributes
			| kWindowStandardHandlerAttribute
			| kWindowInWindowMenuAttribute
			| kWindowHideOnFullScreenAttribute;

		// Create the window
		CreateNewWindow(kDocumentWindowClass, windowAttrs, &windowRect, &mWin);

		// Color the window background black
		SetThemeWindowBackground(mWin, kThemeBrushBlack, true);

		// Set the title of our window
		CFStringRef titleRef = CFStringCreateWithCString(kCFAllocatorDefault, "OIS Input", kCFStringEncodingASCII);
		SetWindowTitleWithCFString(mWin, titleRef);

		// Center our window on the screen
		RepositionWindow(mWin, NULL, kWindowCenterOnMainScreen);

		// Install the event handler for the window
		InstallStandardEventHandler(GetWindowEventTarget(mWin));

		// This will give our window focus, and not lock it to the terminal
		ProcessSerialNumber psn = { 0, kCurrentProcess };
		TransformProcessType(&psn, kProcessTransformToForegroundApplication);
		SetFrontProcess(&psn);

		// Display and select our window
		ShowWindow(mWin);
		SelectWindow(mWin);

		std::ostringstream wnd;
		wnd << (unsigned int)mWin; //cast to int so it gets encoded correctly (else it gets stored as a hex string)
		std::cout << "WindowRef: " << mWin << " WindowRef as int: " << wnd.str() << "\n";
		pl.insert(std::make_pair(std::string("WINDOW"), wnd.str()));
#endif

		// This never returns null.. it will raise an exception on errors
		inputManager = OIS::InputManager::createInputSystem(pl);

		// Lets enable all addons that were compiled in:
		inputManager->enableAddOnFactory(OIS::InputManager::AddOn_All);

		// Print debugging information
		unsigned int v = inputManager->getVersionNumber();
		std::cout << "OIS Version: " << (v >> 16) << "." << ((v >> 8) & 0x000000FF) << "." << (v & 0x000000FF)
			<< "\nRelease Name: " << inputManager->getVersionName()
			<< "\nManager: " << inputManager->inputSystemName()
			<< "\nTotal Keyboards: " << inputManager->getNumberOfDevices(OIS::OISKeyboard)
			<< "\nTotal Mice: " << inputManager->getNumberOfDevices(OIS::OISMouse)
			<< "\nTotal JoySticks: " << inputManager->getNumberOfDevices(OIS::OISJoyStick);

		// Register all devices
		OIS::DeviceList list = inputManager->listFreeDevices();
		for(OIS::DeviceList::iterator i = list.begin(); i != list.end(); ++i)
			std::cout << "\n\tDevice: " << inputDeviceNames[i->first] << " Vendor: " << i->second;
		std::cout << std::endl;

	}


	InputFetcher::~InputFetcher()
	{
	}

	void InputFetcher::Update(NetworkClient& client)
	{

	}
}