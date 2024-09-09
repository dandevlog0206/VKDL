#pragma once

#include "config.h"
#include "../util/uuid.h"

VKDL_BEGIN

class RenderTarget;
class RenderStates;
class RenderOptions;

class Drawable
{
public:
	Drawable(UUID renderpass_uuid, UUID pipeline_uuid);

	UUID getRenderPassUUID() const;
	UUID getPipelineUUID() const;

	virtual void draw(RenderTarget& target, RenderStates& states, const RenderOptions& options) const = 0;

private:
	UUID renderpass_uuid;
	UUID pipeline_uuid;
};

VKDL_END