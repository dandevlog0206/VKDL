#pragma once

#include "descriptor_set_layout.h"

VKDL_BEGIN

class DescriptorSet : public std::enable_shared_from_this<DescriptorSet>
{
public:
	

private:
	vk::DescriptorSet desc_set;
};

VKDL_END