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
#include "chip74151.h"

Chip74151::Chip74151()
{
    set(0,0,4.54,2.25,"8-Line to 1-Line\nData Selector/Multiplexer");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mInputPin[4]); 
    mInputPin[4].set(xpos , 0, this, "Pin 15:DATA INPUT D4"); xpos += 0.54;
    mPin.push_back(&mInputPin[5]);                     
    mInputPin[5].set(xpos , 0, this, "Pin 14:DATA INPUT D5"); xpos += 0.54;
    mPin.push_back(&mInputPin[6]);                     
    mInputPin[6].set(xpos , 0, this, "Pin 13:DATA INPUT D6"); xpos += 0.54;
    mPin.push_back(&mInputPin[7]);                     
    mInputPin[7].set(xpos , 0, this, "Pin 12:DATA INPUT D7"); xpos += 0.54;

    mPin.push_back(&mInputPinG[0]); 
    mInputPinG[0].set(xpos , 0, this, "Pin 11:DATA SELECT A"); xpos += 0.54;
    mPin.push_back(&mInputPinG[1]);                       
    mInputPinG[1].set(xpos , 0, this, "Pin 10:DATA SELECT B"); xpos += 0.54;
    mPin.push_back(&mInputPinG[2]);                       
    mInputPinG[2].set(xpos , 0, this, "Pin 9:DATA SELECT C"); xpos += 0.54;
    
    xpos = 0.15;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(xpos, 1.75, this, "Pin 1:DATA INPUT D3"); xpos += 0.54;
    mPin.push_back(&mInputPin[2]);                      
    mInputPin[2].set(xpos, 1.75, this, "Pin 2:DATA INPUT D2"); xpos += 0.54;
    mPin.push_back(&mInputPin[1]);                      
    mInputPin[1].set(xpos, 1.75, this, "Pin 3:DATA INPUT D1"); xpos += 0.54;
    mPin.push_back(&mInputPin[0]);                      
    mInputPin[0].set(xpos, 1.75, this, "Pin 4:DATA INPUT D0"); xpos += 0.54;

    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos, 1.75, this, "Pin 5:OUTPUT Y"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1]);                   
    mOutputPin[1].set(xpos, 1.75, this, "Pin 6:OUTPUT W"); xpos += 0.54;

    mPin.push_back(&mStrobePin);
    mStrobePin.set(xpos, 1.75, this, "Pin 7:STROBE"); xpos += 0.54;

    mTexture = load_texture("data/chip_16pin.png");
}

void Chip74151::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS151",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip74151::update(float aTick) 
{
    int i;
    if (mInputPin[0].mNet == NULL ||
        mInputPin[1].mNet == NULL ||
        mInputPin[2].mNet == NULL ||
        mInputPin[3].mNet == NULL ||
        mInputPin[4].mNet == NULL ||
        mInputPin[5].mNet == NULL ||
        mInputPin[6].mNet == NULL ||
        mInputPin[7].mNet == NULL ||
        mInputPinG[0].mNet == NULL ||
        mInputPinG[1].mNet == NULL ||
        mInputPinG[2].mNet == NULL ||
        mStrobePin.mNet == NULL ||
        mInputPin[0].mNet->mState == NETSTATE_INVALID ||
        mInputPin[1].mNet->mState == NETSTATE_INVALID ||
        mInputPin[2].mNet->mState == NETSTATE_INVALID ||
        mInputPin[3].mNet->mState == NETSTATE_INVALID ||
        mInputPin[4].mNet->mState == NETSTATE_INVALID ||
        mInputPin[5].mNet->mState == NETSTATE_INVALID ||
        mInputPin[6].mNet->mState == NETSTATE_INVALID ||
        mInputPin[7].mNet->mState == NETSTATE_INVALID ||
        mInputPinG[0].mNet->mState == NETSTATE_INVALID ||
        mInputPinG[1].mNet->mState == NETSTATE_INVALID ||
        mInputPinG[2].mNet->mState == NETSTATE_INVALID ||
        mStrobePin.mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 2; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    int v = ((mInputPinG[2].mNet->mState == NETSTATE_HIGH)?(1 << 2):0) |
            ((mInputPinG[1].mNet->mState == NETSTATE_HIGH)?(1 << 1):0) |
            ((mInputPinG[0].mNet->mState == NETSTATE_HIGH)?(1 << 0):0);

    if (mStrobePin.mNet->mState == NETSTATE_HIGH || mInputPin[v].mNet->mState == NETSTATE_LOW)
    {
        mOutputPin[0].setState(PINSTATE_WRITE_LOW);
        mOutputPin[1].setState(PINSTATE_WRITE_HIGH);
    }
    else
    {
        mOutputPin[0].setState(PINSTATE_WRITE_HIGH);
        mOutputPin[1].setState(PINSTATE_WRITE_LOW);
    }
}    
