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
#include "sedchip.h"

SEDChip::SEDChip()
{
    int i;
    mColor = (gVisualRand.genrand_int31() & 0xffffff) | 0x7f7f7f;
    set(0, 0, 1, 2.5, "Smoke Emitting Diode");
    mPin.push_back(&mInputPin);
    mInputPin.set((mW-0.5)/2, mH-0.5, this, "Input");
    mBaseTexture = load_texture(DATADIR "/sed.png");
    mFlareTexture = load_texture(DATADIR "/sedr.png");
    for (i = 0; i < 32; i++)
        mParticle[i].live = 0;

	mInputPin.mReadOnly = 1;
}

void SEDChip::render(int aChipId)
{
    int i;
    drawtexturedrect(mBaseTexture,mX, mY, mW, mH-0.5, mColor | 0xff000000);
    
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_ONE);
    for (i = 0; i < 32; i++)
    {
        if (mParticle[i].live)
        {
            int xparency = (abs(mParticle[i].live - 1500) * 0x7f) / 1500 + 0x7f;
            drawtexturedrect(mFlareTexture, mParticle[i].x-1, mParticle[i].y-1, 2, 2, 0x9f9fff | (xparency << 24)); 
        }
    }
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);    
}

void SEDChip::update(float aTick) 
{    
    int tick = (int)floor(aTick);
    
    if ((tick & 255) == 0 && (mInputPin.mNet && mInputPin.mNet->mState == NETSTATE_HIGH))
    {
        int p = gPhysicsRand.genrand_int31() & 31;
        if (mParticle[p].live == 0)
        {
            mParticle[p].live = 2000 + (gPhysicsRand.genrand_int31() % 1000);
            mParticle[p].x = mX + mW / 2;
            mParticle[p].y = mY + 0.4;
            mParticle[p].dirx = ((gPhysicsRand.genrand_int31() % 100) - 50) / 100000.0f;
            mParticle[p].diry = 0;
        }
    }

    int i;
    for (i = 0; i < 32; i++)
    {
        if (mParticle[i].live)
        {
            mParticle[i].live--;
            mParticle[i].x += mParticle[i].dirx;
            mParticle[i].y += mParticle[i].diry;
            mParticle[i].diry -= 0.000002;
        }
    }
}    
