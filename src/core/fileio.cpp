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
#include "fileutils.h"
#include "tinyxml.h"
#include "extpin.h"
#include "box.h"
#include <string>

FILE * openfileinsamedir(const char * aFile);
void do_flush_boxloadqueue();

void do_loadoldbinary(File *f)
{
    if (!f)
        return;
    int id = f->readint();
    if (id != 0x00617441) // 'Ata\0'
        return;
    int chipcount = f->readint();
    int wirecount = f->readint();
    int i;
    for (i = 0; i < chipcount; i++)
    {
        char temp[256];
        memset(temp,0,256);
        int len = f->readword();
        if (len > 255) 
        {
            return;
        }
        f->readchars(temp, len);
        // we now have the chip id string, but we need to find
        // the copy which is always in memory so that we get a
        // sane pointer to it for gChipName.
        const char *chipname = NULL;

		// boxes are... special.
		int sl = strlen(temp);
		if (sl > 7 && stricmp(temp+sl-7,".atanua") == 0)
		{
			chipname = mystrdup(temp);
		}
		else
        if (stricmp(temp, "Connection Pin") == 0)
        {
			// Connection pin is a special case, as you can't create it
			// in a "normal" way
            chipname = "Connection Pin";
        }
        else
        {
            int j, k;
            for (k = 0; chipname == NULL && k < 5; k++)
            {
                for (j = 0; chipname == NULL && j < (signed)gAvailableChip[k].size(); j++)
                {
                    if (gAvailableChip[k][j])
                    {
                        if (stricmp(temp, gAvailableChip[k][j]) == 0)
                        {
                            chipname = gAvailableChip[k][j];
                        }
                    }
                }
            }
        }

        if (chipname == NULL)
        {
            // could make a popup here saying 
            // "%temp% not found, missing plugins?"
            return;
        }

        Chip *chip = NULL;
        int j;
        for (j = 0; chip == NULL && j < (signed)gChipFactory.size(); j++)
            chip = gChipFactory[j]->build(chipname);
        
        if (chip == NULL)
        {
            return;
        }

        chip->mX = ((float)f->readint()) / (1 << 24);
        chip->mY = ((float)f->readint()) / (1 << 24);
        
        chip->deserialize(f);

        chip->rotate(0);

        gChip.push_back(chip);
        gChipName.push_back(chipname);     
    }

    for (i = 0; i < wirecount; i++)
    {
        unsigned int id1 = (unsigned)f->readint();
        unsigned int id2 = (unsigned)f->readint();
        unsigned int chip1 = (id1 >> 16) & 0xffff;
        unsigned int chip2 = (id2 >> 16) & 0xffff;
        unsigned int pin1 = id1 & 0xffff;
        unsigned int pin2 = id2 & 0xffff;
        if (chip1 >= gChip.size() ||
            chip2 >= gChip.size() ||
            pin1 >= gChip[chip1]->mPin.size() ||
            pin2 >= gChip[chip2]->mPin.size())
        {
            return;
        }
        gWire.push_back(new Wire(gChip[chip1]->mPin[pin1], gChip[chip2]->mPin[pin2]));    
    }
	
	build_nets();
}

