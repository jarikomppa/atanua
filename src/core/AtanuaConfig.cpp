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
#include "tinyxml2.h"
#include <time.h>

using namespace tinyxml2;

AtanuaConfig::AtanuaConfig()
{
    mAntialiasedLines = 1;
    mPropagateInvalidState = PINSTATE_PROPAGATE_INVALID;
    mWireFry = 1;
    mToolkitWidth = 115;
    mMaxPhysicsMs = 40;
    mWindowWidth = 800;
    mWindowHeight = 600;
    mTooltipDelay = 1500;
    mLinePickTolerance = 0.05f;
    mLineEndTolerance = 0.2f;
    mLineSplitDragDistance = 10;
    mCustomCursors = 1;
    mPerformanceIndicators = 0;
    mChipCloneDragDistance = 10;
    mSwapShiftAndCtrl = 0;
    mAudioEnable = 1;
    mUserInfo = mystrdup("[No user name set]");
    mFontCacheMax = 512;
    mUseVBOs = 0;
    mUseOldFontSystem = 0;
    mUseBlending = 1;
	mMaxActiveBoxes = 200;
	mLEDSamples = 40;
	mPhysicsKHz = 1;
	mAutosaveDir = 0;
	mAutosaveCount = 10;
	mAutosaveEnable = 0;
	mAutosaveInterval = 1;
}

AtanuaConfig::~AtanuaConfig()
{
}

