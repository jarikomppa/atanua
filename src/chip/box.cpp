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
#include "box.h"
#include "fileutils.h"


Box::Box(const char *aFilename, BoxStitchingInformation * aBsi)
{
	gBoxCount++;
	BoxLoadQueueItem * item = new BoxLoadQueueItem;
	item->mBox = this;
	item->mFilename = aFilename;
	item->mBoxId = gBoxCount;
	gBoxLoadQueue.push_back(item);
	

	mBsi = aBsi;
	mPinCount = aBsi->mToolTips.size();
	mExtPin = new Pin[mPinCount];

	mBsi->mExtPinOutside = mExtPin;
	mBoxed = item->mBoxId;

	char tempstr[256];
	tempstr[0] = 0;
	strcat(tempstr, aFilename);
	int i;
	i = strlen(tempstr);
	while (i > 0 && tempstr[i] != '.') i--;
	if (i > 0) tempstr[i] = 0;
	mDisplayString = mystrdup(tempstr);

	float w,h,lln;
	fn.stringmetrics(mDisplayString,w,h,lln,0.75);

	w += 1;
//	h = 1 + mPinCount * 0.5;

    float xposl = 0.5;
	float xposr = 0.5;
	for (i = 0; i < mPinCount; i++)
	{
		mPin.push_back(&mExtPin[i]);
		if (aBsi->mToolTips[i][0] == '<')
		{
			mExtPin[i].set(xposr, 1.5, this, aBsi->mToolTips[i]+1);
			xposr += 0.5;
		}
		else
		{
			if (aBsi->mToolTips[i][0] == '>')
				mExtPin[i].set(xposl, 0, this, aBsi->mToolTips[i]+1);
			else
				mExtPin[i].set(xposl, 0, this, aBsi->mToolTips[i]);
			xposl += 0.5;
		}
	}

	h = (xposr > xposl) ? xposr : xposl;
	h += 0.5;
	if (h > w) w = h;

    set(0,0,w,2,mystrdup(aFilename));

	gActiveBoxes++;
}

Box::~Box()
{
	delete[] mExtPin;
	mExtPin = NULL;
	delete mBsi;
	// unlike every other chip, our tooltip is dynamically loaded.
	delete[] mTooltip;
	delete[] mDisplayString;

	// Need to fry all boxed components too!

    int found = 1;
    while (found)
    {
        found = 0;
        int i = 0;
        while (i < (signed)gChip.size() && gChip[i]->mBox != mBoxed) i++;
        if (i < (signed)gChip.size())
        {
            found = 1;
            delete_chip(gChip[i]);
        }
    }
	gActiveBoxes--;	
}

void Box::render(int aChipId)
{
    drawrect(mX + 0.25, mY+0.25, mW - 0.5, mH - 0.5, 0xff1f1f1f);
    drawrect(mX + 0.35, mY+0.35, mW - 0.7, mH - 0.7, 0xff3f3f3f);

	float w,h,lln;
	fn.stringmetrics(mDisplayString,w,h,lln,0.75);
	
	fn.drawstring(mDisplayString,mX + (mW-w)/2,mY+0.6,0x6fffffff,0.75);
}

void do_flush_boxloadqueue();

void Box::clone(Chip *aOther)
{
	Box * other = (Box *)aOther;
	other->mTooltip = mystrdup(mTooltip);
	other->mDisplayString = mystrdup(mDisplayString);
	do_flush_boxloadqueue();
	build_nets();
}
