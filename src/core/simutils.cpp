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

// For wire frying:
static vector<Chip *> gFryList;

void do_loadxml(FILE * f, int box = 0);
void do_flush_boxloadqueue();
void do_loadoldbinary(File *f);
void do_savexml(FILE * filehandle);
BoxStitchingInformation * do_preparse_box(const char *aFname);
void do_loadxml(FILE * f, int box);
void do_savebinary(File * f);

int gDirtyNets = 0;

void do_build_nets();

void build_nets()
{
    // Clear the drag mode just in case
    do_cancel();

	if (!gDirtyNets)
	{
		// Wipe old nets; this'll make sure we'll crash if do_build_nets is needed somewhere.
		int i, j;
		for (i = 0; i < (signed)gNet.size(); i++)
		{
			delete gNet[i];
		}
		gNet.clear();

		// and mark all wires as free
		for (i = 0; i < (signed)gWire.size(); i++)
		{
			gWire[i]->mFirst->mNet = NULL;
			gWire[i]->mSecond->mNet = NULL;
			//gWire[i]->mNet = NULL;
		}

		// We also have to go through all the pins of all the chips and mark their nets as null.
		for (i = 0; i < (signed)gChip.size(); i++)
			for (j = 0; j < (signed)gChip[i]->mPin.size(); j++)
				gChip[i]->mPin[j]->mNet = NULL;
	}

	gDirtyNets++;
}

void do_build_nets()
{
    int i, j, c;

	if (!gDirtyNets)
		return;

	/*
	char temp[100];
	sprintf(temp,"do_build_nets(%d)\n", gDirtyNets);
	OutputDebugStringA(temp);
	int starttick = SDL_GetTicks();
	*/

	gDirtyNets = 0;

    // Wipe old nets
    for (i = 0; i < (signed)gNet.size(); i++)
    {
        delete gNet[i];
    }
    gNet.clear();

    // and mark all wires as free
    for (i = 0; i < (signed)gWire.size(); i++)
    {
        gWire[i]->mFirst->mNet = NULL;
        gWire[i]->mSecond->mNet = NULL;
        //gWire[i]->mNet = NULL;
    }

    // We also have to go through all the pins of all the chips and mark their nets as null.
    for (i = 0, c = 1; i < (signed)gChip.size(); i++)
	{
        for (j = 0; j < (signed)gChip[i]->mPin.size(); j++, c++)
		{
            gChip[i]->mPin[j]->mNet = NULL;
			gChip[i]->mPin[j]->mNetId = c;
		}
	}

    if (gNewChip)
	{
		for (j = 0; j < (signed)gNewChip->mPin.size(); j++, c++)
		{
			gNewChip->mPin[j]->mNet = NULL;
			gNewChip->mPin[j]->mNetId = c;
		}
	}

	int modified = 1;
	int max = 0;
	while (modified)
	{
		modified = 0;
		max = 0;
		for (i = 0; i < (signed)gWire.size(); i++)
		{
			if (gWire[i]->mFirst->mNetId != gWire[i]->mSecond->mNetId)
			{
				modified = 1;
				int min = (gWire[i]->mFirst->mNetId < gWire[i]->mSecond->mNetId) ? gWire[i]->mFirst->mNetId : gWire[i]->mSecond->mNetId;
				gWire[i]->mFirst->mNetId = min;
				gWire[i]->mSecond->mNetId = min;
			}
			if (gWire[i]->mFirst->mNetId > max) max = gWire[i]->mFirst->mNetId;
		}
	}

	max++;

	int *crossmap = new int[max];
	memset(crossmap, 0, sizeof(int) * max);
	for (i = 0; i < (signed)gWire.size(); i++)
	{
		int idx = gWire[i]->mFirst->mNetId;
		if (crossmap[idx] == 0)
		{
			Net *n = new Net;
			gNet.push_back(n);
			crossmap[idx] = gNet.size() - 1;
		}
	}

	for (i = 0; i < (signed)gWire.size(); i++)
	{
		if (gWire[i]->mFirst->mNet == 0)
		{
			if (!gWire[i]->mFirst->mReadOnly)
			{
				gNet[crossmap[gWire[i]->mFirst->mNetId]]->mPin.push_back(gWire[i]->mFirst);
			}
			else
			{
				gNet[crossmap[gWire[i]->mFirst->mNetId]]->mROPin.push_back(gWire[i]->mFirst);
			}
			gWire[i]->mFirst->mNet = gNet[crossmap[gWire[i]->mFirst->mNetId]];
		}
		if (gWire[i]->mSecond->mNet == 0)
		{
			if (!gWire[i]->mSecond->mReadOnly)
			{
				gNet[crossmap[gWire[i]->mSecond->mNetId]]->mPin.push_back(gWire[i]->mSecond);
			}
			else
			{
				gNet[crossmap[gWire[i]->mSecond->mNetId]]->mROPin.push_back(gWire[i]->mSecond);
			}
			gWire[i]->mSecond->mNet = gNet[crossmap[gWire[i]->mSecond->mNetId]];
		}
	}
	delete[] crossmap;
}

