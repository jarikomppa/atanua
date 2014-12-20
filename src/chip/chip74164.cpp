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
#include "chip74164.h"

Chip74164::Chip74164()
{
    set(0,0,4,2,"8-bit Parallel-Out\nSerial Shift Register with\nAsynchronous Clear");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mOutputPin[7]);
    mOutputPin[7].set(xpos , 0, this, "Pin 13:OUTPUT QH"); xpos += 0.54;
    mPin.push_back(&mOutputPin[6]);                 
    mOutputPin[6].set(xpos , 0, this, "Pin 12:OUTPUT QG"); xpos += 0.54;
    mPin.push_back(&mOutputPin[5]);                 
    mOutputPin[5].set(xpos , 0, this, "Pin 11:OUTPUT QF"); xpos += 0.54;
    mPin.push_back(&mOutputPin[4]);                 
    mOutputPin[4].set(xpos , 0, this, "Pin 10:OUTPUT QE"); xpos += 0.54;

    mPin.push_back(&mClearPin);
    mClearPin.set(xpos , 0, this, "Pin 9:CLEAR"); xpos += 0.54;

    mPin.push_back(&mClockPin);
    mClockPin.set(xpos , 0, this, "Pin 9:CLOCK"); xpos += 0.54;

    xpos = 0.15;
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(xpos, 1.5, this, "Pin 1:SERIAL INPUT A"); xpos += 0.54;
    mPin.push_back(&mInputPin[1]);                       
    mInputPin[1].set(xpos, 1.5, this, "Pin 2:SERIAL INPUT B"); xpos += 0.54;

    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos , 1.5, this, "Pin 3:OUTPUT QA"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1]);                  
    mOutputPin[1].set(xpos , 1.5, this, "Pin 4:OUTPUT QB"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]);                  
    mOutputPin[2].set(xpos , 1.5, this, "Pin 5:OUTPUT QC"); xpos += 0.54;
    mPin.push_back(&mOutputPin[3]);                  
    mOutputPin[3].set(xpos , 1.5, this, "Pin 6:OUTPUT QD"); xpos += 0.54;

    mTexture = load_texture("data/chip_14pin.png");

    mOldClock = 0;
}

void Chip74164::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS164",mX+0.6,mY+0.6,0x5fffffff,0.75);
}

void Chip74164::update(float aTick) 
{
    int i;
    if (mClearPin.mNet == NULL ||
        mClockPin.mNet == NULL ||
        mInputPin[0].mNet == NULL ||
        mInputPin[1].mNet == NULL ||
        mClearPin.mNet->mState == NETSTATE_INVALID ||
        mClockPin.mNet->mState == NETSTATE_INVALID ||
        mInputPin[0].mNet->mState == NETSTATE_INVALID ||
        mInputPin[1].mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 8; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    if (mClearPin.mNet->mState == NETSTATE_LOW)
    {
        for (i = 0; i < 8; i++)
            mOutputPin[i].setState(PINSTATE_WRITE_LOW);
        return;
    }

    if (mClockPin.mNet->mState == NETSTATE_HIGH)
    {
        if (mOldClock == 0)
        {
            mOldClock = 1;
            for (i = 7; i > 0; i--)
            {
                if (mOutputPin[i-1].getState() == PINSTATE_WRITE_HIGH)
                {
                    mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
                }
                else
                {
                    mOutputPin[i].setState(PINSTATE_WRITE_LOW);
                }
            }
            if (mInputPin[0].mNet->mState == NETSTATE_HIGH &&
                mInputPin[1].mNet->mState == NETSTATE_HIGH)
            {
                mOutputPin[0].setState(PINSTATE_WRITE_HIGH);
            }
            else
            {
                mOutputPin[0].setState(PINSTATE_WRITE_LOW);
            }
        }        
    }
    else
    {
        mOldClock = 0;
        if (mOutputPin[0].getState() == PINSTATE_HIGHZ || mOutputPin[0].getState() == PINSTATE_PROPAGATE_INVALID)
        {
            for (i = 0; i < 8; i++)
                mOutputPin[i].setState(PINSTATE_WRITE_LOW);
        }
    }    
}    
