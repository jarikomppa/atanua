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
#include "chip7490.h"

Chip7490::Chip7490()
{
    set(0,0,4,2,"Decade Counter (separate Divide-by-2 and Divide-by-5 sections)");
    float xpos = 0.15;
    
    mPin.push_back(&mInputPinA);
    mInputPinA.set(xpos, 0, this, "Pin 14:Input A"); xpos += 0.54;
    
    xpos += 0.54;
    
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos, 0, this, "Pin 12:QA"); xpos += 0.54;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(xpos , 0, this, "Pin 11:QD"); xpos += 0.54;

    xpos += 0.54;

    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos, 0, this, "Pin 9:QB"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos , 0, this, "Pin 8:QC"); xpos += 0.54;

    xpos = 0.15;
    mPin.push_back(&mInputPinB);
    mInputPinB.set(xpos, 1.5, this, "Pin 1:Input B"); xpos += 0.54;

    mPin.push_back(&mResetPin[0]);
    mResetPin[0].set(xpos, 1.5, this, "Pin 2:R0(1)"); xpos += 0.54;
    mPin.push_back(&mResetPin[1]);
    mResetPin[1].set(xpos, 1.5, this, "Pin 3:R0(2)"); xpos += 0.54;
    
    xpos += 0.54;
    xpos += 0.54;

    mPin.push_back(&mResetPin[2]);
    mResetPin[2].set(xpos, 1.5, this, "Pin 6:R9(1)"); xpos += 0.54;
    mPin.push_back(&mResetPin[3]);
    mResetPin[3].set(xpos, 1.5, this, "Pin 7:R9(2)"); xpos += 0.54;

    mTexture = load_texture(DATADIR "/chip_14pin.png");
    mClockA = 0;
    mClockB = 0;
}

void Chip7490::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS90",mX+0.6,mY+0.6,0x5fffffff,0.75);
}

void Chip7490::update(float aTick) 
{
    int i;
    if (mInputPinA.mNet == NULL ||
        mInputPinB.mNet == NULL ||
        mResetPin[0].mNet == NULL ||
        mResetPin[1].mNet == NULL ||
        mResetPin[2].mNet == NULL ||
        mResetPin[3].mNet == NULL ||
        mResetPin[0].mNet->mState == NETSTATE_INVALID ||
        mResetPin[1].mNet->mState == NETSTATE_INVALID ||
        mResetPin[2].mNet->mState == NETSTATE_INVALID ||
        mResetPin[3].mNet->mState == NETSTATE_INVALID ||
        mInputPinA.mNet->mState == NETSTATE_INVALID ||
        mInputPinB.mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 4; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    if (mResetPin[0].mNet->mState != NETSTATE_LOW &&
        mResetPin[1].mNet->mState != NETSTATE_LOW)
    {
        for (i = 0; i < 4; i++)
            mOutputPin[i].setState(PINSTATE_WRITE_LOW);
        return;
    }

    if (mResetPin[2].mNet->mState != NETSTATE_LOW &&
        mResetPin[3].mNet->mState != NETSTATE_LOW)
    {
        mOutputPin[0].setState(PINSTATE_WRITE_HIGH);
        mOutputPin[1].setState(PINSTATE_WRITE_LOW);
        mOutputPin[2].setState(PINSTATE_WRITE_LOW);
        mOutputPin[3].setState(PINSTATE_WRITE_HIGH);
        return;
    }

    if (mInputPinA.mNet->mState == NETSTATE_LOW)
    {
        if (mClockA == 1)
        {
            mClockA = 0;
            if (mOutputPin[0].getState() != PINSTATE_WRITE_LOW)
            {
                mOutputPin[0].setState(PINSTATE_WRITE_LOW);
            }
            else
            {
                mOutputPin[0].setState(PINSTATE_WRITE_HIGH);
            }
        }
    }
    else
    {
        mClockA = 1;
    }

    if (mInputPinB.mNet->mState == NETSTATE_LOW)
    {
        if (mClockB == 1)
        {
            mClockB = 0;
            int v = ((mOutputPin[3].getState() != PINSTATE_WRITE_LOW)?(1 << 2):0) |
                    ((mOutputPin[2].getState() != PINSTATE_WRITE_LOW)?(1 << 1):0) |
                    ((mOutputPin[1].getState() != PINSTATE_WRITE_LOW)?(1 << 0):0);

            v++;
            if (v >= 5) v = 0;

            for (i = 0; i < 3; i++)
            {
                if (v & (1 << (i)))
                {
                    mOutputPin[i+1].setState(PINSTATE_WRITE_HIGH);
                }
                else
                {
                    mOutputPin[i+1].setState(PINSTATE_WRITE_LOW);
                }
            }
        }
    }
    else
    {
        mClockB = 1;
    }
}    
