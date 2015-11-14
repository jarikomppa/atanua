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
#include "chip74154.h"

Chip74154::Chip74154()
{
    set(0,0,6.8,3.4,"4-Line to 16-Line Decoder/Demultiplexer");
    float xpos = 0.13 + 0.5475;
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(xpos, -0.25, this, "Pin 23:INPUT A"); xpos += 0.5475;
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(xpos, -0.25, this, "Pin 22:INPUT B"); xpos += 0.5475;
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(xpos, -0.25, this, "Pin 21:INPUT C"); xpos += 0.5475;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(xpos, -0.25, this, "Pin 20:INPUT D"); xpos += 0.5475;
    mPin.push_back(&mEnablePin[0]);
    mEnablePin[0].set(xpos, -0.25, this, "Pin 19:G2"); xpos += 0.5475;
    mPin.push_back(&mEnablePin[1]);
    mEnablePin[1].set(xpos, -0.25, this, "Pin 18:G1"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[15]);
    mOutputPin[15].set(xpos, -0.25, this, "Pin 17:OUTPUT 15"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[14]);
    mOutputPin[14].set(xpos, -0.25, this, "Pin 16:OUTPUT 14"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[13]);
    mOutputPin[13].set(xpos, -0.25, this, "Pin 15:OUTPUT 13"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[12]);
    mOutputPin[12].set(xpos, -0.25, this, "Pin 14:OUTPUT 12"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[11]);
    mOutputPin[11].set(xpos, -0.25, this, "Pin 13:OUTPUT 11"); xpos += 0.5475;

    xpos = 0.15;

    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos, 3.1, this, "Pin 1:OUTPUT 0"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos, 3.1, this, "Pin 2:OUTPUT 1"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos, 3.1, this, "Pin 3:OUTPUT 2"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(xpos, 3.1, this, "Pin 4:OUTPUT 3"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[4]);
    mOutputPin[4].set(xpos, 3.1, this, "Pin 5:OUTPUT 4"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[5]);
    mOutputPin[5].set(xpos, 3.1, this, "Pin 6:OUTPUT 5"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[6]);
    mOutputPin[6].set(xpos, 3.1, this, "Pin 7:OUTPUT 6"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[7]);
    mOutputPin[7].set(xpos, 3.1, this, "Pin 8:OUTPUT 7"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[8]);
    mOutputPin[8].set(xpos, 3.1, this, "Pin 9:OUTPUT 8"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[9]);
    mOutputPin[9].set(xpos, 3.1, this, "Pin 10:OUTPUT 9"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[10]);
    mOutputPin[10].set(xpos, 3.1, this, "Pin 11:OUTPUT 10"); xpos += 0.5475;

    mTexture = load_texture(DATADIR "/chip_24pin.png");
}

void Chip74154::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS154",mX+1.6,mY+1.35,0x5fffffff,0.75);
}

void Chip74154::update(float aTick) 
{
    
    int i;
    if (mInputPin[0].mNet == NULL ||
        mInputPin[1].mNet == NULL ||
        mInputPin[2].mNet == NULL ||
        mInputPin[3].mNet == NULL ||
        mEnablePin[0].mNet == NULL ||
        mEnablePin[1].mNet == NULL ||
        mInputPin[0].mNet->mState == NETSTATE_INVALID ||
        mInputPin[1].mNet->mState == NETSTATE_INVALID ||
        mInputPin[2].mNet->mState == NETSTATE_INVALID ||
        mInputPin[3].mNet->mState == NETSTATE_INVALID ||
        mEnablePin[0].mNet->mState == NETSTATE_INVALID ||
        mEnablePin[1].mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 16; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    for (i = 0; i < 16; i++)
        mOutputPin[i].setState(PINSTATE_WRITE_HIGH);

    if (mEnablePin[0].mNet->mState != NETSTATE_LOW ||
        mEnablePin[1].mNet->mState != NETSTATE_LOW)
    {
        return;
    }

    int a = ((mInputPin[3].mNet->mState != NETSTATE_LOW)?(1 << 3):0) |
            ((mInputPin[2].mNet->mState != NETSTATE_LOW)?(1 << 2):0) |
            ((mInputPin[1].mNet->mState != NETSTATE_LOW)?(1 << 1):0) |
            ((mInputPin[0].mNet->mState != NETSTATE_LOW)?(1 << 0):0);
    
    mOutputPin[a].setState(PINSTATE_WRITE_LOW);
}    
