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
#include "chip74191.h"

Chip74191::Chip74191()
{
    set(0,0,4.54,2.25,"Synchronous Up/Down Binary Counter");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(xpos, 0, this, "Pin 15:Data A input"); xpos += 0.54;
    mPin.push_back(&mClockPin);
    mClockPin.set(xpos, 0, this, "Pin 14:Clock input"); xpos += 0.54;
    mPin.push_back(&mRippleClockPin);
    mRippleClockPin.set(xpos, 0, this, "Pin 13:Ripple clock output"); xpos += 0.54;
    mPin.push_back(&mMaxMinPin); 
    mMaxMinPin.set(xpos, 0, this, "Pin 12:Max/min output"); xpos += 0.54;
    mPin.push_back(&mLoadPin);
    mLoadPin.set(xpos, 0, this, "Pin 11:Load input"); xpos += 0.54;
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(xpos, 0, this, "Pin 10:Data C input"); xpos += 0.54;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(xpos, 0, this, "Pin 9:Data D input"); xpos += 0.54;
    
    xpos = 0.15;

    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(xpos, 1.75, this, "Pin 1:Data B input"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos, 1.75, this, "Pin 2:QB output"); xpos += 0.54;
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos, 1.75, this, "Pin 3:QA output"); xpos += 0.54;
    mPin.push_back(&mEnablePin);
    mEnablePin.set(xpos, 1.75, this, "Pin 4:Enable G input"); xpos += 0.54;
    mPin.push_back(&mUpDownPin);
    mUpDownPin.set(xpos, 1.75, this, "Pin 5:Down/up input"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos, 1.75, this, "Pin 6:QC output"); xpos += 0.54;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(xpos, 1.75, this, "Pin 7:QD output"); xpos += 0.54;

    mClock = 0;
    mTexture = load_texture("data/chip_16pin.png");
}

void Chip74191::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS191",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip74191::update(float aTick) 
{
    int i;
    if (mInputPin[0].mNet == NULL ||
        mInputPin[1].mNet == NULL ||
        mInputPin[2].mNet == NULL ||
        mInputPin[3].mNet == NULL ||
        mLoadPin.mNet == NULL ||
        mClockPin.mNet == NULL ||
        mEnablePin.mNet == NULL ||
        mUpDownPin.mNet == NULL ||
        mInputPin[0].mNet->mState == NETSTATE_INVALID ||
        mInputPin[1].mNet->mState == NETSTATE_INVALID ||
        mInputPin[2].mNet->mState == NETSTATE_INVALID ||
        mInputPin[3].mNet->mState == NETSTATE_INVALID ||
        mLoadPin.mNet->mState == NETSTATE_INVALID ||
        mClockPin.mNet->mState == NETSTATE_INVALID ||
        mEnablePin.mNet->mState == NETSTATE_INVALID ||
        mUpDownPin.mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 4; i++)
        mRippleClockPin.setState(gConfig.mPropagateInvalidState);
        mMaxMinPin.setState(gConfig.mPropagateInvalidState);
        return;
    }

    // only act if enable is low
    if (mEnablePin.mNet->mState == NETSTATE_LOW)
    {
        // Data sheet a bit unclear, but let's assume load overrides other actions, and is asynch.
        if (mLoadPin.mNet->mState == NETSTATE_LOW)
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
            // timing diagram looks like rising edge action.
            if (mClockPin.mNet->mState != NETSTATE_LOW)
            {               
                if (mClock == 1)
                {
                    mClock = 0;

                    // Record current state
                    int v = ((mOutputPin[3].getState() != PINSTATE_WRITE_LOW)?(1 << 3):0) |
                            ((mOutputPin[2].getState() != PINSTATE_WRITE_LOW)?(1 << 2):0) |
                            ((mOutputPin[1].getState() != PINSTATE_WRITE_LOW)?(1 << 1):0) |
                            ((mOutputPin[0].getState() != PINSTATE_WRITE_LOW)?(1 << 0):0);
                    
                    if (mUpDownPin.mNet->mState != NETSTATE_LOW)
                    {
                        v++;
                        if (v == 0xf)
                        {
                            mMaxMinPin.setState(PINSTATE_WRITE_HIGH);
                        }
                        else
                        {
                            mMaxMinPin.setState(PINSTATE_WRITE_LOW);
                        }
                    }
                    else
                    {
                        v--;
                        if (v == 0)
                        {
                            mMaxMinPin.setState(PINSTATE_WRITE_HIGH);
                        }
                        else
                        {
                            mMaxMinPin.setState(PINSTATE_WRITE_LOW);
                        }
                    }

                    // Check if we rolled over, if so, pulse the ripple lock.
                    if (v & 0x10)
                    {
                        mRippleClockPin.setState(PINSTATE_WRITE_LOW);
                    }
                    else
                    {
                        mRippleClockPin.setState(PINSTATE_WRITE_HIGH);
                    }

                    // Finally, write out the output pin state.

                    for (i = 0; i < 4; i++)
                    {
                        if (v & (1 << i))
                        {
                            mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
                        }
                        else
                        {
                            mOutputPin[i].setState(PINSTATE_WRITE_LOW);
                        }
                    }
                }
            }
            else
            {
                mClock = 1;
            }                     
        }
    }
}    
