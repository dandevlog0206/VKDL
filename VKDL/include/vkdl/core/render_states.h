#pragma once

#include <optional>
#include "include_vulkan.h"
#include "../util/uuid.h"

VKDL_BEGIN

class RenderTarget;
class RenderOptions;

template <class T>
struct Updatable
{
	VKDL_INLINE bool check_and_update()
	{
		return !std::exchange(updated, true);
	}

	VKDL_INLINE bool check_and_update(const std::optional<T> new_value)
	{
		if (new_value.has_value()) {
			if (value == *new_value) return false;
			reset(*new_value);
		}

		return check_and_update();
	}

	VKDL_INLINE void update_value(const T& new_value)
	{
		if (value != new_value)
			reset(new_value);
	}

	VKDL_INLINE void reset(const T& reset_value = {})
	{
		value   = reset_value;
		updated = false;
	}

	T    value;
	bool updated;
};

class RenderStates
{
public:
	void updateRenderPassUUID(const UUID& uuid);
	void updatePipelineUUID(const UUID& uuid);
	void updateViewport(const vk::Viewport& viewport);
	void updateScissor(vk::Rect2D scissor);

	void reset(const RenderTarget& target);

	void bind(RenderTarget& target, const RenderOptions& options);

private:
	Updatable<UUID>              renderpass_uuid;
	Updatable<UUID>              pipeline_uuid;
	Updatable<vk::Viewport>      viewport;
	Updatable<vk::Rect2D>        scissor;
};

VKDL_END