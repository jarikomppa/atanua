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
#include "extrapin.h"

ExtraPin::ExtraPin()
{
    set(0, 0, 1, 1,NULL);
    mPin.push_back(&mInputPin);
    mInputPin.set((mW-0.5)/2, (mH-0.5)/2, this, "Just a pin");

	mInputPin.mReadOnly = 1;
}

void ExtraPin::render(int aChipId)
{
    if (gUIState.hotitem == aChipId)
        drawrect(mX,mY,mW,mH,0x7f3f3f7f);
}

void ExtraPin::update(float aTick) 
{
}    