void do_savexml(FILE * filehandle)
{
    int masterkey = 0;
    int i;
    for (i = 0; i < (signed)gChip.size(); i++)
        masterkey ^= gChip[i]->mKey;
    for (i = 0; i < (signed)gWire.size(); i++)
        masterkey ^= gWire[i]->mKey;

    TiXmlDocument doc;
    TiXmlDeclaration * decl = new TiXmlDeclaration("1.0","","");
    TiXmlElement * topelement = new TiXmlElement("Atanua");
    {
        char temp[512];
        sprintf(temp, "%s - %s", TITLE, gConfig.mUserInfo);
        topelement->SetAttribute("Version",temp);
    }

	int chipcount = 0;
	int wirecount = 0;
	for (i = 0; i < (signed)gChip.size(); i++)
		if (gChip[i]->mBox == 0)
			chipcount++;

	for (i = 0; i < (signed)gWire.size(); i++)
		if (gWire[i]->mBox == 0)
			wirecount++;

    topelement->SetAttribute("ChipCount",chipcount);
    topelement->SetAttribute("WireCount",wirecount);
    topelement->SetAttribute("key",masterkey);
    topelement->SetAttribute("scale", 16);
    doc.LinkEndChild(decl);
    doc.LinkEndChild(topelement);

    for (i = 0; i < (signed)gChip.size(); i++)
    {
		if (gChip[i]->mBox != 0)
			continue;        
		TiXmlElement *element = new TiXmlElement("Chip");
        topelement->LinkEndChild(element);
        element->SetAttribute("Name", gChipName[i]);
        element->SetAttribute("xpos", (int)floor((1 << 16) * gChip[i]->mX));
        element->SetAttribute("ypos", (int)floor((1 << 16) * gChip[i]->mY));        
        element->SetAttribute("rot", gChip[i]->mAngleIn90DegreeSteps);        
        element->SetAttribute("key", gChip[i]->mKey ^ masterkey);        
        MemoryFile f;
        gChip[i]->serialize(&f);
        if (!f.mData.empty())
        {
            string s;
            int j;
            for (j = 0; j < (signed)f.mData.size(); j++)
            {
                int d = f.mData[j] & 0xff;
                char temp[16];
                sprintf(temp,"%02X",d);
                s.push_back(temp[0]);
                s.push_back(temp[1]);
            }
            TiXmlText *text = new TiXmlText(s.c_str());
            element->LinkEndChild(text);
        }

    }
    for (i = 0; i < (signed)gWire.size(); i++)
    {
		if (gWire[i]->mBox != 0)
			continue;
        TiXmlElement *element = new TiXmlElement("Wire");
        topelement->LinkEndChild(element);
        Pin *pn[2];
        pn[0] = gWire[i]->mFirst;
        pn[1] = gWire[i]->mSecond;
        int j;
        for (j = 0; j < 2; j++)
        {
            int pinid = 0;
            int k, l;
			int chipno = 0;
            for (k = 0; pinid == 0 && k < (signed)gChip.size(); k++)
            {
				if (gChip[k]->mBox == 0)
				{
					for (l = 0; pinid == 0 && l < (signed)gChip[k]->mPin.size(); l++)
					{
						if (gChip[k]->mPin[l] == pn[j])
						{
							pinid = (k << 16) | l;
							if (j == 0)
							{
								element->SetAttribute("chip1", chipno);
								element->SetAttribute("pad1", l);
							}
							else
							{
								element->SetAttribute("chip2", chipno);
								element->SetAttribute("pad2", l);
							}
						}
					}
					chipno++;
                }
            }
        }        
        element->SetAttribute("key", gWire[i]->mKey ^ masterkey);
    }
    
    doc.SaveFile(filehandle);
    fclose(filehandle);
}

void clear_boxcache()
{
	int i;
	for (i = 0; i < (signed)gBoxCache.size(); i++)
	{
		delete[] gBoxCache[i].mName;
		int j;
		for (j = 0; j < (signed)gBoxCache[i].mTooltips.size(); j++)
			delete[] gBoxCache[i].mTooltips[j];
		gBoxCache[i].mTooltips.clear();
		delete gBoxCache[i].mData;
	}
	gBoxCache.clear();
}

File * do_preload_box(const char *aName);
void do_loadxmltobinary(FILE * f, File * outf, BoxcacheData * bd);

