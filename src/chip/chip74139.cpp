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
#include "chip74139.h"

Chip74139::Chip74139()
{
    set(0,0,4.54,2.25,"Dual 2 to 4-line\nDecoder/Demultiplexer");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mEnablePin[0]);
    mEnablePin[0].set(xpos, 0, this, "Pin 15:ENABLE G2"); xpos += 0.54;
    mPin.push_back(&mInputPin[0][0]);
    mInputPin[0][0].set(xpos, 0, this, "Pin 14:SELECT A2"); xpos += 0.54;
    mPin.push_back(&mInputPin[0][1]);
    mInputPin[0][1].set(xpos, 0, this, "Pin 13:SELECT B2"); xpos += 0.54;
    mPin.push_back(&mOutputPin[0][0]);
    mOutputPin[0][0].set(xpos, 0, this, "Pin 12:DATA OUTPUT 2Y0"); xpos += 0.54;
    mPin.push_back(&mOutputPin[0][1]);
    mOutputPin[0][1].set(xpos, 0, this, "Pin 11:DATA OUTPUT 2Y1"); xpos += 0.54;
    mPin.push_back(&mOutputPin[0][2]);
    mOutputPin[0][2].set(xpos, 0, this, "Pin 10:DATA OUTPUT 2Y2"); xpos += 0.54;
    mPin.push_back(&mOutputPin[0][3]);
    mOutputPin[0][3].set(xpos, 0, this, "Pin 9:DATA OUTPUT 2Y3"); xpos += 0.54;
    
    xpos = 0.15;
    mPin.push_back(&mEnablePin[1]);
    mEnablePin[1].set(xpos, 1.75, this, "Pin 1:ENABLE G1"); xpos += 0.54;
    mPin.push_back(&mInputPin[1][0]);
    mInputPin[1][0].set(xpos, 1.75, this, "Pin 2:SELECT A1"); xpos += 0.54;
    mPin.push_back(&mInputPin[1][1]);
    mInputPin[1][1].set(xpos, 1.75, this, "Pin 3:SELECT B1"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1][0]);
    mOutputPin[1][0].set(xpos, 1.75, this, "Pin 4:DATA OUTPUT 1Y0"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1][1]);
    mOutputPin[1][1].set(xpos, 1.75, this, "Pin 5:DATA OUTPUT 1Y1"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1][2]);
    mOutputPin[1][2].set(xpos, 1.75, this, "Pin 6:DATA OUTPUT 1Y2"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1][3]);
    mOutputPin[1][3].set(xpos, 1.75, this, "Pin 7:DATA OUTPUT 1Y3"); xpos += 0.54;

    mTexture = load_texture("data/chip_16pin.png");
}

void Chip74139::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS139",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip74139::update(float aTick) 
{
    int i, j;
    for (j = 0; j < 2; j++)
    {
        if (mEnablePin[j].mNet == NULL ||
            mInputPin[j][0].mNet == NULL ||
            mInputPin[j][1].mNet == NULL ||
            mEnablePin[j].mNet->mState == NETSTATE_INVALID ||
            mInputPin[j][0].mNet->mState == NETSTATE_INVALID ||
            mInputPin[j][1].mNet->mState == NETSTATE_INVALID)
        {
            for (i = 0; i < 4; i++)
                mOutputPin[j][i].setState(gConfig.mPropagateInvalidState);        
        }
        else
        {
            for (i = 0; i < 4; i++)
                mOutputPin[j][i].setState(PINSTATE_WRITE_HIGH);
            if (mEnablePin[j].mNet->mState == NETSTATE_LOW)
            {
                int v = ((mInputPin[j][1].mNet->mState == NETSTATE_HIGH)?(1 << 1):0) |
                        ((mInputPin[j][0].mNet->mState == NETSTATE_HIGH)?(1 << 0):0);
                mOutputPin[j][v].setState(PINSTATE_WRITE_LOW);
            }
        }
    }
}    
