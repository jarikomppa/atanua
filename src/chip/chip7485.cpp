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
#include "chip7485.h"

Chip7485::Chip7485()
{
    set(0,0,4.54,2.25,"4-bit Magnitude Comparator");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mInputPinA[3]);
    mInputPinA[3].set(xpos , 0, this, "Pin 15:A3"); xpos += 0.54;
    mPin.push_back(&mInputPinB[2]);
    mInputPinB[2].set(xpos , 0, this, "Pin 14:B3"); xpos += 0.54;
    mPin.push_back(&mInputPinA[2]);
    mInputPinA[2].set(xpos , 0, this, "Pin 13:A2"); xpos += 0.54;
    mPin.push_back(&mInputPinA[1]);
    mInputPinA[1].set(xpos , 0, this, "Pin 12:A1"); xpos += 0.54;
    mPin.push_back(&mInputPinB[1]);
    mInputPinB[1].set(xpos , 0, this, "Pin 11:B1"); xpos += 0.54;
    mPin.push_back(&mInputPinA[0]);
    mInputPinA[0].set(xpos , 0, this, "Pin 10:A0"); xpos += 0.54;
    mPin.push_back(&mInputPinB[0]);
    mInputPinB[0].set(xpos , 0, this, "Pin 9:B0"); xpos += 0.54;
    
    xpos = 0.15;

    mPin.push_back(&mInputPinB[3]);
    mInputPinB[3].set(xpos , 1.75, this, "Pin 1:B3"); xpos += 0.54;
    mPin.push_back(&mCascadingInputPin[0]);
    mCascadingInputPin[0].set(xpos , 1.75, this, "Pin 2:A<B cascading input"); xpos += 0.54;
    mPin.push_back(&mCascadingInputPin[1]);
    mCascadingInputPin[1].set(xpos , 1.75, this, "Pin 3:A=B cascading input"); xpos += 0.54;
    mPin.push_back(&mCascadingInputPin[2]);
    mCascadingInputPin[2].set(xpos , 1.75, this, "Pin 4:A>B cascading input"); xpos += 0.54;
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos , 1.75, this, "Pin 5:A>B output"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos , 1.75, this, "Pin 1:A=B output"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos , 1.75, this, "Pin 1:A<B output"); xpos += 0.54;


    mTexture = load_texture("data/chip_16pin.png");
}

void Chip7485::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS85",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip7485::update(float aTick) 
{
    int i;
    if (mInputPinA[0].mNet == NULL ||
        mInputPinA[1].mNet == NULL ||
        mInputPinA[2].mNet == NULL ||
        mInputPinA[3].mNet == NULL ||
        mInputPinB[0].mNet == NULL ||
        mInputPinB[1].mNet == NULL ||
        mInputPinB[2].mNet == NULL ||
        mInputPinB[3].mNet == NULL ||
        mCascadingInputPin[0].mNet == NULL || 
        mCascadingInputPin[1].mNet == NULL || 
        mCascadingInputPin[2].mNet == NULL || 
        mInputPinA[0].mNet->mState == NETSTATE_INVALID ||
        mInputPinA[1].mNet->mState == NETSTATE_INVALID ||
        mInputPinA[2].mNet->mState == NETSTATE_INVALID ||
        mInputPinA[3].mNet->mState == NETSTATE_INVALID ||
        mInputPinB[0].mNet->mState == NETSTATE_INVALID ||
        mInputPinB[1].mNet->mState == NETSTATE_INVALID ||
        mInputPinB[2].mNet->mState == NETSTATE_INVALID ||
        mInputPinB[3].mNet->mState == NETSTATE_INVALID ||
        mCascadingInputPin[0].mNet->mState == NETSTATE_INVALID ||
        mCascadingInputPin[1].mNet->mState == NETSTATE_INVALID ||
        mCascadingInputPin[2].mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 3; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    for (i = 3; i >=0; i--)
    {
        if (mInputPinA[i].mNet->mState != NETSTATE_LOW &&
            mInputPinB[i].mNet->mState == NETSTATE_LOW)
        {
            mOutputPin[0].setState(PINSTATE_WRITE_HIGH); // >
            mOutputPin[2].setState(PINSTATE_WRITE_LOW);  // <
            mOutputPin[1].setState(PINSTATE_WRITE_LOW);  // =
            return;
        }
        
        if (mInputPinA[i].mNet->mState == NETSTATE_LOW &&
            mInputPinB[i].mNet->mState != NETSTATE_LOW)
        {
            mOutputPin[0].setState(PINSTATE_WRITE_LOW);  // >
            mOutputPin[2].setState(PINSTATE_WRITE_HIGH); // <
            mOutputPin[1].setState(PINSTATE_WRITE_LOW);  // =
            return;
        }
    }
    
    if (mCascadingInputPin[1].mNet->mState != NETSTATE_LOW)
    {
        mOutputPin[0].setState(PINSTATE_WRITE_LOW);  // >
        mOutputPin[2].setState(PINSTATE_WRITE_LOW);  // <
        mOutputPin[1].setState(PINSTATE_WRITE_HIGH); // =
        return;
    }

    mOutputPin[1].setState(PINSTATE_WRITE_LOW); // =

    if (mCascadingInputPin[0].mNet->mState != NETSTATE_LOW &&
        mCascadingInputPin[2].mNet->mState != NETSTATE_LOW)
    {
        mOutputPin[0].setState(PINSTATE_WRITE_LOW);  // >
        mOutputPin[2].setState(PINSTATE_WRITE_LOW);  // <
        return;
    }

    if (mCascadingInputPin[0].mNet->mState == NETSTATE_LOW &&
        mCascadingInputPin[2].mNet->mState == NETSTATE_LOW)
    {
        mOutputPin[0].setState(PINSTATE_WRITE_HIGH);  // >
        mOutputPin[2].setState(PINSTATE_WRITE_HIGH);  // <
        return;
    }

    if (mCascadingInputPin[0].mNet->mState != NETSTATE_LOW)
    {
        mOutputPin[2].setState(PINSTATE_WRITE_HIGH);  // <
    }
    else
    {
        mOutputPin[2].setState(PINSTATE_WRITE_LOW);   // <
    }

    if (mCascadingInputPin[2].mNet->mState != NETSTATE_LOW)
    {
        mOutputPin[0].setState(PINSTATE_WRITE_HIGH);  // >
    }
    else
    {
        mOutputPin[0].setState(PINSTATE_WRITE_LOW);   // >
    }

}    
