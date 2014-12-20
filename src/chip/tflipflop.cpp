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
#include "tflipflop.h"

TFlipFlop::TFlipFlop()
{
    set(0,0,5,5,NULL);
    mPin.push_back(&mInputPinT);
    mInputPinT.set(0, 1.25, this, "Toggle");
    mPin.push_back(&mClockPin);
    mClockPin.set(0, 3.25, this, "Clock");

    mPin.push_back(&mInputPinS);
    mInputPinS.set(2.25, 0, this, "Set");
    mPin.push_back(&mInputPinR);
    mInputPinR.set(2.25, 4.5, this, "Reset");

    mPin.push_back(&mOutputPinA);
    mOutputPinA.set(4.5, 1.25, this, "Output");
    mPin.push_back(&mOutputPinB);
    mOutputPinB.set(4.5, 3.25, this, "Inverted output");
    mTexture = load_texture("data/t_flipflop.png");
    mState = 0;
    mClock = 0;

	mInputPinR.mReadOnly = 1;
	mInputPinS.mReadOnly = 1;
	mInputPinT.mReadOnly = 1;
}

void TFlipFlop::render(int aChipId)
{
	if (gBlackBackground)
	    drawtexturedrect(mTexture,mX,mY,8,8,0xffffffff);
	else
		drawtexturedrect(mTexture,mX,mY,8,8,0xff000000);
}

void TFlipFlop::update(float aTick) 
{
    // Let's make SR pins optional, but still validated
    if (mInputPinT.mNet == NULL ||
        mClockPin.mNet == NULL ||
        mClockPin.mNet->mState == NETSTATE_INVALID ||
        mInputPinT.mNet->mState == NETSTATE_INVALID ||
        (mInputPinS.mNet != NULL && mInputPinS.mNet->mState == NETSTATE_INVALID) ||
        (mInputPinR.mNet != NULL && mInputPinR.mNet->mState == NETSTATE_INVALID))
    {
        mOutputPinA.setState(gConfig.mPropagateInvalidState);
        mOutputPinB.setState(gConfig.mPropagateInvalidState);
        return;
    }


    if (mClockPin.mNet->mState == NETSTATE_HIGH)
    {
        if (mClock == 0)
        {
            mClock = 1;
            if (mInputPinT.mNet->mState != NETSTATE_LOW)
            {
                mState = !mState;
            }
        }
    }
    else
    {
        mClock = 0;
    }

    // If R or S signals are high, they override the JK/clock signals

    if (mInputPinR.mNet != NULL && mInputPinR.mNet->mState == NETSTATE_HIGH)
    {
        mState = 0;
    }

    if (mInputPinS.mNet != NULL && mInputPinS.mNet->mState == NETSTATE_HIGH)
    {
        mState = 1;
    }


    if (mState)
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
