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
#include "chip7473.h"

Chip7473::Chip7473()
{
    set(0,0,4,2,"Dual J-K Flip-Flop with Clear");
    float xpos = 0.15;
    mPin.push_back(&mInputPinJ[0]);
    mInputPinJ[0].set(xpos, 0, this, "Pin 14:J1"); xpos += 0.54;
    
    mPin.push_back(&mOutputPinB[0]);
    mOutputPinB[0].set(xpos, 0, this, "Pin 13:Q1#"); xpos += 0.54;
    mPin.push_back(&mOutputPinA[0]);
    mOutputPinA[0].set(xpos, 0, this, "Pin 12:Q1"); xpos += 0.54;
    
    xpos += 0.54;

    mPin.push_back(&mInputPinK[1]);
    mInputPinK[1].set(xpos, 0, this, "Pin 10:K2"); xpos += 0.54;
    mPin.push_back(&mOutputPinA[1]);
    mOutputPinA[1].set(xpos, 0, this, "Pin 9:Q2"); xpos += 0.54;
    mPin.push_back(&mOutputPinB[1]);
    mOutputPinB[1].set(xpos, 0, this, "Pin 8:Q2#"); xpos += 0.54;

    xpos = 0.15;
    mPin.push_back(&mClockPin[0]);
    mClockPin[0].set(xpos, 1.5, this, "Pin 1:CLK1"); xpos += 0.54;
    mPin.push_back(&mClearPin[0]);
    mClearPin[0].set(xpos, 1.5, this, "Pin 2:CLR1"); xpos += 0.54;
    mPin.push_back(&mInputPinK[0]);
    mInputPinK[0].set(xpos, 1.5, this, "Pin 3:K1"); xpos += 0.54;

    xpos += 0.54;

    mPin.push_back(&mClockPin[1]);
    mClockPin[1].set(xpos, 1.5, this, "Pin 5:CLK2"); xpos += 0.54;
    mPin.push_back(&mClearPin[1]);
    mClearPin[1].set(xpos, 1.5, this, "Pin 6:CLR2"); xpos += 0.54;
    mPin.push_back(&mInputPinJ[1]);
    mInputPinJ[1].set(xpos, 1.5, this, "Pin 7:J2"); xpos += 0.54;

    mClock[0] = 1;
    mClock[1] = 1;

    mTexture = load_texture(DATADIR "/chip_14pin.png");
}

void Chip7473::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS73",mX+0.6,mY+0.6,0x5fffffff,0.75);
}

void Chip7473::update(float aTick) 
{
    int i;
    for (i = 0; i < 2; i++)
    {
        if (mInputPinJ[i].mNet == NULL ||
            mInputPinK[i].mNet == NULL ||
            mClearPin[i].mNet == NULL ||
            mClockPin[i].mNet == NULL ||
            mInputPinJ[i].mNet->mState == NETSTATE_INVALID ||
            mInputPinK[i].mNet->mState == NETSTATE_INVALID ||
            mClearPin[i].mNet == NULL ||
            mClockPin[i].mNet == NULL)
        {
            mOutputPinA[i].setState(gConfig.mPropagateInvalidState);
            mOutputPinB[i].setState(gConfig.mPropagateInvalidState);
        }
        else
        {
            if (mClearPin[i].mNet->mState == NETSTATE_LOW)
            {
                mOutputPinA[i].setState(PINSTATE_WRITE_LOW);
                mOutputPinB[i].setState(PINSTATE_WRITE_HIGH);
            }
            else
            {
                if (mClockPin[i].mNet->mState == NETSTATE_LOW)
                {
                    if (mClock[i] != 0)
                    {
                        mClock[i] = 0;
                        if (mInputPinJ[i].mNet->mState != NETSTATE_LOW &&
                            mInputPinK[i].mNet->mState != NETSTATE_LOW)
                        {
                            if (mOutputPinA[i].getState() != PINSTATE_WRITE_LOW)
                            {
                                mOutputPinA[i].setState(PINSTATE_WRITE_LOW);
                                mOutputPinB[i].setState(PINSTATE_WRITE_HIGH);
                            }
                            else
                            {
                                mOutputPinA[i].setState(PINSTATE_WRITE_HIGH);
                                mOutputPinB[i].setState(PINSTATE_WRITE_LOW);
                            }
                        }
                        else
                        if (mInputPinJ[i].mNet->mState == NETSTATE_LOW &&
                            mInputPinK[i].mNet->mState != NETSTATE_LOW)
                        {
                            mOutputPinA[i].setState(PINSTATE_WRITE_LOW);
                            mOutputPinB[i].setState(PINSTATE_WRITE_HIGH);
                        }
                        else
                        if (mInputPinJ[i].mNet->mState != NETSTATE_LOW &&
                            mInputPinK[i].mNet->mState == NETSTATE_LOW)
                        {
                            mOutputPinA[i].setState(PINSTATE_WRITE_HIGH);
                            mOutputPinB[i].setState(PINSTATE_WRITE_LOW);
                        }
                        else
                        {
                            // 4th state (low,low) is 'retain state'
                            // write the output to make sure we're not sending invalid signals.
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
                }
                else
                {
                    mClock[i] = 1;
                }
            }
        }
    }
}    
