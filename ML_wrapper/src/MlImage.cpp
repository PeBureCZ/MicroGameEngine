#include "MlImage.h"

#include "ImageHolder.h"
#include "MlGameWrapper.h"

MlImage::MlImage(TextureId usedTextureId, size_t usedLayer, FPoint newPosition)
	: usedTexture(std::move(usedTextureId)), imageId(reinterpret_cast<uintptr_t>(this)/*=unique*/), layer(usedLayer)
{
	ML_wrapper::getGlobalGameWrapper()->getImageHolder().appendImage(*this, usedLayer, newPosition);
	size = ML_wrapper::getGlobalGameWrapper()->getImageHolder().getTextureSize(usedTexture);
	absolutePosition = newPosition;
}

MlImage::MlImage(const MlImage& other)
{
	imageId = reinterpret_cast<uintptr_t>(this);
	layer = other.getLayer();
	usedTexture = other.usedTexture;
	ML_wrapper::getGlobalGameWrapper()->getImageHolder().appendImage(*this, layer, other.getAbsolutePosition());
	size = ML_wrapper::getGlobalGameWrapper()->getImageHolder().getTextureSize(usedTexture);
}

tsmType::Size<int> MlImage::getSize() const noexcept
{
	_ASSERT(size.width != 0 && size.height != 0);
	return size;
}

MlImage::MlImage(const MlImage&& other) noexcept
{
	if (this == &other)
		return;
	imageId = reinterpret_cast<uintptr_t>(this);
	usedTexture = other.usedTexture;
	layer = other.getLayer();
	ML_wrapper::getGlobalGameWrapper()->getImageHolder().appendImage(*this, layer, other.getAbsolutePosition());
}

MlImage::~MlImage()
{
	ML_wrapper::getGlobalGameWrapper()->getImageHolder().removeImage(usedTexture, imageId, layer);
}

SPRITE_ID MlImage::getSpriteID() const noexcept
{
    return imageId;
}

TextureId MlImage::getTextureID() const noexcept
{
	return usedTexture;
}

void MlImage::setColor(tsmType::Color_RGBA& newColor) const noexcept
{
	ML_wrapper::getGlobalGameWrapper()->getImageHolder().setSpriteColor(imageId, newColor, layer);
}

void MlImage::setVisible(bool visible) const noexcept
{
	auto color = ML_wrapper::getGlobalGameWrapper()->getImageHolder().getSpriteColor(imageId, layer);
	color.a = visible ? 255 : 0;
	setColor(color);
}

void MlImage::setOrigin(FPoint newOrigin) const noexcept
{
	ML_wrapper::getGlobalGameWrapper()->getImageHolder().setImageAbsolutePosition(imageId, layer, newOrigin);
	//ML_wrapper::getGlobalGameWrapper()->getImageHolder().setGuiImageOrigin(imageId, newOrigin);
}

void MlImage::setRotation(float newRotation) const noexcept
{
	ML_wrapper::getGlobalGameWrapper()->getImageHolder().setSpriteRotation(imageId, newRotation, layer);
}

[[nodiscard]] float MlImage::getRotation() const noexcept
{
	return ML_wrapper::getGlobalGameWrapper()->getImageHolder().getSpriteRotation(imageId, layer);
}

[[nodiscard]] FPoint MlImage::getAbsolutePosition() const noexcept
{
	return absolutePosition;
}


size_t MlImage::getLayer() const noexcept
{
	return layer;
}

void  MlImage::setImgAbsolutePosition(FPoint newPosition)
{
	absolutePosition = newPosition;
	ML_wrapper::getGlobalGameWrapper()->getImageHolder().setImageAbsolutePosition(getSpriteID(), layer, newPosition);
}


