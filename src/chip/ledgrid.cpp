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
#include "ledgrid.h"
#include "slidingaverage.h"

LedGrid::LedGrid(int aSize, int aColor, int aInverse)
{
    mInverse = aInverse;
	mSize = aSize;
    mColor = aColor & 0xffffff;
	mAvg = new SlidingAverage[aSize*aSize];
    set(0, 0, mSize + 0.5, mSize, NULL);

	mInputPin = new Pin[mSize * 2];

	int i;
	for (i = 0; i < mSize * 2; i++)
		mPin.push_back(&mInputPin[i]);

	mPinDescription = new char*[mSize * 2];

	char temp[128];

	float ypos = 0.2;
	float step = 1;
	for (i = 0; i < mSize; i++)
	{
		sprintf(temp,"Pin %d: Row %d", i+1, i+1);
		mPinDescription[i] = mystrdup(temp);
		mInputPin[i].set(0,i * step + ypos,this,mPinDescription[i]);

		sprintf(temp,"Pin %d: Col %d", i+1+mSize, i+1);
		mPinDescription[i+mSize] = mystrdup(temp);
		mInputPin[i+mSize].set(mSize, (mSize - i - 1) * step + ypos,this,mPinDescription[i+mSize]);
	}

	mBaseTexture = load_texture("data/ledgrid_base.png");
	mLitTexture = load_texture("data/ledgrid_lit.png");

	for (i = 0; i < mSize * 2; i++)
		mInputPin[i].mReadOnly = 1;
}

LedGrid::~LedGrid()
{ 
	delete[] mAvg;
	delete[] mInputPin;	
	int i;
	for (i = 0; i < mSize * 2; i++)
		delete[] mPinDescription[i];
	delete[] mPinDescription;
}

void LedGrid::render(int aChipId)
{
    //drawtexturedrect(mBaseTexture, mX, mY, mW, mH, 0xffffffff); 

    glBindTexture(GL_TEXTURE_2D, mBaseTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glEnable(GL_TEXTURE_2D);
    glColor4f(1,1,1,1);
    glBegin(GL_TRIANGLE_STRIP);
      glTexCoord2f(0,0);
      glVertex2f(mX+0.25,mY);
      glTexCoord2f(0,mSize/2);
      glVertex2f(mX+0.25,mY+mH);
      glTexCoord2f(mSize/2,0);
      glVertex2f(mX+0.25+mW-0.5,mY);
      glTexCoord2f(mSize/2,mSize/2);
      glVertex2f(mX+0.25+mW-0.5,mY+mH);
    glEnd();
    glDisable(GL_TEXTURE_2D);

	
	glBlendFunc(GL_ONE,GL_ONE);

    int i, j, c;
    for (i = 0, c = 0; i < mSize; i++)
	{
		for (j = 0; j < mSize; j++, c++)
		{
			float val = mAvg[c].getAverage();
			if (val > 0)
			{
				if (val > 0.1)
					val = 1.0;
				else
					val *= 10;
				int col = ((int)(((mColor >>  0) & 0xff) * val) <<  0) |
						  ((int)(((mColor >>  8) & 0xff) * val) <<  8) |
						  ((int)(((mColor >> 16) & 0xff) * val) << 16);

				drawtexturedrect(mLitTexture, mX+0.25+j, mY+i, 1, 1, 0xff000000 | col); 
			}
		}
	}
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
}

void LedGrid::update(float aTick) 
{
    int i, j, c;
    for (i = 0, c = 0; i < mSize; i++)
	{
		for (j = 0; j < mSize; j++, c++)
		{
			mAvg[c].setValue((mInputPin[i].mNet && ((mInputPin[i].mNet->mState == NETSTATE_HIGH) ^ (!!mInverse))) &&
				             (mInputPin[j+mSize].mNet && ((mInputPin[j+mSize].mNet->mState == NETSTATE_LOW) ^ (!!mInverse))));
		}
	}
	mDirty = 1;
}    
