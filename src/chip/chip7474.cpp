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
#include "chip7474.h"

Chip7474::Chip7474()
{
    set(0,0,4,2,"Dual D Positive Edge Triggered Flip-Flop with Preset and Clear");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mClearPin[1]);
    mClearPin[1].set(xpos, 0, this, "Pin 13:CLR2"); xpos += 0.54;
    mPin.push_back(&mDPin[1]);
    mDPin[1].set(xpos, 0, this, "Pin 12:D2"); xpos += 0.54;
    mPin.push_back(&mClockPin[1]);
    mClockPin[1].set(xpos, 0, this, "Pin 11:CLK2"); xpos += 0.54;
    mPin.push_back(&mPRPin[1]);
    mPRPin[1].set(xpos, 0, this, "Pin 10:PR2"); xpos += 0.54;    
    mPin.push_back(&mOutputPinA[1]);
    mOutputPinA[1].set(xpos, 0, this, "Pin 9:Q2"); xpos += 0.54;
    mPin.push_back(&mOutputPinB[1]);
    mOutputPinB[1].set(xpos, 0, this, "Pin 8:Q2 (inv)"); xpos += 0.54;

    xpos = 0.15;

    mPin.push_back(&mClearPin[0]);
    mClearPin[0].set(xpos, 1.5, this, "Pin 1:CLR1"); xpos += 0.54;
    mPin.push_back(&mDPin[0]);
    mDPin[0].set(xpos, 1.5, this, "Pin 2:D1"); xpos += 0.54;
    mPin.push_back(&mClockPin[0]);
    mClockPin[0].set(xpos, 1.5, this, "Pin 3:CLK1"); xpos += 0.54;
    mPin.push_back(&mPRPin[0]);
    mPRPin[0].set(xpos, 1.5, this, "Pin 4:PR1"); xpos += 0.54;    
    mPin.push_back(&mOutputPinA[0]);
    mOutputPinA[0].set(xpos, 1.5, this, "Pin 5:Q1"); xpos += 0.54;
    mPin.push_back(&mOutputPinB[0]);
    mOutputPinB[0].set(xpos, 1.5, this, "Pin 6:Q1#"); xpos += 0.54;

    mClock[0] = 0;
    mClock[1] = 0;

    mTexture = load_texture(DATADIR "/chip_14pin.png");
}

void Chip7474::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS74",mX+0.6,mY+0.6,0x5fffffff,0.75);
}

void Chip7474::update(float aTick) 
{
    int i;
    for (i = 0; i < 2; i++)
    {
        if (mClearPin[i].mNet == NULL ||
            mDPin[i].mNet == NULL ||
            mClockPin[i].mNet == NULL ||
            mPRPin[i].mNet == NULL ||
            mClearPin[i].mNet->mState == NETSTATE_INVALID ||
            mDPin[i].mNet->mState == NETSTATE_INVALID ||
            mPRPin[i].mNet == NULL ||
            mClockPin[i].mNet == NULL)
        {
            mOutputPinA[i].setState(gConfig.mPropagateInvalidState);
            mOutputPinB[i].setState(gConfig.mPropagateInvalidState);
        }
        else
        {
            if (mClockPin[i].mNet->mState != NETSTATE_LOW)
            {
                if (mClock[i] == 0)
                {
                    mClock[i] = 1;
                    // clocked action..
                    if (mDPin[i].mNet->mState != NETSTATE_LOW)
                    {
                        mOutputPinA[i].setState(PINSTATE_WRITE_HIGH);
                        mOutputPinB[i].setState(PINSTATE_WRITE_LOW);
                    }
                    else
                    {
                        mOutputPinA[i].setState(PINSTATE_WRITE_LOW);
                        mOutputPinB[i].setState(PINSTATE_WRITE_HIGH);
                    }
                }
                else // keep state - write them so that we're not sending invalid signals
                {
                    if (mOutputPinA[i].getState() != PINSTATE_WRITE_LOW)
                    {
                        mOutputPinA[i].setState(PINSTATE_WRITE_HIGH);
                        mOutputPinB[i].setState(PINSTATE_WRITE_LOW);
                    }
                    else
                    {
                        mOutputPinA[i].setState(PINSTATE_WRITE_LOW);
                        mOutputPinB[i].setState(PINSTATE_WRITE_HIGH);
                    }
                }
            }
            else
            {
                mClock[i] = 0;
            }

			// clear/set pins override clocked action

			if (mClearPin[i].mNet->mState == NETSTATE_LOW &&
                mPRPin[i].mNet->mState == NETSTATE_LOW)
            {
                // Unstable state
                mOutputPinA[i].setState(PINSTATE_WRITE_HIGH);
                mOutputPinB[i].setState(PINSTATE_WRITE_HIGH);
            }
            else
            if (mClearPin[i].mNet->mState == NETSTATE_LOW)
            {
                mOutputPinA[i].setState(PINSTATE_WRITE_LOW);
                mOutputPinB[i].setState(PINSTATE_WRITE_HIGH);
            }
            else
            if (mPRPin[i].mNet->mState == NETSTATE_LOW)
            {
                mOutputPinA[i].setState(PINSTATE_WRITE_HIGH);
                mOutputPinB[i].setState(PINSTATE_WRITE_LOW);
            }

        }
    }
}    
