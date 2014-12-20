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
#include "chip7404.h"

Chip7404::Chip7404()
{
    set(0,0,4,2,"Hex Inverter");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(xpos , 0, this, "Pin 13:A6"); xpos += 0.54;
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos, 0, this, "Pin 12:Y6"); xpos += 0.54;

    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(xpos , 0, this, "Pin 11:A5"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos, 0, this, "Pin 10:Y5"); xpos += 0.54;

    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(xpos , 0, this, "Pin 9:A4"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos, 0, this, "Pin 8:Y4"); xpos += 0.54;


    xpos = 0.15;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(xpos , 1.5, this, "Pin 1:A1"); xpos += 0.54;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(xpos, 1.5, this, "Pin 2:Y1"); xpos += 0.54;

    mPin.push_back(&mInputPin[4]);
    mInputPin[4].set(xpos , 1.5, this, "Pin 3:A2"); xpos += 0.54;
    mPin.push_back(&mOutputPin[4]);
    mOutputPin[4].set(xpos, 1.5, this, "Pin 4:Y2"); xpos += 0.54;

    mPin.push_back(&mInputPin[5]);
    mInputPin[5].set(xpos , 1.5, this, "Pin 5:A3"); xpos += 0.54;
    mPin.push_back(&mOutputPin[5]);
    mOutputPin[5].set(xpos, 1.5, this, "Pin 6:Y3"); xpos += 0.54;

    mTexture = load_texture("data/chip_14pin.png");
}

void Chip7404::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS04",mX+0.6,mY+0.6,0x5fffffff,0.75);
}

void Chip7404::update(float aTick) 
{
    int i;
    for (i = 0; i < 6; i++)
    {
        if (mInputPin[i].mNet == NULL ||
            mInputPin[i].mNet->mState == NETSTATE_INVALID)
        {
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        }
        else
        if ((mInputPin[i].mNet->mState == NETSTATE_HIGH || mInputPin[i].mNet->mState == NETSTATE_NC))
            mOutputPin[i].setState(PINSTATE_WRITE_LOW);
        else
            mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
    }
}    
