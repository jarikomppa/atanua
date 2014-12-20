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
#include "pluginchip.h"
#include "fileutils.h"

PluginChip::PluginChip(DLLHANDLETYPE aDllHandle, const char *aChipname)
{
    memset(&mChipInfo,0,sizeof(mChipInfo));

    dllcreate = (createproc)getdllproc(aDllHandle, "create");
    dllupdate = (updateproc)getdllproc(aDllHandle, "update");
    dllrender = (renderproc)getdllproc(aDllHandle, "render");
    dllcleanup = (cleanupproc)getdllproc(aDllHandle, "cleanup");
    if (dllcreate == NULL ||
        dllupdate == NULL ||
        dllrender == NULL ||
        dllcleanup == NULL ||
        dllcreate(&mChipInfo, aChipname) == 0)
    {
        dllcreate = NULL;
        return;
    }
    
    set(0, 0, mChipInfo.mWidth, mChipInfo.mHeight, mChipInfo.mTooltip);

    int i;
    for (i = 0; i < mChipInfo.mPinCount; i++)
    {
        const char *tt = NULL;
        Pin *p = new Pin;
        mPin.push_back(p);
        if (mChipInfo.mPinTooltips)
            tt = mChipInfo.mPinTooltips[i];
        if (tt == NULL) 
            tt = "-";
        p->set(mChipInfo.mPinCoordinates[i*2+0], mChipInfo.mPinCoordinates[i*2+1], this, tt);
    }
    
    mTexture = 0;
    if (mChipInfo.mTextureFilename)
        mTexture = load_texture((char*)mChipInfo.mTextureFilename);

}

PluginChip::~PluginChip()
{
    if (!dllcreate) return;
    dllcleanup(&mChipInfo);
}

static void handle_async_call(chipinfostruct &chip)
{
    switch (chip.mAsyncCall)
    {
    case ATANUA_ASYNC_CALL_OKCANCEL:
        chip.mAsyncCallReturn = (void*)okcancel((const char*)chip.mAsyncCallParam);
        break;
    case ATANUA_ASYNC_CALL_OPENFILE:
        chip.mAsyncCallReturn = (void*)openfiledialog((const char*)chip.mAsyncCallParam);
        break;
    case ATANUA_ASYNC_CALL_SAVEFILE:
        chip.mAsyncCallReturn = (void*)savefiledialog((const char*)chip.mAsyncCallParam);
        break;
    case ATANUA_ASYNC_CALL_STORE_UNDO:
        save_undo();
        break;
    }
}

void PluginChip::render(int aChipId)
{
    if (!dllcreate) return;
    int ch = 0;
    if (gUIState.kbditem == aChipId)
        ch = gUIState.keyentered;
    int plugin_did_rendering = 0;

    glPushMatrix();
    glTranslatef((mX + mW / 2), (mY + mH / 2), 0);
    glColor4f(1,1,1,1);
    if (mTexture)
    {
        glBindTexture(GL_TEXTURE_2D, mTexture);
        glEnable(GL_TEXTURE_2D);
    }

    do
    {
        mChipInfo.mAsyncCall = ATANUA_ASYNC_CALL_NONE;
        plugin_did_rendering = dllrender(&mChipInfo, ch);
        handle_async_call(mChipInfo);
    }
    while (mChipInfo.mAsyncCall != ATANUA_ASYNC_CALL_NONE);
    glPopMatrix();

    if (mTexture)
        glDisable(GL_TEXTURE_2D);

    if (plugin_did_rendering == 0)
    {
        // Render the chip
        if (mTexture)
        {
            drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
        }
        else
        {
            drawrect(mX+0.25, mY+0.25, mW-0.5, mH-0.5, 0xff3f3f3f); 
            drawrect(mX+0.35, mY+0.35, mW-0.7, mH-0.7, 0xff5f5f5f); 
        }
        float w,h,l;
        fn.stringmetrics(mChipInfo.mChipName,w,h,l,0.75);
            
        fn.drawstring(mChipInfo.mChipName,mX+(mW-w)/2,mY+(mH-h)/2,0x5fffffff,0.75);
    }
}

void PluginChip::update(float aTick)
{
    int i;
    if (!dllcreate) return;

    // nets to chipinfo
    for (i = 0; i < mChipInfo.mPinCount; i++)
    {
        mChipInfo.mPinInput[i] = NETSTATE_NC;
        if (mPin[i]->mNet)
        {
            mChipInfo.mPinInput[i] = mPin[i]->mNet->mState;
        }
    }
    
    // do update
    do
    {
        mChipInfo.mAsyncCall = ATANUA_ASYNC_CALL_NONE;
        dllupdate(&mChipInfo, aTick);
        handle_async_call(mChipInfo);
    }
    while (mChipInfo.mAsyncCall != ATANUA_ASYNC_CALL_NONE);

    // chipinfo to pins
    for (i = 0; i < mChipInfo.mPinCount; i++)
    {
        mPin[i]->setState(mChipInfo.mPinOutput[i]);
    }

	mDirty = 1;
}

void PluginChip::serialize(File *f)
{
    if (!dllcreate) return;
    if (mChipInfo.mPersistentDataCount)
    {
        int i;
        for (i = 0; i < mChipInfo.mPersistentDataCount; i++)
            f->writebyte(mChipInfo.mPersistentDataPtr[i]);
    }
}

void PluginChip::deserialize(File *f)
{
    if (!dllcreate) return;
    if (mChipInfo.mPersistentDataCount)
    {
        int i;
        for (i = 0; i < mChipInfo.mPersistentDataCount; i++)
            mChipInfo.mPersistentDataPtr[i] = f->readbyte();
    }
}

void PluginChip::clone(Chip *aOther)
{
    if (!dllcreate) return;
    if (mChipInfo.mPersistentDataCount)
    {
        PluginChip *o = (PluginChip*)aOther;
        memcpy(o->mChipInfo.mPersistentDataPtr, mChipInfo.mPersistentDataPtr, mChipInfo.mPersistentDataCount);
    }
}
