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
#include "dxchip.h"

DxChip::DxChip()
{
    set(0,0,5,8,NULL);
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(0, 1.25, this, "G0");
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(0, 2.25, this, "G1");
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(0, 3.25, this, "G2");

    mPin.push_back(&mEnablePin[0]);
    mEnablePin[0].set(0, 5.25, this, "Enable");
    mPin.push_back(&mEnablePin[1]);
    mEnablePin[1].set(0, 6.25, this, "Enable");
    mPin.push_back(&mEnablePin[2]);
    mEnablePin[2].set(0, 7.25, this, "Enable");

    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(4.5, 0.25, this, "Output 0");
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(4.5, 1.25, this, "Output 1");
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(4.5, 2.25, this, "Output 2");
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(4.5, 3.25, this, "Output 3");
    mPin.push_back(&mOutputPin[4]);
    mOutputPin[4].set(4.5, 4.25, this, "Output 4");
    mPin.push_back(&mOutputPin[5]);
    mOutputPin[5].set(4.5, 5.25, this, "Output 5");
    mPin.push_back(&mOutputPin[6]);
    mOutputPin[6].set(4.5, 6.25, this, "Output 6");
    mPin.push_back(&mOutputPin[7]);
    mOutputPin[7].set(4.5, 7.25, this, "Output 7");

    mTexture = load_texture("data/dx.png");

	mInputPin[0].mReadOnly = 1;
	mInputPin[1].mReadOnly = 1;
	mInputPin[2].mReadOnly = 1;
	mEnablePin[0].mReadOnly = 1;
	mEnablePin[1].mReadOnly = 1;
	mEnablePin[2].mReadOnly = 1;
}

void DxChip::render(int aChipId)
{
	if (gBlackBackground)
	    drawtexturedrect(mTexture,mX,mY,8,8,0xffffffff);
	else
		drawtexturedrect(mTexture,mX,mY,8,8,0xff000000);
}

void DxChip::update(float aTick) 
{
    int i;
    if (mInputPin[0].mNet == NULL ||
        mInputPin[1].mNet == NULL ||
        mInputPin[2].mNet == NULL ||
        mEnablePin[0].mNet == NULL ||
        mEnablePin[1].mNet == NULL ||
        mEnablePin[2].mNet == NULL ||
        mInputPin[0].mNet->mState == NETSTATE_INVALID ||
        mInputPin[1].mNet->mState == NETSTATE_INVALID ||
        mInputPin[2].mNet->mState == NETSTATE_INVALID ||
        mEnablePin[0].mNet->mState == NETSTATE_INVALID ||
        mEnablePin[1].mNet->mState == NETSTATE_INVALID ||
        mEnablePin[2].mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 8; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    for (i = 0; i < 8; i++)
        mOutputPin[i].setState(PINSTATE_WRITE_HIGH);

    if (mEnablePin[0].mNet->mState == NETSTATE_LOW ||
        mEnablePin[1].mNet->mState == NETSTATE_HIGH ||
        mEnablePin[2].mNet->mState == NETSTATE_HIGH)
    {
        return;
    }

    int v = ((mInputPin[2].mNet->mState == NETSTATE_HIGH)?(1 << 2):0) |
            ((mInputPin[1].mNet->mState == NETSTATE_HIGH)?(1 << 1):0) |
            ((mInputPin[0].mNet->mState == NETSTATE_HIGH)?(1 << 0):0);

    mOutputPin[v].setState(PINSTATE_WRITE_LOW);
}    
