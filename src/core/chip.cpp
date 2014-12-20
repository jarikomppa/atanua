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


Chip::Chip() 
{ 
	mDirty = 1;
	mBox = 0;
    mRotatedX = mX = 0; 
    mRotatedY = mY = 0; 
    mRotatedW = mW = 2.5; 
    mRotatedH = mH = 3;
    mKey = SDL_GetTicks();
    mTooltip = NULL;
    mMultiSelectState = 0;
    mAngleIn90DegreeSteps = 0;
}

Chip::~Chip() 
{
    mPin.clear();
}

void Chip::set(float aX, float aY, float aW, float aH, const char *aTooltip)
{
    mRotatedX = mX = aX;
    mRotatedY = mY = aY;
    mRotatedW = mW = aW;
    mRotatedH = mH = aH;
    mTooltip = aTooltip;
}

void Chip::render(int aChipId) 
{ 
    drawrect(mX+0.25, mY, mW-0.5, mH, 0xff9f9f9f); 
    drawrect(mX+0.45, mY+0.2, mW-0.9, mH-0.4, 0xffbfbfbf); 
    fn.drawstring("?",mX+1,mY+0.5,0xff000000,1); 
//    if (gZoomFactor > 10) // TODO: implement LOD thingies for these
        fn.drawstring("Undefined\nchip design",mX+1,mY+1.5,0xff000000,0.1); 
}

void Chip::rotate(int aIn90DegreeSteps)
{
    mAngleIn90DegreeSteps = aIn90DegreeSteps;
    float radians = (M_PI / 2) * aIn90DegreeSteps;

    if (aIn90DegreeSteps & 1)
    {
        mRotatedW = mH;
        mRotatedH = mW;
    }
    else
    {
        mRotatedW = mW;
        mRotatedH = mH;
    }
    mRotatedX = mX + mW / 2 - mRotatedW / 2;
    mRotatedY = mY + mH / 2 - mRotatedH / 2;

    int i;
    for (i = 0; i < (signed)mPin.size(); i++)
    {
        // pin upper corner moved to chip origin offset and pin origin offset (0.25)
        mPin[i]->mRotatedX = mPin[i]->mX - (mW / 2) + 0.25; 
        mPin[i]->mRotatedY = mPin[i]->mY - (mH / 2) + 0.25;
        // rotated
        rotate2d(mPin[i]->mRotatedX, mPin[i]->mRotatedY, radians);
        // and moved back
        mPin[i]->mRotatedX += mRotatedW / 2 - 0.25;
        mPin[i]->mRotatedY += mRotatedH / 2 - 0.25;
    }
}

void Chip::update(float aTick)
{
}

void Chip::serialize(File *f)
{
}

void Chip::deserialize(File *f)
{
}

void Chip::clone(Chip *aOther)
{
}
