/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	MainWindow.cpp																		  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#include "MainWindow.h"
#include "Graphics.h"
#include "ChiliException.h"
#include "Game.h"
#include <assert.h>

#ifdef _WIN32
// ==================== Windows Implementation ====================
#include "Resource.h"

MainWindow::MainWindow( HINSTANCE hInst,wchar_t * pArgs )
	:
	args( pArgs ),
	hInst( hInst )
{
	// register window class
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ),CS_CLASSDC,_HandleMsgSetup,0,0,
		hInst,nullptr,nullptr,nullptr,nullptr,
		wndClassName,nullptr };
	wc.hIconSm = (HICON)LoadImage( hInst,MAKEINTRESOURCE( IDI_APPICON ),IMAGE_ICON,16,16,0 );
	wc.hIcon = (HICON)LoadImage( hInst,MAKEINTRESOURCE( IDI_APPICON ),IMAGE_ICON,32,32,0 );
	wc.hCursor = LoadCursor( nullptr,IDC_ARROW );
	RegisterClassEx( &wc );

	// create window & get hWnd
	RECT wr;
	wr.left = 350;
	wr.right = Graphics::ScreenWidth + wr.left;
	wr.top = 100;
	wr.bottom = Graphics::ScreenHeight + wr.top;
	AdjustWindowRect( &wr,WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,FALSE );
	hWnd = CreateWindow( wndClassName,L"Chili DirectX Framework",
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		wr.left,wr.top,wr.right - wr.left,wr.bottom - wr.top,
		nullptr,nullptr,hInst,this );

	// throw exception if something went terribly wrong
	if( hWnd == nullptr )
	{
		throw Exception( _CRT_WIDE( __FILE__ ),__LINE__,
			L"Failed to get valid window handle." );
	}

	// show and update
	ShowWindow( hWnd,SW_SHOWDEFAULT );
	UpdateWindow( hWnd );
}

MainWindow::~MainWindow()
{
	// unregister window class
	UnregisterClass( wndClassName,hInst );
}

bool MainWindow::IsActive() const
{
	return GetActiveWindow() == hWnd;
}

bool MainWindow::IsMinimized() const
{
	return IsIconic( hWnd ) != 0;
}

void MainWindow::ShowMessageBox( const std::wstring& title,const std::wstring& message ) const
{
	MessageBox( hWnd,message.c_str(),title.c_str(),MB_OK );
}

void MainWindow::Kill()
{
	PostQuitMessage( 0 );
}

