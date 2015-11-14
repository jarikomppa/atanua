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
#include "and8chip.h"

AND8Chip::AND8Chip(int aUS)
{
    set(0,0,4,4,NULL);
    mPin.push_back(&mInputPin[0]);
    mPin.push_back(&mInputPin[1]);
    mPin.push_back(&mInputPin[2]);
    mPin.push_back(&mInputPin[3]);
    mPin.push_back(&mInputPin[4]);
    mPin.push_back(&mInputPin[5]);
    mPin.push_back(&mInputPin[6]);
    mPin.push_back(&mInputPin[7]);
    mPin.push_back(&mOutputPin);

	float ypos = -0.05;
	float yadd = 0.5;

    mInputPin[0].set(0, ypos, this, "Input A"); ypos += yadd;
    mInputPin[1].set(0, ypos, this, "Input B"); ypos += yadd;
    mInputPin[2].set(0, ypos, this, "Input C"); ypos += yadd;
    mInputPin[3].set(0, ypos, this, "Input D"); ypos += yadd;
    mInputPin[4].set(0, ypos, this, "Input E"); ypos += yadd;
    mInputPin[5].set(0, ypos, this, "Input F"); ypos += yadd;
    mInputPin[6].set(0, ypos, this, "Input G"); ypos += yadd;
    mInputPin[7].set(0, ypos, this, "Input H");
    mOutputPin.set(3.5, 1.75, this, "Output");
    if (aUS)
        mTexture = load_texture(DATADIR "/and8_us.png");
    else
        mTexture = load_texture(DATADIR "/and8_fi.png");

	int i;
	for (i = 0; i < 8; i++)
		mInputPin[i].mReadOnly = 1;

}

void AND8Chip::render(int aChipId)
{
	if (gBlackBackground)
	    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
	else
		drawtexturedrect(mTexture,mX,mY,mW,mH,0xff000000);
}

void AND8Chip::update(float aTick) 
{

	int i;

	for (i = 0; i < 8; i++)
	{
		if (mInputPin[i].mNet == NULL ||
			mInputPin[i].mNet->mState == NETSTATE_INVALID)
		{
			mOutputPin.setState(gConfig.mPropagateInvalidState);
			return;
		}
	}

	for (i = 0; i < 8; i++)
	{
		if (!(mInputPin[i].mNet->mState == NETSTATE_HIGH || mInputPin[i].mNet->mState == NETSTATE_NC))
		{
			mOutputPin.setState(PINSTATE_WRITE_LOW);
			return;
		}
	}
	mOutputPin.setState(PINSTATE_WRITE_HIGH);	
}    
