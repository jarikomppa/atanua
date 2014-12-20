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
#include "tinyxml.h"
#include <time.h>

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

static int h2i(char d)
{
    if (d >= '0' && d <= '9')
        return d - '0';
    if (d >= 'A' && d <= 'F')
        return d - 'A' + 10;
    return -1;
}


void AtanuaConfig::load()
{
    TiXmlDocument doc;
    FILE * f = fopen("atanua.xml", "rb");

    if (!f)
    {
        // Save config
        TiXmlDeclaration * decl = new TiXmlDeclaration("1.0","","");
        TiXmlElement * topelement = new TiXmlElement("AtanuaConfig");
        topelement->SetAttribute("GeneratedWith",TITLE);
        doc.LinkEndChild(decl);
        doc.LinkEndChild(topelement);

        TiXmlElement *element;

        element = new TiXmlElement("PropagateInvalidState");
        topelement->LinkEndChild(element);
        element->SetAttribute("value", mPropagateInvalidState == PINSTATE_PROPAGATE_INVALID);

        element = new TiXmlElement("CustomCursors");
        topelement->LinkEndChild(element);
        element->SetAttribute("value", mCustomCursors);

        element = new TiXmlElement("PerformanceIndicators");
        topelement->LinkEndChild(element);
        element->SetAttribute("value", mPerformanceIndicators);

        element = new TiXmlElement("SwapShiftAndCtrl");
        topelement->LinkEndChild(element);
        element->SetAttribute("value", mSwapShiftAndCtrl);

        element = new TiXmlElement("WireFry");
        topelement->LinkEndChild(element);
        element->SetAttribute("value", mWireFry);

        element = new TiXmlElement("AudioEnable");
        topelement->LinkEndChild(element);
        element->SetAttribute("value", mAudioEnable);

        element = new TiXmlElement("ToolkitWidth");
        topelement->LinkEndChild(element);
        element->SetAttribute("value", mToolkitWidth);

        element = new TiXmlElement("MaxPhysicsMs");
        topelement->LinkEndChild(element);
        element->SetAttribute("value", mMaxPhysicsMs);

        element = new TiXmlElement("InitialWindow");
        topelement->LinkEndChild(element);
        element->SetAttribute("width", mWindowWidth);
        element->SetAttribute("height", mWindowHeight);

        element = new TiXmlElement("TooltipDelay");
        topelement->LinkEndChild(element);
        element->SetAttribute("value", mTooltipDelay);

        element = new TiXmlElement("LinePickTolerance");
        topelement->LinkEndChild(element);
        element->SetDoubleAttribute("value", mLinePickTolerance);

        element = new TiXmlElement("LineEndTolerance");
        topelement->LinkEndChild(element);
        element->SetDoubleAttribute("value", mLineEndTolerance);

        element = new TiXmlElement("LineSplitDragDistance");
        topelement->LinkEndChild(element);
        element->SetDoubleAttribute("value", mLineSplitDragDistance);

        element = new TiXmlElement("ChipCloneDragDistance");
        topelement->LinkEndChild(element);
        element->SetDoubleAttribute("value", mChipCloneDragDistance);

        element = new TiXmlElement("User");
        topelement->LinkEndChild(element);
        element->SetAttribute("name", "[No user name set]");

        element = new TiXmlElement("FontSystem");
        topelement->LinkEndChild(element);
        element->SetAttribute("CacheKeys", mFontCacheMax);
        element->SetAttribute("VBO", mUseVBOs);
        element->SetAttribute("SafeMode", mUseOldFontSystem);

        element = new TiXmlElement("PerformanceOptions");
        topelement->LinkEndChild(element);
        element->SetAttribute("Blending", mUseBlending);
        element->SetAttribute("AntialiasedLines", mAntialiasedLines);

        element = new TiXmlElement("Limits");
        topelement->LinkEndChild(element);
		element->SetAttribute("MaxBoxes", mMaxActiveBoxes);
		element->SetAttribute("PhysicsKHz", mPhysicsKHz);

        element = new TiXmlElement("LED");
        topelement->LinkEndChild(element);
		element->SetAttribute("Samples", mLEDSamples);

        element = new TiXmlElement("Autosave");
        topelement->LinkEndChild(element);
		element->SetAttribute("Directory", "");
		element->SetAttribute("Enable", "0");
		element->SetAttribute("SaveCount", "10");
		element->SetAttribute("Interval", "1");

        f = fopen("atanua.xml", "wb");
        doc.SaveFile(f);
        fclose(f);
    }
    else
    {
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

                            if (stricmp(part->Value(), "FontSystem") == 0)
                            {
                                ((TiXmlElement*)part)->QueryIntAttribute("CacheKeys", &mFontCacheMax);
                                ((TiXmlElement*)part)->QueryIntAttribute("VBO", &mUseVBOs);
                                ((TiXmlElement*)part)->QueryIntAttribute("SafeMode", &mUseOldFontSystem);
                            }
                            else
                            if (stricmp(part->Value(), "PerformanceOptions") == 0)
                            {
                                ((TiXmlElement*)part)->QueryIntAttribute("AntialiasedLines", &mAntialiasedLines);
                                ((TiXmlElement*)part)->QueryIntAttribute("Blending", &mUseBlending);
                            }
                            else
                            if (stricmp(part->Value(), "PropagateInvalidState") == 0)
                            {
                                int propagate;
                                ((TiXmlElement*)part)->QueryIntAttribute("value", &propagate);
                                if (propagate)
                                    mPropagateInvalidState = PINSTATE_PROPAGATE_INVALID;
                                else
                                    mPropagateInvalidState = PINSTATE_HIGHZ;
                            }
                            else
                            if (stricmp(part->Value(), "CustomCursors") == 0)
                            {
                                ((TiXmlElement*)part)->QueryIntAttribute("value", &mCustomCursors);
                            }
                            else
                            if (stricmp(part->Value(), "PerformanceIndicators") == 0)
                            {
                                ((TiXmlElement*)part)->QueryIntAttribute("value", &mPerformanceIndicators);
                            }
                            else
                            if (stricmp(part->Value(), "SwapShiftAndCtrl") == 0)
                            {
                                ((TiXmlElement*)part)->QueryIntAttribute("value", &mSwapShiftAndCtrl);
                            }
                            else
                            if (stricmp(part->Value(), "WireFry") == 0)
                            {
                                ((TiXmlElement*)part)->QueryIntAttribute("value", &mWireFry);
                            }
                            else
                            if (stricmp(part->Value(), "AudioEnable") == 0)
                            {
                                ((TiXmlElement*)part)->QueryIntAttribute("value", &mAudioEnable);
                            }
                            else
                            if (stricmp(part->Value(), "ToolkitWidth") == 0)
                            {
                                ((TiXmlElement*)part)->QueryIntAttribute("value", &mToolkitWidth);
                            }
                            else
                            if (stricmp(part->Value(), "MaxPhysicsMs") == 0)
                            {
                                ((TiXmlElement*)part)->QueryIntAttribute("value", &mMaxPhysicsMs);
                            }
                            else
                            if (stricmp(part->Value(), "InitialWindow") == 0)
                            {
                                ((TiXmlElement*)part)->QueryIntAttribute("width", &mWindowWidth);
                                ((TiXmlElement*)part)->QueryIntAttribute("height", &mWindowHeight);
                            }
                            else
                            if (stricmp(part->Value(), "TooltipDelay") == 0)
                            {
                                ((TiXmlElement*)part)->QueryIntAttribute("value", &mTooltipDelay);
                            }
                            else
                            if (stricmp(part->Value(), "LinePickTolerance") == 0)
                            {
                                ((TiXmlElement*)part)->QueryFloatAttribute("value", &mLinePickTolerance);
                            }
                            else
                            if (stricmp(part->Value(), "LineEndTolerance") == 0)
                            {
                                ((TiXmlElement*)part)->QueryFloatAttribute("value", &mLineEndTolerance);
                            }
                            else
                            if (stricmp(part->Value(), "LineSplitDragDistance") == 0)
                            {
                                ((TiXmlElement*)part)->QueryFloatAttribute("value", &mLineSplitDragDistance);
                            }
                            else
                            if (stricmp(part->Value(), "ChipCloneDragDistance") == 0)
                            {
                                ((TiXmlElement*)part)->QueryFloatAttribute("value", &mChipCloneDragDistance);
                            }
                            else
							if (stricmp(part->Value(), "Limits") == 0)
							{
								((TiXmlElement*)part)->QueryIntAttribute("MaxBoxes", &mMaxActiveBoxes);
								((TiXmlElement*)part)->QueryIntAttribute("PhysicsKHz", &mPhysicsKHz);
							}
							else
							if (stricmp(part->Value(), "LED") == 0)
							{
								((TiXmlElement*)part)->QueryIntAttribute("Samples", &mLEDSamples);
								if (mLEDSamples <= 0) mLEDSamples = 1;
								if (mLEDSamples > 10000) mLEDSamples = 10000;
							}
							else
							if (stricmp(part->Value(), "User") == 0)
							{
								const char *t = ((TiXmlElement*)part)->Attribute("name");
								if (t && strlen(t) > 0)
								{
									delete[] mUserInfo;
									mUserInfo = mystrdup(t);
								}
							}
							if (stricmp(part->Value(), "Autosave") == 0)
							{
								((TiXmlElement*)part)->QueryIntAttribute("Enable", &mAutosaveEnable);
								((TiXmlElement*)part)->QueryIntAttribute("SaveCount", &mAutosaveCount);
								((TiXmlElement*)part)->QueryIntAttribute("Interval", &mAutosaveInterval);
								const char * dir = ((TiXmlElement*)part)->Attribute("Directory");
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