bool MainWindow::ProcessMessage()
{
	MSG msg;
	while( PeekMessage( &msg,nullptr,0,0,PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
		if( msg.message == WM_QUIT )
		{
			return false;
		}
	}
	return true;
}

LRESULT WINAPI MainWindow::_HandleMsgSetup( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam )
{
	// use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
	if( msg == WM_NCCREATE )
	{
		// extract ptr to window class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>( lParam );
		MainWindow* const pWnd = reinterpret_cast<MainWindow*>( pCreate->lpCreateParams );
		// sanity check
		assert( pWnd != nullptr );
		// set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr( hWnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>( pWnd ) );
		// set message proc to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr( hWnd,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>( &MainWindow::_HandleMsgThunk ) );
		// forward message to window class handler
		return pWnd->HandleMsg( hWnd,msg,wParam,lParam );
	}
	// if we get a message before the WM_NCCREATE message, handle with default handler
	return DefWindowProc( hWnd,msg,wParam,lParam );
}

LRESULT WINAPI MainWindow::_HandleMsgThunk( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam )
{
	// retrieve ptr to window class
	MainWindow* const pWnd = reinterpret_cast<MainWindow*>( GetWindowLongPtr( hWnd,GWLP_USERDATA ) );
	// forward message to window class handler
	return pWnd->HandleMsg( hWnd,msg,wParam,lParam );
}

LRESULT MainWindow::HandleMsg( HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam )
{
	switch( msg )
	{
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;

		// ************ KEYBOARD MESSAGES ************ //
	case WM_KEYDOWN:
		if( !(lParam & 0x40000000) || kbd.AutorepeatIsEnabled() ) // no thank you on the autorepeat
		{
			kbd.OnKeyPressed( static_cast<unsigned char>(wParam) );
		}
		break;
	case WM_KEYUP:
		kbd.OnKeyReleased( static_cast<unsigned char>(wParam) );
		break;
	case WM_CHAR:
		kbd.OnChar( static_cast<unsigned char>(wParam) );
		break;
		// ************ END KEYBOARD MESSAGES ************ //

		// ************ MOUSE MESSAGES ************ //
	case WM_MOUSEMOVE:
	{
		POINTS pt = MAKEPOINTS( lParam );
		if( pt.x > 0 && pt.x < Graphics::ScreenWidth && pt.y > 0 && pt.y < Graphics::ScreenHeight )
		{
			mouse.OnMouseMove( pt.x,pt.y );
			if( !mouse.IsInWindow() )
			{
				SetCapture( hWnd );
				mouse.OnMouseEnter();
			}
		}
		else
		{
			if( wParam & (MK_LBUTTON | MK_RBUTTON) )
			{
				pt.x = std::max( short( 0 ),pt.x );
				pt.x = std::min( short( Graphics::ScreenWidth - 1 ),pt.x );
				pt.y = std::max( short( 0 ),pt.y );
				pt.y = std::min( short( Graphics::ScreenHeight - 1 ),pt.y );
				mouse.OnMouseMove( pt.x,pt.y );
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
				mouse.OnLeftReleased( pt.x,pt.y );
				mouse.OnRightReleased( pt.x,pt.y );
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnLeftPressed( pt.x,pt.y );
		SetForegroundWindow( hWnd );
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnRightPressed( pt.x,pt.y );
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnLeftReleased( pt.x,pt.y );
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnRightReleased( pt.x,pt.y );
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS( lParam );
		if( GET_WHEEL_DELTA_WPARAM( wParam ) > 0 )
		{
			mouse.OnWheelUp( pt.x,pt.y );
		}
		else if( GET_WHEEL_DELTA_WPARAM( wParam ) < 0 )
		{
			mouse.OnWheelDown( pt.x,pt.y );
		}
		break;
	}
	// ************ END MOUSE MESSAGES ************ //
	}

	return DefWindowProc( hWnd,msg,wParam,lParam );
}

#else
// ==================== SDL3 Implementation ====================

MainWindow::MainWindow( int argc,char* argv[] )
{
	if( !SDL_Init( SDL_INIT_VIDEO ) )
	{
		throw Exception( L"MainWindow.cpp",__LINE__,
			L"Failed to initialize SDL3." );
	}

	pWindow = SDL_CreateWindow(
		"Chili Framework",
		Graphics::ScreenWidth,
		Graphics::ScreenHeight,
		0
	);

	if( pWindow == nullptr )
	{
		throw Exception( L"MainWindow.cpp",__LINE__,
			L"Failed to create SDL window." );
	}
}

MainWindow::~MainWindow()
{
	if( pWindow )
	{
		SDL_DestroyWindow( pWindow );
		pWindow = nullptr;
	}
	SDL_Quit();
}

bool MainWindow::IsActive() const
{
	return isActive;
}

bool MainWindow::IsMinimized() const
{
	return isMinimized;
}

void MainWindow::ShowMessageBox( const std::wstring& title,const std::wstring& message ) const
{
	// Convert wide strings to narrow for SDL
	std::string narrowTitle( title.begin(),title.end() );
	std::string narrowMsg( message.begin(),message.end() );
	SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR,narrowTitle.c_str(),narrowMsg.c_str(),pWindow );
}

void MainWindow::Kill()
{
	SDL_Event quitEvent;
	quitEvent.type = SDL_EVENT_QUIT;
	SDL_PushEvent( &quitEvent );
}

bool MainWindow::ProcessMessage()
{
	SDL_Event e;
	while( SDL_PollEvent( &e ) )
	{
		switch( e.type )
		{
		case SDL_EVENT_QUIT:
			return false;

		// ************ WINDOW EVENTS ************ //
		case SDL_EVENT_WINDOW_FOCUS_GAINED:
			isActive = true;
			break;
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			isActive = false;
			kbd.ClearState();
			break;
		case SDL_EVENT_WINDOW_MINIMIZED:
			isMinimized = true;
			break;
		case SDL_EVENT_WINDOW_RESTORED:
			isMinimized = false;
			break;

		// ************ KEYBOARD MESSAGES ************ //
		case SDL_EVENT_KEY_DOWN:
		{
			if( !e.key.repeat || kbd.AutorepeatIsEnabled() )
			{
				const unsigned char vk = SDLKeyToVK( e.key.key );
				if( vk != 0 )
				{
					kbd.OnKeyPressed( vk );
				}
			}
			break;
		}
		case SDL_EVENT_KEY_UP:
		{
			const unsigned char vk = SDLKeyToVK( e.key.key );
			if( vk != 0 )
			{
				kbd.OnKeyReleased( vk );
			}
			break;
		}
		case SDL_EVENT_TEXT_INPUT:
		{
			// Process each character in the UTF-8 text (usually just one)
			if( e.text.text && e.text.text[0] != '\0' )
			{
				kbd.OnChar( static_cast<unsigned char>( e.text.text[0] ) );
			}
			break;
		}

		// ************ MOUSE MESSAGES ************ //
		case SDL_EVENT_MOUSE_MOTION:
		{
			const int mx = static_cast<int>( e.motion.x );
			const int my = static_cast<int>( e.motion.y );
			if( mx >= 0 && mx < Graphics::ScreenWidth && my >= 0 && my < Graphics::ScreenHeight )
			{
				mouse.OnMouseMove( mx,my );
				if( !mouse.IsInWindow() )
				{
					mouse.OnMouseEnter();
				}
			}
			else
			{
				if( mouse.LeftIsPressed() || mouse.RightIsPressed() )
				{
					const int clampedX = std::max( 0,std::min( Graphics::ScreenWidth - 1,mx ) );
					const int clampedY = std::max( 0,std::min( Graphics::ScreenHeight - 1,my ) );
					mouse.OnMouseMove( clampedX,clampedY );
				}
				else
				{
					mouse.OnMouseLeave();
				}
			}
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			const int mx = static_cast<int>( e.button.x );
			const int my = static_cast<int>( e.button.y );
			if( e.button.button == SDL_BUTTON_LEFT )
			{
				mouse.OnLeftPressed( mx,my );
			}
			else if( e.button.button == SDL_BUTTON_RIGHT )
			{
				mouse.OnRightPressed( mx,my );
			}
			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			const int mx = static_cast<int>( e.button.x );
			const int my = static_cast<int>( e.button.y );
			if( e.button.button == SDL_BUTTON_LEFT )
			{
				mouse.OnLeftReleased( mx,my );
			}
			else if( e.button.button == SDL_BUTTON_RIGHT )
			{
				mouse.OnRightReleased( mx,my );
			}
			break;
		}
		case SDL_EVENT_MOUSE_WHEEL:
		{
			const int mx = static_cast<int>( e.wheel.mouse_x );
			const int my = static_cast<int>( e.wheel.mouse_y );
			if( e.wheel.y > 0 )
			{
				mouse.OnWheelUp( mx,my );
			}
			else if( e.wheel.y < 0 )
			{
				mouse.OnWheelDown( mx,my );
			}
			break;
		}
		// ************ END MOUSE MESSAGES ************ //
		}
	}
	return true;
}

#endif
