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
#include "chip309.h"
#include "slidingaverage.h"

Chip309::Chip309()
{
	mAvg = new SlidingAverage[8];
	set(0, 0, 3, 6,NULL);
    mTooltip = "Numeric display with logic";
    float yp = 1;
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(0, yp, this, "Pin 1:QB"); yp += 0.5;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(0, yp, this, "Pin 2:QC"); yp += 0.5;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(0, yp, this, "Pin 3:QD"); yp += 0.5;
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(0, yp, this, "Pin 4:QA"); yp += 0.5;
    mPin.push_back(&mLS);
    mLS.set(0, yp, this, "Pin 5:LS#"); yp += 0.5;
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(0, yp, this, "Pin 6:C"); yp += 0.5;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(0, yp, this, "Pin 7:D"); yp += 0.5;
    
    yp=1.5f;
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(2.5, yp, this, "Pin 10:B"); yp += 0.5;
    mPin.push_back(&mBI);
    mBI.set(2.5, yp, this, "Pin 11:BI#"); yp += 0.5;
    mPin.push_back(&mInputPin[4]);
    mInputPin[4].set(2.5, yp, this, "Pin 12:DP"); yp += 0.5;
    mPin.push_back(&mLT);
    mLT.set(2.5, yp, this, "Pin 13:LT#"); yp += 0.5;
    mPin.push_back(&mOutputPin[4]);
    mOutputPin[4].set(2.5, yp, this, "Pin 14:QDP"); yp += 0.5;
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(2.5, yp, this, "Pin 15:A"); yp += 0.5;

    mTexture[0] = load_texture("data/309.png");
    mTexture[1] = load_texture("data/309_b.png");
    mTexture[2] = load_texture("data/309_c.png");
    mTexture[3] = load_texture("data/309_d.png");
    mTexture[4] = load_texture("data/309_e.png");
    mTexture[5] = load_texture("data/309_f.png");
    mTexture[6] = load_texture("data/309_a.png");
    mTexture[7] = load_texture("data/309_g.png");
    mTexture[8] = load_texture("data/309_h.png");

    mData = 0;
    mVisibleData = 0;
}

Chip309::~Chip309()
{
	delete[] mAvg;
}

void Chip309::render(int aChipId)
{
    drawtexturedrect(mTexture[0], mX, mY, mW, mH, 0xffffffff); 
    glBlendFunc(GL_ONE,GL_ONE);    

    int i;
    for (i = 0; i < 8; i++)
    {
		float val = mAvg[i].getAverage();
        if (val > 0) { //mVisibleData & (1 << i))
			if (val > 0.1)
				val = 1.0;
			else
				val *= 10;
			drawtexturedrect(mTexture[i+1], mX, mY, mW, mH/2, 0xff000000 | (int)(0xff * val) * 0x010101); 
		}
    }
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}

void Chip309::update(float aTick) 
{
    int i;
    if (mLT.mNet != NULL && mLT.mNet->mState == NETSTATE_LOW)
    {
        mVisibleData = 0xff;
		for (i = 0; i < 8; i++)
			mAvg[i].setValue((mVisibleData & (1 << i)) != 0);
        return;
    }

    if (mBI.mNet != NULL && mBI.mNet->mState == NETSTATE_LOW)
    {
        mVisibleData = 0;
		for (i = 0; i < 8; i++)
			mAvg[i].setValue((mVisibleData & (1 << i)) != 0);
        return;
    }

    if (mInputPin[0].mNet == NULL ||
        mInputPin[1].mNet == NULL ||
        mInputPin[2].mNet == NULL ||
        mInputPin[3].mNet == NULL ||
        mInputPin[4].mNet == NULL ||
        mInputPin[0].mNet->mState == NETSTATE_INVALID ||
        mInputPin[1].mNet->mState == NETSTATE_INVALID ||
        mInputPin[2].mNet->mState == NETSTATE_INVALID ||
        mInputPin[3].mNet->mState == NETSTATE_INVALID ||
        mInputPin[4].mNet->mState == NETSTATE_INVALID)
    {
        mVisibleData = 0;
        mData = 0;
        for (i = 0; i < 5; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
		for (i = 0; i < 8; i++)
			mAvg[i].setValue((mVisibleData & (1 << i)) != 0);
        return;
    }
    
    if (mLS.mNet == NULL || mLS.mNet->mState == NETSTATE_LOW)
    {
        mData = ((mInputPin[0].mNet->mState != NETSTATE_LOW)?(1 << 0):0) |
                ((mInputPin[1].mNet->mState != NETSTATE_LOW)?(1 << 1):0) |
                ((mInputPin[2].mNet->mState != NETSTATE_LOW)?(1 << 2):0) |
                ((mInputPin[3].mNet->mState != NETSTATE_LOW)?(1 << 3):0) |
                ((mInputPin[4].mNet->mState != NETSTATE_LOW)?(1 << 4):0);
    }

    for (i = 0; i < 5; i++)
        mOutputPin[i].setState((mData & (1 << i))?PINSTATE_WRITE_HIGH:PINSTATE_WRITE_LOW);

    static int leds[] = {
        63,//0111111
        3,//0000011
        109,//1101101
        103,//1100111
        83,//1010011
        118,//1110110
        126,//1111110
        35,//0100011
        127,//1111111
        119,//1110111
        123,//1111011
        64,//1000000
        60,//0111100
        0,//0000000
        124,//1111100
        120//1111000
    };

    mVisibleData = leds[mData & 0xf] | ((mData & (1<<4))?(1<<7):0);

	for (i = 0; i < 8; i++)
		mAvg[i].setValue((mVisibleData & (1 << i)) != 0);
	mDirty = 1;
}    
