# Cocos2d-x SVGSprite

![Alt][1]

Support cocos2d-x display SVG as sprite or path

ref: NanoSVG ( https://github.com/memononen/nanosvg )

# Setup:
Add source code to your Class folder or anywhere

# Using: 
```c++
#include "SVGSprite.h"
...
SVGSprite* testSVG = SVGSprite::create("tiger.svg", 0.5f);
this->addChild(testSVG);
```

[1]: /demo_tiger.png "Demo Tiger"