void AtanuaConfig::load()
{
	XMLDocument doc;
	XMLDeclaration *pDec;
	XMLElement *pRoot;
	XMLElement *pElement;
    
	FILE * f = fopen("atanua.xml", "rb");

    if (!f)
    {
		// Save config
		pDec = doc.NewDeclaration();
		doc.InsertEndChild(pDec);

		pRoot = doc.NewElement("AtanuaConfig");
		pRoot->SetAttribute("GeneratedWith",TITLE);
		doc.InsertEndChild(pRoot);

		pElement = doc.NewElement("PropagateInvalidState");
		pElement->SetAttribute("value", mPropagateInvalidState == PINSTATE_PROPAGATE_INVALID);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("CustomCursors");
		pElement->SetAttribute("value", mCustomCursors);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("PerformanceIndicators");
		pElement->SetAttribute("value", mPerformanceIndicators);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("SwapShiftAndCtrl");
		pElement->SetAttribute("value", mSwapShiftAndCtrl);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("WireFry");
		pElement->SetAttribute("value", mWireFry);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("AudioEnable");
		pElement->SetAttribute("value", mAudioEnable);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("ToolkitWidth");
		pElement->SetAttribute("value", mToolkitWidth);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("MaxPhysicsMs");
		pElement->SetAttribute("value", mMaxPhysicsMs);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("InitialWindow");
		pElement->SetAttribute("width", mWindowWidth);
		pElement->SetAttribute("height", mWindowHeight);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("TooltipDelay");
		pElement->SetAttribute("value", mTooltipDelay);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("LinePickTolerance");
		pElement->SetAttribute("value", mLinePickTolerance);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("LineEndTolerance");
		pElement->SetAttribute("value", mLineEndTolerance);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("LineSplitDragDistance");
		pElement->SetAttribute("value", mLineSplitDragDistance);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("ChipCloneDragDistance");
		pElement->SetAttribute("value", mChipCloneDragDistance);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("User");
		pElement->SetAttribute("name", "[No user name set]");
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("FontSystem");
		pElement->SetAttribute("CacheKeys", mFontCacheMax);
		pElement->SetAttribute("VBO", mUseVBOs);
		pElement->SetAttribute("SafeMode", mUseOldFontSystem);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("PerformanceOptions");
		pElement->SetAttribute("Blending", mUseBlending);
		pElement->SetAttribute("AntialiasedLines", mAntialiasedLines);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("Limits");
		pElement->SetAttribute("MaxBoxes", mMaxActiveBoxes);
		pElement->SetAttribute("PhysicsKHz", mPhysicsKHz);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("LED");
		pElement->SetAttribute("Samples", mLEDSamples);
		pRoot->InsertEndChild(pElement);

		pElement = doc.NewElement("Autosave");
		pElement->SetAttribute("Directory", "");
		pElement->SetAttribute("Enable", "0");
		pElement->SetAttribute("SaveCount", "10");
		pElement->SetAttribute("Interval", "1");
		pRoot->InsertEndChild(pElement);

		f = fopen("atanua.xml", "wb");
		doc.SaveFile(f);
		fclose(f);
    }
    else
    {
        if (doc.LoadFile(f))
        {
            fclose(f);
            return;
        }
        fclose(f);
        // Load config
        XMLNode *root;
        for (root = doc.FirstChild(); root != 0; root = root->NextSibling())
        {
            if (root->ToElement())
            {
                if (stricmp(root->Value(), "AtanuaConfig")==0)
                {
                    XMLNode *part;
                    for (part = root->FirstChild(); part != 0; part = part->NextSibling())
                    {
                        if (part->ToElement())
                        {
                            if (stricmp(part->Value(), "FontSystem") == 0)
                            {
                                ((XMLElement*)part)->QueryIntAttribute("CacheKeys", &mFontCacheMax);
                                ((XMLElement*)part)->QueryIntAttribute("VBO", &mUseVBOs);
                                ((XMLElement*)part)->QueryIntAttribute("SafeMode", &mUseOldFontSystem);
                            }
                            else
                            if (stricmp(part->Value(), "PerformanceOptions") == 0)
                            {
                                ((XMLElement*)part)->QueryIntAttribute("AntialiasedLines", &mAntialiasedLines);
                                ((XMLElement*)part)->QueryIntAttribute("Blending", &mUseBlending);
                            }
                            else
                            if (stricmp(part->Value(), "PropagateInvalidState") == 0)
                            {
                                int propagate;
                                ((XMLElement*)part)->QueryIntAttribute("value", &propagate);
                                if (propagate)
                                    mPropagateInvalidState = PINSTATE_PROPAGATE_INVALID;
                                else
                                    mPropagateInvalidState = PINSTATE_HIGHZ;
                            }
                            else
                            if (stricmp(part->Value(), "CustomCursors") == 0)
                            {
                                ((XMLElement*)part)->QueryIntAttribute("value", &mCustomCursors);
                            }
                            else
                            if (stricmp(part->Value(), "PerformanceIndicators") == 0)
                            {
                                ((XMLElement*)part)->QueryIntAttribute("value", &mPerformanceIndicators);
                            }
                            else
                            if (stricmp(part->Value(), "SwapShiftAndCtrl") == 0)
                            {
                                ((XMLElement*)part)->QueryIntAttribute("value", &mSwapShiftAndCtrl);
                            }
                            else
                            if (stricmp(part->Value(), "WireFry") == 0)
                            {
                                ((XMLElement*)part)->QueryIntAttribute("value", &mWireFry);
                            }
                            else
                            if (stricmp(part->Value(), "AudioEnable") == 0)
                            {
                                ((XMLElement*)part)->QueryIntAttribute("value", &mAudioEnable);
                            }
                            else
                            if (stricmp(part->Value(), "ToolkitWidth") == 0)
                            {
                                ((XMLElement*)part)->QueryIntAttribute("value", &mToolkitWidth);
                            }
                            else
                            if (stricmp(part->Value(), "MaxPhysicsMs") == 0)
                            {
                                ((XMLElement*)part)->QueryIntAttribute("value", &mMaxPhysicsMs);
                            }
                            else
                            if (stricmp(part->Value(), "InitialWindow") == 0)
                            {
                                ((XMLElement*)part)->QueryIntAttribute("width", &mWindowWidth);
                                ((XMLElement*)part)->QueryIntAttribute("height", &mWindowHeight);
                            }
                            else
                            if (stricmp(part->Value(), "TooltipDelay") == 0)
                            {
                                ((XMLElement*)part)->QueryIntAttribute("value", &mTooltipDelay);
                            }
                            else
                            if (stricmp(part->Value(), "LinePickTolerance") == 0)
                            {
                                ((XMLElement*)part)->QueryFloatAttribute("value", &mLinePickTolerance);
                            }
                            else
                            if (stricmp(part->Value(), "LineEndTolerance") == 0)
                            {
                                ((XMLElement*)part)->QueryFloatAttribute("value", &mLineEndTolerance);
                            }
                            else
                            if (stricmp(part->Value(), "LineSplitDragDistance") == 0)
                            {
                                ((XMLElement*)part)->QueryFloatAttribute("value", &mLineSplitDragDistance);
                            }
                            else
                            if (stricmp(part->Value(), "ChipCloneDragDistance") == 0)
                            {
                                ((XMLElement*)part)->QueryFloatAttribute("value", &mChipCloneDragDistance);
                            }
                            else
							if (stricmp(part->Value(), "Limits") == 0)
							{
								((XMLElement*)part)->QueryIntAttribute("MaxBoxes", &mMaxActiveBoxes);
								((XMLElement*)part)->QueryIntAttribute("PhysicsKHz", &mPhysicsKHz);
							}
							else
							if (stricmp(part->Value(), "LED") == 0)
							{
								((XMLElement*)part)->QueryIntAttribute("Samples", &mLEDSamples);
								if (mLEDSamples <= 0) mLEDSamples = 1;
								if (mLEDSamples > 10000) mLEDSamples = 10000;
							}
							else
							if (stricmp(part->Value(), "User") == 0)
							{
								const char *t = ((XMLElement*)part)->Attribute("name");
								if (t && strlen(t) > 0)
								{
									delete[] mUserInfo;
									mUserInfo = mystrdup(t);
								}
							}
							if (stricmp(part->Value(), "Autosave") == 0)
							{
								((XMLElement*)part)->QueryIntAttribute("Enable", &mAutosaveEnable);
								((XMLElement*)part)->QueryIntAttribute("SaveCount", &mAutosaveCount);
								((XMLElement*)part)->QueryIntAttribute("Interval", &mAutosaveInterval);
								const char * dir = ((XMLElement*)part)->Attribute("Directory");
								if (dir)
								{
									int len = strlen(dir);
									if (dir[len] == '/' || dir[len] == '\\')
									{
										mAutosaveDir = mystrdup(dir);
									}
									else
									{
										char temp[1024];
										sprintf(temp, "%s/", dir);
										mAutosaveDir = mystrdup(temp);
									}									
								}								
                                else
                                {
                                    mAutosaveDir = mystrdup("");
                                }
							}
                        }
                    }
                }
            }
        }
    }
}

