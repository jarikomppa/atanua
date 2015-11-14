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
#include "chip74195.h"

Chip74195::Chip74195()
{
    set(0,0,4.54,2.25,"4-bit Parallel-Access Shift Register");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mOutputPin[0]); 
    mOutputPin[0].set(xpos , 0, this, "Pin 15:Q0"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1]); 
    mOutputPin[1].set(xpos , 0, this, "Pin 14:Q1"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]); 
    mOutputPin[2].set(xpos , 0, this, "Pin 13:Q2"); xpos += 0.54;
    mPin.push_back(&mOutputPin[3]); 
    mOutputPin[3].set(xpos , 0, this, "Pin 12:Q3"); xpos += 0.54;
    mPin.push_back(&mOutputPin[4]); 
    mOutputPin[4].set(xpos , 0, this, "Pin 11:Q3#"); xpos += 0.54;

    mPin.push_back(&mClockPin); 
    mClockPin.set(xpos , 0, this, "Pin 10:CP"); xpos += 0.54;
    mPin.push_back(&mParallelEnablePin); 
    mParallelEnablePin.set(xpos , 0, this, "Pin 9:PE#"); xpos += 0.54;
   
    xpos = 0.15;
    mPin.push_back(&mResetPin);
    mResetPin.set(xpos, 1.75, this, "Pin 1:MR#"); xpos += 0.54;

    mPin.push_back(&mInputPinJ);
    mInputPinJ.set(xpos, 1.75, this, "Pin 2:J"); xpos += 0.54;
    mPin.push_back(&mInputPinK);
    mInputPinK.set(xpos, 1.75, this, "Pin 3:K"); xpos += 0.54;

    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(xpos, 1.75, this, "Pin 4:P0"); xpos += 0.54;
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(xpos, 1.75, this, "Pin 5:P1"); xpos += 0.54;
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(xpos, 1.75, this, "Pin 6:P2"); xpos += 0.54;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(xpos, 1.75, this, "Pin 7:P3"); xpos += 0.54;

    mTexture = load_texture(DATADIR "/chip_16pin.png");
    
    mClock = 0;
}

void Chip74195::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS195",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip74195::update(float aTick) 
{
    int i;
    if (mInputPin[0].mNet == NULL ||
        mInputPin[1].mNet == NULL ||
        mInputPin[2].mNet == NULL ||
        mInputPin[3].mNet == NULL ||
        mClockPin.mNet == NULL ||
        mInputPinJ.mNet == NULL ||
        mInputPinK.mNet == NULL ||
        mResetPin.mNet == NULL ||
        mInputPin[0].mNet->mState == NETSTATE_INVALID ||
        mInputPin[1].mNet->mState == NETSTATE_INVALID ||
        mInputPin[2].mNet->mState == NETSTATE_INVALID ||
        mInputPin[3].mNet->mState == NETSTATE_INVALID ||
        mClockPin.mNet->mState == NETSTATE_INVALID ||
        mInputPinJ.mNet->mState == NETSTATE_INVALID ||
        mInputPinK.mNet->mState == NETSTATE_INVALID ||
        mResetPin.mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 5; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    if (mResetPin.mNet->mState == NETSTATE_LOW)
    {
        for (i = 0; i < 4; i++)
            mOutputPin[i].setState(PINSTATE_WRITE_LOW);
        mOutputPin[4].setState(PINSTATE_WRITE_HIGH);
        return;
    }

    if (mClockPin.mNet->mState != NETSTATE_LOW)
    {
        if (mClock == 0)
        {
            mClock = 1;
            if (mParallelEnablePin.mNet->mState == NETSTATE_LOW)
            {
                for (i = 0; i < 4; i++)
                {
                    if (mInputPin[i].mNet->mState != NETSTATE_LOW)
                    {
                        mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
                    }
                    else
                    {
                        mOutputPin[i].setState(PINSTATE_WRITE_LOW);
                    }
                }
            }
            else
            {
                // shiftin'
                for (i = 3; i > 0; i--)
                    mOutputPin[i].setState(mOutputPin[i-1].getState());
                
                // state of the first output pin depends on J and K inputs:
                
                if (mInputPinJ.mNet->mState != NETSTATE_LOW &&
                    mInputPinK.mNet->mState != NETSTATE_LOW)
                {
                    mOutputPin[0].setState(PINSTATE_WRITE_HIGH);
                }
                else
                if (mInputPinJ.mNet->mState == NETSTATE_LOW &&
                    mInputPinK.mNet->mState == NETSTATE_LOW)
                {
                    mOutputPin[0].setState(PINSTATE_WRITE_LOW);
                }
                else
                if (mInputPinJ.mNet->mState != NETSTATE_LOW &&
                    mInputPinK.mNet->mState == NETSTATE_LOW)
                {
                    if (mOutputPin[0].getState() != PINSTATE_WRITE_LOW)
                        mOutputPin[0].setState(PINSTATE_WRITE_LOW);
                    else
                        mOutputPin[0].setState(PINSTATE_WRITE_HIGH);
                }
                // 4th case is "retain value in first stage", which is a NOP here
            }

            // last output bit is inverted to the 5th pin
            if (mOutputPin[3].getState() != PINSTATE_WRITE_LOW)
            {
                mOutputPin[4].setState(PINSTATE_WRITE_LOW);
            }
            else
            {
                mOutputPin[4].setState(PINSTATE_WRITE_HIGH);
            }
        }
    }
    else
    {
        mClock = 0;
    }
}    
