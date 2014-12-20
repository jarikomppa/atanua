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
#include "chip74138.h"

Chip74138::Chip74138()
{
    set(0,0,4.54,2.25,"3 to 8-line Decoder/Demultiplexer");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos , 0, this, "Pin 15:DATA OUTPUT Y0"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos , 0, this, "Pin 14:DATA OUTPUT Y1"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos , 0, this, "Pin 13:DATA OUTPUT Y2"); xpos += 0.54;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(xpos , 0, this, "Pin 12:DATA OUTPUT Y3"); xpos += 0.54;
    mPin.push_back(&mOutputPin[4]);
    mOutputPin[4].set(xpos , 0, this, "Pin 11:DATA OUTPUT Y4"); xpos += 0.54;
    mPin.push_back(&mOutputPin[5]);
    mOutputPin[5].set(xpos , 0, this, "Pin 10:DATA OUTPUT Y5"); xpos += 0.54;
    mPin.push_back(&mOutputPin[6]);
    mOutputPin[6].set(xpos , 0, this, "Pin 9:DATA OUTPUT Y6"); xpos += 0.54;
    
    xpos = 0.15;
    mPin.push_back(&mInputPinA[0]);
    mInputPinA[0].set(xpos, 1.75, this, "Pin 1:SELECT A"); xpos += 0.54;
    mPin.push_back(&mInputPinA[1]);
    mInputPinA[1].set(xpos, 1.75, this, "Pin 2:SELECT B"); xpos += 0.54;
    mPin.push_back(&mInputPinA[2]);
    mInputPinA[2].set(xpos, 1.75, this, "Pin 3:SELECT C"); xpos += 0.54;

    mPin.push_back(&mInputPinB[0]);
    mInputPinB[0].set(xpos, 1.75, this, "Pin 4:ENABLE G2A"); xpos += 0.54;
    mPin.push_back(&mInputPinB[1]);
    mInputPinB[1].set(xpos, 1.75, this, "Pin 5:ENABLE G2B"); xpos += 0.54;
    mPin.push_back(&mInputPinB[2]);
    mInputPinB[2].set(xpos, 1.75, this, "Pin 6:ENABLE G1"); xpos += 0.54;

    mPin.push_back(&mOutputPin[7]);
    mOutputPin[7].set(xpos , 1.75, this, "Pin 7:DATA OUTPUT Y7"); xpos += 0.54;

    mTexture = load_texture("data/chip_16pin.png");
}

void Chip74138::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS138",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip74138::update(float aTick) 
{
    int i;
    if (mInputPinA[0].mNet == NULL ||
        mInputPinA[1].mNet == NULL ||
        mInputPinA[2].mNet == NULL ||
        mInputPinB[0].mNet == NULL ||
        mInputPinB[1].mNet == NULL ||
        mInputPinB[2].mNet == NULL ||
        mInputPinA[0].mNet->mState == NETSTATE_INVALID ||
        mInputPinA[1].mNet->mState == NETSTATE_INVALID ||
        mInputPinA[2].mNet->mState == NETSTATE_INVALID ||
        mInputPinB[0].mNet->mState == NETSTATE_INVALID ||
        mInputPinB[1].mNet->mState == NETSTATE_INVALID ||
        mInputPinB[2].mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 8; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    for (i = 0; i < 8; i++)
        mOutputPin[i].setState(PINSTATE_WRITE_HIGH);

    if (mInputPinB[0].mNet->mState == NETSTATE_HIGH ||
        mInputPinB[1].mNet->mState == NETSTATE_HIGH ||
        mInputPinB[2].mNet->mState == NETSTATE_LOW)
    {
        return;
    }

    int v = ((mInputPinA[2].mNet->mState == NETSTATE_HIGH)?(1 << 2):0) |
            ((mInputPinA[1].mNet->mState == NETSTATE_HIGH)?(1 << 1):0) |
            ((mInputPinA[0].mNet->mState == NETSTATE_HIGH)?(1 << 0):0);
    
    mOutputPin[v].setState(PINSTATE_WRITE_LOW);
}    
