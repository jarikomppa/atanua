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
Pin::Pin() 
{ 
    mRotatedX = mRotatedY = mX = mY = 0; 
    mHost = 0; 
    mState = PINSTATE_READ; 
    mNet = NULL; 
    mTooltip = NULL;
	mReadOnly = 0;
}

Pin::Pin(float aX, float aY, Chip *aHost, const char *aTooltip)
{
    mRotatedX = mX = aX;
    mRotatedY = mY = aY;
    mHost = aHost;
    mState = PINSTATE_READ;
    mTooltip = aTooltip;
    mNet = NULL;
	mNetId = 0;
}

void Pin::set(float aX, float aY, Chip *aHost, const char *aTooltip)
{
    mRotatedX = mX = aX;
    mRotatedY = mY = aY;
    mHost = aHost;
	mNetId = 0;
    mTooltip = aTooltip;
}

void Pin::setState(int aState)
{
	if (mState != aState && mNet)
		mNet->mDirty = 1;

	mState = aState;
}

int Pin::getState()
{
	return mState;
}
