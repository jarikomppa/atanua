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
#include "chip7489.h"

Chip7489::Chip7489()
{
    set(0,0,4.54,2.25,"64-bit Random Access Read/Write Memory");
    float xpos = 0.15 + 0.54;
    mPin.push_back(&mAddressPin[1]);
    mAddressPin[1].set(xpos , 0, this, "Pin 15:A1"); xpos += 0.54;
    mPin.push_back(&mAddressPin[2]);
    mAddressPin[2].set(xpos , 0, this, "Pin 14:A2"); xpos += 0.54;
    mPin.push_back(&mAddressPin[3]);
    mAddressPin[3].set(xpos , 0, this, "Pin 13:A3"); xpos += 0.54;
    mPin.push_back(&mDataPin[3]);
    mDataPin[3].set(xpos , 0, this, "Pin 12:D4"); xpos += 0.54;
    mPin.push_back(&mSensePin[3]);
    mSensePin[3].set(xpos , 0, this, "Pin 11:S4"); xpos += 0.54;
    mPin.push_back(&mDataPin[2]);
    mDataPin[2].set(xpos , 0, this, "Pin 10:D3"); xpos += 0.54;
    mPin.push_back(&mSensePin[2]);
    mSensePin[2].set(xpos , 0, this, "Pin 9:S3"); xpos += 0.54;
    
    xpos = 0.15;

    mPin.push_back(&mAddressPin[0]);
    mAddressPin[0].set(xpos , 1.75, this, "Pin 1:A0"); xpos += 0.54;
    mPin.push_back(&mEnablePin[0]);
    mEnablePin[0].set(xpos , 1.75, this, "!ME"); xpos += 0.54;
    mPin.push_back(&mEnablePin[1]);
    mEnablePin[1].set(xpos , 1.75, this, "!WE"); xpos += 0.54;
    mPin.push_back(&mDataPin[0]);
    mDataPin[0].set(xpos , 1.75, this, "D1"); xpos += 0.54;
    mPin.push_back(&mSensePin[0]);
    mSensePin[0].set(xpos , 1.75, this, "S1"); xpos += 0.54;
    mPin.push_back(&mDataPin[1]);
    mDataPin[1].set(xpos , 1.75, this, "D2"); xpos += 0.54;
    mPin.push_back(&mSensePin[1]);
    mSensePin[1].set(xpos , 1.75, this, "S2"); xpos += 0.54;

    mTexture = load_texture(DATADIR "/chip_16pin.png");
    
    int i;
    for (i = 0; i < 16; i++)
    	mData[i] = 0;
}

void Chip7489::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring("74LS89",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip7489::update(float aTick) 
{
	    int i;
    if (mEnablePin[0].mNet == NULL ||
		mEnablePin[1].mNet == NULL ||
		mAddressPin[0].mNet == NULL ||
		mAddressPin[1].mNet == NULL ||
		mAddressPin[2].mNet == NULL ||
		mAddressPin[3].mNet == NULL ||
		mDataPin[0].mNet == NULL ||
		mDataPin[1].mNet == NULL ||
		mDataPin[2].mNet == NULL ||
		mDataPin[3].mNet == NULL ||
		mEnablePin[0].mNet->mState == NETSTATE_INVALID ||
		mEnablePin[1].mNet->mState == NETSTATE_INVALID ||
		mDataPin[0].mNet->mState == NETSTATE_INVALID ||
		mDataPin[1].mNet->mState == NETSTATE_INVALID ||
		mDataPin[2].mNet->mState == NETSTATE_INVALID ||
		mDataPin[3].mNet->mState == NETSTATE_INVALID ||
		mAddressPin[0].mNet->mState == NETSTATE_INVALID ||
		mAddressPin[1].mNet->mState == NETSTATE_INVALID ||
		mAddressPin[2].mNet->mState == NETSTATE_INVALID ||
		mAddressPin[3].mNet->mState == NETSTATE_INVALID)
    {
        for (i = 0; i < 4; i++)
            mSensePin[i].setState(gConfig.mPropagateInvalidState);
        return;
    }

	if (mEnablePin[0].mNet->mState != NETSTATE_LOW ||
		mEnablePin[1].mNet->mState == NETSTATE_LOW)
	{
        for (i = 0; i < 4; i++)
			mSensePin[i].setState(PINSTATE_HIGHZ);
	}

	if (mEnablePin[0].mNet->mState != NETSTATE_LOW)
		return;

	int address = 0;

	for (i = 0; i < 4; i++)
		address |= (mAddressPin[i].mNet->mState != NETSTATE_LOW) ? (1 << i) : 0;

	if (mEnablePin[1].mNet->mState == NETSTATE_LOW)
	{
		// memory enable 0 and write enable 0 - write mode

		int data = 0;

		for (i = 0; i < 4; i++)
			data |= (mDataPin[i].mNet->mState != NETSTATE_LOW) ? (1 << i) : 0;
		
		mData[address] = data;
//		return;
	}

	// Otherwise, read mode.

    int data = mData[address];

	// Note: output is complement of data stored in memory!

    for (i = 0; i < 4; i++)
        mSensePin[i].setState((data & (1 << i)) ? PINSTATE_WRITE_LOW : PINSTATE_WRITE_HIGH);

}    
