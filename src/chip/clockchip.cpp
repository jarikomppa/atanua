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
#include "clockchip.h"

ClockChip::ClockChip(float aFreqHz)
{
    mReqFreq = aFreqHz;
    mMsPerEvent = 500.0 / aFreqHz;
	mMsBucket = 0;
	mLastTick = 0;
    mLastState = -1;
    set(0, 0, 2, 2.25,NULL);
    mPin.push_back(&mOutputPin);
    mOutputPin.set(0.75, 1.75, this, "Output");
    mTexture = load_texture("data/clock.png");
	if (aFreqHz*2/1000 > gConfig.mPhysicsKHz)
		mNagOnce = 1;
	else
		mNagOnce = 0;
}

void ClockChip::render(int aChipId)
{
	// Display nag here instead of ctor so tooltip won't show it
	if (mNagOnce)
	{
		okcancel("Note:\n"
			     "\n"
				 "Atanua's current simulation clock rate\n"
			     "is too low for this clock chip.\n"
				 "\n"
				 "Edit atanua.xml and set the PhysicsKHz\n"
				 "higher if you wish to use this part.\n"
				 "\n"
				 "Currently the part will only run at the\n"
				 "maximum simulated clock.\n"
				 "\n"
				 "The simulation clock must be twice as\n"
				 "fast as the desired clock chip rate.");
		mNagOnce = 0;
	}
    drawtexturedrect(mTexture,mX,mY,mW,mH-0.25,0xffffffff);
    fn.drawstring("Clock",mX+0.5,mY+0.4,0x7f000000,0.3);
    char temp[64];
    sprintf(temp, "%3.1fHz", mReqFreq);
    fn.drawstring(temp,mX+0.5,mY+0.7,0x7f000000,0.25);
	float timepos = mMsBucket / mMsPerEvent;
    drawrect(mX+0.4+(mW-0.85)*timepos,mY+1,0.05,0.2,0x7f000000);
    drawrect(mX+0.4,mY+1,mW-0.8,0.2,(!mLastState)?0x3f000000:0x3fffffff);
}

void ClockChip::update(float aTick)
{
	if (mLastTick > aTick)
		mLastTick = aTick;
	mMsBucket += aTick - mLastTick;
	mLastTick = aTick;

	if (mMsBucket >= mMsPerEvent || mLastState == -1)
    {
        mLastState = !mLastState;
        mOutputPin.setState((mLastState)?PINSTATE_WRITE_HIGH : PINSTATE_WRITE_LOW);
		mMsBucket = 0;
    }
	mDirty = 1;
}
/*
ClockChip::ClockChip(float aFreqHz)
{
    mReqFreq = aFreqHz;
    mMsPerEvent = (int)floor(1000 / aFreqHz);
    mLastState = -1;
    set(0, 0, 2, 2.25,NULL);
    mPin.push_back(&mOutputPin);
    mOutputPin.set(0.75, 1.75, this, "Output");
    mTexture = load_texture("data/clock.png");
    mTimePos = 0;
}

void ClockChip::render(int aChipId)
{
    drawtexturedrect(mTexture,mX,mY,mW,mH-0.25,0xffffffff);
    fn.drawstring("Clock",mX+0.5,mY+0.4,0x7f000000,0.3);
    char temp[64];
    sprintf(temp, "%3.1fHz", mReqFreq);
    fn.drawstring(temp,mX+0.5,mY+0.7,0x7f000000,0.25);
    drawrect(mX+0.4+(mW-0.85)*mTimePos,mY+1,0.05,0.2,0x7f000000);
    drawrect(mX+0.4,mY+1,mW-0.8,0.2,(!mLastState)?0x3f000000:0x3fffffff);
}

void ClockChip::update(float aTick)
{
    // Since two clock state changes make a clock cycle, 
    // make incoming ticks run twice as fast - not really clean fix, but
    // since whatever % 1 == 0, it's the easiest fix.
    int tick = (int)floor(aTick * 2);
    if (mLastState == -1)
        mLastState = (tick / mMsPerEvent) & 1;
    int thistick = tick % mMsPerEvent;
    mTimePos = thistick / (float)mMsPerEvent;
    int lasttick = (tick - 2) % mMsPerEvent;
    if (mMsPerEvent == 2 || thistick < lasttick)
    {
        mLastState = !mLastState;
        mOutputPin.setState((mLastState)?PINSTATE_WRITE_HIGH : PINSTATE_WRITE_LOW);
    }
	mDirty = 1;
}
*/