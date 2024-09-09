#include "../include/vkdl/core/drawable.h"

VKDL_BEGIN

Drawable::Drawable(UUID renderpass_uuid, UUID pipeline_uuid) :
	renderpass_uuid(renderpass_uuid), 
	pipeline_uuid(pipeline_uuid)
{
}

UUID Drawable::getRenderPassUUID() const
{
	return renderpass_uuid;
}

UUID Drawable::getPipelineUUID() const
{
	return pipeline_uuid;
}

VKDL_END