void delete_chip(Chip *c)
{
    // First let's find the chip's index.
    int idx = 0;
    int i;
    for (i = 0; i < (signed)gChip.size(); i++)
    {
        if (gChip[i] == c)
        {
            idx = i;
            break;
        }
    }


    // Special case: if chip is "connection pin" which has two connections, fuse the two wires together.
    // Otherwise work as usual.
    if (stricmp(gChipName[idx], "Connection Pin") == 0)
    {
        int wires[2];
        int wirecount = 0;
        int w;
        wires[0] = wires[1] = 0;
        for (w = 0; w < (signed)gWire.size(); w++)
        {
            if (gWire[w]->mFirst == gChip[idx]->mPin[0] ||
                gWire[w]->mSecond == gChip[idx]->mPin[0])
            {
                if (wirecount == 2)
                {
                    wirecount++;
                    break;
                }
                wires[wirecount] = w;
                wirecount++;
            }
        }

        if (wirecount == 2)
        {
            // exactly two wires found
            Pin *loosepin = NULL;
            if (gWire[wires[0]]->mFirst == gChip[idx]->mPin[0])
            {
                loosepin = gWire[wires[0]]->mSecond;
            }
            else
            {
                loosepin = gWire[wires[0]]->mFirst;
            }

            if (gWire[wires[1]]->mFirst == gChip[idx]->mPin[0])
            {
                gWire[wires[1]]->mFirst = loosepin;
            }
            else
            {
                gWire[wires[1]]->mSecond = loosepin;
            }
            
            delete gWire[wires[0]];            
            gWire.erase(gWire.begin() + wires[0]);

            delete gChip[idx];
            gChip.erase(gChip.begin() + idx);
            gChipName.erase(gChipName.begin() + idx);
            return; 
        }
    }


    // Delete all wires connected to all pins. 
    int p;
    for (p = 0; p < (signed)c->mPin.size(); p++)
    {
        int w;
        for (w = 0; w < (signed)gWire.size(); w++)
        {
            if (gWire[w]->mFirst == c->mPin[p] ||
                gWire[w]->mSecond == c->mPin[p])
            {
                if (gConfig.mWireFry)
                {
                    //////////////////////////////////////////////////////////
                    // wire fry code
                    Pin *query = NULL;
                    
                    if (gWire[w]->mFirst == c->mPin[p])
                        query = gWire[w]->mSecond;
                    else
                        query = gWire[w]->mFirst;


                    int chipidx = 0;
                    int i;
                    for (i = 0; i < (signed)gChip.size(); i++)
                    {
                        if (gChip[i] == query->mHost)
                        {
                            chipidx = i;
                            break;
                        }
                    }

                    if (stricmp(gChipName[chipidx], "Connection Pin") == 0)
                    {
                        // Make sure it's not on the list already
                        int found = 0;
                        int k;
                        for (k = 0; !found && k < (signed)gFryList.size(); k++)
                            if (gFryList[k] == query->mHost)
                                found = 1;
                        if (!found)
                            gFryList.push_back(query->mHost);
                    }

                    // wire fry code
                    //////////////////////////////////////////////////////////
                }

                delete gWire[w];
                gWire.erase(gWire.begin() + w);
                w--; // back one step
            }
        }
    }

    if (gConfig.mWireFry)
    {
        while (!gFryList.empty())
        {
            Chip * chiptofry = gFryList.back();
            gFryList.pop_back();
            int wirecount = 0;
            int w;
            for (w = 0; w < (signed)gWire.size(); w++)
            {
                if (gWire[w]->mFirst == chiptofry->mPin[0] ||
                    gWire[w]->mSecond == chiptofry->mPin[0])
                {
                    wirecount++;
                }
            }
            if (wirecount <= 2)
            {
                delete_chip(chiptofry);
            }
        }
    }

    // now we're ready to delete the chip. 
    for (i = 0; i < (signed)gChip.size(); i++)
    {
        if (gChip[i] == c)
        {
			// tricky - deleting a chip may nuke lots of chips as it may be a box
			Chip *chip_to_delete = gChip[i];
            gChip.erase(gChip.begin() + i);
            gChipName.erase(gChipName.begin() + i);
            delete chip_to_delete;
            return; 
        }
    }

	// Remember to rebuild the nets after deletion! 
	// (Moved outside function for performance reasons)
    //build_nets();
}

