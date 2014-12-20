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
#ifndef PLUGINCHIP_H
#define PLUGINCHIP_H

#define NO_ATANUA_ENUMS
#define NO_ATANUA_DLL_PROTOTYPES
#include "plugin/atanua_dll_interface.h"

typedef int (ATANUACALL *createproc)(chipinfostruct *aChipInfo, const char *aChipname);
typedef void (ATANUACALL *updateproc)(chipinfostruct *aChipInfo, float aTick);
typedef int (ATANUACALL *renderproc)(chipinfostruct *aChipInfo, int aKey);
typedef void (ATANUACALL *cleanupproc)(chipinfostruct *aChipInfo);

class PluginChip : public Chip
{
    int mTexture;
    chipinfostruct mChipInfo;
    createproc dllcreate;
    updateproc dllupdate;
    renderproc dllrender;
    cleanupproc dllcleanup;

public:
    PluginChip(DLLHANDLETYPE aDllHandle, const char *aChipname); // Ctor
    ~PluginChip(); // Dtor

    virtual void render(int aChipId);
    virtual void update(float aTick);
    virtual void serialize(File *f);
    virtual void deserialize(File *f);
    virtual void clone(Chip *aOther);
};

#endif
