#pragma once

#include "EventSystem.h"

#include <string>

#include "MgeObject.h"

enum MgeEventType : uint64_t
{
    MgeEventType_None = 0,
    MgeScreenEvent,
    MgeMouseClick,
    MgeWheel,
    ExitApp,
    ResizeRenderWindow,
    CloseMgeWindow,
    KeyboardPressEvent,
    KeyboardReleaseEvent,
	TextKeyEvent,
    //add new event types here...

    STATIC_CHECKER,
    lastMgeReserved = 20000
    //user-defined events below...
};

static_assert(STATIC_CHECKER < lastMgeReserved);

#define MGE_EVENT_TYPE static constexpr uint64_t mgeEventType

//EXAMPLE:
//struct TemplatedEvent
//{
//    MGE_EVENT_TYPE = MgeEventType::lastMgeReserved + 1; // Example event type
//    std::string data = "test";
//};

struct TerminateApp
{
	MGE_EVENT_TYPE = MgeEventType::ExitApp;
};

struct ResizeWindowEvent
{
    MGE_EVENT_TYPE = MgeEventType::ResizeRenderWindow;
};

struct CloseMgeWindowEvent
{
    MGE_EVENT_TYPE = MgeEventType::CloseMgeWindow;
    uintptr_t m_widgetId = 0;
};

namespace MgeKeys
{
    enum class MgeKey
    {
        Unknown = -1,

        A, B, C, D, E, F, G, H, I, J, K,            
        L, M, N, O, P, Q, R, S, T, U, V,            
        W, X, Y, Z,

        Num0, Num1, Num2, Num3, Num4,         
        Num5, Num6, Num7, Num8, Num9,  

        Escape,       
        LControl,      
        LShift,       
        LAlt,         
        LSystem,      
        RControl,     
        RShift,       
        RAlt,         
        RSystem,      
        Menu,         
        LBracket,     
        RBracket,     
        Semicolon,    
        Comma,        
        Period,       
        Apostrophe,   
        Slash,        
        Backslash,    
        Grave,        
        Equal,        
        Hyphen,       
        Space,        
        Enter,        
        Backspace,    
        Tab,          
        PageUp,       
        PageDown,     
        End,          
        Home,         
        Insert,       
        Delete,       
        Add,          
        Subtract,     
        Multiply,     
        Divide,  

        Left, Right, Up, Down,  

        Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
        Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,      

        F1, F2, F3, F4, F5, F6, F7, F8,
        F9, F10, F11, F12, F13, F14, F15,

        Pause,        

        // Extend as needed.
    };

    enum class KeyModifier : uint8_t
    {
        None = 0,
        Shift = 1 << 0,
        Control = 1 << 1,
        Alt = 1 << 2,
        System = 1 << 3


    };

    constexpr MgeKeys::KeyModifier operator|(MgeKeys::KeyModifier lhs, MgeKeys::KeyModifier rhs) noexcept
    {
        return static_cast<MgeKeys::KeyModifier>(
            static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
    }
}

struct KeyPressEvent
{
    //KeyCode — identifies the physical / logical key, such as A, Space, or Escape.
    //KeyAction — identifies whether the key was pressed or released.
    //KeyModifiers — records whether Ctrl, Shift, Alt, or System was held.
    MGE_EVENT_TYPE = MgeEventType::KeyboardPressEvent;
    MgeKeys::MgeKey m_key = MgeKeys::MgeKey::Unknown;
    MgeKeys::KeyModifier m_modifiers = MgeKeys::KeyModifier::None;
};

struct KeyReleaseEvent
{
    //KeyCode — identifies the physical / logical key, such as A, Space, or Escape.
    //KeyAction — identifies whether the key was pressed or released.
    //KeyModifiers — records whether Ctrl, Shift, Alt, or System was held.
    MGE_EVENT_TYPE = MgeEventType::KeyboardReleaseEvent;
    MgeKeys::MgeKey m_key = MgeKeys::MgeKey::Unknown;
};

struct TextKeyEnteredEvent
{
    MGE_EVENT_TYPE = MgeEventType::TextKeyEvent;
    std::string m_textAsUtf8;
};
