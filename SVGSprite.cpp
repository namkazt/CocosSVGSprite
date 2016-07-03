#include "SVGSprite.h"

USING_NS_CC;

SVGSprite* SVGSprite::create(std::string path, float scale, std::string unit, float dpi)
{
	SVGSprite* ref = new (std::nothrow) SVGSprite();
	if(ref && ref->init(path, scale, unit, dpi))
	{
		ref->autorelease();
		return ref;
	}else
	{
		CC_SAFE_DELETE(ref);
		ref = nullptr;
		return nullptr;
	}
}

bool SVGSprite::init(std::string path, float scale, std::string unit, float dpi)
{
	NSVGimage *image = NULL;
	NSVGrasterizer *rast = NULL;
	unsigned char* img = NULL;
	int w, h;

	image = nsvgParseFromFile(path.c_str(), unit.c_str(), dpi);
	if (image == NULL) {
		log("Could not open SVG image.\n");
		nsvgDeleteRasterizer(rast);
		nsvgDelete(image);
		return false;
	}

	w = (int)image->width * scale;
	h = (int)image->height * scale;
	rast = nsvgCreateRasterizer();
	if (rast == NULL) {
		log("Could not init Rasterizer.\n");
		nsvgDeleteRasterizer(rast);
		nsvgDelete(image);
		return false;
	}

	img = (unsigned char*)malloc(w*h * 4);
	if (img == NULL) {
		log("Could not alloc image buffer.\n");
		nsvgDeleteRasterizer(rast);
		nsvgDelete(image);
		return false;
	}

	log("rasterizing image %d x %d\n", w, h);
	nsvgRasterize(rast, image, 0, 0, scale, img, w, h, w * 4);

	Texture2D* texture = new Texture2D();
	texture->initWithData(img, w*h * 4, Texture2D::PixelFormat::RGBA8888, w, h, Size(w, h));

	return Sprite::initWithTexture(texture);
}
