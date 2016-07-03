#ifndef _SVG_SPRITE_PATH_H_
#define _SVG_SPRITE_PATH_H_

#include "cocos2d.h"

#include <glfw3.h>
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

class SVGSpritePath : public cocos2d::Node
{
public:
	~SVGSpritePath();

	static SVGSpritePath* create(std::string path, std::string meter, float size);

	bool init(std::string path, std::string meter, float size);

	virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags) override;

	void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
private:
	cocos2d::CustomCommand _command;

	NSVGimage* _gImage = NULL;

	float distPtSeg(float x, float y, float px, float py, float qx, float qy);
	void cubicBez(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tol, int level);
	void drawPath(float* pts, int npts, char closed, float tol);
	void drawControlPts(float* pts, int npts);
};
#endif /* _TILE_MAP_H_ */