BoxStitchingInformation * do_preparse_box(const char *aFname)
{
	BoxStitchingInformation *d = new BoxStitchingInformation;

	int i, idx = -1;
	File *bf = 0;
	FILE * f = openfileinsamedir(aFname);
	if (!f)
	{
		return NULL;
	}

	for (i = 0; idx == -1 && i < (signed)gBoxCache.size(); i++)
	{
		if (stricmp(gBoxCache[i].mName, aFname) == 0)
		{
			bf = gBoxCache[i].mData;
			idx = i;
		}
	}

	if (!bf)
	{
		bf = new MemoryFile;
		BoxcacheData bd;
		do_loadxmltobinary(f, bf, &bd);
		bd.mData = bf;
		bd.mName = mystrdup(aFname);
		gBoxCache.push_back(bd);
		idx = gBoxCache.size() - 1;
	}
	fclose(f);

	for (i = 0; i < (signed)gBoxCache[idx].mTooltips.size(); i++)
	{
		d->mToolTips.push_back(mystrdup(gBoxCache[idx].mTooltips[i]));
	}

	if (d->mToolTips.empty() && okcancel("Trying to box an .atanua file with no external pins!\nBuild it anyway?") == 0)
	{
		delete d;
		return NULL;
	}
	return d;
}

void do_loadxml(FILE * f, int box)
{
    TiXmlDocument doc;
    TiXmlNode *pChild;

	// Need to know the existing chip count in case we're merging
	int old_chips = gChip.size();

    if (!doc.LoadFile(f))
	{
        return;
	}

    for (pChild = doc.FirstChild(); pChild != 0; pChild = pChild->NextSibling())
    {
        if (pChild->Type() == TiXmlNode::TINYXML_ELEMENT)
        {
            if (stricmp(pChild->Value(), "Atanua") == 0)
            {
                int masterkey = 0;
                int scale = 24;
                ((TiXmlElement*)pChild)->QueryIntAttribute("key", &masterkey);
                ((TiXmlElement*)pChild)->QueryIntAttribute("scale", &scale);
                TiXmlNode *part;
                for (part = pChild->FirstChild(); part != 0; part = part->NextSibling())
                {
                    if (part->Type() == TiXmlNode::TINYXML_ELEMENT)
                    {
                        if (stricmp(part->Value(), "Chip")==0)
                        {
                            // we now have the chip id string, but we need to find
                            // the copy which is always in memory so that we get a
                            // sane pointer to it for gChipName.
                            const char *chipname = NULL;
                            const char *temp = ((TiXmlElement*)part)->Attribute("Name");
                            if (temp)
                            {
								// boxes are... special.
								int i = strlen(temp);
								if (i > 7 && stricmp(temp+i-7,".atanua") == 0)
								{
									chipname = mystrdup(temp);
									FILE * f = openfileinsamedir(temp);
									if (!f)
									{
                                        char temp[1024];
                                        sprintf(temp,"Boxed atanua file '%s' not found.\nTry to continue loading?\n\n(it will show up as box with no pins)", chipname);
                                        if (okcancel(temp) == 0)
                                        {
											delete[] chipname;
                                            build_nets();
                                            return;
                                        }
									}
									else
									{
										fclose(f);
									}
								}
								else
                                if (stricmp(temp, "Connection Pin") == 0)
                                {
									// Connection pin is a special case, as you can't create it
									// in a "normal" way
                                    chipname = "Connection Pin";
                                }
                                else
                                {
                                    int j, k;
                                    for (k = 0; chipname == NULL && k < 5; k++)
                                    {
                                        for (j = 0; chipname == NULL && j < (signed)gAvailableChip[k].size(); j++)
                                        {
                                            if (gAvailableChip[k][j])
                                            {
                                                if (stricmp(temp, gAvailableChip[k][j]) == 0)
                                                {
                                                    chipname = gAvailableChip[k][j];
                                                }
                                            }
                                        }
                                    }
                                }
                                
                                if (chipname != NULL)
                                {
                                    Chip *chip = NULL;
                                    int j;
                                    for (j = 0; chip == NULL && j < (signed)gChipFactory.size(); j++)
                                        chip = gChipFactory[j]->build(chipname);
                                    
                                    if (chip == NULL)
                                    {
                                        return;
                                    }
                                    int x, y, key, angle;
                                    x = y = key = angle = 0;                                    
                                    ((TiXmlElement*)part)->QueryIntAttribute("xpos", &x);
                                    ((TiXmlElement*)part)->QueryIntAttribute("ypos", &y);
                                    ((TiXmlElement*)part)->QueryIntAttribute("key", &key);
                                    ((TiXmlElement*)part)->QueryIntAttribute("rot", &angle);
                                    chip->mAngleIn90DegreeSteps = angle;
                                    chip->mX = (float)x / (1 << scale);
                                    chip->mY = (float)y / (1 << scale);
                                    chip->mKey = key ^ masterkey;

                                    TiXmlNode *text;
                                    for (text = part->FirstChild(); text != 0; text = text->NextSibling())
                                    {
                                        if (text->Type() == TiXmlNode::TINYXML_TEXT)
                                        {

                                            MemoryFile f;

                                            const char *v = text->Value();
                                            int wholebyte = 0;
                                            int data = 0;
                                            while (*v)
                                            {
                                                if (*v >= '0' && *v <= '9')
                                                {
                                                    data <<= 4;
                                                    data |= *v - '0';
                                                    wholebyte++;
                                                }
                                                else
                                                if (*v >= 'A' && *v <= 'F')
                                                {
                                                    data <<= 4;
                                                    data |= *v - 'A' + 10;
                                                    wholebyte++;
                                                }
                                                else
                                                if (*v >= 'a' && *v <= 'f')
                                                {
                                                    data <<= 4;
                                                    data |= *v - 'a' + 10;
                                                    wholebyte++;
                                                }
                                                // ignore other values, including whitespace
                                                if (wholebyte == 2)
                                                {
                                                    f.mData.push_back(data);
                                                    data = 0;
                                                    wholebyte = 0;
                                                }
                                                v++;
                                            }
                                            if (wholebyte != 0)
                                            {
                                                char temp[1024];
                                                sprintf(temp,"Decoding chip-specific data while loading '%s' failed.\nTry to continue loading?", chipname);
                                                if (okcancel(temp) == 0)
                                                {
                                                    return;
                                                }
                                            }
                                            chip->deserialize(&f); 
                                        }
                                    }
                                    chip->rotate(chip->mAngleIn90DegreeSteps);
                                    chip->mBox = box;
									gChip.push_back(chip);
                                    gChipName.push_back(chipname);     
                                }
                                else
                                {
                                    char mtemp[1024];
                                    sprintf(mtemp,"Chip called '%s' requested by savefile but it is\n"
                                                  "not supported by this version, or a plugin is missing.\n"
                                                  "\n"
                                                  "Try to continue loading?", temp);
                                    if (okcancel(mtemp) == 0)
                                    {
                                        build_nets();
                                        return;
                                    }
                                }
                            }
                        }
                        if (stricmp(part->Value(), "Wire")==0)
                        {
                            int chip1, pin1, chip2, pin2, key;
                            ((TiXmlElement*)part)->QueryIntAttribute("chip1", &chip1);
                            ((TiXmlElement*)part)->QueryIntAttribute("chip2", &chip2);
                            ((TiXmlElement*)part)->QueryIntAttribute("pad1", &pin1);
                            ((TiXmlElement*)part)->QueryIntAttribute("pad2", &pin2);
                            ((TiXmlElement*)part)->QueryIntAttribute("key", &key);
                            if (chip1 < 0 || chip2 < 0 || pin1 < 0 || pin2 < 0 ||
                                chip1+old_chips >= (signed)gChip.size() ||
                                chip2+old_chips >= (signed)gChip.size() ||
                                pin1 >= (signed)gChip[chip1+old_chips]->mPin.size() ||
                                pin2 >= (signed)gChip[chip2+old_chips]->mPin.size())
                            {
                                if (okcancel("Invalid wire definition found.\nTry to continue loading?") == 0)
                                {
                                    build_nets();
                                    return;
                                }
                            }
                            else
                            {
                                Wire *w = new Wire(gChip[chip1+old_chips]->mPin[pin1], gChip[chip2+old_chips]->mPin[pin2]);
                                w->mKey = key ^ masterkey;
								w->mBox = box;
                                gWire.push_back(w);    
                            }
                        }
                    }
                }
            }
        }
    }
	
	if (box == 0)
	{
		do_flush_boxloadqueue();
	}
	
    build_nets();    	
}

