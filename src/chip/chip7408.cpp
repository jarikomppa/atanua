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
#include "chip7408.h"

Chip7408::Chip7408()
{
    set(0,0,4,2,"Quad 2-input AND Gate");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mInputPinA[0]);
    mInputPinA[0].set(xpos, 0, this, "Pin 13:B4"); xpos += 0.54;
    mPin.push_back(&mInputPinB[0]);
    mInputPinB[0].set(xpos, 0, this, "Pin 12:A4"); xpos += 0.54;
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos , 0, this, "Pin 11:Y4"); xpos += 0.54;

    mPin.push_back(&mInputPinA[1]);
    mInputPinA[1].set(xpos, 0, this, "Pin 10:B3"); xpos += 0.54;
    mPin.push_back(&mInputPinB[1]);
    mInputPinB[1].set(xpos, 0, this, "Pin 9:A3"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos, 0, this, "Pin 8:Y3"); xpos += 0.54;

    xpos = 0.15;
    mPin.push_back(&mInputPinA[2]);
    mInputPinA[2].set(xpos, 1.5, this, "Pin 1:A1"); xpos += 0.54;
    mPin.push_back(&mInputPinB[2]);
    mInputPinB[2].set(xpos, 1.5, this, "Pin 2:B1"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos , 1.5, this, "Pin 3:Y1"); xpos += 0.54;

    mPin.push_back(&mInputPinA[3]);
    mInputPinA[3].set(xpos, 1.5, this, "Pin 4:A2"); xpos += 0.54;
    mPin.push_back(&mInputPinB[3]);
    mInputPinB[3].set(xpos, 1.5, this, "Pin 5:B2"); xpos += 0.54;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(xpos, 1.5, this, "Pin 6:Y2"); xpos += 0.54;

    mTexture = load_texture(DATADIR "/chip_14pin.png");
}

void Chip7408::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS08",mX+0.6,mY+0.6,0x5fffffff,0.75);
}

void Chip7408::update(float aTick) 
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if (mInputPinA[i].mNet == NULL ||
            mInputPinB[i].mNet == NULL ||
            mInputPinA[i].mNet->mState == NETSTATE_INVALID ||
            mInputPinB[i].mNet->mState == NETSTATE_INVALID)
        {
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        }
        else
        if ((mInputPinA[i].mNet->mState == NETSTATE_HIGH || mInputPinA[i].mNet->mState == NETSTATE_NC) &&
            (mInputPinB[i].mNet->mState == NETSTATE_HIGH || mInputPinB[i].mNet->mState == NETSTATE_NC))
            mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
        else
            mOutputPin[i].setState(PINSTATE_WRITE_LOW);
    }
}    