float line_point_distance(float x0, float y0, float x1, float y1, float x2, float y2)
{   
    // line-point distance
    float linelen = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
    float lpd;
    if (linelen == 0)
        lpd = 1000000; // far enough
    else
        lpd = fabs((x2-x1)*(y1-y0)-(x1-x0)*(y2-y1)) / linelen;

    // point-vtx0 distance
    float pvd0 = sqrt((x0-x1) * (x0-x1) + (y0-y1) * (y0-y1));

    // point-vtx1 distance
    float pvd1 = sqrt((x0-x2) * (x0-x2) + (y0-y2) * (y0-y2));

    if (pvd1 < linelen && pvd0 < linelen)
        return MIN(MIN(lpd, pvd0), pvd1);
    else
        return MIN(pvd0, pvd1);
}



void add_wire(Pin *aFirst, Pin *aSecond)
{
    int i;
    for (i = 0; i < (signed)gWire.size(); i++)
        if ((gWire[i]->mFirst == aFirst && gWire[i]->mSecond == aSecond) ||
            (gWire[i]->mFirst == aSecond && gWire[i]->mSecond == aFirst))
            return; // no duplicate wires
    gWire.push_back(new Wire(aFirst, aSecond));                        
    build_nets();
}

extern void resetfilename();

void do_reset()
{
    do_cancel();
	
	// Wipe away all wires first so we won't need to do wire frying.
    int i;
    for (i = 0; i < (signed)gWire.size(); i++)
    {
        delete gWire[i];
    }
    gWire.clear();

	// Deleting chips is trickier, because boxes delete their internal chips recursively.
	while (gChip.size())
	{
		Chip * t = gChip.back();
		gChip.pop_back();
		delete t;		
	}
    gChip.clear();
    gChipName.clear();
    build_nets();
}


FILE * openfileinsamedir(const char * aFile);

void do_loadxmltobinary(FILE * f, File * outf, BoxcacheData *bd);

File * do_preload_box(const char *aName)
{
	int i;
	File *bf = 0;
	FILE * f = openfileinsamedir(aName);
	if (!f)
	{
		return NULL;
	}

	for (i = 0; i < (signed)gBoxCache.size(); i++)
	{
		if (stricmp(gBoxCache[i].mName, aName) == 0)
			bf = gBoxCache[i].mData;
	}

	if (!bf)
	{
		bf = new MemoryFile;
		BoxcacheData bd;
		do_loadxmltobinary(f, bf, &bd);
		bd.mData = bf;
		bd.mName = mystrdup(aName);
		gBoxCache.push_back(bd);
	}
	fclose(f);
	return bf;
}

