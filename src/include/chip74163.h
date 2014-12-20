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
#ifndef CHIP74163_H
#define CHIP74163_H

class Chip74163 : public Chip
{
    Pin mRippleCarryPin;
    Pin mOutputPin[4];
    Pin mEnableTPin;
    Pin mLoadPin;
    Pin mClearPin;
    Pin mClockPin;
    Pin mInputPin[4];
    Pin mEnablePPin;
    int mTexture;
    int mOldClock;
    int mValue;
public:
    Chip74163(); // Ctor

    virtual void render(int aChipId);
    virtual void update(float aTick);
};

#endif
