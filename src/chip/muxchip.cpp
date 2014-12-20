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
#include "muxchip.h"

MuxChip::MuxChip()
{
    set(0,0,5,8,NULL);
    mPin.push_back(&mInputPinG[0]);
    mInputPinG[0].set(0, 1.25, this, "G0");
    mPin.push_back(&mInputPinG[1]);
    mInputPinG[1].set(0, 2.25, this, "G1");
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(0, 3.25, this, "0");
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(0, 4.25, this, "1");
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(0, 5.25, this, "2");
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(0, 6.25, this, "3");
    mPin.push_back(&mEnablePin);
    mEnablePin.set(0, 7.25, this, "Enable");

    mPin.push_back(&mOutputPinA);
    mOutputPinA.set(4.5, 1.25, this, "Output");
    mPin.push_back(&mOutputPinB);
    mOutputPinB.set(4.5, 3.25, this, "Inverted output");
    mTexture = load_texture("data/mux.png");

	mInputPin[0].mReadOnly = 1;
	mInputPin[1].mReadOnly = 1;
	mInputPin[2].mReadOnly = 1;
	mInputPin[3].mReadOnly = 1;
	mInputPinG[0].mReadOnly = 1;
	mInputPinG[1].mReadOnly = 1;
	mEnablePin.mReadOnly = 1;
}

void MuxChip::render(int aChipId)
{
	if (gBlackBackground)
	    drawtexturedrect(mTexture,mX,mY,8,8,0xffffffff);
	else
		drawtexturedrect(mTexture,mX,mY,8,8,0xff000000);
}

void MuxChip::update(float aTick) 
{
    if (mInputPinG[0].mNet == NULL ||
        mInputPinG[1].mNet == NULL ||
        mInputPin[0].mNet == NULL ||
        mInputPin[1].mNet == NULL ||
        mInputPin[2].mNet == NULL ||
        mInputPin[3].mNet == NULL ||
        mEnablePin.mNet == NULL ||
        mInputPinG[0].mNet->mState == NETSTATE_INVALID ||
        mInputPinG[1].mNet->mState == NETSTATE_INVALID ||
        mInputPin[0].mNet->mState == NETSTATE_INVALID ||
        mInputPin[1].mNet->mState == NETSTATE_INVALID ||
        mInputPin[2].mNet->mState == NETSTATE_INVALID ||
        mInputPin[3].mNet->mState == NETSTATE_INVALID ||
        mEnablePin.mNet->mState == NETSTATE_INVALID)
    {
        mOutputPinA.setState(gConfig.mPropagateInvalidState);
        mOutputPinB.setState(gConfig.mPropagateInvalidState);
        return;
    }

    if (mEnablePin.mNet->mState == NETSTATE_HIGH)
    {
        mOutputPinA.setState(PINSTATE_WRITE_LOW);
        mOutputPinB.setState(PINSTATE_WRITE_HIGH);
        return;
    }

    int v = ((mInputPinG[1].mNet->mState == NETSTATE_HIGH)?(1 << 1):0) |
            ((mInputPinG[0].mNet->mState == NETSTATE_HIGH)?(1 << 0):0);

    if (mInputPin[v].mNet->mState != NETSTATE_LOW)
    {
        mOutputPinA.setState(PINSTATE_WRITE_HIGH);
        mOutputPinB.setState(PINSTATE_WRITE_LOW);
    }
    else
    {
        mOutputPinA.setState(PINSTATE_WRITE_LOW);
        mOutputPinB.setState(PINSTATE_WRITE_HIGH);
    }
}    
