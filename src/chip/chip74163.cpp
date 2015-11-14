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
#include "chip74163.h"

Chip74163::Chip74163()
{
    set(0,0,4.54,2.25,"Synchronous 4-bit\nBinary Counter with\nSynchronous Clear");
    float xpos = 0.15 + 0.54;

#define DEF_PIN(x,y,d) mPin.push_back(&x); x.set(xpos, y, this, d); xpos += 0.54;

    DEF_PIN(mRippleCarryPin, 0, "Pin 15:RIPPLE CARRY OUTPUT");
    DEF_PIN(mOutputPin[0], 0, "Pin 14:OUTPUT QA");
    DEF_PIN(mOutputPin[1], 0, "Pin 13:OUTPUT QB");
    DEF_PIN(mOutputPin[2], 0, "Pin 12:OUTPUT QC");
    DEF_PIN(mOutputPin[3], 0, "Pin 11:OUTPUT QD");
    DEF_PIN(mEnableTPin, 0, "Pin 10:ENABLE T");   
    DEF_PIN(mLoadPin, 0, "Pin 9:LOAD");   
    xpos = 0.15;
    DEF_PIN(mClearPin, 1.75, "Pin 1:CLEAR");
    DEF_PIN(mClockPin, 1.75, "Pin 2:CLOCK");
    DEF_PIN(mInputPin[0], 1.75, "Pin 3:DATA INPUT A");
    DEF_PIN(mInputPin[1], 1.75, "Pin 4:DATA INPUT B");
    DEF_PIN(mInputPin[2], 1.75, "Pin 5:DATA INPUT C");
    DEF_PIN(mInputPin[3], 1.75, "Pin 6:DATA INPUT D");
    DEF_PIN(mEnablePPin, 1.75, "Pin 7:ENABLE P");

    mTexture = load_texture(DATADIR "/chip_16pin.png");

    mOldClock = 0;
    mValue = 0;
}

void Chip74163::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS163",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip74163::update(float aTick) 
{
#define CHECKVALID(pin) pin.mNet == NULL || pin.mNet->mState == NETSTATE_INVALID
    int i;
    if (CHECKVALID(mEnableTPin) ||
        CHECKVALID(mLoadPin) ||
        CHECKVALID(mClearPin) ||
        CHECKVALID(mClockPin) ||
        CHECKVALID(mEnablePPin) ||
        CHECKVALID(mInputPin[0]) ||
        CHECKVALID(mInputPin[1]) ||
        CHECKVALID(mInputPin[2]) ||
        CHECKVALID(mInputPin[3]))
    {
        for (i = 0; i < 4; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    if (mClockPin.mNet->mState == NETSTATE_HIGH)
    {
        if (mOldClock == 0)
        {
            mOldClock = 1;
            if (mClearPin.mNet->mState == NETSTATE_LOW)
            {
                mValue = 0;
            }
            else
            if (mLoadPin.mNet->mState == NETSTATE_LOW)
            {
                mValue = ((mInputPin[3].mNet->mState == NETSTATE_HIGH)?(1 << 3):0) |
                         ((mInputPin[2].mNet->mState == NETSTATE_HIGH)?(1 << 2):0) |
                         ((mInputPin[1].mNet->mState == NETSTATE_HIGH)?(1 << 1):0) |
                         ((mInputPin[0].mNet->mState == NETSTATE_HIGH)?(1 << 0):0);
            }
            else
            if (mEnableTPin.mNet->mState == NETSTATE_HIGH &&
                mEnablePPin.mNet->mState == NETSTATE_HIGH)
            {
                mValue++;
            }
        }
    }
    else
    {
        mOldClock = 0;
    }

    for (i = 0; i < 4; i++)
    {
        if (mValue & (1 << i))
        {
            mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
        }
        else
        {
            mOutputPin[i].setState(PINSTATE_WRITE_LOW);
        }
    }

    if (mEnableTPin.mNet->mState == NETSTATE_HIGH)
    {
        if (mValue & (1 << 4))
        {
            mRippleCarryPin.setState(PINSTATE_WRITE_HIGH);
            mValue &= 15;
        }
        else
        {
            mRippleCarryPin.setState(PINSTATE_WRITE_LOW);
        }
    }
    else
    {
        mRippleCarryPin.setState(PINSTATE_WRITE_LOW);
    }
}    
