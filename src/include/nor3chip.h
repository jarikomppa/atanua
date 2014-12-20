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
#ifndef NOR3CHIP_H
#define NOR3CHIP_H

class NOR3Chip : public Chip
{
    Pin mInputPinA; // Input pin A for this chip
    Pin mInputPinB; // Input pin B for this chip
    Pin mInputPinC; // Input pin C for this chip
    Pin mOutputPin; // Output pin for this chip
    int mTexture;
public:
    NOR3Chip(int aUS); // Ctor

    virtual void render(int aChipId);
    virtual void update(float aTick);
};

#endif
