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

#include "basechipfactory.h"
#include "pluginchipfactory.h"

#include <stb_image_write.h>

#define C_MENUBG 0xff3f4f4f
#define C_WIDGETBG 0xff3f5f6f
#define C_WIDGETTHUMB 0xff7f8f9f
#define C_WIDGETHOT 0xff9fcfff

#define WORLDTOSCREENX(x) ((((x)+gWorldOfsX) * gZoomFactor) + gConfig.mToolkitWidth)
#define WORLDTOSCREENY(y) ((((y)+gWorldOfsY) * gZoomFactor) + 40)

ACFont fn, fn14;

class Box;
vector<BoxLoadQueueItem *> gBoxLoadQueue;
Box *gBoxBeingLoaded = NULL;
vector<char*> gBoxNames;
int gBoxCount = 0;
int gActiveBoxes = 0;

vector<Chip*> gChip;
vector<const char*> gChipName;
vector<Wire*> gWire;
vector<Net*> gNet;
vector<ChipFactory*> gChipFactory;

vector<char *> gAvailableChip[5];

vector<File *> gUndoStack;
vector<File *> gRedoStack;

vector<BoxcacheData> gBoxCache;

vector<Chip*> gMultiSelectChip;
vector<Wire*> gMultiSelectWire;
int gMultiselectDirty = 1;

AtanuaConfig gConfig;
int gVisibleChiplist = 0;

Chip * gNewChip = NULL;
const char * gNewChipName = NULL;
Pin * gWireStartDrag = NULL;
int gKeyState[SDLK_LAST];

float gWorldOfsX = 0, gWorldOfsY = 0;
float gZoomFactor = 20.0f;
int gDragMode = DRAGMODE_NONE;
int gSnap = 1;
int gLiveWires = 1;
int gBlackBackground = 1;

int gSelectKeyMask;
int gCloneKeyMask;

int gSavePNG = 0;

char * gSidebarTooltip = NULL;
int gSidebarTooltipId = -1;

SDL_AudioSpec *gAudioSpec = NULL;

SDL_Cursor *cursor_normal, *cursor_drag, *cursor_scissors;

#define AUDIOBUF_SIZE 512
unsigned char gAudioBuffer[AUDIOBUF_SIZE];
int gRecordHead = AUDIOBUF_SIZE/2;
float gPlayHead = 0;
unsigned char *gAudioOut;

void initvideo();

void handle_key(int keysym, int down)
{
    switch(keysym)
    {
    case SDLK_ESCAPE:
        if (down)
        {
            do_cancel();
        }
        break;        
    }
    if (keysym >= 0 && keysym < SDLK_LAST)
        gKeyState[keysym] = down;
}

void do_rotate()
{
    if (IS_CHIP_ID(gUIState.kbditem))
    {
        int id = GET_CHIP_ID(gUIState.kbditem);
        gChip[id]->mAngleIn90DegreeSteps++;
        gChip[id]->mAngleIn90DegreeSteps &= 0x03;
        gChip[id]->rotate(gChip[id]->mAngleIn90DegreeSteps);
    }
}

void do_screengrab()
{
	FILE * f = NULL;
	char tempname[256];
	int i = 0;
	do
	{
		if (f) fclose(f);
		i++;
		sprintf(tempname, "atanua%03d.png", i);
		f = fopen(tempname, "rb");
	}
	while(f);

	int x0 = gConfig.mToolkitWidth;
	int y0 = 40;
	int w = gScreenWidth - x0;
	int h = gScreenHeight - y0;
	
	char * data = new char[w*h*4];
	char * flipdata = new char[w*h*4];
	glReadPixels(x0,0,w,h,GL_RGBA,GL_UNSIGNED_BYTE,data);
	
	for (i = 0; i < h; i++)
		memcpy(flipdata+(h-i-1)*w*4,data+i*w*4,w*4);

	stbi_write_png(tempname,w,h,4,flipdata,w*4);
	delete[] data;
	delete[] flipdata;

	char tempout[512];
	sprintf(tempout, "%s saved.", tempname);

	okcancel(tempout);
}

void process_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) 
    {
        switch (event.type) 
        {
        case SDL_KEYDOWN:
            handle_key(event.key.keysym.sym, 1);
            // If a key is pressed, report it to the widgets
            gUIState.keyentered = event.key.keysym.sym;
            gUIState.keymod = event.key.keysym.mod;
            if (event.key.keysym.sym == SDLK_LCTRL) gUIState.keymod |= KMOD_LCTRL;
            if (event.key.keysym.sym == SDLK_RCTRL) gUIState.keymod |= KMOD_RCTRL;
            if (event.key.keysym.sym == SDLK_LSHIFT) gUIState.keymod |= KMOD_LSHIFT;
            if (event.key.keysym.sym == SDLK_RSHIFT) gUIState.keymod |= KMOD_RSHIFT;
            if (event.key.keysym.sym == SDLK_LALT) gUIState.keymod |= KMOD_LALT;
            if (event.key.keysym.sym == SDLK_RALT) gUIState.keymod |= KMOD_RALT;

            // Alias for 'del', as accessing it may be difficult on laptops etc.
            if (event.key.keysym.sym == SDLK_d &&
                event.key.keysym.mod & KMOD_CTRL)
                gUIState.keyentered = SDLK_DELETE;


            // if key is ASCII, accept it as character input
            if ((event.key.keysym.unicode & 0xFF80) == 0)
                gUIState.keychar = event.key.keysym.unicode & 0x7f;                
            break;
        case SDL_KEYUP:
            gUIState.keymod = event.key.keysym.mod;
            handle_key(event.key.keysym.sym, 0);
            if (event.key.keysym.sym == SDLK_z &&
                event.key.keysym.mod & KMOD_META)
                do_undo();
            if (event.key.keysym.sym == SDLK_z &&
                event.key.keysym.mod & KMOD_CTRL)
                do_undo();
            if (event.key.keysym.sym == SDLK_BACKSPACE &&
                event.key.keysym.mod & KMOD_ALT)
                do_undo();
            if (event.key.keysym.sym == SDLK_y &&
                event.key.keysym.mod & KMOD_META)
                do_redo();
            if (event.key.keysym.sym == SDLK_y &&
                event.key.keysym.mod & KMOD_CTRL)
                do_redo();
            if (event.key.keysym.sym == SDLK_z &&
                event.key.keysym.mod & KMOD_CTRL &&
                event.key.keysym.mod & KMOD_SHIFT)
                do_redo();
            if (event.key.keysym.sym == SDLK_z &&
                event.key.keysym.mod & KMOD_CTRL &&
                event.key.keysym.mod & KMOD_ALT)
                do_redo();
            if (event.key.keysym.sym == SDLK_BACKSPACE &&
                event.key.keysym.mod & KMOD_ALT &&
                event.key.keysym.mod & KMOD_SHIFT)
                do_redo();
            if (event.key.keysym.sym == SDLK_s &&
                event.key.keysym.mod & KMOD_CTRL)
                do_savedialog();
            if (event.key.keysym.sym == SDLK_l &&
                event.key.keysym.mod & KMOD_CTRL)
                do_loaddialog();
            if (event.key.keysym.sym == SDLK_m &&
                event.key.keysym.mod & KMOD_CTRL)
                do_loaddialog(1);
            if (event.key.keysym.sym == SDLK_b &&
                event.key.keysym.mod & KMOD_CTRL)
                do_loaddialog(2);
            if (event.key.keysym.sym == SDLK_n &&
                event.key.keysym.mod & KMOD_CTRL)
                do_resetdialog();
            if (event.key.keysym.sym == SDLK_h &&
                event.key.keysym.mod & KMOD_CTRL)
                do_home();
            if (event.key.keysym.sym == SDLK_e &&
                event.key.keysym.mod & KMOD_CTRL)
                do_zoomext();
            if (event.key.keysym.sym == SDLK_p &&
                event.key.keysym.mod & KMOD_CTRL)
                gSnap = !gSnap;
            if (event.key.keysym.sym == SDLK_w &&
                event.key.keysym.mod & KMOD_CTRL)
                gLiveWires = !gLiveWires;
            if (event.key.keysym.sym == SDLK_r &&
                event.key.keysym.mod & KMOD_CTRL)
                do_rotate();
            if (event.key.keysym.sym == SDLK_o &&
                event.key.keysym.mod & KMOD_CTRL)
			{
				save_undo();
                do_optimize_box(0);
			}
            if (event.key.keysym.sym == SDLK_g &&
                event.key.keysym.mod & KMOD_CTRL)
                do_screengrab();

            break;
        case SDL_MOUSEMOTION:
            // update mouse position
            gUIState.mousex = event.motion.x;
            gUIState.mousey = event.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            // update button down state if left-clicking
            if (event.button.button == 1)
            {
                gUIState.mousedown = 1;
                gUIState.mousedownx = event.motion.x;
                gUIState.mousedowny = event.motion.y;
                gUIState.mousedownkeymod = gUIState.keymod;
            }
            if (event.button.button == 3)
            {
                do_cancel();
            }

            if (event.button.button == 4)
            {
                gUIState.scroll = +1;
            }
            if (event.button.button == 5)
            {
                gUIState.scroll = -1;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            // update button down state if left-clicking
            if (event.button.button == 1)
                gUIState.mousedown = 0;
            break;
        case SDL_QUIT:
            SDL_Quit();
            exit(0);
            break;
        case SDL_VIDEORESIZE:
            gScreenWidth = event.resize.w;
            gScreenHeight = event.resize.h;
            initvideo();
            break;
        }
    }
}

