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
#include "chip74241.h"

Chip74241::Chip74241()
{
    set(0,0,5.7,2.8,"Octal Buffer with Noninverted Three-State Outputs");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mOutputControlPin[1]);
    mOutputControlPin[1].set(xpos, 0.15, this, "Pin 19:2G"); xpos += 0.54;    
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos, 0.15, this, "Pin 18:1Y1"); xpos += 0.54;
    mPin.push_back(&mInputPin[4]);
    mInputPin[4].set(xpos, 0.15, this,  "Pin 17:2A4"); xpos += 0.54;    
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos, 0.15, this, "Pin 16:1Y2"); xpos += 0.54;    
    mPin.push_back(&mInputPin[5]);
    mInputPin[5].set(xpos, 0.15, this,  "Pin 15:2A3"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos, 0.15, this, "Pin 14:1Y3"); xpos += 0.54;
    mPin.push_back(&mInputPin[6]);
    mInputPin[6].set(xpos, 0.15, this,  "Pin 13:2A2"); xpos += 0.54;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(xpos, 0.15, this, "Pin 12:1Y4"); xpos += 0.54;
    mPin.push_back(&mInputPin[7]);
    mInputPin[7].set(xpos, 0.15, this,  "Pin 11:2A1"); xpos += 0.54;

    xpos = 0.15;

    mPin.push_back(&mOutputControlPin[0]);
    mOutputControlPin[0].set(xpos, 2.1, this, "Pin 1:1G"); xpos += 0.54;
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(xpos, 2.1, this,  "Pin 2:1A1"); xpos += 0.54;
    mPin.push_back(&mOutputPin[4]);
    mOutputPin[4].set(xpos, 2.1, this, "Pin 3:2Y4"); xpos += 0.54;
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(xpos, 2.1, this,  "Pin 4:1A2"); xpos += 0.54;
    mPin.push_back(&mOutputPin[5]);
    mOutputPin[5].set(xpos, 2.1, this, "Pin 5:2Y3"); xpos += 0.54;
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(xpos, 2.1, this,  "Pin 6:1A3"); xpos += 0.54;
    mPin.push_back(&mOutputPin[6]);
    mOutputPin[6].set(xpos, 2.1, this, "Pin 7:2Y2"); xpos += 0.54;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(xpos, 2.1, this,  "Pin 8:1A4"); xpos += 0.54;
    mPin.push_back(&mOutputPin[7]);
    mOutputPin[7].set(xpos, 2.1, this, "Pin 9:2Y1"); xpos += 0.54;
    
    mTexture = load_texture(DATADIR "/chip_20pin.png");    
}

void Chip74241::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS241",mX+0.6,mY+1,0x5fffffff,0.75);
}

void Chip74241::update(float aTick) 
{
    if (mOutputControlPin[0].mNet == NULL ||
        mOutputControlPin[1].mNet == NULL ||
        mInputPin[0].mNet == NULL ||
        mInputPin[1].mNet == NULL ||
        mInputPin[2].mNet == NULL ||
        mInputPin[3].mNet == NULL ||
        mInputPin[4].mNet == NULL ||
        mInputPin[5].mNet == NULL ||
        mInputPin[6].mNet == NULL ||
        mInputPin[7].mNet == NULL ||
        mOutputControlPin[0].mNet->mState == NETSTATE_INVALID ||
        mOutputControlPin[1].mNet->mState == NETSTATE_INVALID ||
        mInputPin[0].mNet->mState == NETSTATE_INVALID ||
        mInputPin[1].mNet->mState == NETSTATE_INVALID ||
        mInputPin[2].mNet->mState == NETSTATE_INVALID ||
        mInputPin[3].mNet->mState == NETSTATE_INVALID ||
        mInputPin[4].mNet->mState == NETSTATE_INVALID ||
        mInputPin[5].mNet->mState == NETSTATE_INVALID ||
        mInputPin[6].mNet->mState == NETSTATE_INVALID ||
        mInputPin[7].mNet->mState == NETSTATE_INVALID)
    {
        int i;
        for (i = 0; i < 8; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    int i;
    for (i = 0; i < 8; i++)
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

    if (mOutputControlPin[0].mNet->mState != NETSTATE_LOW)
    {
        for (i = 0; i < 4; i++)
            mOutputPin[i].setState(PINSTATE_HIGHZ);
    }
    if (mOutputControlPin[1].mNet->mState == NETSTATE_LOW)
    {
        for (i = 0; i < 4; i++)
            mOutputPin[i+4].setState(PINSTATE_HIGHZ);
    }
}    
