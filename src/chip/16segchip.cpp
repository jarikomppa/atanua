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
#include "16segchip.h"
#include "slidingaverage.h"

SixteenSegChip::SixteenSegChip(int aColor, int aInverse)
{
	mAvg = new SlidingAverage[17];
    mInverse = aInverse;
    mColor = aColor & 0xffffff;
    set(0, 0, 3.5, 4, NULL);

	mPin.push_back(&mInputPin[0]);
	mPin.push_back(&mInputPin[1]);
	mPin.push_back(&mInputPin[2]);
	mPin.push_back(&mInputPin[3]);
	mPin.push_back(&mInputPin[4]);
	mPin.push_back(&mInputPin[5]);
	mPin.push_back(&mInputPin[6]);
	mPin.push_back(&mInputPin[7]);
	mPin.push_back(&mInputPin[8]);
	mPin.push_back(&mInputPin[9]);
	mPin.push_back(&mInputPin[10]);
	mPin.push_back(&mInputPin[11]);
	mPin.push_back(&mInputPin[12]);
	mPin.push_back(&mInputPin[13]);
	mPin.push_back(&mInputPin[14]);
	mPin.push_back(&mInputPin[15]);
	mPin.push_back(&mInputPin[16]);

	int i;
	for (i = 0; i < 17; i++)
		mInputPin[i].mReadOnly = 1;

	float ypos = 0.1;
	float step = 0.42;
	mInputPin[0].set(0.03,ypos,this,"Pin 1: A1"); ypos += step;
	mInputPin[1].set(0.03,ypos,this,"Pin 2: A2"); ypos += step;
	mInputPin[2].set(0.03,ypos,this,"Pin 3: J"); ypos += step;
	mInputPin[3].set(0.03,ypos,this,"Pin 4: H"); ypos += step;
	mInputPin[4].set(0.03,ypos,this,"Pin 5: F"); ypos += step;
	mInputPin[5].set(0.03,ypos,this,"Pin 6: E"); ypos += step;
	mInputPin[6].set(0.03,ypos,this,"Pin 7: N"); ypos += step;
	mInputPin[7].set(0.03,ypos,this,"Pin 8: D1"); ypos += step;
	mInputPin[8].set(0.03,ypos,this,"Pin 9: D2"); ypos += step;
	ypos -= step;
	ypos -= step;
	mInputPin[9].set(2.95,ypos,this,"Pin 11: K"); ypos -= step;
	mInputPin[10].set(2.95,ypos,this,"Pin 12: B"); ypos -= step;
	mInputPin[11].set(2.95,ypos,this,"Pin 13: G2"); ypos -= step;
	mInputPin[12].set(2.95,ypos,this,"Pin 14: G1"); ypos -= step;
	mInputPin[13].set(2.95,ypos,this,"Pin 15: C"); ypos -= step;
	mInputPin[14].set(2.95,ypos,this,"Pin 16: L"); ypos -= step;
	mInputPin[15].set(2.95,ypos,this,"Pin 17: M"); ypos -= step;
	mInputPin[16].set(2.95,ypos,this,"Pin 18: DP"); ypos -= step;

    mTexture[0] = load_texture(DATADIR "/16seg_base.png");
    mTexture[1] = load_texture(DATADIR "/16seg_a1.png");
    mTexture[2] = load_texture(DATADIR "/16seg_a2.png");
    mTexture[3] = load_texture(DATADIR "/16seg_j.png");
    mTexture[4] = load_texture(DATADIR "/16seg_h.png");
    mTexture[5] = load_texture(DATADIR "/16seg_f.png");
    mTexture[6] = load_texture(DATADIR "/16seg_e.png");
    mTexture[7] = load_texture(DATADIR "/16seg_n.png");
    mTexture[8] = load_texture(DATADIR "/16seg_d1.png");
    mTexture[9] = load_texture(DATADIR "/16seg_d2.png");
    mTexture[10] = load_texture(DATADIR "/16seg_k.png");
    mTexture[11] = load_texture(DATADIR "/16seg_b.png");
    mTexture[12] = load_texture(DATADIR "/16seg_g1.png");
    mTexture[13] = load_texture(DATADIR "/16seg_g2.png");
    mTexture[14] = load_texture(DATADIR "/16seg_c.png");
    mTexture[15] = load_texture(DATADIR "/16seg_l.png");
    mTexture[16] = load_texture(DATADIR "/16seg_m.png");
    mTexture[17] = load_texture(DATADIR "/16seg_dp.png");
}

SixteenSegChip::~SixteenSegChip()
{
	delete[] mAvg;
}

void SixteenSegChip::render(int aChipId)
{
    drawtexturedrect(mTexture[0], mX-0.25, mY, mW+0.5, mH, 0xffffffff); 
    glBlendFunc(GL_ONE,GL_SRC_ALPHA);

    int i;
    for (i = 0; i < 17; i++)
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

			drawtexturedrect(mTexture[i+1], mX-0.25, mY, mW+0.5, mH, 0xff000000 | col); 
		}
    }
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void SixteenSegChip::update(float aTick) 
{
    int i;
    for (i = 0; i < 17; i++)
    {
        mAvg[i].setValue(mInputPin[i].mNet && ((mInputPin[i].mNet->mState == NETSTATE_HIGH) ^ (!!mInverse)));
	}
	mDirty = 1;
}    
