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
#include "chip74574.h"

Chip74574::Chip74574()
{
    set(0,0,5.7,2.8,"Octal D-Type Edge-Triggered Flip-Flop with Three-State Outputs");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos, 0.15, this, "Pin 19:1Q"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos, 0.15, this, "Pin 18:2Q"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos, 0.15, this, "Pin 17:3Q"); xpos += 0.54;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(xpos, 0.15, this, "Pin 16:4Q"); xpos += 0.54;
    mPin.push_back(&mOutputPin[4]);
    mOutputPin[4].set(xpos, 0.15, this, "Pin 15:5Q"); xpos += 0.54;
    mPin.push_back(&mOutputPin[5]);
    mOutputPin[5].set(xpos, 0.15, this, "Pin 14:6Q"); xpos += 0.54;
    mPin.push_back(&mOutputPin[6]);
    mOutputPin[6].set(xpos, 0.15, this, "Pin 13:7Q"); xpos += 0.54;
    mPin.push_back(&mOutputPin[7]);
    mOutputPin[7].set(xpos, 0.15, this, "Pin 12:8Q"); xpos += 0.54;
    mPin.push_back(&mClockPin);
    mClockPin.set(xpos, 0.15, this, "Pin 11:Clock"); xpos += 0.54;

    xpos = 0.15;

    mPin.push_back(&mOutputControlPin);
    mOutputControlPin.set(xpos, 2.1, this, "Pin 1:Output control"); xpos += 0.54;
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(xpos, 2.1, this, "Pin 2:1D"); xpos += 0.54;
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(xpos, 2.1, this, "Pin 2:2D"); xpos += 0.54;
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(xpos, 2.1, this, "Pin 3:3D"); xpos += 0.54;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(xpos, 2.1, this, "Pin 4:4D"); xpos += 0.54;
    mPin.push_back(&mInputPin[4]);
    mInputPin[4].set(xpos, 2.1, this, "Pin 5:5D"); xpos += 0.54;
    mPin.push_back(&mInputPin[5]);
    mInputPin[5].set(xpos, 2.1, this, "Pin 6:6D"); xpos += 0.54;
    mPin.push_back(&mInputPin[6]);
    mInputPin[6].set(xpos, 2.1, this, "Pin 7:7D"); xpos += 0.54;
    mPin.push_back(&mInputPin[7]);
    mInputPin[7].set(xpos, 2.1, this, "Pin 8:8D"); xpos += 0.54;
    
    mTexture = load_texture(DATADIR "/chip_20pin.png");
    
    mState = 0;
    mClock = 0;
}

void Chip74574::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74HC574",mX+0.6,mY+1,0x5fffffff,0.75);
}

void Chip74574::update(float aTick) 
{
    if (mClockPin.mNet == NULL ||
        mOutputControlPin.mNet == NULL ||
        mInputPin[0].mNet == NULL ||
        mInputPin[1].mNet == NULL ||
        mInputPin[2].mNet == NULL ||
        mInputPin[3].mNet == NULL ||
        mInputPin[4].mNet == NULL ||
        mInputPin[5].mNet == NULL ||
        mInputPin[6].mNet == NULL ||
        mInputPin[7].mNet == NULL ||
        mClockPin.mNet->mState == NETSTATE_INVALID ||
        mOutputControlPin.mNet->mState == NETSTATE_INVALID ||
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

    if (mClockPin.mNet->mState != NETSTATE_LOW)
    {
        if (mClock == 0)
        {
            mClock = 1;
            mState = 0;
            int i;
            for (i = 0; i < 8; i++)
                if (mInputPin[i].mNet->mState != NETSTATE_LOW)
                    mState |= (1 << i);                    
        }
    }
    else
    {
        mClock = 0;
    }

    if (mOutputControlPin.mNet->mState != NETSTATE_LOW)
    {
        int i;
        for (i = 0; i < 8; i++)
            mOutputPin[i].setState(PINSTATE_HIGHZ);
        return;
    }

    int i;
    for (i = 0; i < 8; i++)
    {
        if (mState & (1 << i))
        {
            mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
        }
        else
        {
            mOutputPin[i].setState(PINSTATE_WRITE_LOW);
        }
    }
}    
