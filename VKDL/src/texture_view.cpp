#include "../include/vkdl/graphics/texture_view.h"

VKDL_BEGIN

TextureView::TextureView() :
	Drawable(),
	Offset(),
	Size(),
	UV0(),
	UV1()
{
}

bool TextureView::empty() const
{

}

void TextureView::draw(RenderTarget& target, RenderStates& states, const RenderOptions& options) const
{

}

VKDL_END