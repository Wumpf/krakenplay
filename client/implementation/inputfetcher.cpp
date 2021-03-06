#include "../inputfetcher.h"
#include "../mousefetcher.h"
#include "../keyboardfetcher.h"
#include "../gamepadfetcher.h"
#include "../networkclient.h"

#include "../../krakenplay/time/time.h"

#include <iostream>
#include <sstream>
#include <thread>
#include <cstring>

#pragma warning(push, 0)
#include <OISInputManager.h>
#include <OISException.h>
#pragma warning(pop)

#if defined OIS_WIN32_PLATFORM
#include <windows.h>

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

#elif defined OIS_LINUX_PLATFORM
	#include <X11/Xlib.h>
	#include <X11/Xatom.h>
	void checkX11Events();

#elif defined OIS_APPLE_PLATFORM
	#include <Carbon/Carbon.h>
	void checkMacEvents();
#endif

namespace Krakenplay
{
	InputFetcher::InputFetcher()
	{
		// Init OIS
		OIS::ParamList pl;
#if defined OIS_WIN32_PLATFORM
		// Need to create a capture window for input grabbing.
        WNDCLASS wc;
        memset(&wc, 0, sizeof(wc));
		wc.lpfnWndProc = &WndProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
		wc.lpszClassName = "krakenplay";
		if(!RegisterClass(&wc))
			OIS_EXCEPT(OIS::E_General, "Failed to create Win32 window class!");
		HWND hWnd = CreateWindow(wc.lpszClassName, "Krakenplay Client", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 100, 100, 0, 0, wc.hInstance, NULL);
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
		Display* xDisp;
		if(!(xDisp = XOpenDisplay(0)))
			OIS_EXCEPT(OIS::E_General, "Error opening X!");
		//Create a window
		Window xWin = XCreateSimpleWindow(xDisp, DefaultRootWindow(xDisp), 0, 0, 100, 100, 0, 0, 0);
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
					<< "\nManager: " << inputManager->inputSystemName() << std::endl;
	}


	InputFetcher::~InputFetcher()
	{
		for (auto fetcher : devices)
		{
			delete fetcher;
		}
		devices.clear();
		OIS::InputManager::destroyInputSystem(inputManager);
	}

	void InputFetcher::Update(NetworkClient& client)
	{
#ifdef OIS_WIN32_PLATFORM
		// Need to keep window up to date.
		MSG  msg;
		while(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
#endif

		// Add new devices if there are any.
		AddFreeDevices();

		// Capture all devices.
		bool immediateUpdateRequest = false;
		for (auto fetcher : devices)
		{
			if (fetcher->FetchState())
				immediateUpdateRequest = true;
		}

		// Sleep until we are ready to send a message.
		int sleepTimeMS = 0;
		if (immediateUpdateRequest)
			sleepTimeMS = static_cast<int>(Krakenplay::g_minDurationBetweenMessage_ms - timeSinceLastUpdatePackage.GetRunningTotal().GetMilliseconds());
		else
			sleepTimeMS = static_cast<int>(Krakenplay::g_maxDurationBetweenMessage_ms - timeSinceLastUpdatePackage.GetRunningTotal().GetMilliseconds());
		if (sleepTimeMS > 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMS));

		// Combine messages until buffer is full.
		unsigned int messageBufferPos = 0;
		for (auto fetcher : devices)
		{
			unsigned int stateInfoSize = 0;
			const void* stateInfo = fetcher->GetState(stateInfoSize);

			// Buffer full?
			if (messageBufferPos + stateInfoSize + sizeof(MessageChunkHeader) >= g_maxMessageSize)
			{
				// send message
				client.Send(messageBuffer, messageBufferPos);
				messageBufferPos = 0;
			}

			// Add message Header.
			MessageChunkHeader header = fetcher->GetStateMessageHeader();
			memcpy(&messageBuffer[messageBufferPos], &header, sizeof(MessageChunkHeader));
			messageBufferPos += sizeof(Krakenplay::MessageChunkHeader);

			// Add status message itself.
			memcpy(&messageBuffer[messageBufferPos], stateInfo, stateInfoSize);
			messageBufferPos += stateInfoSize;
		}

		// Send message.
		client.Send(messageBuffer, messageBufferPos);

		// Restart stopwatch.
		timeSinceLastUpdatePackage.StopAndReset();
		timeSinceLastUpdatePackage.Resume();
	}

	void InputFetcher::Disconnect(NetworkClient& client)
	{
		// Send disconnect messages for each device.
		for (auto fetcher : devices)
		{
			MessageChunkHeader header = fetcher->GetDisconnectMessageHeader();
			client.Send(reinterpret_cast<char*>(&header), sizeof(MessageChunkHeader));
		}
	}

	void InputFetcher::AddFreeDevices()
	{
		// Register all new devices.
		OIS::DeviceList list = inputManager->listFreeDevices();
		for (OIS::DeviceList::iterator i = list.begin(); i != list.end(); ++i)
		{
			try
			{
				switch (i->first)
				{
				case OIS::OISMouse:
					std::cout << "Connected Mouse. " << " Vendor: " << i->second << "\n";
					devices.push_back(new MouseFetcher(inputManager));
					break;

				case OIS::OISKeyboard:
					std::cout << "Connected Keyboard. " << " Vendor: " << i->second << "\n";
					devices.push_back(new KeyboardFetcher(inputManager));
					break;

				case OIS::OISJoyStick:
					std::cout << "Connected Gamepad. " << " Vendor: " << i->second << "\n";
					devices.push_back(new GamepadFetcher(inputManager));
					break;

				default:
					std::cout << "Unknown input device connected. " << " Vendor: " << i->second << "\n";
				}
			}
			catch (OIS::Exception e)
			{
				std::cerr << "Failed to create input device fetcher.\n";
				std::cerr << e.what() << std::endl;
			}
		}
	}
}