int getChipIdForPad(Pin *p)
{
    int k, l;
    for (k = 0; k < (signed)gChip.size(); k++)
    {
        for (l = 0; l < (signed)gChip[k]->mPin.size(); l++)
        {
            if (gChip[k]->mPin[l] == p)
            {
                return CHIP_ID(l+1, k);
            }
        }
    }
    return 0;
}

int split_wire(int aDoSplit)
{
    float worldmousex = ((gUIState.mousex - gConfig.mToolkitWidth) / gZoomFactor) - gWorldOfsX;
    float worldmousey = ((gUIState.mousey - 40) / gZoomFactor) - gWorldOfsY;
    float pos1[2], pos2[2], pos3[2];
    int wireid;
    if (aDoSplit)
        wireid = GET_WIRE_ID(gUIState.activeitem);
    else
        wireid = GET_WIRE_ID(gUIState.hotitem);
    pos1[0] = gWire[wireid]->mFirst->mHost->mRotatedX + gWire[wireid]->mFirst->mRotatedX + 0.25;
    pos1[1] = gWire[wireid]->mFirst->mHost->mRotatedY + gWire[wireid]->mFirst->mRotatedY + 0.25;
    pos2[0] = gWire[wireid]->mSecond->mHost->mRotatedX + gWire[wireid]->mSecond->mRotatedX + 0.25;
    pos2[1] = gWire[wireid]->mSecond->mHost->mRotatedY + gWire[wireid]->mSecond->mRotatedY + 0.25;
    pos1[0] = WORLDTOSCREENX(pos1[0]);
    pos1[1] = WORLDTOSCREENY(pos1[1]);
    pos2[0] = WORLDTOSCREENX(pos2[0]);
    pos2[1] = WORLDTOSCREENY(pos2[1]);
    if (aDoSplit)
    {
        pos3[0] = gUIState.mousedownx;
        pos3[1] = gUIState.mousedowny;
    }
    else
    {
        pos3[0] = gUIState.mousex;
        pos3[1] = gUIState.mousey;
    }

    float wirelen = sqrt((pos1[0]-pos2[0])*(pos1[0]-pos2[0]) + (pos1[1]-pos2[1])*(pos1[1]-pos2[1]));
    float p1dist = sqrt((pos1[0]-pos3[0])*(pos1[0]-pos3[0]) + (pos1[1]-pos3[1])*(pos1[1]-pos3[1]));
    float p2dist = sqrt((pos2[0]-pos3[0])*(pos2[0]-pos3[0]) + (pos2[1]-pos3[1])*(pos2[1]-pos3[1]));
        
    if (p1dist < wirelen * gConfig.mLineEndTolerance)
    {
        if (!aDoSplit) return 0;
        // start new line from pin 1
        gDragMode = DRAGMODE_WIRE;
        gWireStartDrag = gWire[wireid]->mFirst;
        gUIState.activeitem = getChipIdForPad(gWireStartDrag);
    }
    else
    if (p2dist < wirelen * gConfig.mLineEndTolerance)
    {
        if (!aDoSplit) return 0;
        // start new line from pin 2
        gDragMode = DRAGMODE_WIRE;
        gWireStartDrag = gWire[wireid]->mSecond;
        gUIState.activeitem = getChipIdForPad(gWireStartDrag);
    }
    else
    {
        if (!aDoSplit) return 1;
        // do the split
        save_undo();
        Chip *newpin = gChipFactory[0]->build("Connection Pin");
        gChip.push_back(newpin);
        gChipName.push_back("Connection Pin");        
        newpin->mX = worldmousex-0.5;
        newpin->mY = worldmousey-0.5;
        newpin->rotate(0);
        gUIState.mousedownx = gUIState.mousex;
        gUIState.mousedowny = gUIState.mousey;
        Pin *second = gWire[wireid]->mSecond;
        gWire[wireid]->mSecond = newpin->mPin[0];
        add_wire(newpin->mPin[0], second);
        gUIState.activeitem = CHIP_ID(0, gChip.size() - 1);
    }
    return 0;
}


void multiselect_active()
{
    if (IS_CHIP_ID(gUIState.kbditem))
    {
        int id = GET_CHIP_ID(gUIState.kbditem);
        if (gChip[id]->mMultiSelectState == 0)
        {
            gMultiSelectChip.push_back(gChip[id]);
            gChip[id]->mMultiSelectState = 1;
        }
    }

    if (IS_WIRE_ID(gUIState.kbditem))
    {
        int id = GET_WIRE_ID(gUIState.kbditem);
        if (gWire[id]->mMultiSelectState == 0)
        {
            gMultiSelectWire.push_back(gWire[id]);
            gWire[id]->mMultiSelectState = 1;
        }
    }
}

void move_chip(Chip *c, int charcode)
{
	switch (charcode)
    {
    case SDLK_LEFT:
        c->mX-=0.5;
        break;
    case SDLK_RIGHT:
        c->mX+=0.5;
        break;
    case SDLK_UP:
        c->mY-=0.5;
        break;
    case SDLK_DOWN:
        c->mY+=0.5;
        break;
    }
    c->rotate(c->mAngleIn90DegreeSteps);
}

void do_build_nets();

