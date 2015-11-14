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
#include "nand3chip.h"

NAND3Chip::NAND3Chip(int aUS)
{
    set(0,0,4,2,NULL);
    mPin.push_back(&mInputPinA);
    mInputPinA.set(0, 0.25, this, "Input A");
    mPin.push_back(&mInputPinB);
    mInputPinB.set(0, 0.75, this, "Input B");
    mPin.push_back(&mInputPinC);
    mInputPinC.set(0, 1.25, this, "Input C");
    mPin.push_back(&mOutputPin);
    mOutputPin.set(3.5, 0.75, this, "Output");
    if (aUS)
        mTexture = load_texture(DATADIR "/nand3_us.png");
    else
        mTexture = load_texture(DATADIR "/nand3_fi.png");

	mInputPinA.mReadOnly = 1;
	mInputPinB.mReadOnly = 1;
	mInputPinC.mReadOnly = 1;
}

void NAND3Chip::render(int aChipId)
{
	if (gBlackBackground)
	    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
	else
		drawtexturedrect(mTexture,mX,mY,mW,mH,0xff000000);
}

void NAND3Chip::update(float aTick) 
{
    if (mInputPinA.mNet == NULL ||
        mInputPinB.mNet == NULL ||
        mInputPinC.mNet == NULL ||
        mInputPinA.mNet->mState == NETSTATE_INVALID ||
        mInputPinB.mNet->mState == NETSTATE_INVALID ||
        mInputPinC.mNet->mState == NETSTATE_INVALID)
    {
        mOutputPin.setState(gConfig.mPropagateInvalidState);
        return;
    }

    if ((mInputPinA.mNet->mState == NETSTATE_HIGH || mInputPinA.mNet->mState == NETSTATE_NC) &&
        (mInputPinB.mNet->mState == NETSTATE_HIGH || mInputPinB.mNet->mState == NETSTATE_NC) &&
        (mInputPinC.mNet->mState == NETSTATE_HIGH || mInputPinC.mNet->mState == NETSTATE_NC))
        mOutputPin.setState(PINSTATE_WRITE_LOW);
    else
        mOutputPin.setState(PINSTATE_WRITE_HIGH);
}    
