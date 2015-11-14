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
#include "chip74181.h"

Chip74181::Chip74181()
{
    set(0,0,6.8,3.4,"4-bit Arithmetic Logic Unit and Function Generator");
    float xpos = 0.13 + 0.5475;
    mPin.push_back(&mInputPinA[1]);
    mInputPinA[1].set(xpos, -0.25, this, "Pin 23:A1"); xpos += 0.5475;
    mPin.push_back(&mInputPinB[1]);
    mInputPinB[1].set(xpos, -0.25, this, "Pin 22:B1"); xpos += 0.5475;
    mPin.push_back(&mInputPinA[2]);
    mInputPinA[2].set(xpos, -0.25, this, "Pin 21:A2"); xpos += 0.5475;
    mPin.push_back(&mInputPinB[2]);
    mInputPinB[2].set(xpos, -0.25, this, "Pin 20:B2"); xpos += 0.5475;
    mPin.push_back(&mInputPinA[3]);
    mInputPinA[3].set(xpos, -0.25, this, "Pin 19:A3"); xpos += 0.5475;
    mPin.push_back(&mInputPinB[3]);
    mInputPinB[3].set(xpos, -0.25, this, "Pin 18:B3"); xpos += 0.5475;
    mPin.push_back(&mCarryGeneratePin);
    mCarryGeneratePin.set(xpos, -0.25, this, "Pin 17:G"); xpos += 0.5475;
    mPin.push_back(&mCarryOutputPin);
    mCarryOutputPin.set(xpos, -0.25, this, "Pin 16:Cn+4"); xpos += 0.5475;
    mPin.push_back(&mCarryPropagatePin);
    mCarryPropagatePin.set(xpos, -0.25, this, "Pin 15:P"); xpos += 0.5475;
    mPin.push_back(&mComparatorOutputPin);
    mComparatorOutputPin.set(xpos, -0.25, this, "Pin 14:A=B"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(xpos, -0.25, this, "Pin 13:F3"); xpos += 0.5475;

    xpos = 0.15;

    mPin.push_back(&mInputPinB[0]);
    mInputPinB[0].set(xpos, 3.1, this, "Pin 1:B0"); xpos += 0.5475;
    mPin.push_back(&mInputPinA[0]);
    mInputPinA[0].set(xpos, 3.1, this, "Pin 2:A0"); xpos += 0.5475;
    mPin.push_back(&mInputPinS[3]);
    mInputPinS[3].set(xpos, 3.1, this, "Pin 3:S3"); xpos += 0.5475;
    mPin.push_back(&mInputPinS[2]);
    mInputPinS[2].set(xpos, 3.1, this, "Pin 4:S2"); xpos += 0.5475;
    mPin.push_back(&mInputPinS[1]);
    mInputPinS[1].set(xpos, 3.1, this, "Pin 5:S1"); xpos += 0.5475;
    mPin.push_back(&mInputPinS[0]);
    mInputPinS[0].set(xpos, 3.1, this, "Pin 6:S0"); xpos += 0.5475;
    mPin.push_back(&mCarryInputPin);
    mCarryInputPin.set(xpos, 3.1, this, "Pin 7:Cn"); xpos += 0.5475;
    mPin.push_back(&mModeControlPin);
    mModeControlPin.set(xpos, 3.1, this, "Pin 8:M"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos, 3.1, this, "Pin 9:F0"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos, 3.1, this, "Pin 10:F1"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos, 3.1, this, "Pin 11:F2"); xpos += 0.5475;

    mTexture = load_texture(DATADIR "/chip_24pin.png");
}

void Chip74181::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS181",mX+1.6,mY+1.35,0x5fffffff,0.75);
}

