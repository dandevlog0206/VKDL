#include <iostream>
#include <thread>
#include <chrono>
#include <vkdl/core/context.h>
#include <vkdl/system/platform_window.h>
#include <vkdl/builder/pipeline_builder.h>
#include <vkdl/graphics/texture.h>
#include <vkdl/graphics/drawlist_2d.h>

#include <random>

using namespace std;
using namespace vkdl;

void EventMsgHandler(PlatformWindow& window, WindowEvent& msg)
{
	cout << msg.getEventTypeName() << endl;
}

int entry()
{
	auto& ctx = Context::get();

	vkdl::PlatformWindow window(1280, 720, "sample window");

	//window.OnEventRecieved += make_delegate(EventMsgHandler);

	DrawList2D drawlist;
	ColorImage image;

	image.loadFromFile("lenna.png");

	auto& desc_set_layout = ctx.getPipeline(drawlist.getPipelineUUID()).getPipelineLayout().getDescriptorSetLayout(0);

	auto texture = TextureCreator()
		.setImageFormat(image.format())
		.setImageUsage(vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst)
		.setImageExtent(image.width(), image.height())
		.setDescriptorSetLayout(desc_set_layout)
		.create();

	texture->update(image.data());

	random_device rd;
	mt19937 rnd(rd());
	normal_distribution<float> dist(0, 100);
	normal_distribution<float> dist_off(0, 1280);
	normal_distribution<float> dist_c(0, 1);

	float t = 0.f;

	bool exit = false;
	while (!exit) {
		WindowEvent e;
		while (window.pollEvent(e)) {
			if (e.type == WindowEvent::Close)
				exit = true;
		}

		window.clear();
		drawlist.clear();

		//for (int i = 0; i < 1000; ++i) {
		//	vec2 off(dist_off(rnd), dist_off(rnd));
		//	Vertex2D v0{ off + vec2(dist(rnd), dist(rnd)), Color(dist_c(rnd), dist_c(rnd), dist_c(rnd)) };
		//	Vertex2D v1{ off + vec2(dist(rnd), dist(rnd)), Color(dist_c(rnd), dist_c(rnd), dist_c(rnd)) };
		//	Vertex2D v2{ off + vec2(dist(rnd), dist(rnd)), Color(dist_c(rnd), dist_c(rnd), dist_c(rnd)) };

		//	drawlist.addRawTriangle(v0, v1, v2);
		//}
		////drawlist.addRawTriangle(
		////	Vertex2D(vec2{ 640, 0 }, Colors::Red),
		////	Vertex2D(vec2{ 0, 720 }, Colors::Blue),
		////	Vertex2D(vec2{ 1280, 720 }, Colors::Green)
		////	);
	
		drawlist.pushTransform(
			Transform2D()
			.translate(256 + 50, 256 + 50)
			.rotate(t)
			.translate(-256 - 50, -256 - 50));
		drawlist.addImage(*texture, vec2(50, 50), vec2(512, 512), vec2(0, 0), vec2(1, 1));

		window.render(drawlist);
		window.display();
		ctx.device.waitIdle();

		this_thread::sleep_for(30ms);
		t = t < 2.f * 3.141592f ? t + 0.02f : 0.f;
	}

	return 0;
}

int main() 
{
	vk::ApplicationInfo app_info = {};
	app_info.apiVersion         = VK_API_VERSION_1_3;
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);

	auto ctx = ContextCreator()
		.setApplicationInfo(app_info)
		.setPhysicalDeviceType(vk::PhysicalDeviceType::eDiscreteGpu)
		.enableDebug(5)
		.create();

	return entry();
}