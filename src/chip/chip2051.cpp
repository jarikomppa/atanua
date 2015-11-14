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
#include "atanua_internal.h"
#include "chip2051.h"
#include "fileutils.h"

static int sfrread(struct em8051 *aCPU, int aRegister)
{
    return ((Chip2051*)aCPU->mUserPtr)->sfrread(aRegister);    
}

int Chip2051::sfrread(int aRegister)
{
    int i, v;
    switch (aRegister - 0x80)
    {
    case REG_P1:
        v = 0;
        for (i = 0; i < 8; i++)
        {
            if (mPin1[i].mNet && mPin1[i].mNet->mState != NETSTATE_LOW)
            {
                v |= (1 << i);
            }
        }
        return v;
    case REG_P3:
        v = 0;
        for (i = 0; i < 6; i++)
        {
            if (mPin3[i].mNet && mPin3[i].mNet->mState != NETSTATE_LOW)
            {
                v |= (1 << i);
            }
        }
        if (mPin3[6].mNet && mPin3[6].mNet->mState != NETSTATE_LOW)
            v |= (1 << 7);
        return v;
    }
    return mCore.mSFR[aRegister - 0x80];
}

Chip2051::Chip2051()
{
    set(0,0,5.7,2.8,"20-pin 8051-variant\n"
                    "8-bit Microcontroller with\n"
                    "2K Bytes Flash and\n"
                    "15 Programmable I/O Lines\n\n"
                    "Click on the 2051 and press L\n"
                    "to load a intel hex format\n"
                    "object to the flash");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mPin1[7]);
    mPin1[7].set(xpos, 0.15, this, "Pin 19:P1.7"); xpos += 0.54;    
    mPin.push_back(&mPin1[6]);
    mPin1[6].set(xpos, 0.15, this, "Pin 18:P1.6"); xpos += 0.54;    
    mPin.push_back(&mPin1[5]);
    mPin1[5].set(xpos, 0.15, this, "Pin 17:P1.5"); xpos += 0.54;    
    mPin.push_back(&mPin1[4]);
    mPin1[4].set(xpos, 0.15, this, "Pin 16:P1.4"); xpos += 0.54;    
    mPin.push_back(&mPin1[3]);
    mPin1[3].set(xpos, 0.15, this, "Pin 15:P1.3"); xpos += 0.54;    
    mPin.push_back(&mPin1[2]);
    mPin1[2].set(xpos, 0.15, this, "Pin 14:P1.2"); xpos += 0.54;    
    mPin.push_back(&mPin1[1]);
    mPin1[1].set(xpos, 0.15, this, "Pin 13:P1.1"); xpos += 0.54;    
    mPin.push_back(&mPin1[0]);
    mPin1[0].set(xpos, 0.15, this, "Pin 12:P1.0"); xpos += 0.54;    
    mPin.push_back(&mPin3[6]);
    mPin3[6].set(xpos, 0.15, this, "Pin 11:P3.7"); xpos += 0.54;    

    xpos = 0.15;

    mPin.push_back(&mRSTPin);
    mRSTPin.set(xpos, 2.1, this, "Pin 1:RST/VPP"); xpos += 0.54;
    mPin.push_back(&mPin3[0]);
    mPin3[0].set(xpos, 2.1, this, "Pin 2:P3.0"); xpos += 0.54;    
    mPin.push_back(&mPin3[1]);
    mPin3[1].set(xpos, 2.1, this, "Pin 3:P3.1"); xpos += 0.54;    
    xpos += 0.54;
    mPin.push_back(&mClockPin);
    mClockPin.set(xpos, 2.1, this, "Pin 5:XTAL1"); xpos += 0.54;    
    mPin.push_back(&mPin3[2]);
    mPin3[2].set(xpos, 2.1, this, "Pin 6:P3.2"); xpos += 0.54;    
    mPin.push_back(&mPin3[3]);
    mPin3[3].set(xpos, 2.1, this, "Pin 7:P3.3"); xpos += 0.54;    
    mPin.push_back(&mPin3[4]);
    mPin3[4].set(xpos, 2.1, this, "Pin 8:P3.4"); xpos += 0.54;    
    mPin.push_back(&mPin3[5]);
    mPin3[5].set(xpos, 2.1, this, "Pin 9:P3.5"); xpos += 0.54;    
    
    memset(&mCore, 0, sizeof(mCore));

    mCore.mUserPtr = (void*)this;
    mCore.mCodeMem = mCodeMem; 
    mCore.mCodeMemSize = 2048; 
    mCore.mLowerData = mLowerMem;
    mCore.mSFR = mSFR;
    mCore.sfrread = ::sfrread; 

    reset(&mCore, 1);

    mClock = 0;
    mResetCounter = 0;

    mTexture = load_texture(DATADIR "/chip_20pin.png");    
}

