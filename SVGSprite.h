#ifndef _SVG_SPRITE_H_
#define _SVG_SPRITE_H_

#include "cocos2d.h"

#include <glfw3.h>
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

class SVGSprite : public cocos2d::Sprite
{
public:
	static SVGSprite* create(std::string path, float scale = 1.0f, std::string unit = "px", float dpi = 96.0f);

	bool init(std::string path, float scale, std::string unit, float dpi);
private:
	cocos2d::CustomCommand _command;
};
#endif /* _TILE_MAP_H_ */
