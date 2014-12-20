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
#ifndef SEVENSEGCHIP_H
#define SEVENSEGCHIP_H

class SlidingAverage;

class SevenSegChip : public Chip
{
    int mColor;       // Color for the LED. 24bit value
    Pin mInputPin[8]; // Chip's input pins
    int mTexture[9];  // Chip's textures
    int mInverse;     // use inverse logic
	SlidingAverage *mAvg;
public:
    SevenSegChip(int aColor, int aInverse);
	~SevenSegChip();

    virtual void render(int aChipId);
    virtual void update(float aTick);
	virtual int isUselessInBox() { return 1; }
};

#endif