void do_loadbinary(File *f, int box)
{
	int old_chips = gChip.size();

    if (!f)
        return;
    int id = f->readint();
    if (id != 0x02617441) // 'Ata' + 2
        return;
    int chipcount = f->readint();
    int wirecount = f->readint();
    int i;
    for (i = 0; i < chipcount; i++)
    {
        char temp[256];
        memset(temp,0,256);
        int len = f->readword();
        if (len > 255) 
        {
            return;
        }
        f->readchars(temp, len);

		// we now have the chip id string, but we need to find
        // the copy which is always in memory so that we get a
        // sane pointer to it for gChipName.

		const char *chipname = NULL;

		// boxes are... special.
		int sl = strlen(temp);
		if (sl > 7 && stricmp(temp+sl-7,".atanua") == 0)
		{
			chipname = mystrdup(temp);
		}
		else
        if (stricmp(temp, "Connection Pin") == 0)
        {
			// Connection pin is a special case, as you can't create it
			// in a "normal" way
            chipname = "Connection Pin";
        }
        else
        {
            int j, k;
            for (k = 0; chipname == NULL && k < 5; k++)
            {
                for (j = 0; chipname == NULL && j < (signed)gAvailableChip[k].size(); j++)
                {
                    if (gAvailableChip[k][j])
                    {
                        if (stricmp(temp, gAvailableChip[k][j]) == 0)
                        {
                            chipname = gAvailableChip[k][j];
                        }
                    }
                }
            }
        }

        if (chipname == NULL)
        {
            // could make a popup here saying 
            // "%temp% not found, missing plugins?"
            return;
        }

        Chip *chip = NULL;
        int j;
        for (j = 0; chip == NULL && j < (signed)gChipFactory.size(); j++)
            chip = gChipFactory[j]->build(chipname);
        
        if (chip == NULL)
        {
            return;
        }

        chip->mX = ((float)f->readint()) / (1 << 16);
        chip->mY = ((float)f->readint()) / (1 << 16);
        chip->mAngleIn90DegreeSteps = f->readint();
		chip->mBox = f->readint();
		if (box != -1) chip->mBox = box;
        
        chip->deserialize(f);

        chip->rotate(chip->mAngleIn90DegreeSteps);	

        gChip.push_back(chip);
        gChipName.push_back(chipname);     
    }

    for (i = 0; i < wirecount; i++)
    {
        unsigned int id1 = (unsigned)f->readint();
        unsigned int id2 = (unsigned)f->readint();
        unsigned int chip1 = (id1 >> 16) & 0xffff;
        unsigned int chip2 = (id2 >> 16) & 0xffff;
        unsigned int pin1 = id1 & 0xffff;
        unsigned int pin2 = id2 & 0xffff;
		chip1 += old_chips;
		chip2 += old_chips;
        if (chip1 >= gChip.size() ||
            chip2 >= gChip.size() ||
            pin1 >= gChip[chip1]->mPin.size() ||
            pin2 >= gChip[chip2]->mPin.size())
        {
            return;
        }
		Wire * w = new Wire(gChip[chip1]->mPin[pin1], gChip[chip2]->mPin[pin2]);
		w->mBox = f->readint();
		if (box != -1)
			w->mBox = box;
        gWire.push_back(w);    
    }
	if (box <= 0)
	{
		do_flush_boxloadqueue();
	}
	build_nets();
}