static void draw_screen()
{
    int i;
    int tick = SDL_GetTicks();     
    static int slidervalue = 0;
    float worldmousex = ((gUIState.mousex - gConfig.mToolkitWidth) / gZoomFactor) - gWorldOfsX;
    float worldmousey = ((gUIState.mousey - 40) / gZoomFactor) - gWorldOfsY;
    float worldmousedownx = ((gUIState.mousedownx - gConfig.mToolkitWidth) / gZoomFactor) - gWorldOfsX;
    float worldmousedowny = ((gUIState.mousedowny - 40) / gZoomFactor) - gWorldOfsY;
    static float physicstick = 0;
    static int lasttick = 0;
    int mousemode = 0;
    static int lastmousemode = 0;


	if (gSavePNG)
	{
		do_screengrab();
		gSavePNG = 0;
	}

    ////////////////////////////////////
    // Physics
    ////////////////////////////////////

    if (tick - lasttick > 500)
    {
        // time warp..
        lasttick = tick;
    }

    if (tick - lasttick < 10)
    {
        SDL_Delay(5);
        return;
    }

	// avoid physics getting stuck
	if (physicstick > 10000.0)
	{
		physicstick = 0;
	}

    static int physics_iterations = 0;
    int physms = SDL_GetTicks();

	do_build_nets();

	int chips = (signed)gChip.size();
	int nets = (signed)gNet.size();

    while (lasttick < tick)
    {
		int k;
		for (k = 0; k < gConfig.mPhysicsKHz; k++)
		{

			gAudioOut = gAudioBuffer + gRecordHead;
			gRecordHead++;
			gRecordHead &= AUDIOBUF_SIZE-1;

//#pragma omp parallel default(shared) num_threads(4)

			// Chip updates could be split into jobs for a thread pool.
//#pragma omp for 
			for (i = 0; i < chips; i++)
			{
				if (gChip[i]->mDirty)
				{
					gChip[i]->mDirty = 0;
					gChip[i]->update(physicstick);
				}
			}
		
			// Net updates are relatively simple, and there's relatively few nets..
//#pragma omp for 
			for (i = 0; i < nets; i++)
			{
				if (gNet[i]->mDirty)
				{
					gNet[i]->mDirty = 0;
					gNet[i]->update();
				}
				else
				{
					gNet[i]->mHighFreqChanges = 0;
				}
			}
			physics_iterations++;
        
			physicstick += 1.0 / gConfig.mPhysicsKHz;
		}
       
        lasttick += 1;

        // don't allow for physics to drop framerate too low
        
        if ((signed)SDL_GetTicks() - tick > gConfig.mMaxPhysicsMs)
        {
            lasttick = tick;
        }        
    }

    physms = SDL_GetTicks() - physms;

    ////////////////////////////////////
    // Rendering
    ////////////////////////////////////

    if (gBlackBackground)
		glClearColor(0,0,0,1.0);
	else
		glClearColor(0.8,0.8,0.8,1.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	drawrect(0, 0, gConfig.mToolkitWidth, gScreenHeight, C_MENUBG);
	drawrect(0, 0, gScreenWidth, 40, C_MENUBG);

    imgui_prepare();

    glEnable(GL_SCISSOR_TEST);
    glScissor(0,0,gConfig.mToolkitWidth-20,gScreenHeight - 40);

    int loc = -1;
    if (gUIState.scroll && gUIState.mousex < gConfig.mToolkitWidth && gUIState.mousey > 40)
    {
        slidervalue -= gUIState.scroll * 26 * 3;
        int max = ((signed)gAvailableChip[gVisibleChiplist].size() * 26) - (gScreenHeight - 40);
        if (slidervalue < 0) slidervalue = 0;
        if (slidervalue > max) slidervalue = max;
    }
    if (gUIState.mousex < gConfig.mToolkitWidth-20 && gUIState.mousey > 40)
    {
        loc = (gUIState.mousey - 40 + slidervalue) / 26;
    }    

    for (i = 0; i < (signed)gAvailableChip[gVisibleChiplist].size(); i++)
    {
        if (gAvailableChip[gVisibleChiplist][i])
        {
            if (loc == i)
            {
                gUIState.hotitem = NEWCHIP_ID(loc);
                if(gDragMode == DRAGMODE_NEWCHIP && gUIState.activeitem == 0 && gUIState.mousedown)
                {
                    do_cancel();
                }
                else
                if (gDragMode == DRAGMODE_NONE && gUIState.activeitem == 0 && gUIState.mousedown)
                {
                    // clear multiselect if any
                    gMultiSelectChip.clear();
                    gMultiSelectWire.clear();
                    gMultiselectDirty = 1;                    
                    int j;
                    for (j = 0; gNewChip == NULL && j < (signed)gChipFactory.size(); j++)
                        gNewChip = gChipFactory[j]->build(gAvailableChip[gVisibleChiplist][i]);
                    if (gNewChip)
                    {
                        gNewChipName = gAvailableChip[gVisibleChiplist][i];
                        gDragMode = DRAGMODE_NEWCHIP;
	                    gUIState.mousedownkeymod &= ~gCloneKeyMask; // stop cloning (if pressed)
                    }
                    gUIState.activeitem = gUIState.hotitem;
                }
                if (gUIState.hotitem == gUIState.activeitem)
                    drawrect(0, 40+i*26-slidervalue, gConfig.mToolkitWidth-20, 26, 0xff000000);
                else
                    drawrect(0, 40+i*26-slidervalue, gConfig.mToolkitWidth-20, 26, 0xff7f7f7f);
            }
            if (!(gVisibleChiplist == 3 && i == 8))
            fn14.drawstring(gAvailableChip[gVisibleChiplist][i], 0, 46 + i * 26 - slidervalue); 
        }    
    }
    glDisable(GL_SCISSOR_TEST);    

    imgui_slider(GEN_ID,gConfig.mToolkitWidth-20,40,20,gScreenHeight-40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,((signed)gAvailableChip[gVisibleChiplist].size() * 26) - (gScreenHeight - 40),slidervalue, (gScreenHeight - 40), 26);

	int xofs = 0;

    if (imgui_button(GEN_ID,fn14,"Base",xofs,0,33,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        int active = gUIState.kbditem;
        do_cancel();
        gUIState.kbditem = active;
        gVisibleChiplist = 0;
        slidervalue = 0;
    }
	xofs += 33;
    if (imgui_button(GEN_ID,fn14,"Chips",xofs,0,33,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        int active = gUIState.kbditem;
        do_cancel();
        gUIState.kbditem = active;
        gVisibleChiplist = 1;
        slidervalue = 0;
    }
	xofs += 33;
    if (imgui_button(GEN_ID,fn14,"In",xofs,0,33,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        int active = gUIState.kbditem;
        do_cancel();
        gUIState.kbditem = active;
        gVisibleChiplist = 2;
        slidervalue = 0;
    }
	xofs += 33;
    if (imgui_button(GEN_ID,fn14,"Out",xofs,0,33,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        int active = gUIState.kbditem;
        do_cancel();
        gUIState.kbditem = active;
        gVisibleChiplist = 3;
        slidervalue = 0;
    }
	xofs += 33;
    if (imgui_button(GEN_ID,fn14,"Misc",xofs,0,33,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        int active = gUIState.kbditem;
        do_cancel();
        gUIState.kbditem = active;
        gVisibleChiplist = 4;
        slidervalue = 0;
    }
	xofs += 40;

	xofs += 20;


    if (imgui_button(GEN_ID,fn14,"New\nCtrl-N",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        do_resetdialog();
    }
	xofs += 40;
    if (imgui_button(GEN_ID,fn14,"Load\nCtrl-L",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        do_loaddialog();
    }
	xofs += 40;
    if (imgui_button(GEN_ID,fn14,"Merge\nCtrl-M",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        do_loaddialog(1);
    }
	xofs += 40;
    if (imgui_button(GEN_ID,fn14,"Box\nCtrl-B",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        do_loaddialog(2);
    }
	xofs += 40;
    if (imgui_button(GEN_ID,fn14,"Save\nCtrl-S",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        do_savedialog();
    }
	xofs += 40;

	xofs += 20;

    if (imgui_button(GEN_ID,fn14,"Undo\nCtrl-Z",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        int active = gUIState.kbditem;
        do_undo();
        gUIState.kbditem = active;
    }
	xofs += 40;
    if (imgui_button(GEN_ID,fn14,"Redo\nCtrl-Y",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        int active = gUIState.kbditem;
        do_redo();
        gUIState.kbditem = active;
    }
	xofs += 40;

	xofs += 20;

    if (imgui_button(GEN_ID,fn14,"Home",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        do_home();
    }
	xofs += 40;

    if (imgui_button(GEN_ID,fn14,"Zoom\next",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        do_zoomext();
    }
	xofs += 40;

    if (imgui_button(GEN_ID,fn14,gSnap?"Snap\n(on)":"Snap\n(off)",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        gSnap = !gSnap;
    }
	xofs += 40;

    if (imgui_button(GEN_ID,fn14,gLiveWires?"View\n(live)":"View\n(grey)",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        gLiveWires = !gLiveWires;
		gBlackBackground ^= gLiveWires;
    }
	xofs += 40;
    if (imgui_button(GEN_ID,fn14,"PNG it\nCtrl-G",xofs,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        gSavePNG = 1;
    }
	xofs += 40;

    if (imgui_button(GEN_ID,fn14,"Quit",800-42,0,40,40,C_WIDGETBG,C_WIDGETTHUMB,C_WIDGETHOT,0xff000000))
    {
        if (okcancel("Are you sure you want to exit?\nAny unsaved changes will be lost."))
		{
            exit(0);
		}
    }
  
    if (gUIState.mousex > gConfig.mToolkitWidth && gUIState.mousey > 40)
    {
        if (gDragMode == DRAGMODE_NONE)
        {
            // Check for collisions with wires
            for (i = 0; i < (signed)gWire.size(); i++)
            {
				if (gWire[i]->mBox != 0)
					continue;
                Pin * a, * b;
                a = gWire[i]->mFirst;
                b = gWire[i]->mSecond;
                if (line_point_distance(worldmousex,
                                        worldmousey,
                                        a->mHost->mRotatedX + a->mRotatedX + 0.25,
                                        a->mHost->mRotatedY + a->mRotatedY + 0.25,
                                        b->mHost->mRotatedX + b->mRotatedX + 0.25,
                                        b->mHost->mRotatedY + b->mRotatedY + 0.25) < gConfig.mLinePickTolerance)
                {
                    gUIState.hotitem = WIRE_ID(i);
                    int newly_multiselected = 0;
                    if (gUIState.keymod & gSelectKeyMask && gUIState.mousedown && gWire[i]->mMultiSelectState == 0)
                    {
                        newly_multiselected = 1;
                        multiselect_active();
                        gMultiSelectWire.push_back(gWire[i]);
                        gWire[i]->mMultiSelectState = 1;
                        gMultiselectDirty = 1;
                    } 

                    if (gUIState.activeitem == 0 && gUIState.mousedown)
                    {
                        gUIState.activeitem = gUIState.hotitem;
                        if (gUIState.activeitem == WIRE_ID(i))
                            gUIState.kbditem = gUIState.activeitem;
                        if (gWire[i]->mMultiSelectState == 0 && !(gUIState.keymod & gSelectKeyMask))
                        {
                            gMultiselectDirty = 1;
                            gMultiSelectWire.clear();
                            gMultiSelectChip.clear();
                        }
                    }                    
                }
            }
        }

        if (!IS_WIRE_ID(gUIState.hotitem) || gDragMode != DRAGMODE_NONE)
        {
            // Check collisions with chips
            for (i = 0; i < (signed)gChip.size(); i++)
            {
                if (gChip[i]->mBox == 0 &&
					worldmousex > gChip[i]->mRotatedX &&
                    worldmousey > gChip[i]->mRotatedY &&
                    worldmousex < gChip[i]->mRotatedX + gChip[i]->mRotatedW &&
                    worldmousey < gChip[i]->mRotatedY + gChip[i]->mRotatedH)
                {
                    gUIState.hotitem = CHIP_ID(0, i);

                    int j;
                    for (j = 0; j < (signed)gChip[i]->mPin.size(); j++)
                    {
                        if (worldmousex > gChip[i]->mRotatedX + gChip[i]->mPin[j]->mRotatedX &&
                            worldmousey > gChip[i]->mRotatedY + gChip[i]->mPin[j]->mRotatedY &&
                            worldmousex < gChip[i]->mRotatedX + gChip[i]->mPin[j]->mRotatedX + 0.5 &&
                            worldmousey < gChip[i]->mRotatedY + gChip[i]->mPin[j]->mRotatedY + 0.5)
                        {
                            gUIState.hotitem = CHIP_ID(j + 1, i);
                        }
                    }
                    if (gUIState.keymod & gSelectKeyMask && gUIState.mousedown && gChip[i]->mMultiSelectState == 0)
                    {
                        multiselect_active();
                        if (gChip[i]->mMultiSelectState == 0)
                        {
                            gMultiSelectChip.push_back(gChip[i]);
                            gChip[i]->mMultiSelectState = 1;
                        }
                        gMultiselectDirty = 1;
                    }                    

                    if (gUIState.activeitem == 0 && gUIState.mousedown)
                    {
                        gUIState.activeitem = gUIState.hotitem;
                        if (gUIState.activeitem == CHIP_ID(0,i))
                        {
                            gUIState.kbditem = gUIState.activeitem;
                        }
                        if (gChip[i]->mMultiSelectState == 0 && !(gUIState.keymod & gSelectKeyMask))
                        {
                            gMultiselectDirty = 1;
                            gMultiSelectWire.clear();
                            gMultiSelectChip.clear();
                        }
                    }
                    
                }
            }
        }

        if (IS_CHIP_ID(gUIState.activeitem))
        {
            if (GET_PIN_ID(gUIState.activeitem) > 0)
            {
                Pin *hotpin = gChip[GET_CHIP_ID(gUIState.activeitem)]->mPin[GET_PIN_ID(gUIState.activeitem)-1];

                if (gDragMode == DRAGMODE_WIRE)
                {
                    // Accept clicking on a pad to make connection
                    if (gUIState.activeitem == gUIState.hotitem && !gUIState.mousedown)
                    {
                        if (gWireStartDrag != hotpin)
                        {
                            save_undo();
                            add_wire(gWireStartDrag, hotpin);
                            gDragMode = DRAGMODE_NONE;
                        }
                    }
                    // Accept drag-end to make connection
                    if (gUIState.activeitem != gUIState.hotitem && !gUIState.mousedown && IS_CHIP_ID(gUIState.hotitem) && GET_PIN_ID(gUIState.hotitem) > 0)
                    {
                        hotpin = gChip[GET_CHIP_ID(gUIState.hotitem)]->mPin[GET_PIN_ID(gUIState.hotitem)-1];
                        if (gWireStartDrag != hotpin)
                        {
                            save_undo();
                            add_wire(gWireStartDrag, hotpin);
                            gDragMode = DRAGMODE_NONE;
                        }
                    }
                }
                else
                if (gDragMode == DRAGMODE_NONE)
                {
                    gDragMode = DRAGMODE_WIRE;
                    gWireStartDrag = hotpin;
                }
            }
            else
            {
                i = GET_CHIP_ID(gUIState.activeitem);
                // Don't actually move a chip if ctrl is pressed
                if (!(gUIState.mousedownkeymod & gCloneKeyMask))
                {
                    // handle dragging
                    float movex = worldmousex - worldmousedownx;
                    float movey = worldmousey - worldmousedowny;

                    if (gSnap)
                    {
                        float newx = floor((gChip[i]->mX+movex)*2+0.5)/2;
                        float newy = floor((gChip[i]->mY+movey)*2+0.5)/2;
                        movex = newx - gChip[i]->mX;
                        movey = newy - gChip[i]->mY;
                    }
                    if (!gMultiSelectChip.empty())
                    {
                        // multi-select move
                        int c;
                        for (c = 0; c < (signed)gMultiSelectChip.size(); c++)
                        {
                            gMultiSelectChip[c]->mX += movex;
                            gMultiSelectChip[c]->mY += movey;
                            gMultiSelectChip[c]->rotate(gMultiSelectChip[c]->mAngleIn90DegreeSteps);
                        }
                    }
                    else
                    {
                        // not multi-select
                        gChip[i]->mX += movex;
                        gChip[i]->mY += movey;
                        gChip[i]->rotate(gChip[i]->mAngleIn90DegreeSteps);
                    }
                    gUIState.mousedownx += movex * gZoomFactor;
                    gUIState.mousedowny += movey * gZoomFactor;
                }
            }
        }   

        // If nothing is active so far, we're in "move the world" mode
        if (gDragMode == DRAGMODE_NONE && gUIState.activeitem == -1 && gUIState.mousedown)
        {
            if (gUIState.keymod & gSelectKeyMask)
            {
                // Move to select-drag mode
                gDragMode = DRAGMODE_SELECT;
            }
            else
            {
                int dx = (int)floor(gUIState.mousedownx - gUIState.mousex);
                int dy = (int)floor(gUIState.mousedowny - gUIState.mousey);
                gWorldOfsX -= dx / gZoomFactor;
                gWorldOfsY -= dy / gZoomFactor;
                gUIState.mousedownx -= dx;
                gUIState.mousedowny -= dy;
                gUIState.kbditem = 0; 
                mousemode = 1;
                if (!(gUIState.keymod & gSelectKeyMask) && (!gMultiSelectWire.empty() || !gMultiSelectChip.empty()))
                {
                    gMultiselectDirty = 1;
                    gMultiSelectWire.clear();
                    gMultiSelectChip.clear();
                }
            }
        }

        if (gDragMode == DRAGMODE_SELECT && !gUIState.mousedown)
        {
            gDragMode = DRAGMODE_NONE;
            gMultiselectDirty = 1;
            for (i = 0; i < (signed)gChip.size(); i++)
            {
				if (gChip[i]->mBox != 0)
					continue;
				
				if (rect_rect_collide(worldmousex, worldmousey, 
									  worldmousedownx, worldmousedowny,
									  gChip[i]->mRotatedX, gChip[i]->mRotatedY, 
									  gChip[i]->mRotatedX + gChip[i]->mRotatedW, gChip[i]->mRotatedY + gChip[i]->mRotatedH))
				{
					if (gChip[i]->mMultiSelectState == 0)
					{
						gMultiSelectChip.push_back(gChip[i]);
						gChip[i]->mMultiSelectState = 1;                        
					}
				}
				
            }
            for (i = 0; i < (signed)gWire.size(); i++)            
            {
				if (gWire[i]->mBox != 0)
					continue;
                Pin * a, * b;
                a = gWire[i]->mFirst;
                b = gWire[i]->mSecond;
                if (rect_line_collide(worldmousex, worldmousey, 
                                      worldmousedownx, worldmousedowny,
                                      a->mHost->mRotatedX + a->mRotatedX + 0.25, a->mHost->mRotatedY + a->mRotatedY + 0.25,
                                      b->mHost->mRotatedX + b->mRotatedX + 0.25, b->mHost->mRotatedY + b->mRotatedY + 0.25))
                {
                    if (gWire[i]->mMultiSelectState == 0)
                    {
                        gMultiSelectWire.push_back(gWire[i]);
                        gWire[i]->mMultiSelectState = 1;
                    }
                }
            }
        }

        // If wire is being dragged, when far enough, split the wire with pin
        if (gDragMode == DRAGMODE_NONE && IS_WIRE_ID(gUIState.activeitem))
        {
            float dist = sqrt(((float)gUIState.mousex - (float)gUIState.mousedownx) * ((float)gUIState.mousex - (float)gUIState.mousedownx) +
                              ((float)gUIState.mousey - (float)gUIState.mousedowny) * ((float)gUIState.mousey - (float)gUIState.mousedowny));
            if (dist > gConfig.mLineSplitDragDistance) 
            {
                // clear multiselect if any
                gMultiSelectChip.clear();
                gMultiSelectWire.clear();
                gMultiselectDirty = 1;                    
                // First check if distance from the dragged position to one of the original pins was 
                // short enough, and draw a new line from said pin instead of splitting the wire.
                split_wire(1);
            }
        }

        // If a chip is ctrl-dragged, clone the chip
        if (gDragMode == DRAGMODE_NONE && IS_CHIP_ID(gUIState.activeitem) && (gUIState.mousedownkeymod & gCloneKeyMask))
        {
            float dist = sqrt(((float)gUIState.mousex - (float)gUIState.mousedownx) * ((float)gUIState.mousex - (float)gUIState.mousedownx) +
                              ((float)gUIState.mousey - (float)gUIState.mousedowny) * ((float)gUIState.mousey - (float)gUIState.mousedowny));
            if (dist > gConfig.mChipCloneDragDistance) 
            {
				save_undo();
                // clear multiselect if any
                gMultiSelectChip.clear();
                gMultiSelectWire.clear();
                gMultiselectDirty = 1;                    
                int oldchipid = GET_CHIP_ID(gUIState.activeitem);
                gNewChip = NULL;
                for (i = 0; gNewChip == NULL && i < (signed)gChipFactory.size(); i++)
				{
                    gNewChip = gChipFactory[i]->build(gChipName[oldchipid]);
				}

                if (gNewChip)
                {
                    gNewChipName = gChipName[oldchipid];

                    gChip.push_back(gNewChip);
                    gChipName.push_back(gNewChipName);
                    gNewChip->mX = worldmousex - gNewChip->mW / 2;
                    gNewChip->mY = worldmousey - gNewChip->mH / 2;
					
                    gNewChip->rotate(gChip[oldchipid]->mAngleIn90DegreeSteps);
                    gUIState.mousedownx = gUIState.mousex;
                    gUIState.mousedowny = gUIState.mousey;
                    gUIState.mousedownkeymod &= ~gCloneKeyMask; // stop cloning
                    gChip[oldchipid]->clone(gNewChip);
				
					int i;
					for (i = 0; i < (signed)gChip.size(); i++)
					{
						if (gChip[i] == gNewChip)
						{
							gUIState.activeitem = CHIP_ID(0, i);
						}
					}
                    gUIState.hotitem = gUIState.activeitem;
                    gUIState.kbditem = gUIState.activeitem;
                    gNewChip = NULL;
                    gNewChipName = NULL;
				}               
            }
        }

        if ((gUIState.activeitem == 0 && gUIState.scroll) || 
			gUIState.keyentered == SDLK_KP_PLUS || 
			gUIState.keyentered == SDLK_KP_MINUS ||
			gUIState.keyentered == SDLK_PAGEUP ||
			gUIState.keyentered == SDLK_PAGEDOWN)
        {
            float oldfactor = gZoomFactor;
            if (gUIState.scroll > 0 || 
				gUIState.keyentered == SDLK_KP_PLUS ||
				gUIState.keyentered == SDLK_PAGEUP)
            {
                if (gZoomFactor < 30000)
                    gZoomFactor *= 1.2;
            }
            else
            {
                if (gZoomFactor > 1)
                    gZoomFactor /= 1.2;
            }

            gWorldOfsX += (gUIState.mousex - gConfig.mToolkitWidth) / gZoomFactor - (gUIState.mousex - gConfig.mToolkitWidth) / oldfactor;
            gWorldOfsY += (gUIState.mousey - 40) / gZoomFactor - (gUIState.mousey - 40) / oldfactor;
        }
    }

    // Handle keyboard events for selected objects
    if ((!gMultiSelectChip.empty()) || (!gMultiSelectWire.empty()))
    {    
        // multiselect mode
        if (gUIState.keyentered == SDLK_LEFT ||
            gUIState.keyentered == SDLK_RIGHT ||
            gUIState.keyentered == SDLK_UP ||
            gUIState.keyentered == SDLK_DOWN)
        {
            for (i = 0; i < (signed)gMultiSelectChip.size(); i++)
                move_chip(gMultiSelectChip[i], gUIState.keyentered);
        }

        if (gUIState.keyentered == SDLK_DELETE)
        {
            save_undo();
            // Tricky delete operation.
            // Since wirefry is a recursive operation and deletes chips as well
            // (connection pins), we have no idea where the index goes after
            // deletion. Thus:
            // - go through the whole list of chips
            // - delete the first object which has multi-select on
            // - if no such is found, we're done, otherwise start over
            int found = 1;
            while (found)
            {
                found = 0;
                i = 0;
                while (i < (signed)gChip.size() && gChip[i]->mMultiSelectState == 0) i++;
                if (i < (signed)gChip.size())
                {
                    found = 1;
                    delete_chip(gChip[i]);
                }
            }
            
            // Deletion of wires is easier (although it's unlikely that any remain 
            // at this point due to wirefry..)

            // This operation would be more efficient if done from end to the beginning.
            // Simpler code leads to fewer bugs though, so optimize only if needed.
            for (i = 0; i < (signed)gWire.size(); i++)
            {
                if (gWire[i]->mMultiSelectState)
                {
                    delete gWire[i];
                    gWire.erase(gWire.begin() + i);
                    i--; // go back in indices since the indices have moved.
                }
            }
            // Clean up after work
            gMultiSelectChip.clear();
            gMultiSelectWire.clear();
            gUIState.kbditem = 0;
            gUIState.activeitem = 0;
            gUIState.hotitem = 0;
            build_nets(); // wire removed, rebuild the nets
        }
    }
    else
    {
        // not multi-select..

        if (IS_CHIP_ID(gUIState.kbditem) && GET_PIN_ID(gUIState.kbditem) == 0)
        {
            Chip *c = gChip[GET_CHIP_ID(gUIState.kbditem)];
		    switch (gUIState.keyentered)
            {
            case SDLK_LEFT:
            case SDLK_RIGHT:
            case SDLK_UP:
            case SDLK_DOWN:
                move_chip(c, gUIState.keyentered);
                break;
            case SDLK_DELETE:
                save_undo();
                delete_chip(c);
				build_nets();
                gUIState.kbditem = 0;
                gUIState.activeitem = 0;
                gUIState.hotitem = 0;
                break;
            }
        }

        if (IS_WIRE_ID(gUIState.kbditem))
        {
		    switch (gUIState.keyentered)
            {
            case SDLK_DELETE:
                save_undo();
                delete gWire[GET_WIRE_ID(gUIState.kbditem)];
                gWire.erase(gWire.begin() + GET_WIRE_ID(gUIState.kbditem));
                gUIState.kbditem = 0;
                gUIState.activeitem = 0;
                gUIState.hotitem = 0;
                build_nets(); // wire removed, rebuild the nets
                break;
            }
        }
    }

    if (gMultiselectDirty)
    {
        // Reset and set chips' and wires' multiselect states
        for (i = 0; i < (signed)gChip.size(); i++)
            gChip[i]->mMultiSelectState = 0;
        for (i = 0; i < (signed)gWire.size(); i++)
            gWire[i]->mMultiSelectState = 0;
        for (i = 0; i < (signed)gMultiSelectChip.size(); i++)
            gMultiSelectChip[i]->mMultiSelectState = 1;
        for (i = 0; i < (signed)gMultiSelectWire.size(); i++)
            gMultiSelectWire[i]->mMultiSelectState = 1;
        gMultiselectDirty = 0;
    }

	do_build_nets();

    glEnable(GL_SCISSOR_TEST);
    glScissor(gConfig.mToolkitWidth,0,gScreenWidth-gConfig.mToolkitWidth,gScreenHeight-40);

    glPushMatrix();
    glTranslatef(gConfig.mToolkitWidth, 40, 0);
    glScalef(gZoomFactor, gZoomFactor, 1);
    glTranslatef(gWorldOfsX, gWorldOfsY, 0);

    // Draw grid
	if (gBlackBackground)
		glColor4f(0.15,0.2,0.15,1);
	else
		glColor4f(0.75,0.75,0.75,1);
    glBegin(GL_LINES);    
    for (i = 0; i < 20; i++)
    {
            glVertex2f(i * 10, 0);
            glVertex2f(i * 10, 190);
            glVertex2f(0     , i * 10);
            glVertex2f(190   , i * 10);
    }
    glEnd();
    fn.drawstring(TITLE,0.5,0.5,0xff003f00,1);
    fn.drawstring(gConfig.mUserInfo,0.5,1.5,0xff003f00,1);
    fn.drawstring("http://iki.fi/sol/",0.5,2.5,0xff003f00,0.5);

    if (gZoomFactor > 100)
    {
        fn.drawstring("Congrats, you can zoom.",0.57,2.3,0xff003f00,0.05);
        if (gZoomFactor > 1000)
        {
            fn.drawstring("Far enough.",0.63,2.342,0xff003f00,0.005);
            if (gZoomFactor > 10000)
            {
                fn.drawstring("Are we there yet?",0.6515,2.346,0xff003f00,0.0005);
            }
        }
    }
    

    if (gDragMode == DRAGMODE_SELECT)
    {
        drawrect(
            worldmousedownx,
            worldmousedowny, 
            worldmousex - worldmousedownx, 
            worldmousey - worldmousedowny, 
            0x3fffff00);
    }

	int color_hotitem1 = 0xffffafaf;
	int color_hotitem2 = 0xffffcfcf;
	int color_hotpin1 = 0x7fffffff;
	int color_hotpin2 = 0x7fffffff;
	int color_pinhilight = 0x9fff0000;
	int color_normalpin = 0x3fffffff;
	int color_hotchip = 0x7fffcf00;
	int color_kbdchip = 0x3fffff00;
	int color_multiselect = 0x1fffff00;

	if (!gBlackBackground)
	{
		color_hotitem1 = 0xff7f6f6f;
		color_hotitem2 = 0xff7f5f5f;
		color_hotpin1 = 0x7f7f7f7f;
		color_hotpin2 = 0x7f7f7f7f;
		color_pinhilight = 0x9f7f0000;
		color_normalpin = 0x3f7f7f7f;
		color_hotchip = 0x7f7f5f00;
		color_kbdchip = 0x3f7f7f00;
		color_multiselect = 0x1f7f7f00;
	}

    for (i = 0; i < (signed)gChip.size(); i++)
    {
		// Don't draw items in boxes
		if (gChip[i]->mBox != 0)
			continue;
        // Render chips rotated with opengl matrices so that all texture stuff etc. works out automatically.
        glPushMatrix();
        glTranslatef(gChip[i]->mX + gChip[i]->mW / 2, gChip[i]->mY + gChip[i]->mH / 2, 0);
        glRotatef(gChip[i]->mAngleIn90DegreeSteps * 90, 0, 0, 1);
        glTranslatef(-(gChip[i]->mX + gChip[i]->mW / 2), -(gChip[i]->mY + gChip[i]->mH / 2), 0);
        if (gUIState.hotitem == CHIP_ID(0,i))
            drawrect(gChip[i]->mX-0.5,gChip[i]->mY-0.5,gChip[i]->mW+1,gChip[i]->mH+1,color_hotchip);
        else
        if (gUIState.kbditem == CHIP_ID(0,i))
            drawrect(gChip[i]->mX-0.5,gChip[i]->mY-0.5,gChip[i]->mW+1,gChip[i]->mH+1,color_kbdchip);
        else
        if (gChip[i]->mMultiSelectState)
            drawrect(gChip[i]->mX-0.5,gChip[i]->mY-0.5,gChip[i]->mW+1,gChip[i]->mH+1,color_multiselect);
            
        gChip[i]->render(CHIP_ID(0, i));



        int j;
        for (j = 0; j < (signed)gChip[i]->mPin.size(); j++)
        {
            if (gUIState.hotitem == CHIP_ID(j+1,i))
            {
                drawrect(gChip[i]->mX + gChip[i]->mPin[j]->mX, 
                         gChip[i]->mY + gChip[i]->mPin[j]->mY, 0.5, 0.5, color_hotitem1);
                drawrect(gChip[i]->mX + gChip[i]->mPin[j]->mX + 0.1, 
                         gChip[i]->mY + gChip[i]->mPin[j]->mY + 0.1, 0.3, 0.3, color_hotitem2);
            }
            else
            if (IS_CHIP_ID(gUIState.hotitem) && GET_CHIP_ID(gUIState.hotitem) == i)
            {
                drawrect(gChip[i]->mX + gChip[i]->mPin[j]->mX, 
                         gChip[i]->mY + gChip[i]->mPin[j]->mY, 0.5, 0.5, color_hotpin1);
                drawrect(gChip[i]->mX + gChip[i]->mPin[j]->mX + 0.1, 
                         gChip[i]->mY + gChip[i]->mPin[j]->mY + 0.1, 0.3, 0.3, color_hotpin2);
            }
            else
            {
                if (gUIState.keymod & KMOD_ALT && gChip[i]->mPin[j]->getState() == PINSTATE_READ && gChip[i]->mPin[j]->mNet == NULL)
                {
                    drawrect(gChip[i]->mX + gChip[i]->mPin[j]->mX + 0.1, 
                            gChip[i]->mY + gChip[i]->mPin[j]->mY + 0.1, 0.3, 0.3, color_pinhilight);
                }
                else
                {
                    drawrect(gChip[i]->mX + gChip[i]->mPin[j]->mX + 0.1, 
                            gChip[i]->mY + gChip[i]->mPin[j]->mY + 0.1, 0.3, 0.3, color_normalpin);
                }
            }
        }
        glPopMatrix();
    }

    if (gConfig.mAntialiasedLines)
    {
        glEnable(GL_LINE_SMOOTH);
        glLineWidth(0.075 * gZoomFactor);
    }

    glBegin(GL_LINES);
    for (i = 0; i < (signed)gWire.size(); i++)
    {
		// Don't draw items in boxes
		if (gWire[i]->mBox != 0)
			continue;

		float rc, bc, gc;
        switch(gWire[i]->mFirst->mNet->mState)
        {
        case NETSTATE_NC:
            rc = 0.5; gc = 0.5; bc = 0.5;
            break;
        case NETSTATE_HIGH:
            rc = 0; gc = 1; bc = 0;
            break;
        case NETSTATE_LOW:
            rc = 0; gc = 0.5; bc = 0;
            break;
        default:
        //case NETSTATE_INVALID:
            rc = 0.75; gc = 0; bc = 0;
            break;
        }
      
        if (gUIState.kbditem == WIRE_ID(i))
        {
            rc = (rc + 1) / 2;
            gc = (gc + 1) / 2;
            bc = (bc + 0.5) / 2;
        }
        else        
        if (gWire[i]->mMultiSelectState)
        {
            rc = (rc + 0.75) / 2;
            gc = (gc + 0.75) / 2;
            bc = (bc + 0.25) / 2;
        }

		if (!gBlackBackground)
		{
			rc *= 0.75;
			gc *= 0.75;
			bc *= 0.75;
		}

        if (gLiveWires)
        {
            glColor4f(rc, gc, bc, 1);
        }
        else
        {
			if (gBlackBackground)
				glColor4f(0.75f,0.75f,0.75f,1.0f);
			else
				glColor4f(0,0,0,1.0f);
        }

        Pin * a, * b;
        a = gWire[i]->mFirst;
        b = gWire[i]->mSecond;

        if (IS_WIRE_ID(gUIState.hotitem) &&
            gWire[GET_WIRE_ID(gUIState.hotitem)]->mFirst->mNet ==
            gWire[i]->mFirst->mNet)
        {           
            glColor4f(1,1,0,0.5);
        }

        if (gUIState.hotitem == WIRE_ID(i))
        {
            glColor4f(1,1,1,0.5);
            if (split_wire(0))
            {
                float xv = (a->mHost->mRotatedX + a->mRotatedX + 0.25) - (b->mHost->mRotatedX + b->mRotatedX + 0.25);
                float yv = (a->mHost->mRotatedY + a->mRotatedY + 0.25) - (b->mHost->mRotatedY + b->mRotatedY + 0.25);
                
                float l = sqrt(xv*xv+yv*yv);
                if (l!=0)
                {
                    xv /= l;
                    yv /= l;
                }
                else
                {
                    xv = yv = 0;
                }

                glVertex2f(worldmousex-(yv*16)/gZoomFactor,
                           worldmousey+(xv*16)/gZoomFactor);
                glVertex2f(worldmousex+(yv*16)/gZoomFactor,
                           worldmousey-(xv*16)/gZoomFactor);

                mousemode = 2;

            }
            glColor4f(1,1,0,1);
        }

        glVertex2f(a->mHost->mRotatedX + a->mRotatedX + 0.25,
                   a->mHost->mRotatedY + a->mRotatedY + 0.25);
        glVertex2f(b->mHost->mRotatedX + b->mRotatedX + 0.25,
                   b->mHost->mRotatedY + b->mRotatedY + 0.25);
    }


    if (gDragMode == DRAGMODE_WIRE)
    {
        glColor4f(0.5,1,0.5,1);
        glVertex2f(worldmousex, worldmousey);
        glVertex2f(gWireStartDrag->mHost->mRotatedX + gWireStartDrag->mRotatedX + 0.25,
                   gWireStartDrag->mHost->mRotatedY + gWireStartDrag->mRotatedY + 0.25);
    }
    glEnd();

    if (gConfig.mAntialiasedLines)
    {
        glLineWidth(1);
        glDisable(GL_LINE_SMOOTH);
    }

    if (gDragMode == DRAGMODE_NEWCHIP && gUIState.mousex > gConfig.mToolkitWidth && gUIState.mousey > 40 && gUIState.mousedown)
    {
        save_undo();
        gChip.push_back(gNewChip);
        gChipName.push_back(gNewChipName);
        gDragMode = DRAGMODE_NONE;
        gNewChip->mX = worldmousex - gNewChip->mW / 2;
        gNewChip->mY = worldmousey - gNewChip->mH / 2;
        gUIState.mousedownx = gUIState.mousex;
        gUIState.mousedowny = gUIState.mousey;
        gUIState.activeitem = CHIP_ID(0, gChip.size() - 1);
        gUIState.kbditem = gUIState.activeitem;
        gNewChip = NULL;
    }

    glPopMatrix();
    glScissor(0,0,gScreenWidth,gScreenHeight);

    // Tooltips
    if (gUIState.activeitem == 0 && gUIState.hotitem != 0 && (tick - gUIState.lasthottick) > gConfig.mTooltipDelay)
    {
        const char *tooltip = NULL;

        if (loc != -1 && !(gVisibleChiplist == 2 && loc == 8))
        {
            if ((loc | (gVisibleChiplist << 16)) != gSidebarTooltipId)
            {
                gSidebarTooltipId = loc | (gVisibleChiplist << 16);
                // Now, *this* is quite wasteful.
                Chip * nChip = NULL;
                int j;
                for (j = 0; nChip == NULL && j < (signed)gChipFactory.size(); j++)
                    nChip = gChipFactory[j]->build(gAvailableChip[gVisibleChiplist][loc]);
                if (nChip)
                {
                    delete[] gSidebarTooltip;
                    if (nChip->mTooltip)
                        gSidebarTooltip = mystrdup(nChip->mTooltip);
                    else
                        gSidebarTooltip = mystrdup(gAvailableChip[gVisibleChiplist][loc]);
                    delete nChip;
                    nChip = NULL;
                    tooltip = gSidebarTooltip;
                }
                else
                {
                    tooltip = NULL;
                }                
            }
            else
            {
                tooltip = gSidebarTooltip;
            }
        }

        if (IS_CHIP_ID(gUIState.hotitem))
        {
            if (GET_PIN_ID(gUIState.hotitem) == 0)
            {
                if (gChip[GET_CHIP_ID(gUIState.hotitem)]->mTooltip == NULL)
                {
                    tooltip = gChipName[GET_CHIP_ID(gUIState.hotitem)];
                }
                else
                {
                    tooltip = gChip[GET_CHIP_ID(gUIState.hotitem)]->mTooltip;
                }
            }
            else
            {
                tooltip = gChip[GET_CHIP_ID(gUIState.hotitem)]->mPin[GET_PIN_ID(gUIState.hotitem)-1]->mTooltip;
            }
        }
        if (IS_WIRE_ID(gUIState.hotitem))
        {
            Wire *w = gWire[GET_WIRE_ID(gUIState.hotitem)];
            switch (w->mFirst->mNet->mState)
            {
            case NETSTATE_NC:
                tooltip = "Not connected:\nNet not connected\nto an input";
                break;
            case NETSTATE_INVALID:
                tooltip = "Invalid state:\nTwo or more outputs\nconnected together\nor invalid wiring\non a chip.";
                break;
            case NETSTATE_HIGH:
                tooltip = "Signal 'High'";
                break;
            case NETSTATE_LOW:
                tooltip = "Signal 'Low'";
                break;
            }
        }
        if (tooltip)
        {
            float w, h, llw;
            fn14.stringmetrics(tooltip, w, h, llw);
            drawrect(gUIState.mousex + 16, gUIState.mousey + 16, w+8, h+8, 0xafcfcf9f);
            fn14.drawstring(tooltip, gUIState.mousex + 20, gUIState.mousey + 20, 0xff000000);
        }
    }

    imgui_finish();

    if (gConfig.mPerformanceIndicators)
    {
        static int frame = 0; frame++;
        #define PERF_FRAMES 50
        static int perf_idx = 0;
        static int perf_data1[PERF_FRAMES];
        static int perf_data2[PERF_FRAMES];
        perf_data1[perf_idx] = tick;
        perf_data2[perf_idx] = physics_iterations;
        char perf_temp[200];
        float fps = 1000.0f / ((float)(perf_data1[perf_idx] - perf_data1[(perf_idx+1) % PERF_FRAMES]) / PERF_FRAMES);
        float mspf = (float)(perf_data1[perf_idx] - perf_data1[(perf_idx+1) % PERF_FRAMES]) / PERF_FRAMES;
        float ppf = (float)(perf_data2[perf_idx] - perf_data2[(perf_idx+1) % PERF_FRAMES]) / PERF_FRAMES;
		sprintf(perf_temp, "%3.0f%%rt, %3.0ffps, %3.0fmspf, %dms phys", ppf*fps / 10.0f / gConfig.mPhysicsKHz, fps, mspf, physms);
        fn.drawstring(perf_temp, gConfig.mToolkitWidth+10, gScreenHeight - 40,0x3fffffff,32);
        sprintf(perf_temp, "font1:%d/%d, font2:%d/%d",fn.mFontCacheTrashed,fn.mFontCacheUse,fn14.mFontCacheTrashed,fn14.mFontCacheUse);
        fn.drawstring(perf_temp, gConfig.mToolkitWidth+10, gScreenHeight - 72,0x3fffffff,32);
		sprintf(perf_temp, "Chips:%d Wires:%d Nets:%d",(int)gChip.size(), (int)gWire.size(), (int)gNet.size());
        fn.drawstring(perf_temp, gConfig.mToolkitWidth+10, gScreenHeight - 104,0x3fffffff,32);
        perf_idx++;
        perf_idx %= PERF_FRAMES;
	}
    fn.fontcacheframe();
    fn14.fontcacheframe();

    if (gDragMode == DRAGMODE_NEWCHIP)
    {
        int w = gNewChip->mW * gZoomFactor;
        int h = gNewChip->mH * gZoomFactor;
        drawrect(gUIState.mousex - w/2, gUIState.mousey - h/2, w, h, 0x7fffffff);
    }

    if (gConfig.mCustomCursors && mousemode != lastmousemode)
    {
        lastmousemode = mousemode;
        switch (mousemode)
        {
        case 1:
            SDL_SetCursor(cursor_drag);
            break;
        case 2:
            SDL_SetCursor(cursor_scissors);
            break;
        default:
            SDL_SetCursor(cursor_normal);
        }
    }

    SDL_Delay(10);
    glFinish();
    SDL_GL_SwapBuffers();
}

void initvideo()
{
    const SDL_VideoInfo *info = NULL;
    int bpp = 0;
    int flags = 0;
    info = SDL_GetVideoInfo();

    if (!info) 
    {
        fprintf(stderr, "Video query failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(0);
    }

    bpp = info->vfmt->BitsPerPixel;
    flags = SDL_OPENGL | SDL_RESIZABLE;

    if (SDL_SetVideoMode(gScreenWidth, gScreenHeight, bpp, flags) == 0) 
    {
        fprintf( stderr, "Video mode set failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(0);
    }

    glViewport( 0, 0, gScreenWidth, gScreenHeight );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );

    gluOrtho2D(0,gScreenWidth,gScreenHeight,0);

    if (gConfig.mUseBlending)
        glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    reload_textures();    
}

void audiomixer(void *userdata, Uint8 *stream, int len)
{
    static int filter = 0;
    int n = len / 2;
    short * buf = (short*)stream;
    memset(stream,0,len);
    float step = 1000.0f / gAudioSpec->freq;
    int i;
    for (i = 0; i < n; i++)
    {
        int p = (int)floor(gPlayHead);
        int d = (gAudioBuffer[p] - 127) * 250;
        filter = (d + filter * 15) / 16;
        buf[i] = filter;
        gPlayHead += step;
        if (gPlayHead >= AUDIOBUF_SIZE)
            gPlayHead -= AUDIOBUF_SIZE;
    }
}


int main(int argc, char** args)
{
    memset(gAudioBuffer,0,AUDIOBUF_SIZE);

    gotoappdirectory(argc, args);

    gConfig.load();

    if (gConfig.mSwapShiftAndCtrl)
    {
        gSelectKeyMask = KMOD_CTRL;
        gCloneKeyMask = KMOD_SHIFT;
    }
    else
    {
        gSelectKeyMask = KMOD_SHIFT;
        gCloneKeyMask = KMOD_CTRL;
    }


    memset(gKeyState,0,sizeof(int) * 256);

    gVisualRand.init_genrand(0xc0cac01a);

    int sdlflags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;
    
    if (gConfig.mAudioEnable)
        sdlflags |= SDL_INIT_AUDIO;

    if (SDL_Init(sdlflags) < 0) 
    {
        fprintf(stderr, "Video initialization failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(0);
    }

    if (gConfig.mAudioEnable)
    {
        SDL_AudioSpec *as = new SDL_AudioSpec;
        as->freq = 44100;
        as->format = AUDIO_S16;
        as->channels = 1;
        as->samples = 4096;
        as->callback = audiomixer;
        as->userdata = NULL;
        if (SDL_OpenAudio(as, NULL) < 0)
        {
            fprintf(stderr, "Unable to init SDL audio: %s\n", SDL_GetError());
            exit(1);
        }
        gAudioSpec = as;
        // audio is now started only when the audio device is created, to avoid popping sounds..
        //SDL_PauseAudio(0);
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    gScreenWidth = gConfig.mWindowWidth;
    gScreenHeight = gConfig.mWindowHeight;

    initvideo();

    {
        char temp[256];
        sprintf(temp, "%s - %s", TITLE, gConfig.mUserInfo);
        SDL_WM_SetCaption(temp, NULL);  
    }

    // For imgui - Enable keyboard repeat to make sliders more tolerable
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    // For imgui - Enable keyboard UNICODE processing for the text field.
    SDL_EnableUNICODE(1);

    fn.load("data/vera31.fnt");
    fn14.load("data/vera14.fnt");

#ifndef __APPLE__ // Use the higher-resolution OSX icon instead
	int x, y, n;
	unsigned char *data = stbi_load("data/icon.png", &x, &y, &n, 4);
    if (data)
	{	
		SDL_Surface *icon = SDL_CreateRGBSurfaceFrom(data,x,y,32,x*4,0x000000ff,0x0000ff00, 0x00ff0000, 0xff000000);
		SDL_WM_SetIcon(icon, NULL);
		SDL_FreeSurface(icon);
		stbi_image_free(data);
	}
#endif

    SDL_ShowCursor(1);

    if (gConfig.mCustomCursors)
    {
        cursor_drag = load_cursor("data/cursor_drag.png", 10, 0);
        cursor_scissors = load_cursor("data/cursor_scissors.png", 6, 6);
        cursor_normal = load_cursor("data/cursor_ptr.png", 0, 0);
        SDL_SetCursor(cursor_normal);
    }

    gChipFactory.push_back(new BaseChipFactory);
    gChipFactory.push_back(new PluginChipFactory);


    int i;
    for (i = 0; i < (signed)gChipFactory.size(); i++)
        gChipFactory[i]->getSupportedChips(gAvailableChip);

    if (argc > 1)
        do_loaddialog(0, args[1]);

    while (1) 
    {
        process_events();
        draw_screen();
    }

    return 0;
}
