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
#include "switchchip.h"

SwitchChip::SwitchChip(int aCharacter)
{
    mCharacter = aCharacter;
    set(0,0,2,2.5,NULL);
    mPin.push_back(&mOutputPin);
    mOutputPin.set(0.75, 1.9, this, "Output");
    mTexture = load_texture(DATADIR "/switch.png");
    mPressed = 0;
	mSwitchedOn = 0;
}

void SwitchChip::render(int aChipId)
{
	
    if (gUIState.activeitem == aChipId)
	{
		if (mPressed == 0)
		{
			mPressed = 1;
			mSwitchedOn = !mSwitchedOn;
		}
	}
	else
	{
		mPressed = 0;
	}

	if (gUIState.keyentered == mCharacter)
    {
        mSwitchedOn = !mSwitchedOn;
    }

    glBindTexture(GL_TEXTURE_2D, mTexture);
    glEnable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);
    glBegin(GL_TRIANGLE_STRIP);
      glTexCoord2f(0, 1 - mSwitchedOn);
      glVertex2f(mX, mY);
      glTexCoord2f(0, mSwitchedOn);
      glVertex2f(mX, mY + mH - 0.5);
      glTexCoord2f(1, 1 - mSwitchedOn);
      glVertex2f(mX + mW, mY);
      glTexCoord2f(1, mSwitchedOn);
      glVertex2f(mX + mW, mY + mH - 0.5);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    if (mCharacter == 32)
	{
		fn.drawstring("Spc",mX+0.7,mY+0.7 + 0.2 * mSwitchedOn,0xff000000,0.4);
	}
	else
	if (mCharacter == SDLK_LSHIFT)
	{
		fn.drawstring("Shift",mX+0.6,mY+0.7 + 0.2 * mSwitchedOn,0xff000000,0.4);
	}
	else
	if (mCharacter == SDLK_RETURN)
	{
		fn.drawstring("Ret",mX+0.7,mY+0.7 + 0.2 * mSwitchedOn,0xff000000,0.4);
	}
	else
	{
		char temp[2];
		temp[0] = mCharacter;
		temp[1] = 0;
		fn.drawstring(temp, mX + 0.75, mY + 0.5 + 0.2 * mSwitchedOn, 0xff000000, 1); 
	}
}

void SwitchChip::update(float aTick) 
{
    if (mSwitchedOn)
        mOutputPin.setState(PINSTATE_WRITE_HIGH);
    else
        mOutputPin.setState(PINSTATE_WRITE_LOW);
	mDirty = 1;
}    