void do_savebinary(File * f)
{
	int i;

	if (!f)
        return;

	f->writeint(0x02617441); // 'Ata' + 2	
	
	int chipcount = 0;
	for (i = 0; i < (signed)gChip.size(); i++)
		if (gChip[i]->mBox == 0)
			chipcount++;
	f->writeint(chipcount);
	
	int wirecount = 0;
	for (i = 0; i < (signed)gWire.size(); i++)
		if (gWire[i]->mBox == 0)
			wirecount++;
    
    f->writeint(wirecount);
    
    for (i = 0; i < (signed)gChip.size(); i++)
    {
		if (gChip[i]->mBox != 0)
			continue;
        int len = strlen(gChipName[i]);
        f->writeword(len);
        f->writechars(gChipName[i], len);
        f->writeint((int)floor((1 << 16) * gChip[i]->mX));
        f->writeint((int)floor((1 << 16) * gChip[i]->mY));
        f->writeint(gChip[i]->mAngleIn90DegreeSteps);
		f->writeint(gChip[i]->mBox);
        gChip[i]->serialize(f);
    }
    for (i = 0; i < (signed)gWire.size(); i++)
    {
		if (gWire[i]->mBox != 0)
			continue;
        Pin *pn[2];
        pn[0] = gWire[i]->mFirst;
        pn[1] = gWire[i]->mSecond;
        int j;
        for (j = 0; j < 2; j++)
        {
            int pinid = 0;
            int k, l;
            for (k = 0; pinid == 0 && k < (signed)gChip.size(); k++)
            {
                for (l = 0; pinid == 0 && l < (signed)gChip[k]->mPin.size(); l++)
                {
                    if (gChip[k]->mPin[l] == pn[j])
                    {
                        pinid = (k << 16) | l;
                    }
                }
            }
            f->writeint(pinid);
        }
		f->writeint(gWire[i]->mBox);
    }
}


