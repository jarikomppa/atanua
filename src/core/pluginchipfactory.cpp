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
#include "pluginchipfactory.h"
#include "tinyxml.h"
#include "pluginchip.h"

PluginChipFactory::~PluginChipFactory()
{
}

PluginChipFactory::PluginChipFactory()
{
    TiXmlDocument doc;
    FILE * f = fopen("atanua.xml", "rb");
    if (!doc.LoadFile(f))
    {
        fclose(f);
        return;
    }
    fclose(f);
    // Load config
    TiXmlNode *root;
    for (root = doc.FirstChild(); root != 0; root = root->NextSibling())
    {
        if (root->Type() == TiXmlNode::ELEMENT)
        {
            if (stricmp(root->Value(), "AtanuaConfig")==0)
            {
                TiXmlNode *part;
                for (part = root->FirstChild(); part != 0; part = part->NextSibling())
                {
                    if (part->Type() == TiXmlNode::ELEMENT)
                    {
                        if (stricmp(part->Value(), "Plugin") == 0)
                        {
                            const char *dll = ((TiXmlElement*)part)->Attribute("dll");
                            DLLHANDLETYPE h = opendll(dll);
                            if (h)
                            {
                                getatanuadllinfoproc getdllinfo = (getatanuadllinfoproc)getdllproc(h, "getatanuadllinfo");
                                if (getdllinfo)
                                {
                                    atanuadllinfo *dllinfo = new atanuadllinfo;
                                    getdllinfo(dllinfo);
                                    if (dllinfo->mDllVersion <= ATANUA_PLUGIN_DLL_VERSION)
                                    {
                                        mDllInfo.push_back(dllinfo);
                                        mDllHandle.push_back(h);
                                    }
                                    else
                                    {
                                        char temp[512];
                                        sprintf(temp, "Plug-in \"%s\" version is incompatible \n"
                                                     "with the current version of Atanua.\n"
                                                     "\n"
                                                     "Try to use it anyway?", dll);
                                        if (okcancel(temp))
                                        {
                                            mDllInfo.push_back(dllinfo);
                                            mDllHandle.push_back(h);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void PluginChipFactory::getSupportedChips(vector<char *> aChipList[5])
{
    int i;
    int dirtycats[5];
    dirtycats[0] = dirtycats[1] = dirtycats[2] = dirtycats[3] = dirtycats[4] =  0;
    for (i = 0; i < (signed)mDllInfo.size(); i++)
    {
        int j;
        for (j = 0; j < mDllInfo[i]->mChipCount; j++)
        {
            int cat = 1;
            if (mDllInfo[i]->mChipCategory != NULL &&
                mDllInfo[i]->mChipCategory[j] != 0)
                cat = (mDllInfo[i]->mChipCategory[j] - 1) % 3;
            aChipList[cat].push_back((char*)(mDllInfo[i]->mChipName[j]));
            dirtycats[cat] = 1;
        }
    }
    
    // Push the divider to the categories where chips were inserted
    for (i = 0; i < 5; i++)
        if (dirtycats[i])
            aChipList[i].push_back(NULL);
}

Chip * PluginChipFactory::build(const char *aChipId)
{
    int i;
    for (i = 0; i < (signed)mDllInfo.size(); i++)
    {
        int j;
        for (j = 0; j < mDllInfo[i]->mChipCount; j++)
        {
            if (strcmp(aChipId, mDllInfo[i]->mChipName[j]) == 0)
            {
                PluginChip * c = new PluginChip(mDllHandle[i], mDllInfo[i]->mChipName[j]);
                return c;
            }
        }
    }
    return NULL;
}
