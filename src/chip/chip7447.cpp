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
#include "chip7447.h"

Chip7447::Chip7447()
{
    set(0,0,4.54,2.25,"BCD to 7-segment Decoder/Driver");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mOutputPin[5]); // f
    mOutputPin[5].set(xpos , 0, this, "Pin 15:f"); xpos += 0.54;
    mPin.push_back(&mOutputPin[6]); // g
    mOutputPin[6].set(xpos , 0, this, "Pin 14:g"); xpos += 0.54;
    mPin.push_back(&mOutputPin[0]); // a
    mOutputPin[0].set(xpos , 0, this, "Pin 13:a"); xpos += 0.54;
    mPin.push_back(&mOutputPin[1]); // b
    mOutputPin[1].set(xpos , 0, this, "Pin 12:b"); xpos += 0.54;
    mPin.push_back(&mOutputPin[2]); // c
    mOutputPin[2].set(xpos , 0, this, "Pin 11:c"); xpos += 0.54;
    mPin.push_back(&mOutputPin[3]); // d
    mOutputPin[3].set(xpos , 0, this, "Pin 10:d"); xpos += 0.54;
    mPin.push_back(&mOutputPin[4]); // e
    mOutputPin[4].set(xpos , 0, this, "Pin 9:e"); xpos += 0.54;
    
    xpos = 0.15;
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(xpos, 1.75, this, "Pin 1:A1"); xpos += 0.54;
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(xpos, 1.75, this, "Pin 2:A2"); xpos += 0.54;
    mPin.push_back(&mLTPin);
    mLTPin.set(xpos, 1.75, this, "Pin 3:LT"); xpos += 0.54;
    mPin.push_back(&mBIRBOPin);
    mBIRBOPin.set(xpos, 1.75, this, "Pin 4:BI/RBO"); xpos += 0.54;
    mPin.push_back(&mRBIPin);
    mRBIPin.set(xpos, 1.75, this, "Pin 5:RBI"); xpos += 0.54;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(xpos, 1.75, this, "Pin 6:A3"); xpos += 0.54;
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(xpos, 1.75, this, "Pin 7:A0"); xpos += 0.54;

    mTexture = load_texture("data/chip_16pin.png");
}

void Chip7447::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS47",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip7447::update(float aTick) 
{
    // TODO: handle BIRBO output bit differently
    int i;
    if (mInputPin[0].mNet == NULL ||
        mInputPin[1].mNet == NULL ||
        mInputPin[2].mNet == NULL ||
        mInputPin[3].mNet == NULL ||
        mLTPin.mNet == NULL ||
        mBIRBOPin.mNet == NULL ||
        mRBIPin.mNet == NULL ||
        mInputPin[0].mNet->mState == NETSTATE_INVALID ||
        mInputPin[1].mNet->mState == NETSTATE_INVALID ||
        mInputPin[2].mNet->mState == NETSTATE_INVALID ||
        mInputPin[3].mNet->mState == NETSTATE_INVALID ||
        mLTPin.mNet->mState == NETSTATE_INVALID ||
        mBIRBOPin.mNet->mState == NETSTATE_INVALID ||
        mRBIPin.mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 7; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    // make sure we're not blanking ourselves
    if ((mBIRBOPin.getState() != PINSTATE_READ_OR_WRITE_LOW) && (mBIRBOPin.mNet->mState == NETSTATE_LOW))
    {
        for (i = 0; i < 7; i++)
            mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
        return;
    }

    if (mLTPin.mNet->mState == NETSTATE_LOW)
    {
        for (i = 0; i < 7; i++)
            mOutputPin[i].setState(PINSTATE_WRITE_LOW);
        return;
    }

    if (mRBIPin.mNet->mState == NETSTATE_LOW)
    {
        mBIRBOPin.setState(PINSTATE_READ_OR_WRITE_LOW);
        for (i = 0; i < 7; i++)
            mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
        return;
    }
    else
    {
        mBIRBOPin.setState(PINSTATE_READ_OR_WRITE_HIGH);
    }

    int v = ((mInputPin[3].mNet->mState == NETSTATE_HIGH)?(1 << 3):0) |
            ((mInputPin[2].mNet->mState == NETSTATE_HIGH)?(1 << 2):0) |
            ((mInputPin[1].mNet->mState == NETSTATE_HIGH)?(1 << 1):0) |
            ((mInputPin[0].mNet->mState == NETSTATE_HIGH)?(1 << 0):0);

    static const int outstates[16][7] = {
        {0,0,0,0,0,0,1},
        {1,0,0,1,1,1,1},
        {0,0,1,0,0,1,0},
        {0,0,0,0,1,1,0},

        {1,0,0,1,1,0,0},
        {0,1,0,0,1,0,0},
        {1,1,0,0,0,0,0},
        {0,0,0,1,1,1,1},
        {0,0,0,0,0,0,0},

        {0,0,0,1,1,0,0},
        {1,1,1,0,0,1,0},
        {1,1,0,0,1,1,0},
        {1,0,1,1,1,0,0},
        {0,1,1,0,1,0,0},

        {1,1,1,0,0,0,0},
        {1,1,1,1,1,1,1}
    };

    for (i = 0; i < 7; i++)
    {
        if (outstates[v][i])
        {
            mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
        }
        else
        {
            mOutputPin[i].setState(PINSTATE_WRITE_LOW);
        }
    }
}    
