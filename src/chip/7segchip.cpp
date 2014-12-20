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

/*

 65 01
 GF AB
  ---       aaa
 |   |     f   b
  ---       ggg
 |   |     e   c
  ---  .    ddd  h
 ED CH
 43 27
*/

#include "atanua.h"
#include "7segchip.h"
#include "slidingaverage.h"

SevenSegChip::SevenSegChip(int aColor, int aInverse)
{
	mAvg = new SlidingAverage[8];
    mInverse = aInverse;
    mColor = aColor & 0xffffff;
    set(0, 0, 3, 5,NULL);
    mPin.push_back(&mInputPin[6]);
    mInputPin[6].set(0, 0, this,mInverse?"Pin 10:G#":"Pin 10:G");
    mPin.push_back(&mInputPin[5]);
    mInputPin[5].set(0.5, 0, this,mInverse?"Pin 9:F#":"Pin 9:F");
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(1.5, 0, this,mInverse?"Pin 7:A#":"Pin 7:A");
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(2.0, 0, this,mInverse?"Pin 6:B#":"Pin 6:B");

    mPin.push_back(&mInputPin[4]);
    mInputPin[4].set(0, 4.5, this,mInverse?"Pin 1:E#":"Pin 1:E");
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(0.5, 4.5, this,mInverse?"Pin 2:D#":"Pin 2:D");
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(1.5, 4.5, this,mInverse?"Pin 4:C#":"Pin 4:C");
    mPin.push_back(&mInputPin[7]);
    mInputPin[7].set(2.0, 4.5, this,mInverse?"Pin 5:D.P#":"Pin 5:D.P");

    mTexture[0] = load_texture("data/7seg_base.png");
    mTexture[1] = load_texture("data/7seg_a.png");
    mTexture[2] = load_texture("data/7seg_b.png");
    mTexture[3] = load_texture("data/7seg_c.png");
    mTexture[4] = load_texture("data/7seg_d.png");
    mTexture[5] = load_texture("data/7seg_e.png");
    mTexture[6] = load_texture("data/7seg_f.png");
    mTexture[7] = load_texture("data/7seg_g.png");
    mTexture[8] = load_texture("data/7seg_h.png");

	int i;
	for (i = 0; i < 8; i++)
		mInputPin[i].mReadOnly = 1;
}

SevenSegChip::~SevenSegChip()
{
	delete[] mAvg;
}

void SevenSegChip::render(int aChipId)
{
    drawtexturedrect(mTexture[0], mX-0.75, mY+0.5, mW+1, mH-1, 0xffffffff); 
    glBlendFunc(GL_ONE,GL_ONE);

    int i;
    for (i = 0; i < 8; i++)
    {
		float val = mAvg[i].getAverage();
        if (val > 0)
		{
			if (val > 0.1)
				val = 1.0;
			else
				val *= 10;
			int col = ((int)(((mColor >>  0) & 0xff) * val) <<  0) |
					  ((int)(((mColor >>  8) & 0xff) * val) <<  8) |
					  ((int)(((mColor >> 16) & 0xff) * val) << 16);
            drawtexturedrect(mTexture[i+1], mX-0.75, mY+0.5, mW+1, mH-1, 0xff000000 | col);
		}
    }
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void SevenSegChip::update(float aTick) 
{
    int i;
    for (i = 0; i < 8; i++)
    {
		mAvg[i].setValue(mInputPin[i].mNet && ((mInputPin[i].mNet->mState == NETSTATE_HIGH) ^ (!!mInverse)));
	}
	mDirty = 1;
}    
