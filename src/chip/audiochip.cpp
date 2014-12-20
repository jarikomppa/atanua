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
#include "audiochip.h"
#include "fileutils.h"


AudioChip::AudioChip()
{
    set(0,0,6.5,6,"8-pin audio DAC\n"
                    "modelled after the\n"
                    "COVOX Speech Thing.\n\n"
                    "If no audio is heard,\n"
                    "make sure audio is not\n"
                    "disabled in atanua.xml");
    float ypos = 1.5;
    mPin.push_back(&mPinA[7]);
    mPinA[7].set(0, ypos, this, "A7"); ypos += 0.5;    
    mPin.push_back(&mPinA[6]);
    mPinA[6].set(0, ypos, this, "A6"); ypos += 0.5;    
    mPin.push_back(&mPinA[5]);
    mPinA[5].set(0, ypos, this, "A5"); ypos += 0.5;    
    mPin.push_back(&mPinA[4]);
    mPinA[4].set(0, ypos, this, "A4"); ypos += 0.5;    
    mPin.push_back(&mPinA[3]);
    mPinA[3].set(0, ypos, this, "A3"); ypos += 0.5;    
    mPin.push_back(&mPinA[2]);
    mPinA[2].set(0, ypos, this, "A2"); ypos += 0.5;    
    mPin.push_back(&mPinA[1]);
    mPinA[1].set(0, ypos, this, "A1"); ypos += 0.5;    
    mPin.push_back(&mPinA[0]);
    mPinA[0].set(0, ypos, this, "A0"); ypos += 0.5;    

    mTexture = load_texture("data/audio.png");    

	int i;
	for (i = 0; i < 8; i++)
		mPinA[i].mReadOnly = 1;	
}

static int gAudioStarted = 0;

void AudioChip::render(int aChipId)
{
    if (!gAudioStarted)
    {
        // Moved audio start here so that the tooltip from toolbox does not cause audio to start
        SDL_PauseAudio(0);
        gAudioStarted++;
    }

    drawtexturedrect(mTexture,mX+0.25,mY,mW-0.5,mH,0xffffffff);
}

extern unsigned char *gAudioOut;

void AudioChip::update(float aTick) 
{
    int data = 0;
    int i;
    for (i = 0; i < 8; i++)
    {
        if (mPinA[i].mNet && mPinA[i].mNet->mState == NETSTATE_HIGH)
            data |= (1 << i);
    }
    *gAudioOut = data;
	mDirty = 1;
}    
