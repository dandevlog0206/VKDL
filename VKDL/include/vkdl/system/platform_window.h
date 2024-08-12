#pragma once

#include <string>
#include "../system/event_msg.h"
#include "../math/vector_type.h"
#include "../util/property.hpp"
#include "../util/event.hpp"
#include "../util/flags.h"

VKDL_BEGIN
VKDL_PRIV_BEGIN

struct PlatformWindowImpl;

VKDL_PRIV_END

typedef Event<PlatformWindow&, EventMsg&> PlatformWindowEventMsgHandler;

enum class PlatformWindowStyle
{
	None      = 0,
	Resizable = 1 << 0,
	TopMost   = 1 << 1,
	Visible   = 1 << 2,
	Minimize  = 1 << 3,
	Maximize  = 1 << 4,
	Close     = 1 << 5,
	Titlebar  = Minimize | Maximize | Close,
	Default   = Titlebar | Resizable | Visible
};

enum class PlatformWindowState
{
	Normal,
	Minimized,
	Maximized
};

typedef Flags<PlatformWindowStyle> PlatformWindowStyleFlags;

class PlatformWindow
{
	PROPERTY_INIT(PlatformWindow);
	VKDL_NOCOPY(PlatformWindow);
	VKDL_NOCOPYASS(PlatformWindow);

public:
	PlatformWindow();
	PlatformWindow(PlatformWindow&& rhs);
	PlatformWindow(uint32_t width, uint32_t height, const char* title, PlatformWindowStyleFlags style = PlatformWindowStyle::Default);
	PlatformWindow(uint32_t width, uint32_t height, const char* title, PlatformWindow* parent, PlatformWindowStyleFlags style = PlatformWindowStyle::Default);
	~PlatformWindow();

	PlatformWindow& operator=(PlatformWindow&& rhs);

	void create(uint32_t width, uint32_t height, const char* title, PlatformWindowStyleFlags style = PlatformWindowStyle::Default);
	void create(uint32_t width, uint32_t height, const char* title, PlatformWindow* parent, PlatformWindowStyleFlags style = PlatformWindowStyle::Default);
	void destroy();

	void processEvent(bool block = false);
	bool pollEvent(EventMsg& event);
	bool waitEvent(EventMsg& event);
	bool hasEvent() const;

	PROPERTY {
		PROPERTY_GET_SET(ivec2,               Position);
		PROPERTY_GET_SET(uvec2,               Size);
		PROPERTY_GET_SET(uvec2,               FrameBufferSize);
		PROPERTY_GET_SET(uvec2,               MinSizeLimit);
		PROPERTY_GET_SET(uvec2,               MaxSizeLimit);
		PROPERTY_GET_SET(std::string,         Title);
		PROPERTY_GET_SET(PlatformWindowState, WindowState);
		PROPERTY_GET_SET(float,               Transparency);
		PROPERTY_GET_SET(bool,                Visible);
		PROPERTY_GET_SET(bool,                Focussed);
		PROPERTY_GET_SET(bool,                Resizable);
		PROPERTY_GET_SET(bool,                VSync);
		PROPERTY_GET_SET(PlatformWindow*,     Parent);
		PROPERTY_GET(bool,                    IsClosed);
		PROPERTY_GET(bool,                    IsDestroyed);
		PROPERTY_GET_SET(void*,               UserPtr);
	};

	PlatformWindowEventMsgHandler OnEventMsgRecieved;

	void* getNativeHandle() const;

private:
	PROPERTY_DECL_GET(Position);
	PROPERTY_DECL_SET(Position);
	PROPERTY_DECL_GET(Size);
	PROPERTY_DECL_SET(Size);
	PROPERTY_DECL_GET(FrameBufferSize);
	PROPERTY_DECL_SET(FrameBufferSize);
	PROPERTY_DECL_GET(MinSizeLimit);
	PROPERTY_DECL_SET(MinSizeLimit);
	PROPERTY_DECL_GET(MaxSizeLimit);
	PROPERTY_DECL_SET(MaxSizeLimit);
	PROPERTY_DECL_GET(Title);
	PROPERTY_DECL_SET(Title);
	PROPERTY_DECL_GET(WindowState);
	PROPERTY_DECL_SET(WindowState);
	PROPERTY_DECL_GET(Transparency);
	PROPERTY_DECL_SET(Transparency);
	PROPERTY_DECL_GET(Visible);
	PROPERTY_DECL_SET(Visible);
	PROPERTY_DECL_GET(Focussed);
	PROPERTY_DECL_SET(Focussed);
	PROPERTY_DECL_GET(Resizable);
	PROPERTY_DECL_SET(Resizable);
	PROPERTY_DECL_GET(VSync);
	PROPERTY_DECL_SET(VSync);
	PROPERTY_DECL_GET(Parent);
	PROPERTY_DECL_SET(Parent);
	PROPERTY_DECL_GET(IsClosed);
	PROPERTY_DECL_GET(IsDestroyed);
	PROPERTY_DECL_GET(UserPtr);
	PROPERTY_DECL_SET(UserPtr);

private:
	std::unique_ptr<VKDL_PRIV_NAMESPACE_NAME::PlatformWindowImpl> impl;
};

VKDL_END