void Chip2051::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("89C2051",mX+0.6,mY+1,0x5fffffff,0.75);
    if (gUIState.kbditem == aChipId)
    {
        if (gUIState.keyentered == SDLK_l)
        {
            FILE * f = openfiledialog("Load 2051 Flash Contents");
            if (f)
            {
                load_obj_fileptr(&mCore, f);
                fclose(f);
            }
        }
    }
}

void Chip2051::update(float aTick) 
{
    if (mRSTPin.mNet == NULL || mRSTPin.mNet->mState != NETSTATE_LOW)
    {
        int i;
        for (i = 0; i < 8; i++)
        {
            mPin1[i].setState(PINSTATE_WRITE_HIGH);
        }
        for (i = 0; i < 6; i++)
        {
            mPin3[i].setState(PINSTATE_WRITE_HIGH);
        }
        mPin3[6].setState(PINSTATE_WRITE_HIGH);
    }
    if (mClockPin.mNet == NULL)
    {
        return;
    }
    if (mClockPin.mNet->mState == NETSTATE_HIGH)
    {
        if (mClock == 0)
        {
            mClock = 1;
            if (mRSTPin.mNet == NULL || mRSTPin.mNet->mState != NETSTATE_LOW)
            {
                mResetCounter++;
                if (mResetCounter > 1)
                {
                    mResetCounter = 0;
                    reset(&mCore, 0);
                }
            }
            else
            {
                mResetCounter = 0;

                while (!tick(&mCore)) 
                {
                    // Loop until new instruction is executed - basically this 
                    // causes the core to be a 12x speed 8051 core
                }
                int i;
                for (i = 0; i < 8; i++)
                {
                    if (mSFR[REG_P1] & (1 << i))
                    {
                        mPin1[i].setState(PINSTATE_READ_OR_WRITE_HIGH);
                    }
                    else
                    {
                        mPin1[i].setState(PINSTATE_READ_OR_WRITE_LOW);
                    }                
                }
                for (i = 0; i < 6; i++)
                {
                    if (mSFR[REG_P3] & (1 << i))
                    {
                        mPin3[i].setState(PINSTATE_READ_OR_WRITE_HIGH);
                    }
                    else
                    {
                        mPin3[i].setState(PINSTATE_READ_OR_WRITE_LOW);
                    }                
                }
                if (mSFR[REG_P3] & (1 << 7))
                {
                    mPin3[6].setState(PINSTATE_READ_OR_WRITE_HIGH);
                }
                else
                {
                    mPin3[6].setState(PINSTATE_READ_OR_WRITE_LOW);
                }
            }
        }
    }
    else
    {
        mClock = 0;
    }
}    

void Chip2051::serialize(File *f)
{
    int l;
    l = 2047;
    while (l > 0 && mCodeMem[l] == 0) l--;
    l++;
    f->writeint(l);
    int i;
    for (i = 0; i < l; i++)
        f->writebyte(mCodeMem[i]);
}

void Chip2051::deserialize(File *f)
{
    memset(mCodeMem,0,2048);
    int l = f->readint();
    int i;
    for (i = 0; i < l; i++)
        mCodeMem[i] = f->readbyte();
}

void Chip2051::clone(Chip *aOther)
{
    Chip2051 *d = (Chip2051 *)aOther;
    // Copy of the code memory should be enough..
    memcpy(d->mCodeMem, mCodeMem, 2048);
}
