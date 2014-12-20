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
#include "fileutils.h"
#include "label.h"

Label::Label(float aSize)
{
    set(0, 0, aSize, aSize, NULL);
    mData = "A";
    mSize = aSize;
}

void Label::render(int aChipId)
{
    if (gUIState.kbditem == aChipId)
    {
        if (gUIState.keyentered)
        {
		    switch (gUIState.keyentered)
		    {
		    case SDLK_BACKSPACE:
                if (mData.length())
                    mData.resize(mData.length()-1);
			    break;			
		    }
		    if ((gUIState.keychar >= 32 && gUIState.keychar < 127) || gUIState.keychar == '\r' || gUIState.keychar == '\n')
		    {
                if (gUIState.keychar == '\r')
                    gUIState.keychar = '\n';
                mData.push_back(gUIState.keychar);
		    }
        }

        float w,h,llw;
        fn.stringmetrics(mData.c_str(),w,h,llw,mSize);
        drawrect(mX, mY - mW * 0.1 + h, w, mH * 0.1, 0x7f7f0000);
        drawrect(mX, mY, mW * 0.1, h, 0x7f7f0000);
        
        fn.drawstring(mData.c_str(),mX,mY,0xffbfffbf,mSize);
        
        if ((SDL_GetTicks() >> 9) & 1)
            drawrect(mX + llw, mY + h - mH, mW * 0.05, mH, 0xffffffff);
    }
    else
    {
        fn.drawstring(mData.c_str(),mX,mY,0xff00bf00,mSize);
    }
}

void Label::serialize(File *f)
{
    f->writeint((int)(mSize * 0xffffff));
    f->writeint(mData.length());
    f->writechars(mData.c_str(), mData.length());
}

void Label::deserialize(File *f)
{
    mSize = f->readint() / (float)0xffffff;
    int l = f->readint();
    char *temp = new char[l+1];
    memset(temp,0,l+1);
    f->readchars(temp, l);
    mData = temp;
    delete[] temp;
}

void Label::clone(Chip *aOther)
{
    Label *d = (Label *)aOther;
    d->mData = mData;
}
