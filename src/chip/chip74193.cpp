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
#include "chip74193.h"

Chip74193::Chip74193()
{
    set(0,0,4.54,2.25,"Synchronous up/down binary counter");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(xpos , 0, this, "Pin 15:INPUT DATA A"); xpos += 0.54;
    mPin.push_back(&mClear);
    mClear.set(xpos , 0, this, "Pin 14:CLEAR"); xpos += 0.54;
    mPin.push_back(&mBorrow);
    mBorrow.set(xpos , 0, this, "Pin 13:BORROW"); xpos += 0.54;
    mPin.push_back(&mCarry);
    mCarry.set(xpos , 0, this, "Pin 12:CARRY"); xpos += 0.54;
    mPin.push_back(&mLoad);
    mLoad.set(xpos , 0, this, "Pin 11:LOAD"); xpos += 0.54;
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(xpos , 0, this, "Pin 10:INPUT DATA C"); xpos += 0.54;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(xpos , 0, this, "Pin 9:INPUT DATA D"); xpos += 0.54;
    
    xpos = 0.15;
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(xpos, 1.75, this, "Pin 1:INPUT DATA B"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos, 1.75, this, "Pin 2:QB"); xpos += 0.54;
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos, 1.75, this, "Pin 3:QA"); xpos += 0.54;
    mPin.push_back(&mCountDown);
    mCountDown.set(xpos, 1.75, this, "Pin 4:COUNT DOWN"); xpos += 0.54;
    mPin.push_back(&mCountUp);
    mCountUp.set(xpos, 1.75, this, "Pin 5:COUNT UP"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos, 1.75, this, "Pin 6:QC"); xpos += 0.54;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(xpos, 1.75, this, "Pin 7:QD"); xpos += 0.54;

    mTexture = load_texture("data/chip_16pin.png");

    mValue = 0;
    mCountDownLastState = 0;
    mCountUpLastState = 0;
}

void Chip74193::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS193",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip74193::update(float aTick) 
{
    int i;

    if (
        (mInputPin[0].mNet != NULL && mInputPin[0].mNet->mState == NETSTATE_INVALID) ||
        (mInputPin[1].mNet != NULL && mInputPin[1].mNet->mState == NETSTATE_INVALID) ||
        (mInputPin[2].mNet != NULL && mInputPin[2].mNet->mState == NETSTATE_INVALID) ||
        (mInputPin[3].mNet != NULL && mInputPin[3].mNet->mState == NETSTATE_INVALID) ||
        (mCountDown.mNet != NULL && mCountDown.mNet->mState == NETSTATE_INVALID) ||
        (mCountUp.mNet != NULL && mCountUp.mNet->mState == NETSTATE_INVALID) ||
        (mLoad.mNet != NULL && mLoad.mNet->mState == NETSTATE_INVALID) ||
        (mClear.mNet != NULL && mClear.mNet->mState == NETSTATE_INVALID) ||
        (mLoad.mNet != NULL && mLoad.mNet->mState == NETSTATE_LOW && 
         (mInputPin[0].mNet == NULL ||
          mInputPin[1].mNet == NULL ||
          mInputPin[2].mNet == NULL ||
          mInputPin[3].mNet == NULL))
        )
    {
        for (i = 0; i < 4; i++)        
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        mBorrow.setState(gConfig.mPropagateInvalidState);
        mCarry.setState(gConfig.mPropagateInvalidState);
        
        return;
    }

    if (mLoad.mNet != NULL &&
        mLoad.mNet->mState == NETSTATE_LOW)
    {
        mValue = ((mInputPin[3].mNet->mState != NETSTATE_LOW)?(1 << 3):0) |
                 ((mInputPin[2].mNet->mState != NETSTATE_LOW)?(1 << 2):0) |
                 ((mInputPin[1].mNet->mState != NETSTATE_LOW)?(1 << 1):0) |
                 ((mInputPin[0].mNet->mState != NETSTATE_LOW)?(1 << 0):0);
    }
    else
    if (mClear.mNet != NULL &&
        mClear.mNet->mState == NETSTATE_HIGH)
    {
        mValue = 0;        
    }
    else
    {
        if (mCountDownLastState == NETSTATE_LOW &&
            mCountDown.mNet != NULL &&
            mCountDown.mNet->mState == NETSTATE_HIGH)
        {
            // Rising edge
            mValue--;
        }
        if (mCountUpLastState == NETSTATE_LOW &&
            mCountUp.mNet != NULL &&
            mCountUp.mNet->mState == NETSTATE_HIGH)
        {
            // Rising edge
            mValue++;
        }
    }
    mCarry.setState(PINSTATE_WRITE_HIGH);
    mBorrow.setState(PINSTATE_WRITE_HIGH);

    if (mValue < 0)
    {
        mBorrow.setState(PINSTATE_WRITE_LOW);
        mValue = 15;
    }

    if (mValue > 15)
    {
        mCarry.setState(PINSTATE_WRITE_LOW);
        mValue = 0;
    }

    mOutputPin[0].setState((mValue & (1 << 0)) ? PINSTATE_WRITE_HIGH : PINSTATE_WRITE_LOW);
    mOutputPin[1].setState((mValue & (1 << 1)) ? PINSTATE_WRITE_HIGH : PINSTATE_WRITE_LOW);
    mOutputPin[2].setState((mValue & (1 << 2)) ? PINSTATE_WRITE_HIGH : PINSTATE_WRITE_LOW);
    mOutputPin[3].setState((mValue & (1 << 3)) ? PINSTATE_WRITE_HIGH : PINSTATE_WRITE_LOW);

    mCountDownLastState = NETSTATE_LOW;
    if (mCountDown.mNet != NULL &&
        mCountDown.mNet->mState == NETSTATE_HIGH)
    {
        mCountDownLastState = NETSTATE_HIGH;
    }

    mCountUpLastState = NETSTATE_LOW;
    if (mCountUp.mNet != NULL &&
        mCountUp.mNet->mState == NETSTATE_HIGH)
    {
        mCountUpLastState = NETSTATE_HIGH;
    }

}    
