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
#include "logicprobe.h"

LogicProbe::LogicProbe()
{
    int i, j;
    for (j = 0; j < 8; j++)
    {
        for (i = 0; i < 1000; i++)
        {
            mData[j][i] = 0;
        }
    }
    set(0,0,18,9,"Simple logic probe\n\nClick on the probe to\ngive it keyboard focus, then\npress SPACE to start/stop\nrecording. Zoom in and use\nmouse to analyse data");
    float ypos = 2;
    mPin.push_back(&mInputPin[7]);
    mInputPin[7].set(0, ypos, this, "Bit 7"); ypos += 0.56;
    mPin.push_back(&mInputPin[6]);
    mInputPin[6].set(0, ypos, this, "Bit 6"); ypos += 0.56;
    mPin.push_back(&mInputPin[5]);
    mInputPin[5].set(0, ypos, this, "Bit 5"); ypos += 0.56;
    mPin.push_back(&mInputPin[4]);
    mInputPin[4].set(0, ypos, this, "Bit 4"); ypos += 0.56;
    mPin.push_back(&mInputPin[3]);
    mInputPin[3].set(0, ypos, this, "Bit 3"); ypos += 0.56;
    mPin.push_back(&mInputPin[2]);
    mInputPin[2].set(0, ypos, this, "Bit 2"); ypos += 0.56;
    mPin.push_back(&mInputPin[1]);
    mInputPin[1].set(0, ypos, this, "Bit 1"); ypos += 0.56;
    mPin.push_back(&mInputPin[0]);
    mInputPin[0].set(0, ypos, this, "Bit 0"); ypos += 0.56;

    mTexture = load_texture(DATADIR "/lcd.png");
    mPlayhead = 0;
    mState = 1;
	mLastTick = 0;
	mTimeBucket = 0;

	for (i = 0; i < 8; i++)
		mInputPin[i].mReadOnly = 1;

}

void LogicProbe::render(int aChipId)
{
    int i, j, k;
    drawtexturedrect(mTexture,mX, mY, mW, mH, 0xffffffff);
    float ourmousex = (((gUIState.mousex - gConfig.mToolkitWidth) / gZoomFactor) - gWorldOfsX) - (mX + 1.6);
    float ourmousey = (((gUIState.mousey - 40) / gZoomFactor) - gWorldOfsY) - mY;
    float ystep = ((mH - 5.5) / 8);
    float yht = (ystep * 0.9) / 5;

    char temp[16];
    int data = 0;
    for (i = 0; i < 8; i++)
    {
        if (mData[i][mPlayhead] == 0)
            data |= 1 << i;
    }

    if (ourmousex > 0 &&
        ourmousey > 0 &&
        ourmousex < (mW - 3) &&
        ourmousey < mH)
    {
        int pos = (int)floor((ourmousex * 1000)/(mW - 3));
        int dpos = (((999 - pos) + mPlayhead) % 1000);
        drawrect(mX+1.6+(pos/1000.0f)*(mW-3),mY+2.75,(mW-3)/1000.0f,mH-5.5,0x3f000000);
        for (i = 0; i < 8; i++)
        {
            if (mData[i][dpos] == 0)
                data |= 1 << i;
            drawrect(mX+1.6,mY+2.75 + i * ystep - ystep * 0.05,mW-3,(mW-3)/1000.0f,0x3f000000);
        }
        sprintf(temp, "%4dms",pos);
        fn.drawstring(temp,mX+1.6,mY+2.75+2.5,0x3f000000,1);
    }

    sprintf(temp, "%02X", data);
    fn.drawstring(temp,mX+1.6,mY+2.75,0x3f000000,3);

    //glVertexPointer(2,GL_FLOAT,0,mVtxArray);
    //glEnable(GL_VERTEX_ARRAY);
    glColor4f(0, 1, 0, 0.5f);
    for (j = 0; j < 8; j++)
    {
        glBegin(GL_LINE_STRIP);
        k = mPlayhead;
        for (i = 0; i < 1000; i++)
        {
            //mVtxArray[i*2+0] = mX + 1.6 + (999-i) * ((mW - 3) / 1000.0f);
            //mVtxArray[i*2+1] = mY + 2.75 + (mData[j][k] * yht) + ystep * (7 - j);
            glVertex2f(mX + 1.6 + (999-i) * ((mW - 3) / 1000.0f),
                       mY + 2.75 + (mData[j][k] * yht) + ystep * (7 - j));
            k++;
            k %= 1000;
        }
        glEnd();
      //  glDrawArrays(GL_LINE_STRIP,0,1000);
    }
    //glDisable(GL_VERTEX_ARRAY);
    if (gUIState.kbditem == aChipId)
    {
        if (gUIState.keyentered == ' ')
            mState = !mState;
    }
}

void LogicProbe::update(float aTick) 
{
    int i;
	if (mLastTick > aTick)
		mLastTick = aTick;
	mTimeBucket += aTick - mLastTick;
	mLastTick = aTick;

	if (mTimeBucket < 1.0)
		return;
	mTimeBucket = 0;
	// only record at 1ms
/*
	int tick = floor(aTick);
	if (tick == mLastTick)	
		return;
	mLastTick = tick;
	*/
    if (mState == 1)
    {
        mPlayhead++;
        mPlayhead %= 1000;
        for (i = 0; i < 8; i++)
        {
            if (mInputPin[i].mNet == NULL)
            {
                mData[i][mPlayhead] = 2;
            }
            else
            {
                switch (mInputPin[i].mNet->mState)
                {
                case NETSTATE_HIGH:
                    mData[i][mPlayhead] = 0;
                    break;
                case NETSTATE_LOW:
                    mData[i][mPlayhead] = 5;
                    break;
                default:
                    mData[i][mPlayhead] = 2;
                }
            }
        }
    }
	mDirty = 1;
}    