void do_flush_boxloadqueue()
{
	while (gBoxLoadQueue.size() > 0)
	{
		BoxLoadQueueItem * item = gBoxLoadQueue.back();
		gBoxLoadQueue.pop_back();
		gBoxBeingLoaded = item->mBox;
		File *bf = do_preload_box(item->mFilename);
		if (bf == NULL)
		{
			// pretty catastrophic, but we'll do what we can..
			gBoxBeingLoaded = NULL;
			build_nets();
			return;
		}

		//do_loadxml(f, item->mBoxId);
		bf->seek(0);
		do_loadbinary(bf, item->mBoxId);

		BoxStitchingInformation * bsi = gBoxBeingLoaded->getBoxStitchingInformation();		
		int i;
		for (i = 0; i < (signed)bsi->mExtPinsInside.size(); i++)
		{
			Wire * w = new Wire(bsi->mExtPinsInside[i]->mPin[0], &bsi->mExtPinOutside[i]);
			w->mBox = item->mBoxId;
			gWire.push_back(w);
		}

		do_optimize_box(item->mBoxId);

		delete item;
	}
	gBoxBeingLoaded = 0;
}



void do_cancel()
{
    gMultiselectDirty = 1;
    gMultiSelectChip.clear();
    gMultiSelectWire.clear();
    gUIState.activeitem = 0;
    gUIState.kbditem = 0;
    gUIState.hotitem = 0;
    if (gDragMode == DRAGMODE_NEWCHIP)
    {
        delete gNewChip;
        gNewChip = NULL;
        gNewChipName = NULL; // always points to constant data
    }
    if (gDragMode != DRAGMODE_NONE)
        gDragMode = DRAGMODE_NONE;
}

static int gSaveCycle = 0;
static int gSaveInterval = 0;

void save_undo()
{
	if (gConfig.mAutosaveEnable)
	{
		gSaveInterval++;
		if (gSaveInterval >= gConfig.mAutosaveInterval)
		{
			gSaveInterval = 0;
			char temp[1024];		
			sprintf(temp, "%sautosave%04d.atanua", gConfig.mAutosaveDir, gSaveCycle);
			gSaveCycle = (gSaveCycle + 1) % gConfig.mAutosaveCount;
			FILE * f = fopen(temp, "wb");
			do_savexml(f);
		}
	}
    MemoryFile * state = new MemoryFile();
    do_savebinary(state);
    gUndoStack.push_back(state);
    if (!gRedoStack.empty())
    {
        int i;
        for (i = 0; i < (signed)gRedoStack.size(); i++)
            delete gRedoStack[i];
        gRedoStack.clear();
    }
}

void do_undo()
{
    if (!gUndoStack.empty())
    {
        do_cancel();
        File * state = new MemoryFile();
        do_savebinary(state);
        gRedoStack.push_back(state);
        do_reset();
        state = gUndoStack.back();
        state->seek(0);
        do_loadbinary(state);
        gUndoStack.pop_back();
        delete state;
    }
}

void do_redo()
{
    if (!gRedoStack.empty())
    {
        do_cancel();
        File * state = new MemoryFile();
        do_savebinary(state);
        gUndoStack.push_back(state);
        do_reset();
        state = gRedoStack.back();
        state->seek(0);
        do_loadbinary(state);
        gRedoStack.pop_back();
        delete state;
    }
}

void do_savedialog()
{
    FILE * fh = savefiledialog(NULL);
    if (fh)
    {
        do_savexml(fh);      
    }
}

void storefilename(const char *fn);
extern char *gFilename;
void clear_boxcache();

