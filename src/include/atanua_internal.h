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
#ifndef ATANUA_INTERNAL_H
#define ATANUA_INTERNAL_H


#define ATANUAVERSION "1.3.141220"

#ifdef __APPLE__
#define ATANUAPLATFORM "OSX"
#define DLLHANDLETYPE int
#elif _MSC_VER
#define ATANUAPLATFORM "Win32"
#define DLLHANDLETYPE int
#else
#ifdef __x86_64__
#define ATANUAPLATFORM "Linux64"
#else
#define ATANUAPLATFORM "Linux"
#endif
#define DLLHANDLETYPE void*
#endif

#ifdef _DEBUG
#define TITLE "Atanua" "/" ATANUAPLATFORM " " ATANUAVERSION " (debug)"
#else
#define TITLE "Atanua" "/" ATANUAPLATFORM " " ATANUAVERSION 
#endif

#define CHIP_ID(pin, chip) (0x10000000 | ((pin) << 16) | (chip))
#define WIRE_ID(wire) (0x20000000 | (wire))
#define IS_CHIP_ID(id) ((id & 0xf0000000) == 0x10000000)
#define IS_WIRE_ID(id) ((id & 0xf0000000) == 0x20000000)
#define GET_CHIP_ID(id) (id & 0xffff)
#define GET_PIN_ID(id) ((id >> 16) & 0xff)
#define GET_WIRE_ID(id) (id & 0xffffff)
#define NEWCHIP_ID(loc) (0x30000000 | (loc))

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

enum dragmodes
{
    DRAGMODE_NONE,
    DRAGMODE_WIRE,
    DRAGMODE_NEWCHIP,
    DRAGMODE_SELECT
};

class ExtPin;
class Pin;

class BoxStitchingInformation
{
public:
	vector<char*> mToolTips;
	vector<ExtPin*> mExtPinsInside;
	Pin *mExtPinOutside;
	BoxStitchingInformation();
	~BoxStitchingInformation();
};

struct BoxcacheData
{
	File * mData;
	const char *mName;
	vector<const char*> mTooltips;
};

class Box;

struct BoxLoadQueueItem
{
	Box * mBox;
	const char * mFilename;
	int mBoxId;
};

extern Box *gBoxBeingLoaded;
extern int gBoxCount;
extern int gActiveBoxes;

extern vector<BoxLoadQueueItem *> gBoxLoadQueue;

extern vector<Chip*> gChip;
extern vector<const char*> gChipName;
extern vector<Wire*> gWire;
extern vector<Net*> gNet;

extern vector<ChipFactory*> gChipFactory;
extern vector<char *> gAvailableChip[5];

extern vector<File *> gUndoStack;
extern vector<File *> gRedoStack;

extern vector<BoxcacheData> gBoxCache;

extern vector<Chip*> gMultiSelectChip;
extern vector<Wire*> gMultiSelectWire;
extern int gMultiselectDirty;


extern Pin * gWireStartDrag;
extern int gScreenWidth;
extern int gScreenHeight;

extern float gWorldOfsX, gWorldOfsY;
extern float gZoomFactor;
extern int gDragMode;

extern Chip * gNewChip;
extern const char * gNewChipName;



extern void build_nets();
extern void delete_chip(Chip *c);
extern float line_point_distance(float x0, float y0, float x1, float y1, float x2, float y2);
extern void add_wire(Pin *aFirst, Pin *aSecond);

extern void do_cancel();
extern void do_reset();
extern void do_loadbinary(File *f, int box=-1);
extern void do_savebinary(File *f);
extern void do_optimize_box(int aBox);

extern void save_undo();
extern void do_undo();
extern void do_redo();
extern void do_zoomext();
extern void do_home();
extern void do_resetdialog();
extern void do_loaddialog(int merge = 0, const char *aFilename = 0);
extern void do_savedialog();
extern BoxStitchingInformation * do_preparse_box(const char *aFname);

// from nativefunctions.cpp

extern FILE * openfiledialog(const char *aPrompt);
extern FILE * savefiledialog(const char *aPrompt);
extern int okcancel(const char *prompt);
extern void gotoappdirectory(int parc, char ** pars);
extern DLLHANDLETYPE opendll(const char *dllfilename);
extern void *getdllproc(DLLHANDLETYPE dllhandle, const char *procname);
#endif
