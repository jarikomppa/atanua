/*
Atanua Real-Time Logic Simulator
Copyright (c) 2008-2014 Jari Komppa

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/
#include "atanua.h"
#include "ledchip.h"
#include "slidingaverage.h"

LEDChip::LEDChip(int aColor, int aInverse)
{
	mAvg = new SlidingAverage;
	mColor = aColor & 0xffffff;
	set(0, 0, 1, 2.5, NULL);
	mPin.push_back(&mInputPin);
	mInputPin.set((mW-0.5)/2, mH-0.5, this, mInverse?"Input#":"Input");
	mBaseTexture = load_texture(DATADIR "/led.png");
	mFlareTexture = load_texture(DATADIR "/flare.png");
	mInverse = aInverse;

	mInputPin.mReadOnly = 1;
}

LEDChip::~LEDChip()
{
	delete mAvg;
}

void LEDChip::render(int aChipId)
{
	drawtexturedrect(mBaseTexture,mX, mY, mW, mH-0.5, mColor | 0xff000000);
	float val = mAvg->getAverage();
	if (val > 0)
	{
		if (val > 0.1)
			val = 1.0;
		else
			val *= 10;
		glBlendFunc(GL_ONE,GL_ONE);
		int col = ((int)(((mColor >>  0) & 0xff) * val) <<  0) |
				  ((int)(((mColor >>  8) & 0xff) * val) <<  8) |
				  ((int)(((mColor >> 16) & 0xff) * val) << 16);

		drawtexturedrect(mFlareTexture, mX+(mW/2)-4, mY-4+0.4, 8, 8, col | 0xff000000); 
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
}

void LEDChip::update(float aTick) 
{
	mAvg->setValue(mInputPin.mNet && (((mInputPin.mNet->mState == NETSTATE_HIGH)?1:0) ^ mInverse));
	mDirty = 1;
}    
