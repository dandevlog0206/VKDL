#pragma once

#include "keyboard.h"
#include "mouse.h"

VKDL_BEGIN

struct WindowEvent {
    enum Type {
        Create,
        Destroy,
        EnterResize,
        ExitResize,
        Resize,
        Resizing,
        EnterMove,
        ExitMove,
        Move,
        Close,
        LostFocus,
        Restore,
        Minimize,
        Maximize,
        Focussed,
        KeyPressed,
        KeyReleased,
        WheelScrolled,
        MousePressed,
        MouseReleased,
        MouseMoved,
        MouseLeft,
        MouseEntered,
        TextEntered
    };

    inline const char* getEventTypeName() const 
    {
        switch (type) {
        case Create:        return "Create";
        case Destroy:       return "Destroy";
        case EnterResize:   return "EnterResize";
        case ExitResize:    return "ExitResize";
        case Resize:        return "Resize";
        case Resizing:      return "Resizing";
        case EnterMove:     return "EnterMove";
        case ExitMove:      return "ExitMove";
        case Move:          return "Move";
        case Close:         return "Close";
        case LostFocus:     return "LostFocus";
        case Restore:       return "Restore";
        case Minimize:      return "Minimize";
        case Maximize:      return "Maximize";
        case Focussed:      return "Focussed";
        case KeyPressed:    return "KeyPressed";
        case KeyReleased:   return "KeyReleased";
        case WheelScrolled: return "WheelScrolled";
        case MousePressed:  return "MousePressed";
        case MouseReleased: return "MouseReleased";
        case MouseMoved:    return "MouseMoved";
        case MouseLeft:     return "MouseLeft";
        case MouseEntered:  return "MouseEntered";
        case TextEntered:   return "TextEntered:";
        default:            return "Unknown";
        }
    }

    Type type;
    
    union
    {
        struct
        {
            uvec2 size;
            uvec2 fb_size;
        } resize;

        struct
        {
            ivec2 pos;
        } move;

        struct
        {
            Key key;
            bool ctrl;
            bool alt;
            bool shft;
            bool sys;
        } keyboard;

        struct
        {
            Mouse::Button button;
            ivec2         pos;
        } mouseButton;

        struct
        {
            vec2 delta;
            ivec2 pos;
        } wheel;

        struct
        {
            ivec2 pos;
            ivec2 delta;
        } mouseMove;

        struct
        {
            int unicode;
        } text;
    };
};

VKDL_END