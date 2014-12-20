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

Net::~Net()
{
    mPin.clear();
	mROPin.clear();
}

Net::Net()
{
    mState = NETSTATE_HIGH;
    mHighFreqChanges = 0;
	mResetDelay = 10;
	mDirty = 1;
}

void Net::update()
{
	int next = nextstate();

	if (next == mState && mDirty == 0)
	{
		return;
	}

	int i;
	for (i = 0; i < (signed)mPin.size(); i++)
	{
		mPin[i]->mHost->mDirty = 1;
	}
	for (i = 0; i < (signed)mROPin.size(); i++)
	{
		mROPin[i]->mHost->mDirty = 1;
	}

    if ((next == NETSTATE_HIGH && mState == NETSTATE_LOW) ||
        (next == NETSTATE_LOW && mState == NETSTATE_HIGH))
    {
        mHighFreqChanges++;
        if (mHighFreqChanges > 50)
        {
            mHighFreqChanges = 0;
            if ((gPhysicsRand.genrand_int31() & 0xff) > 127)
            {
                return;
            }
        }
    }
    else
    {
        mHighFreqChanges = 0;
    }
    
    mState = next;
}

int Net::nextstate()
{
    Pin *input = NULL;    
    Pin *maybeInput = NULL;
   
	int i;
    for (i = 0; i < (signed)mPin.size(); i++)
    {
        if (mPin[i]->mState == PINSTATE_PROPAGATE_INVALID)
        {
			// Avoid propagation of invalid state
			if (mResetDelay)
			{
				mResetDelay--;
				mDirty = 1; // keep net dirty if handling reset delay
				return NETSTATE_HIGH;
			}

            return NETSTATE_INVALID;
        }

        if (mPin[i]->mState == PINSTATE_READ_OR_WRITE_HIGH || mPin[i]->mState == PINSTATE_READ_OR_WRITE_LOW)
        {
            // Most likely more trouble than it's worth.
            /*
            if (maybeInput)
            {
                if (maybeInput->mState != mPin[i]->mState)
                {
                    mState = NETSTATE_INVALID;
                    return;
                }
            }
            */

            maybeInput = mPin[i];
        }

        if (mPin[i]->mState == PINSTATE_WRITE_HIGH || mPin[i]->mState == PINSTATE_WRITE_LOW)
        {
            if (input != NULL)
            {
                return NETSTATE_INVALID;
            }
            input = mPin[i];
        }
    }

    if (input == NULL && maybeInput)
        input = maybeInput;

    if (input == NULL) 
    {
        return NETSTATE_NC;
    }

    if (input->mState == PINSTATE_WRITE_HIGH || input->mState == PINSTATE_READ_OR_WRITE_HIGH)
    {
        return NETSTATE_HIGH;        
    }

    if (input->mState == PINSTATE_WRITE_LOW || input->mState == PINSTATE_READ_OR_WRITE_LOW)
    {
        return NETSTATE_LOW;
    }

    return NETSTATE_INVALID;
}
