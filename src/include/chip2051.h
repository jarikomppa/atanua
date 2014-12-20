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
#ifndef CHIP2051_H
#define CHIP2051_H

extern "C"
{
#include "8051/emu8051.h"
}

class Chip2051 : public Chip
{
    Pin mPin1[8]; 
    Pin mPin3[7]; 
    Pin mRSTPin;
    Pin mClockPin;

    em8051 mCore;
    unsigned char mCodeMem[2048];
    unsigned char mLowerMem[128];
    unsigned char mSFR[128];

    int mTexture;

    int mClock;
    int mResetCounter;

public:
    Chip2051(); // Ctor

    int sfrread(int aRegister);
    virtual void render(int aChipId);
    virtual void update(float aTick);
    virtual void serialize(File *f);
    virtual void deserialize(File *f);
    virtual void clone(Chip *aOther);

};

#endif
