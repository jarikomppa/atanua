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
#include "chip27xx.h"
#include "fileutils.h"

Chip27xx::Chip27xx(int aType)
{
    mType = aType;
    const char * title;
    switch (mType)
    {
    case 8:
        title = "1024-word by 8-bit\n"
                "erasable programmable\n"
                "read only memories\n\n"
                "Click chip to give it\n"
                "keyboard focus, then\n"
                "press L to load ROM\n"
                "contents.";
        mMemSize = 1024;
        break;
    case 16:
        title = "2048-word by 8-bit\n"
                "erasable programmable\n"
                "read only memories\n\n"
                "Click chip to give it\n"
                "keyboard focus, then\n"
                "press L to load ROM\n"
                "contents.";
        mMemSize = 2048;
        break;
    default:
        title = "4096-word by 8-bit\n"
                "erasable programmable\n"
                "read only memories\n\n"
                "Click chip to give it\n"
                "keyboard focus, then\n"
                "press L to load ROM\n"
                "contents.";
        mMemSize = 4096;
    }

    mMemory = new unsigned char[mMemSize];

    set(0,0,6.8,3.4,title);
    float xpos = 0.13 + 0.5475;
    mPin.push_back(&mAddressPin[8]);
    mAddressPin[8].set(xpos, -0.25, this, "Pin 23:A8"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[9]);
    mAddressPin[9].set(xpos, -0.25, this, "Pin 22:A9"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[11]);
    mAddressPin[11].set(xpos, -0.25, this, (mType > 16)?"Pin 21:A11":"Pin 21:VBB"); xpos += 0.5475;
    mPin.push_back(&mEnablePin[0]);
    mEnablePin[0].set(xpos, -0.25, this, "Pin 20:G/VPP"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[10]);
    mAddressPin[10].set(xpos, -0.25, this, (mType > 8)?"Pin 19:A10":"Pin 19:VDD"); xpos += 0.5475;
    mPin.push_back(&mEnablePin[1]);
    mEnablePin[1].set(xpos, -0.25, this, "Pin 18:E#"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[7]);
    mOutputPin[7].set(xpos, -0.25, this, "Pin 17:DQ7"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[6]);
    mOutputPin[6].set(xpos, -0.25, this, "Pin 16:DQ6"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[5]);
    mOutputPin[5].set(xpos, -0.25, this, "Pin 15:DQ5"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[4]);
    mOutputPin[4].set(xpos, -0.25, this, "Pin 14:DQ4"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[3]);
    mOutputPin[3].set(xpos, -0.25, this, "Pin 13:DQ3"); xpos += 0.5475;

    xpos = 0.15;

    mPin.push_back(&mAddressPin[7]);
    mAddressPin[7].set(xpos, 3.1, this, "Pin 1:A7"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[6]);
    mAddressPin[6].set(xpos, 3.1, this, "Pin 2:A6"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[5]);
    mAddressPin[5].set(xpos, 3.1, this, "Pin 3:A5"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[4]);
    mAddressPin[4].set(xpos, 3.1, this, "Pin 4:A4"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[3]);
    mAddressPin[3].set(xpos, 3.1, this, "Pin 5:A3"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[2]);
    mAddressPin[2].set(xpos, 3.1, this, "Pin 6:A2"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[1]);
    mAddressPin[1].set(xpos, 3.1, this, "Pin 7:A1"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[0]);
    mAddressPin[0].set(xpos, 3.1, this, "Pin 8:A0"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[0]);
    mOutputPin[0].set(xpos, 3.1, this, "Pin 9:DQ0"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[1]);
    mOutputPin[1].set(xpos, 3.1, this, "Pin 10:DQ1"); xpos += 0.5475;
    mPin.push_back(&mOutputPin[2]);
    mOutputPin[2].set(xpos, 3.1, this, "Pin 11:DQ2"); xpos += 0.5475;

    mTexture = load_texture("data/chip_24pin.png");
}

Chip27xx::~Chip27xx()
{
    delete[] mMemory;
    mMemory = NULL;
}


void Chip27xx::render(int aChipId)
{
    const char * label;
    switch (mType)
    {
    case 8:
        label = "2708";
        break;
    case 16:
        label = "2716";
        break;
    default:
        label = "2732";
    }
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring(label,mX+2.5,mY+1.35,0x5fffffff,0.75);
    
    if (gUIState.kbditem == aChipId)
    {
        if (gUIState.keyentered == SDLK_l)
        {
            FILE * f = openfiledialog("Load EEPROM Contents");
            if (f)
            {
                fread(mMemory,mMemSize,1,f);
                fclose(f);
            }
        }
    }
}

void Chip27xx::update(float aTick) 
{
    int i;
    for (i = 0; i < 8; i++)
        if (mAddressPin[i].mNet)
        mOutputPin[i].setState((mAddressPin[i].mNet->mState != NETSTATE_LOW)?PINSTATE_WRITE_HIGH:PINSTATE_WRITE_LOW);

    // 1. if enable pins are bad, go bad
    
    if (mEnablePin[0].mNet == NULL ||
        mEnablePin[1].mNet == NULL ||
        mEnablePin[0].mNet->mState == NETSTATE_INVALID ||
        mEnablePin[1].mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 8; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    // 2. if enable pins are down, go NC

    if (mEnablePin[0].mNet->mState != NETSTATE_LOW ||
        mEnablePin[1].mNet->mState != NETSTATE_LOW)
    {
        for (i = 0; i < 8; i++)
            mOutputPin[i].setState(NETSTATE_NC);
        return;
    }

    // 3. if input pins are bad, go bad

    int valid = 1;
    int range = 10;
    if (mType > 8) range++;
    if (mType > 16) range++;
    for (i = 0; i < range; i++)
        if (mAddressPin[i].mNet == NULL || mAddressPin[i].mNet->mState == NETSTATE_INVALID)
            valid = 0;

    if (!valid)
    {
        for (i = 0; i < 8; i++)
            mOutputPin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

    // 4. do the thing

    int address = 0;

    for (i = 0; i < range; i++)
        address |= (mAddressPin[i].mNet->mState != NETSTATE_LOW)?(1 << i):0;

    int value = mMemory[address];

    for (i = 0; i < 8; i++)
        mOutputPin[i].setState((value & (1 << i)) ? PINSTATE_WRITE_HIGH : PINSTATE_WRITE_LOW);
}    


void Chip27xx::serialize(File *f)
{
    int l;
    l = mMemSize - 1;
    while (l > 0 && mMemory[l] == 0) l--;
    l++;
    f->writeint(l);
    int i;
    for (i = 0; i < l; i++)
        f->writebyte(mMemory[i]);
}

void Chip27xx::deserialize(File *f)
{
    memset(mMemory,0,mMemSize);
    int l = f->readint();
    int i;
    for (i = 0; i < l; i++)
        mMemory[i] = f->readbyte();
}

void Chip27xx::clone(Chip *aOther)
{
    Chip27xx *d = (Chip27xx *)aOther;
    // Copy of the memory should be enough..
    memcpy(d->mMemory, mMemory, mMemSize);
}