void Chip74181::update(float aTick) 
{
    
    int i;
    if (mInputPinA[0].mNet == NULL ||
        mInputPinA[1].mNet == NULL ||
        mInputPinA[2].mNet == NULL ||
        mInputPinA[3].mNet == NULL ||
        mInputPinB[0].mNet == NULL ||
        mInputPinB[1].mNet == NULL ||
        mInputPinB[2].mNet == NULL ||
        mInputPinB[3].mNet == NULL ||
        mInputPinS[0].mNet == NULL ||
        mInputPinS[1].mNet == NULL ||
        mInputPinS[2].mNet == NULL ||
        mInputPinS[3].mNet == NULL ||
        mCarryInputPin.mNet == NULL ||
        mModeControlPin.mNet == NULL ||
        mInputPinA[0].mNet->mState == NETSTATE_INVALID ||
        mInputPinA[1].mNet->mState == NETSTATE_INVALID ||
        mInputPinA[2].mNet->mState == NETSTATE_INVALID ||
        mInputPinA[3].mNet->mState == NETSTATE_INVALID ||
        mInputPinB[0].mNet->mState == NETSTATE_INVALID ||
        mInputPinB[1].mNet->mState == NETSTATE_INVALID ||
        mInputPinB[2].mNet->mState == NETSTATE_INVALID ||
        mInputPinB[3].mNet->mState == NETSTATE_INVALID ||
        mInputPinS[0].mNet->mState == NETSTATE_INVALID ||
        mInputPinS[1].mNet->mState == NETSTATE_INVALID ||
        mInputPinS[2].mNet->mState == NETSTATE_INVALID ||
        mInputPinS[3].mNet->mState == NETSTATE_INVALID ||
        mCarryInputPin.mNet->mState == NETSTATE_INVALID ||
        mModeControlPin.mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 4; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        mCarryGeneratePin.setState(gConfig.mPropagateInvalidState);
        mCarryPropagatePin.setState(gConfig.mPropagateInvalidState);
        mCarryOutputPin.setState(gConfig.mPropagateInvalidState);
        return;
    }

    int a = ((mInputPinA[3].mNet->mState != NETSTATE_LOW)?(1 << 3):0) |
            ((mInputPinA[2].mNet->mState != NETSTATE_LOW)?(1 << 2):0) |
            ((mInputPinA[1].mNet->mState != NETSTATE_LOW)?(1 << 1):0) |
            ((mInputPinA[0].mNet->mState != NETSTATE_LOW)?(1 << 0):0);
    int b = ((mInputPinB[3].mNet->mState != NETSTATE_LOW)?(1 << 3):0) |
            ((mInputPinB[2].mNet->mState != NETSTATE_LOW)?(1 << 2):0) |
            ((mInputPinB[1].mNet->mState != NETSTATE_LOW)?(1 << 1):0) |
            ((mInputPinB[0].mNet->mState != NETSTATE_LOW)?(1 << 0):0);
    int s = ((mInputPinS[3].mNet->mState != NETSTATE_LOW)?(1 << 3):0) |
            ((mInputPinS[2].mNet->mState != NETSTATE_LOW)?(1 << 2):0) |
            ((mInputPinS[1].mNet->mState != NETSTATE_LOW)?(1 << 1):0) |
            ((mInputPinS[0].mNet->mState != NETSTATE_LOW)?(1 << 0):0);

    int o = 0;

	mCarryOutputPin.setState(PINSTATE_WRITE_HIGH);

#define NOT(x) ((x) ^ 0xf)
    if (mModeControlPin.mNet->mState == NETSTATE_LOW)
    {
        int addmode = 0;
        // Arithmetic
        switch (s)
        {
        case  0: o = a; break;
        case  1: o = a | b; break;
        case  2: o = a | NOT(b); break;
        case  3: o = -1; break;
        case  4: o = a + (a & NOT(b)); addmode = 1; break;
        case  5: o = (a | b) + (a & NOT(b)); addmode = 1; break;
        case  6: o = a - b - 1; break;
        case  7: o = (a & b) - 1; break;
        case  8: o = a + (a & b); addmode = 1; break;
        case  9: o = a + b; addmode = 1; break;
        case 10: o = (a | NOT(b)) + (a & b); addmode = 1; break;
        case 11: o = (a & b) - 1; break;
        case 12: o = a + a; addmode = 1; break;
        case 13: o = (a | b) + a; addmode = 1; break;
        case 14: o = (a | NOT(b)) + a; addmode = 1; break;
        case 15: o = a - 1; break;
        }

        if (addmode)
        {
            // add mode
            // Really not sure whether these should be high/low instead
            if (o >= 15)
                mCarryPropagatePin.setState(PINSTATE_WRITE_LOW);
            else
                mCarryPropagatePin.setState(PINSTATE_WRITE_HIGH);
            if (o >= 16)
                mCarryGeneratePin.setState(PINSTATE_WRITE_LOW);
            else
                mCarryGeneratePin.setState(PINSTATE_WRITE_HIGH);
        }
        else
        {
            // subtract mode
            if (o <= 0)
                mCarryPropagatePin.setState(PINSTATE_WRITE_LOW);
            else
                mCarryPropagatePin.setState(PINSTATE_WRITE_HIGH);
            if (o < 0)
                mCarryGeneratePin.setState(PINSTATE_WRITE_LOW);
            else
                mCarryGeneratePin.setState(PINSTATE_WRITE_HIGH);
        }

        // 'P and G are not affected by carry in', so apply it afterwards
        if (mCarryInputPin.mNet->mState != NETSTATE_LOW)
            o++;
        if (o > 0xf)
            mCarryOutputPin.setState(PINSTATE_WRITE_LOW);
    }
    else 
    {
        // Logic
        switch (s)
        {
        case  0: o = NOT(a);  break;
        case  1: o = NOT(a) | NOT(b);  break;
        case  2: o = NOT(a) & b;  break;
        case  3: o = 0; break;
        case  4: o = NOT(a & b); break;
        case  5: o = NOT(b); break;
        case  6: o = a ^ b; break;
        case  7: o = a & NOT(b); break;
        case  8: o = NOT(a) | b; break;
        case  9: o = NOT(a) ^ NOT(b); break;
        case 10: o = b; break;
        case 11: o = a & b; break;
        case 12: o = 0xf; break;
        case 13: o = a | NOT(b); break;
        case 14: o = a | b; break;
        case 15: o = a; break;
        }
    }

    // This may be reversed as well
    if (o == 0)
    {
        mComparatorOutputPin.setState(PINSTATE_WRITE_HIGH);
    }
    else
    {
        mComparatorOutputPin.setState(PINSTATE_WRITE_LOW);
    }

    for (i = 0; i < 4; i++)
    {
        if (o & (1 << i))
        {
            mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
        }
        else
        {
            mOutputPin[i].setState(PINSTATE_WRITE_LOW);
        }
    }
}    
