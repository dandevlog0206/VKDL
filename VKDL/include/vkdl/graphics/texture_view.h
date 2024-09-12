#include "texture.h"

#include "../core/drawable.h"
#include "transformable_2d.h"

VKDL_BEGIN

class TextureView : public Transformable2D, public Drawable
{
	PROPERTY_INIT(TextureView);

public:
	TextureView();
	TextureView(const Texture& texture);
	TextureView(const Texture& texture, const vec2& offset, const vec2& size);

	PROPERTY{
		PROPERTY_DEFAULT_GET_SET(const Texture*, TexturePtr);
		PROPERTY_DEFAULT_GET_SET(vec2, Offset);
		PROPERTY_DEFAULT_GET_SET(vec2, Size);
		PROPERTY_DEFAULT_GET_SET(Color, FilledColor);
	};

	bool empty() const;

private:
	void draw(RenderTarget& target, RenderStates& states, const RenderOptions& options) const override;
};

VKDL_END