void do_loadxmltobinary(FILE * f, File * outf, BoxcacheData * bd)
{
    TiXmlDocument doc;
    TiXmlNode *pChild;

    if (!doc.LoadFile(f))
	{
        return;
	}

    for (pChild = doc.FirstChild(); pChild != 0; pChild = pChild->NextSibling())
    {
        if (pChild->Type() == TiXmlNode::TINYXML_ELEMENT)
        {
            if (stricmp(pChild->Value(), "Atanua") == 0)
            {
				outf->writeint(0x02617441); // 'Ata' + 2
	
				int chipcount = 0;
                ((TiXmlElement*)pChild)->QueryIntAttribute("ChipCount", &chipcount);
				outf->writeint(chipcount);
	
				int wirecount = 0;   
                ((TiXmlElement*)pChild)->QueryIntAttribute("WireCount", &wirecount);
				outf->writeint(wirecount);

				int scale = 24;
                ((TiXmlElement*)pChild)->QueryIntAttribute("scale", &scale);

                TiXmlNode *part;
                for (part = pChild->FirstChild(); part != 0; part = part->NextSibling())
                {
                    if (part->Type() == TiXmlNode::TINYXML_ELEMENT)
                    {
                        if (stricmp(part->Value(), "Chip")==0)
                        {
                            // we now have the chip id string, but we need to find
                            // the copy which is always in memory so that we get a
                            // sane pointer to it for gChipName.
                            const char *chipname = NULL;
                            const char *temp = ((TiXmlElement*)part)->Attribute("Name");
                            if (temp)
                            {
								// boxes are... special.
								int i = strlen(temp);
								if (i > 7 && stricmp(temp+i-7,".atanua") == 0)
								{
									chipname = mystrdup(temp);
									FILE * f = openfileinsamedir(temp);
									if (!f)
									{
                                        char temp[1024];
                                        sprintf(temp,"Boxed atanua file '%s' not found.\nTry to continue loading?\n\n(it will show up as box with no pins)", chipname);
                                        if (okcancel(temp) == 0)
                                        {
											delete[] chipname;
                                            build_nets();
                                            return;
                                        }
									}
									else
									{
										fclose(f);
									}
								}
								else
                                if (stricmp(temp, "Connection Pin") == 0)
                                {
									// Connection pin is a special case, as you can't create it
									// in a "normal" way
                                    chipname = "Connection Pin";
                                }
                                else
                                {
                                    int j, k;
                                    for (k = 0; chipname == NULL && k < 5; k++)
                                    {
                                        for (j = 0; chipname == NULL && j < (signed)gAvailableChip[k].size(); j++)
                                        {
                                            if (gAvailableChip[k][j])
                                            {
                                                if (stricmp(temp, gAvailableChip[k][j]) == 0)
                                                {
                                                    chipname = gAvailableChip[k][j];
                                                }
                                            }
                                        }
                                    }
                                }
                                
                                if (chipname != NULL)
                                {
                                    int x, y, key, angle;
                                    x = y = key = angle = 0;                                    
                                    ((TiXmlElement*)part)->QueryIntAttribute("xpos", &x);
                                    ((TiXmlElement*)part)->QueryIntAttribute("ypos", &y);
                                    ((TiXmlElement*)part)->QueryIntAttribute("key", &key);
                                    ((TiXmlElement*)part)->QueryIntAttribute("rot", &angle);
                                    int AngleIn90DegreeSteps = angle;
                                    float X = (float)x / (1 << scale);
                                    float Y = (float)y / (1 << scale);

									int len = strlen(chipname);
									outf->writeword(len);
									outf->writechars(chipname, len);
									outf->writeint((int)floor((1 << 16) * X));
									outf->writeint((int)floor((1 << 16) * Y));
									outf->writeint(AngleIn90DegreeSteps);
									outf->writeint(0);

                                    TiXmlNode *text;
                                    for (text = part->FirstChild(); text != 0; text = text->NextSibling())
                                    {
                                        if (text->Type() == TiXmlNode::TINYXML_TEXT)
                                        {
                                            const char *v = text->Value();
                                            int wholebyte = 0;
                                            int data = 0;
											MemoryFile mf;
                                            while (*v)
                                            {
                                                if (*v >= '0' && *v <= '9')
                                                {
                                                    data <<= 4;
                                                    data |= *v - '0';
                                                    wholebyte++;
                                                }
                                                else
                                                if (*v >= 'A' && *v <= 'F')
                                                {
                                                    data <<= 4;
                                                    data |= *v - 'A' + 10;
                                                    wholebyte++;
                                                }
                                                else
                                                if (*v >= 'a' && *v <= 'f')
                                                {
                                                    data <<= 4;
                                                    data |= *v - 'a' + 10;
                                                    wholebyte++;
                                                }
                                                // ignore other values, including whitespace
                                                if (wholebyte == 2)
                                                {
													mf.writebyte(data);
                                                    data = 0;
                                                    wholebyte = 0;
                                                }
                                                v++;
                                            }

											if (stricmp(chipname, "External Pin") == 0)
											{
												if (bd)
												{
													mf.seek(0);
													mf.readint();
													int l = mf.readint();
													char *temp = new char[l+1];
													memset(temp,0,l+1);
													mf.readchars(temp, l);
													bd->mTooltips.push_back(temp);
												}
											}
											
											int i;
											for (i = 0; i < (signed)mf.mData.size(); i++)
												outf->writebyte(mf.mData[i]);											

                                            if (wholebyte != 0)
                                            {
                                                char temp[1024];
                                                sprintf(temp,"Decoding chip-specific data while loading '%s' failed.\nTry to continue loading?", chipname);
                                                if (okcancel(temp) == 0)
                                                {
                                                    return;
                                                }
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    char mtemp[1024];
                                    sprintf(mtemp,"Chip called '%s' requested by savefile but it is\n"
                                                  "not supported by this version, or a plugin is missing.\n"
                                                  "\n"
                                                  "Try to continue loading?", temp);
                                    if (okcancel(mtemp) == 0)
                                    {
                                        build_nets();
                                        return;
                                    }
                                }
                            }
                        }
                        if (stricmp(part->Value(), "Wire")==0)
                        {
                            int chip1, pin1, chip2, pin2;
                            ((TiXmlElement*)part)->QueryIntAttribute("chip1", &chip1);
                            ((TiXmlElement*)part)->QueryIntAttribute("chip2", &chip2);
                            ((TiXmlElement*)part)->QueryIntAttribute("pad1", &pin1);
                            ((TiXmlElement*)part)->QueryIntAttribute("pad2", &pin2);
							
							outf->writeint((chip1<<16) | pin1);
							outf->writeint((chip2<<16) | pin2);
							outf->writeint(0);
						}
                    }
                }
            }
        }
    }	
}
