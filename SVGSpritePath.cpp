#include "SVGSpritePath.h"

USING_NS_CC;

static unsigned char bgColor[4] = { 205,202,200,255 };
static unsigned char lineColor[4] = { 0,160,192,255 };

SVGSpritePath::~SVGSpritePath()
{
	nsvgDelete(_gImage);
}

SVGSpritePath* SVGSpritePath::create(std::string path, std::string meter, float size)
{
	SVGSpritePath* ref = new (std::nothrow) SVGSpritePath();
	if(ref && ref->init(path, meter, size))
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

bool SVGSpritePath::init(std::string path, std::string meter, float size)
{
	if (!Node::init()) return false;

	setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_NAME_POSITION_LENGTH_TEXTURE_COLOR));

	_gImage = nsvgParseFromFile(path.c_str(), meter.c_str(), size);

	return true;
}

void SVGSpritePath::visit(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags)
{
	Node::visit(renderer, parentTransform, parentFlags);
	_command.init(_globalZOrder, parentTransform, parentFlags);
	_command.func = CC_CALLBACK_0(SVGSpritePath::onDraw, this, parentTransform, parentFlags);
	renderer->addCommand(&_command);
}

void SVGSpritePath::onDraw(const Mat4 &transform, uint32_t flags)
{
	if (_gImage == NULL) return;

	auto glProgram = getGLProgram();
	glProgram->use();
	glProgram->setUniformsForBuiltins(transform);

	kmGLPushMatrix();
	kmGLTranslatef(getPositionX(), getPositionX(), 0.0f);
	kmGLRotatef(getRotation(), 0.0f, 0.0f, 1.0f);
	kmGLScalef(_gImage->width, _gImage->height, 1.0f);

	int width = Director::getInstance()->getVisibleSize().width;
	int height = Director::getInstance()->getVisibleSize().height;

	//float view[4], cx, cy, hw, hh, aspect, px;
	//// Fit view to bounds
	//cx = _gImage->width*0.5f;
	//cy = _gImage->height*0.5f;
	//hw = _gImage->width*0.5f;
	//hh = _gImage->height*0.5f;

	//if (width / hw < height / hh) {
	//	aspect = (float)height / (float)width;
	//	view[0] = cx - hw * 1.2f;
	//	view[2] = cx + hw * 1.2f;
	//	view[1] = cy - hw * 1.2f * aspect;
	//	view[3] = cy + hw * 1.2f * aspect;
	//}
	//else {
	//	aspect = (float)width / (float)height;
	//	view[0] = cx - hh * 1.2f * aspect;
	//	view[2] = cx + hh * 1.2f * aspect;
	//	view[1] = cy - hh * 1.2f;
	//	view[3] = cy + hh * 1.2f;
	//}

	//// Size of one pixel.
	//px = (view[2] - view[1]) / (float)width;

	// Draw bounds
	glColor4ub(0, 0, 0, 64);
	glBegin(GL_LINE_LOOP);
	glVertex2f(0, 0);
	glVertex2f(_gImage->width, 0);
	glVertex2f(_gImage->width, _gImage->height);
	glVertex2f(0, _gImage->height);
	glEnd();

	NSVGshape* _shape;
	NSVGpath* _path;
	for (_shape = _gImage->shapes; _shape != NULL; _shape = _shape->next) {
		for (_path = _shape->paths; _path != NULL; _path = _path->next) {
			drawPath(_path->pts, _path->npts, _path->closed, 1.0f);
			drawControlPts(_path->pts, _path->npts);
		}
	}
	kmGLPopMatrix();
	CC_INCREMENT_GL_DRAWS(1);

	
}

float SVGSpritePath::distPtSeg(float x, float y, float px, float py, float qx, float qy)
{
	float pqx, pqy, dx, dy, d, t;
	pqx = qx - px;
	pqy = qy - py;
	dx = x - px;
	dy = y - py;
	d = pqx*pqx + pqy*pqy;
	t = pqx*dx + pqy*dy;
	if (d > 0) t /= d;
	if (t < 0) t = 0;
	else if (t > 1) t = 1;
	dx = px + t*pqx - x;
	dy = py + t*pqy - y;
	return dx*dx + dy*dy;
}

void SVGSpritePath::cubicBez(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tol, int level)
{
	float x12, y12, x23, y23, x34, y34, x123, y123, x234, y234, x1234, y1234;
	float d;

	if (level > 12) return;

	x12 = (x1 + x2)*0.5f;
	y12 = (y1 + y2)*0.5f;
	x23 = (x2 + x3)*0.5f;
	y23 = (y2 + y3)*0.5f;
	x34 = (x3 + x4)*0.5f;
	y34 = (y3 + y4)*0.5f;
	x123 = (x12 + x23)*0.5f;
	y123 = (y12 + y23)*0.5f;
	x234 = (x23 + x34)*0.5f;
	y234 = (y23 + y34)*0.5f;
	x1234 = (x123 + x234)*0.5f;
	y1234 = (y123 + y234)*0.5f;

	d = distPtSeg(x1234, y1234, x1, y1, x4, y4);
	if (d > tol*tol) {
		cubicBez(x1, y1, x12, y12, x123, y123, x1234, y1234, tol, level + 1);
		cubicBez(x1234, y1234, x234, y234, x34, y34, x4, y4, tol, level + 1);
	}
	else {
		glVertex2f(x4, y4);
	}
}

void SVGSpritePath::drawPath(float* pts, int npts, char closed, float tol)
{
	int i;
	glBegin(GL_LINE_STRIP);
	glColor4ubv(lineColor);
	glVertex2f(pts[0], pts[1]);
	for (i = 0; i < npts - 1; i += 3) {
		float* p = &pts[i * 2];
		cubicBez(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], tol, 0);
	}
	if (closed) {
		glVertex2f(pts[0], pts[1]);
	}
	glEnd();
}

void SVGSpritePath::drawControlPts(float* pts, int npts)
{
	int i;

	// Control lines
	glColor4ubv(lineColor);
	glBegin(GL_LINES);
	for (i = 0; i < npts - 1; i += 3) {
		float* p = &pts[i * 2];
		glVertex2f(p[0], p[1]);
		glVertex2f(p[2], p[3]);
		glVertex2f(p[4], p[5]);
		glVertex2f(p[6], p[7]);
	}
	glEnd();

	// Points
	glPointSize(6.0f);
	glColor4ubv(lineColor);

	glBegin(GL_POINTS);
	glVertex2f(pts[0], pts[1]);
	for (i = 0; i < npts - 1; i += 3) {
		float* p = &pts[i * 2];
		glVertex2f(p[6], p[7]);
	}
	glEnd();

	// Points
	glPointSize(3.0f);

	glBegin(GL_POINTS);
	glColor4ubv(bgColor);
	glVertex2f(pts[0], pts[1]);
	for (i = 0; i < npts - 1; i += 3) {
		float* p = &pts[i * 2];
		glColor4ubv(lineColor);
		glVertex2f(p[2], p[3]);
		glVertex2f(p[4], p[5]);
		glColor4ubv(bgColor);
		glVertex2f(p[6], p[7]);
	}
	glEnd();
}