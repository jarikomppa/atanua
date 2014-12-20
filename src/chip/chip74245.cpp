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
#include "chip74245.h"

Chip74245::Chip74245()
{
    set(0,0,5.7,2.8,"Octal Bus Transceiver with Noninverted Three-State Outputs");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mEnablePin);
    mEnablePin.set(xpos, 0.15, this, "Pin 19:Enable G"); xpos += 0.54;    
    mPin.push_back(&mBusBPin[0]);
    mBusBPin[0].set(xpos, 0.15, this, "Pin 18:B1"); xpos += 0.54;
    mPin.push_back(&mBusBPin[1]);
    mBusBPin[1].set(xpos, 0.15, this, "Pin 17:B2"); xpos += 0.54;
    mPin.push_back(&mBusBPin[2]);
    mBusBPin[2].set(xpos, 0.15, this, "Pin 16:B3"); xpos += 0.54;
    mPin.push_back(&mBusBPin[3]);
    mBusBPin[3].set(xpos, 0.15, this, "Pin 15:B4"); xpos += 0.54;
    mPin.push_back(&mBusBPin[4]);
    mBusBPin[4].set(xpos, 0.15, this, "Pin 14:B5"); xpos += 0.54;
    mPin.push_back(&mBusBPin[5]);
    mBusBPin[5].set(xpos, 0.15, this, "Pin 13:B6"); xpos += 0.54;
    mPin.push_back(&mBusBPin[6]);
    mBusBPin[6].set(xpos, 0.15, this, "Pin 12:B7"); xpos += 0.54;
    mPin.push_back(&mBusBPin[7]);
    mBusBPin[7].set(xpos, 0.15, this, "Pin 11:B8"); xpos += 0.54;

    xpos = 0.15;

    mPin.push_back(&mDirectionPin);
    mDirectionPin.set(xpos, 2.1, this, "Pin 1:DIR"); xpos += 0.54;
    mPin.push_back(&mBusAPin[0]);
    mBusAPin[0].set(xpos, 2.1, this, "Pin 2:A1"); xpos += 0.54;
    mPin.push_back(&mBusAPin[1]);
    mBusAPin[1].set(xpos, 2.1, this, "Pin 3:A2"); xpos += 0.54;
    mPin.push_back(&mBusAPin[2]);
    mBusAPin[2].set(xpos, 2.1, this, "Pin 4:A3"); xpos += 0.54;
    mPin.push_back(&mBusAPin[3]);
    mBusAPin[3].set(xpos, 2.1, this, "Pin 5:A4"); xpos += 0.54;
    mPin.push_back(&mBusAPin[4]);
    mBusAPin[4].set(xpos, 2.1, this, "Pin 6:A5"); xpos += 0.54;
    mPin.push_back(&mBusAPin[5]);
    mBusAPin[5].set(xpos, 2.1, this, "Pin 7:A6"); xpos += 0.54;
    mPin.push_back(&mBusAPin[6]);
    mBusAPin[6].set(xpos, 2.1, this, "Pin 8:A7"); xpos += 0.54;
    mPin.push_back(&mBusAPin[7]);
    mBusAPin[7].set(xpos, 2.1, this, "Pin 9:A8"); xpos += 0.54;
    
    mTexture = load_texture("data/chip_20pin.png");    
}

void Chip74245::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS245",mX+0.6,mY+1,0x5fffffff,0.75);
}

void Chip74245::update(float aTick) 
{
    if (mEnablePin.mNet == NULL ||
        mDirectionPin.mNet == NULL ||
        mEnablePin.mNet->mState == NETSTATE_INVALID ||
        mDirectionPin.mNet->mState == NETSTATE_INVALID)
    {
        int i;
        for (i = 0; i < 8; i++)
        {
            mBusAPin[i].setState(gConfig.mPropagateInvalidState);
            mBusBPin[i].setState(gConfig.mPropagateInvalidState);
        }
        return;
    }

    int i;

    if (mEnablePin.mNet->mState != NETSTATE_LOW)
    {
        for (i = 0; i < 8; i++)
        {
            mBusAPin[i].setState(PINSTATE_HIGHZ);
            mBusBPin[i].setState(PINSTATE_HIGHZ);
        }
        return;
    }

    if (mDirectionPin.mNet->mState != NETSTATE_LOW)
    {
        // A -> B
        for (i = 0; i < 8; i++)
        {
            mBusAPin[i].setState(PINSTATE_READ);;
            if (!mBusAPin[i].mNet || mBusAPin[i].mNet->mState == NETSTATE_INVALID)
            {
                mBusBPin[i].setState(gConfig.mPropagateInvalidState);
            }
            else
            if (mBusAPin[i].mNet->mState != NETSTATE_LOW)
            {
                mBusBPin[i].setState(PINSTATE_WRITE_HIGH);
            }
            else
            {
                mBusBPin[i].setState(PINSTATE_WRITE_LOW);
            }
        }
    }
    else
    {
        // B -> A
        for (i = 0; i < 8; i++)
        {
            mBusBPin[i].setState(PINSTATE_READ);;
            if (!mBusBPin[i].mNet || mBusBPin[i].mNet->mState == NETSTATE_INVALID)
            {
                mBusAPin[i].setState(gConfig.mPropagateInvalidState);
            }
            else
            if (mBusBPin[i].mNet->mState != NETSTATE_LOW)
            {
                mBusAPin[i].setState(PINSTATE_WRITE_HIGH);
            }
            else
            {
                mBusAPin[i].setState(PINSTATE_WRITE_LOW);
            }
        }
    }
}    
