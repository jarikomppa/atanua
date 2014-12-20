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
#include "buttonchip.h"

ButtonChip::ButtonChip(int aCharacter)
{
    mCharacter = aCharacter;
    set(0,0,2,2,NULL);
    mPin.push_back(&mOutputPin);
    mOutputPin.set(0.75, 1.5, this, "Output");
    mTexture = load_texture("data/button.png");
    mPressed = 0;
}

void ButtonChip::render(int aChipId)
{
    if (gUIState.activeitem == aChipId)
    {
        mPressed = 1;
    }
    else
    {
        mPressed = 0;
    }
    drawtexturedrect(mTexture, mX, mY, mW, mH, 0xffffffff);
    if (mCharacter == 32)
	{
		fn.drawstring("Spc",mX+0.4,mY+0.5,0xff000000,0.7);
	}
	else
	if (mCharacter == SDLK_LSHIFT)
	{
		fn.drawstring("Shift",mX+0.3,mY+0.5,0xff000000,0.7);
	}
	else
	if (mCharacter == SDLK_RETURN)
	{
		fn.drawstring("Ret",mX+0.5,mY+0.5,0xff000000,0.7);
	}
	else
	{
		char temp[2];
		temp[0] = mCharacter;
		temp[1] = 0;
		fn.drawstring(temp,mX+0.75,mY+0.25,0xff000000,1);
	}
}

void ButtonChip::update(float aTick) 
{
    if (gKeyState[mCharacter] || mPressed)
        mOutputPin.setState(PINSTATE_WRITE_HIGH);
    else
        mOutputPin.setState(PINSTATE_WRITE_LOW);
	mDirty = 1;
}    