void do_loaddialog(int merge, const char *aFilename)
{
	clear_boxcache();
    FILE *fh = 0;
	char *boxfn = NULL;
	char *origactivefilename = NULL;
	if (gFilename)
		origactivefilename = mystrdup(gFilename);
    if (aFilename)
    {
        fh = fopen(aFilename, "rb");
		storefilename(aFilename);
    }
    else
    {
		if (merge == 2)
		{
			// pop up the dialog
			fh = openfiledialog(NULL);
			
			if (fh)
			{
				// find the last part of the file without path
				char * s1 = strrchr(gFilename,'\\');
			
				if (s1 == NULL)
				{
					s1 = strrchr(gFilename,'/');
				}
			
				// if above fails, just use the whole name
				if (s1 == NULL)
				{
					s1 = gFilename;
				}
				else
				{
					// skip the last slash/backslash
					s1++;
				}
			
				// this shall be the box filename
				boxfn = mystrdup(s1);
			}
		}
		else
		{
			fh = openfiledialog(NULL);
		}
    }

    if (fh)
    {
        save_undo();
		if (!merge)
			do_reset();
        File * f = new File(fh);
        int id = f->readint();
        f->seek(0);
        if (id == 0x00617441) // 'Ata\0'
        {
            // It's binary
            do_loadoldbinary(f);
        }
        else
        {
            // It's XML
			if (merge == 2)
			{
				// boxing
				BoxStitchingInformation * bsi = do_preparse_box(boxfn);
				if (bsi == NULL)
				{
					// box loading failed
					delete[] boxfn;
					delete f;
					storefilename(origactivefilename);
					delete[] origactivefilename;
					return;
				}
				Box * b = new Box(boxfn, bsi);

				// do actual loading
				do_flush_boxloadqueue();

				gNewChip = b;
				gNewChipName = boxfn;

				do_build_nets(); // causes cancel; otherwise the drag mode gets cancelled.

				gDragMode = DRAGMODE_NEWCHIP;
				storefilename(origactivefilename);
				delete[] origactivefilename;
			}
			else
			{
				int oldmaxchip = gChip.size();
				int oldmaxwire = gWire.size();
				do_loadxml(fh, 0);
				if (merge)
				{
					int i;
					for (i = oldmaxchip; i < (signed)gChip.size(); i++)
					{
						if (gChip[i]->mBox == 0)
						{
							gMultiSelectChip.push_back(gChip[i]);
							gChip[i]->mMultiSelectState = 1;
						}
					}
					for (i = oldmaxwire; i < (signed)gWire.size(); i++)
					{
						if (gWire[i]->mBox == 0)
						{
							gMultiSelectWire.push_back(gWire[i]);
							gWire[i]->mMultiSelectState = 1;
						}
					}
					storefilename(origactivefilename);
					delete[] origactivefilename;
				}
			}
        }
        delete f; // closes handle
    }
}

void do_resetdialog()
{
    save_undo();
    if (okcancel("Are you sure you want to reset?\nAny unsaved changes will be lost."))
	{
		resetfilename();
        do_reset();
	}
}

void do_home()
{
    gWorldOfsX = 0;
    gWorldOfsY = 0;
    gZoomFactor = 20;
}

void do_zoomext()
{
    float minx=10000, miny=10000, maxx=-10000, maxy=-10000;
    int i;
    for (i = 0; i < (signed)gChip.size(); i++)
    {
        if (minx > gChip[i]->mX)
            minx = gChip[i]->mX;

        if (miny > gChip[i]->mY)
            miny = gChip[i]->mY;

        if (maxx < gChip[i]->mX + gChip[i]->mW)
            maxx = gChip[i]->mX + gChip[i]->mW;

        if (maxy < gChip[i]->mY + gChip[i]->mH)
            maxy = gChip[i]->mY + gChip[i]->mH;
    }
    if (minx != 10000)
    {
        float w = maxx - minx;
        float h = maxy - miny;
        minx -= w * 0.05;
        maxx += w * 0.05;
        miny -= h * 0.05;
        maxy += h * 0.05;
        w = maxx - minx;
        h = maxy - miny;

        gWorldOfsX = -minx;
        gWorldOfsY = -miny;
        
        if (h > w)
        {
            gZoomFactor = (gScreenHeight - 40) / h;
        }
        else
        {
            gZoomFactor = (gScreenWidth - gConfig.mToolkitWidth) / w;
        }       
    }
}

void do_optimize_box(int aBox)
{
	unsigned int i = 0;
	do
	{
		if (gChip.size() &&
			gChip[i]->mBox == aBox &&
			gChip[i]->isUselessInBox())
		{
			delete_chip(gChip[i]);
		}
		else
		{
			i++;
		}
	}
	while (i < gChip.size());	
}

