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
#include "stepper.h"
#include "fileutils.h"


StepperMotor::StepperMotor(int aMode)
{
    if (aMode == 0)
    {
        mUnipolar = 1;
        set(0,0,5,5,"Ideal 6-wire unipolar\n"
                    "TTL-voltage stepper motor.\n"
                    "- Common wires are not simulated.\n"
                    "- Full and half step modes work.\n"
                    "- 200 half-steps per rotation");
    }
    else
    {
        mUnipolar = 0;
        set(0,0,5,5,"Ideal 4-wire bipolar\n"
                    "TTL-voltage stepper motor.\n"
                    "- Full step mode works.\n"
                    "- 100 steps per rotation");
    }
    float xpos = 1.25;
    mPin.push_back(&mCoilPin[3]);
    mCoilPin[3].set(xpos, 4.5, this, "4"); xpos += 1;    
    mPin.push_back(&mCoilPin[2]);
    mCoilPin[2].set(xpos, 4.5, this, "3"); xpos += 1;    
    mPin.push_back(&mCoilPin[1]);
    mCoilPin[1].set(xpos, 4.5, this, "2"); xpos += 1;    
    mPin.push_back(&mCoilPin[0]);
    mCoilPin[0].set(xpos, 4.5, this, "1"); xpos += 1;    

    mCurrentBits = 3;
    mCurrentAngle = 0;
    mTexture = load_texture("data/stepper.png");    
}

void drawarrow(float x, float y, float angle, int color)
{
    glColor4f(((color >> 16) & 0xff) / 256.0f,
              ((color >> 8) & 0xff) / 256.0f,
              ((color >> 0) & 0xff) / 256.0f,
              ((color >> 24) & 0xff) / 256.0f);
    glPushMatrix();
    glTranslatef(x,y,0);
    glRotatef(angle,0,0,1);
    glBegin(GL_TRIANGLE_STRIP);
      glVertex2f(-0.1,0.2);
      glVertex2f(0.1,0.2);
      glVertex2f(-0.1,0.8);
      glVertex2f(0.1,0.8);

      glVertex2f(-0.3,0.8);
      glVertex2f(0.3,0.8);
      glVertex2f(0,1.2);
    glEnd();
    glPopMatrix();
}


void StepperMotor::render(int aChipId)
{
    drawtexturedrect(mTexture,mX+0.25,mY,mW,mH,0xffffffff);
    char temp[32];
    sprintf(temp,"%03.1f'",mCurrentAngle / 10.0f);
    float w,h,ll;
    fn.stringmetrics(temp,w,h,ll,0.75);
    fn.drawstring(temp,mX+4.5-w,mY-0.4,0xff00ff00,0.75);

    float angle = mCurrentAngle / 10.0f;//(mCurrentAngle / 3600.0f) * M_PI * 2;
    drawarrow(mX+2.9,mY+1.9,angle+180.0f,0x7f000000);
}

void StepperMotor::update(float aTick) 
{
    if (mCoilPin[0].mNet == NULL ||
        mCoilPin[1].mNet == NULL ||
        mCoilPin[2].mNet == NULL ||
        mCoilPin[3].mNet == NULL ||
        mCoilPin[0].mNet->mState == NETSTATE_INVALID ||
        mCoilPin[1].mNet->mState == NETSTATE_INVALID ||
        mCoilPin[2].mNet->mState == NETSTATE_INVALID ||
        mCoilPin[3].mNet->mState == NETSTATE_INVALID)
        return;

    static int legal_unipolar[] =
    {
     1, //1000
     3, //1100
     2, //0100
     6, //0110
     4, //0010
     12,//0011
     8, //0001
     9  //1001
    };

    static int legal_bipolar[] =
    {
     10,//0101
     6, //0110
     5, //1010
     9  //1001
    };

    int a = ((mCoilPin[3].mNet->mState != NETSTATE_LOW)?(1 << 3):0) |
            ((mCoilPin[2].mNet->mState != NETSTATE_LOW)?(1 << 2):0) |
            ((mCoilPin[1].mNet->mState != NETSTATE_LOW)?(1 << 1):0) |
            ((mCoilPin[0].mNet->mState != NETSTATE_LOW)?(1 << 0):0);

    if (mUnipolar)
    {
        if (legal_unipolar[(mCurrentBits+8-1)&7] == a)
        {
            mCurrentAngle -= 18;
            mCurrentBits = (mCurrentBits+8-1)&7;
        } else
        if (legal_unipolar[(mCurrentBits+8-2)&7] == a)
        {
            mCurrentAngle -= 36;
            mCurrentBits = (mCurrentBits+8-2)&7;    
        } else
        if (legal_unipolar[(mCurrentBits+2)&7] == a)
        {
            mCurrentAngle += 36;
            mCurrentBits = (mCurrentBits+2)&7;
        } else
        if (legal_unipolar[(mCurrentBits+1)&7] == a)
        {
            mCurrentAngle += 18;
            mCurrentBits = (mCurrentBits+1)&7;
        }
    }
    else
    {
        // bipolar
        if (legal_bipolar[(mCurrentBits+4-1)&3] == a)
        {
            mCurrentAngle -= 36;
            mCurrentBits = (mCurrentBits+4-1)&3;
        } else
        if (legal_bipolar[(mCurrentBits+1)&3] == a)
        {
            mCurrentAngle += 36;
            mCurrentBits = (mCurrentBits+1)&3;
        }
    }

    while (mCurrentAngle < 0) mCurrentAngle += 3600;
    while (mCurrentAngle >= 3600) mCurrentAngle -= 3600;
	mDirty = 1;
}    
