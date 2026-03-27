#pragma once
// ChiliSDL.h - SDL3 platform header replacing ChiliWin.h on macOS
// Provides VK_* compatibility constants so Game code using Windows virtual key codes
// continues to work unchanged.

#include <SDL3/SDL.h>

// Windows virtual key code defines for cross-platform compatibility
// These match the Windows VK_* values that Game code may reference
#ifndef VK_BACK
#define VK_BACK        0x08
#define VK_TAB         0x09
#define VK_RETURN      0x0D
#define VK_SHIFT       0x10
#define VK_CONTROL     0x11
#define VK_MENU        0x12  // Alt
#define VK_PAUSE       0x13
#define VK_CAPITAL     0x14
#define VK_ESCAPE      0x1B
#define VK_SPACE       0x20
#define VK_PRIOR       0x21  // Page Up
#define VK_NEXT        0x22  // Page Down
#define VK_END         0x23
#define VK_HOME        0x24
#define VK_LEFT        0x25
#define VK_UP          0x26
#define VK_RIGHT       0x27
#define VK_DOWN        0x28
#define VK_INSERT      0x2D
#define VK_DELETE      0x2E
// 0-9 are the same as ASCII '0'-'9' (0x30-0x39)
// A-Z are the same as ASCII 'A'-'Z' (0x41-0x5A)
#define VK_F1          0x70
#define VK_F2          0x71
#define VK_F3          0x72
#define VK_F4          0x73
#define VK_F5          0x74
#define VK_F6          0x75
#define VK_F7          0x76
#define VK_F8          0x77
#define VK_F9          0x78
#define VK_F10         0x79
#define VK_F11         0x7A
#define VK_F12         0x7B
#define VK_LSHIFT      0xA0
#define VK_RSHIFT      0xA1
#define VK_LCONTROL    0xA2
#define VK_RCONTROL    0xA3
#define VK_LMENU       0xA4
#define VK_RMENU       0xA5
#endif

// Convert an SDL3 keycode to a Windows-compatible VK code (unsigned char).
// Returns 0 if no mapping exists.
inline unsigned char SDLKeyToVK( SDL_Keycode key )
{
	// Letters: SDL3 uses lowercase ASCII, VK uses uppercase
	if( key >= SDLK_A && key <= SDLK_Z )
	{
		return static_cast<unsigned char>( key - SDLK_A + 'A' );
	}
	// Digits and common ASCII keys that map directly
	if( key >= SDLK_0 && key <= SDLK_9 )
	{
		return static_cast<unsigned char>( key );
	}

	switch( key )
	{
	case SDLK_RETURN:    return VK_RETURN;
	case SDLK_ESCAPE:    return VK_ESCAPE;
	case SDLK_BACKSPACE: return VK_BACK;
	case SDLK_TAB:       return VK_TAB;
	case SDLK_SPACE:     return VK_SPACE;
	case SDLK_DELETE:    return VK_DELETE;
	case SDLK_RIGHT:     return VK_RIGHT;
	case SDLK_LEFT:      return VK_LEFT;
	case SDLK_DOWN:      return VK_DOWN;
	case SDLK_UP:        return VK_UP;
	case SDLK_INSERT:    return VK_INSERT;
	case SDLK_HOME:      return VK_HOME;
	case SDLK_END:       return VK_END;
	case SDLK_PAGEUP:    return VK_PRIOR;
	case SDLK_PAGEDOWN:  return VK_NEXT;
	case SDLK_F1:        return VK_F1;
	case SDLK_F2:        return VK_F2;
	case SDLK_F3:        return VK_F3;
	case SDLK_F4:        return VK_F4;
	case SDLK_F5:        return VK_F5;
	case SDLK_F6:        return VK_F6;
	case SDLK_F7:        return VK_F7;
	case SDLK_F8:        return VK_F8;
	case SDLK_F9:        return VK_F9;
	case SDLK_F10:       return VK_F10;
	case SDLK_F11:       return VK_F11;
	case SDLK_F12:       return VK_F12;
	case SDLK_LSHIFT:    return VK_LSHIFT;
	case SDLK_RSHIFT:    return VK_RSHIFT;
	case SDLK_LCTRL:     return VK_LCONTROL;
	case SDLK_RCTRL:     return VK_RCONTROL;
	case SDLK_LALT:      return VK_LMENU;
	case SDLK_RALT:      return VK_RMENU;
	case SDLK_PAUSE:     return VK_PAUSE;
	case SDLK_CAPSLOCK:  return VK_CAPITAL;
	default:             return 0;
	}
}
