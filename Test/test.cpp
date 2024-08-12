#include <iostream>
#include <vkdl/core/context.h>
#include <vkdl/system/platform_window.h>

using namespace std;
using namespace vkdl;

void EventMsgHandler(PlatformWindow& sender, EventMsg& msg)
{
	cout << msg.getEventTypeName() << endl;
}

int main() 
{
	vk::ApplicationInfo info = {};
	info.apiVersion         = VK_API_VERSION_1_3;
	info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);

	vkdl::Context ctx(info, {}, {}, true);

	vkdl::PlatformWindow window(1280, 720, "sample window");

	window.OnEventMsgRecieved += make_delegate(EventMsgHandler);

	while (true) {
		window.processEvent();
	}

	return 0;
}