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
#include "dchip.h"

DChip::DChip()
{
    set(0,0,4,2,NULL);
    mPin.push_back(&mInputPinA);
    mInputPinA.set(0, 0.25, this, "Data");
    mPin.push_back(&mInputPinB);
    mInputPinB.set(0, 1.25, this, "Enable");
    mPin.push_back(&mOutputPinA);
    mOutputPinA.set(3.5, 0.25, this, "Output");
    mPin.push_back(&mOutputPinB);
    mOutputPinB.set(3.5, 1.25, this, "Inverted output");
    mTexture = load_texture(DATADIR "/d.png");
    mOldState = 0;

	mInputPinA.mReadOnly = 1;
	mInputPinB.mReadOnly = 1;
}

void DChip::render(int aChipId)
{
	if (gBlackBackground)
	    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
	else
		drawtexturedrect(mTexture,mX,mY,mW,mH,0xff000000);
}

void DChip::update(float aTick) 
{
    if (mInputPinA.mNet == NULL ||
        mInputPinB.mNet == NULL ||
        mInputPinA.mNet->mState == NETSTATE_INVALID ||
        mInputPinB.mNet->mState == NETSTATE_INVALID)
    {
        mOutputPinA.setState(gConfig.mPropagateInvalidState);
        mOutputPinB.setState(gConfig.mPropagateInvalidState);
        return;
    }


    if (mInputPinB.mNet->mState == NETSTATE_HIGH)
    {
        if (mInputPinA.mNet->mState == NETSTATE_HIGH)
            mOldState = 1;
        else
            mOldState = 0;
    }


    if (mOldState)
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
