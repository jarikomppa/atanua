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
#include "chip74165.h"

Chip74165::Chip74165()
{
    set(0,0,4.54,2.25,"Parallel-Load 8-Bit Shift Register");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mClock[1]);
    mClock[1].set(xpos , 0, this, "Pin 15:CLOCK2"); xpos += 0.54;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(xpos , 0, this, "Pin 14:P3"); xpos += 0.54;
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(xpos , 0, this, "Pin 13:P2"); xpos += 0.54;
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(xpos , 0, this, "Pin 12:P1"); xpos += 0.54;
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(xpos , 0, this, "Pin 11:P0"); xpos += 0.54;
    mPin.push_back(&mSerialData);
    mSerialData.set(xpos , 0, this, "Pin 10:SERIAL DATA (DS)"); xpos += 0.54;
    mPin.push_back(&mOutput);
    mOutput.set(xpos , 0, this, "Pin 9:OUTPUT Q7"); xpos += 0.54;
    
    xpos = 0.15;
    mPin.push_back(&mLoad);
    mLoad.set(xpos, 1.75, this, "Pin 1:PARALLEL LOAD (PL)#"); xpos += 0.54;
    mPin.push_back(&mClock[0]);
    mClock[0].set(xpos, 1.75, this, "Pin 2:CLOCK1"); xpos += 0.54;
    mPin.push_back(&mInputPin[4]);
    mInputPin[4].set(xpos, 1.75, this, "Pin 3:P4"); xpos += 0.54;
    mPin.push_back(&mInputPin[5]);
    mInputPin[5].set(xpos, 1.75, this, "Pin 4:P5"); xpos += 0.54;
    mPin.push_back(&mInputPin[6]);
    mInputPin[6].set(xpos, 1.75, this, "Pin 5:P6"); xpos += 0.54;
    mPin.push_back(&mInputPin[7]);
    mInputPin[7].set(xpos, 1.75, this, "Pin 6:P7"); xpos += 0.54;
    mPin.push_back(&mNegOutput);
    mNegOutput.set(xpos, 1.75, this, "Pin 7:OUTPUT (Q7)#"); xpos += 0.54;

    mTexture = load_texture("data/chip_16pin.png");
    mLastClock = 0;
    mValue = 0;
}

void Chip74165::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS165",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip74165::update(float aTick) 
{
    if (
        (mInputPin[0].mNet != NULL && mInputPin[0].mNet->mState == NETSTATE_INVALID) ||
        (mInputPin[1].mNet != NULL && mInputPin[1].mNet->mState == NETSTATE_INVALID) ||
        (mInputPin[2].mNet != NULL && mInputPin[2].mNet->mState == NETSTATE_INVALID) ||
        (mInputPin[3].mNet != NULL && mInputPin[3].mNet->mState == NETSTATE_INVALID) ||
        (mInputPin[4].mNet != NULL && mInputPin[4].mNet->mState == NETSTATE_INVALID) ||
        (mInputPin[5].mNet != NULL && mInputPin[5].mNet->mState == NETSTATE_INVALID) ||
        (mInputPin[6].mNet != NULL && mInputPin[6].mNet->mState == NETSTATE_INVALID) ||
        (mInputPin[7].mNet != NULL && mInputPin[7].mNet->mState == NETSTATE_INVALID) ||
        (mSerialData.mNet != NULL && mSerialData.mNet->mState == NETSTATE_INVALID) ||

        mClock[0].mNet == NULL || mClock[0].mNet->mState == NETSTATE_INVALID ||
        mClock[1].mNet == NULL || mClock[1].mNet->mState == NETSTATE_INVALID ||

        (mLoad.mNet != NULL && mLoad.mNet->mState == NETSTATE_LOW && 
         (mInputPin[0].mNet == NULL ||
          mInputPin[1].mNet == NULL ||
          mInputPin[2].mNet == NULL ||
          mInputPin[3].mNet == NULL ||
          mInputPin[4].mNet == NULL ||
          mInputPin[5].mNet == NULL ||
          mInputPin[6].mNet == NULL ||
          mInputPin[7].mNet == NULL))
        )
    {
        mOutput.setState(gConfig.mPropagateInvalidState);
        mNegOutput.setState(gConfig.mPropagateInvalidState);
        
        return;
    }

    if (mLoad.mNet != NULL &&
        mLoad.mNet->mState == NETSTATE_LOW)
    {
        mValue = ((mInputPin[7].mNet->mState != NETSTATE_LOW)?(1 << 7):0) |
                 ((mInputPin[6].mNet->mState != NETSTATE_LOW)?(1 << 6):0) |
                 ((mInputPin[5].mNet->mState != NETSTATE_LOW)?(1 << 5):0) |
                 ((mInputPin[4].mNet->mState != NETSTATE_LOW)?(1 << 4):0) |
                 ((mInputPin[3].mNet->mState != NETSTATE_LOW)?(1 << 3):0) |
                 ((mInputPin[2].mNet->mState != NETSTATE_LOW)?(1 << 2):0) |
                 ((mInputPin[1].mNet->mState != NETSTATE_LOW)?(1 << 1):0) |
                 ((mInputPin[0].mNet->mState != NETSTATE_LOW)?(1 << 0):0);
    }
    else
    {
        int clock = !((mClock[0].mNet->mState != NETSTATE_LOW) &&
                      (mClock[1].mNet->mState != NETSTATE_LOW));
        if (mLastClock == 0 && clock != 0)
        {
            // Rising edge
            mValue <<= 1;
            if (mSerialData.mNet != NULL && mSerialData.mNet->mState != NETSTATE_LOW)
                mValue |= 1;
        }
        mLastClock = clock;
    }
    
    mOutput.setState(PINSTATE_WRITE_LOW);
    mNegOutput.setState(PINSTATE_WRITE_HIGH);

    if (mValue & (1 << 7))
    {
        mOutput.setState(PINSTATE_WRITE_HIGH);
        mNegOutput.setState(PINSTATE_WRITE_LOW);
    }
}    
