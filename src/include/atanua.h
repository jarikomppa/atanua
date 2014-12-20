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
#include <vector>
#include "toolkit.h"

using namespace std;


enum pinmodes
{
    PINSTATE_HIGHZ,
    PINSTATE_READ,
    PINSTATE_WRITE_HIGH,
    PINSTATE_WRITE_LOW,
    PINSTATE_READ_OR_WRITE_HIGH,
    PINSTATE_READ_OR_WRITE_LOW,
    PINSTATE_PROPAGATE_INVALID
};

enum netstates
{
    NETSTATE_NC,      // not connected - zero inputs
    NETSTATE_INVALID, // invalid state - mode than one input
    NETSTATE_HIGH,
    NETSTATE_LOW
};

class Pin;
class Chip;
class Wire;
class File;

class Net
{
public:
    int mHighFreqChanges;
    int mState;
	int mResetDelay;
	int mDirty;
    vector<Pin *> mPin;
    vector<Pin *> mROPin;
    int nextstate();
    void update();
    ~Net();
    Net();
};


class Pin
{
public:	
	// Current pin state
    int mState;
	// The net the pin is connected to
    Net *mNet;
	// Net id. Used while building nets.
	int mNetId;
	// Tooltip string (NULL is ok)
    const char *mTooltip;
	// Coordinates
    float mX, mY; // relative to chip
	// Rotated coordinates
    float mRotatedX, mRotatedY;
	// Pointer to the owner chip
    Chip *mHost;    
	// Read-only flag. Pin will never write to the net. Optimization, 0 is safe value.
	int mReadOnly;
	// Ctor
    Pin();
	// Ctor with setup
    Pin(float aX, float aY, Chip *aHost, const char *aTooltip);
	// Setup
    void set(float aX, float aY, Chip *aHost, const char *aTooltip);
	// set state
	void setState(int aState);
	// get state
	int getState();
};

class Chip
{
public:
	// Flag: does this chip need to get executed on the next run?
	int mDirty;
	// Which box we're in (0 for foreground)
	int mBox;
    // Flag whether this chip has been multi-selected or not.
    int mMultiSelectState;
    // Creation key - used for homework validation
    int mKey;
    // Position
    float mX, mY;
    // Size
    float mW, mH;
    // Rotated coordinates and sizes
    float mRotatedX, mRotatedY, mRotatedW, mRotatedH;
    // Pointer to chip's tooltip. May be NULL.
    const char *mTooltip;
    // Rotation angle, in 90 degree steps.
    int mAngleIn90DegreeSteps;
    // Vector of pin pointers - the simulation uses these to access 
    // the chip's pins
    vector<Pin*> mPin;
    // Ctor
    Chip();
    // Rotate chip - updates the rotated coordinates, as well as pins
    void rotate(int aIn90DegreeSteps);
    // Set common values in one call
    void set(float aX, float aY, float aW, float aH, const char *tooltip);
    // Destructor
    virtual ~Chip();
    // Render chip
    virtual void render(int aChipId);
    // Update chip state
    virtual void update(float aTick);
    // Serialize any extra information (optional)
    virtual void serialize(File *f);
    // Deserialize any extra information (optional)
    virtual void deserialize(File *f);
    // Clone internal data to a new copy (optional)
    virtual void clone(Chip *aOther);
	// Should this chip exist in a box? (optional)
	virtual int isUselessInBox() { return 0; }
};

class Wire
{
public:
	// Which box we're in (0 for foreground)
	int mBox;
    // Flag whether this wire has been multi-selected or not.
    int mMultiSelectState;
    // Creation key - used for homework validation
    int mKey;
    // Pointers to the wire's end pins
    Pin *mFirst, *mSecond;
    // Ctor
    Wire();
    // CTor with value setup
    Wire(Pin *aFirst, Pin *aSecond);
};

class ChipFactory
{
public:
    // Build a chip based on its name.
    virtual Chip * build(const char *aChipId) = 0;
    // Get the list of supported chips
    virtual void getSupportedChips(vector<char *> aChipList[5]) = 0;
    // Dtor
    virtual ~ChipFactory() {};
};

class AtanuaConfig
{
public:
    AtanuaConfig();
    ~AtanuaConfig();
    void load();
    int mToolkitWidth;
    int mPropagateInvalidState;
    int mAntialiasedLines;
    int mCustomCursors;
    int mWireFry;
    int mMaxPhysicsMs;
    int mWindowWidth;
    int mWindowHeight;
    int mTooltipDelay;
    int mPerformanceIndicators;
    int mSwapShiftAndCtrl;
    int mAudioEnable;
    char *mUserInfo;
    float mLinePickTolerance;
    float mLineEndTolerance;
    float mLineSplitDragDistance;
    float mChipCloneDragDistance;
    int mFontCacheMax;
    int mUseVBOs;
    int mUseOldFontSystem;
    int mUseBlending;
	int mMaxActiveBoxes;
	int mLEDSamples;
	int mPhysicsKHz;
	char *mAutosaveDir;
	int mAutosaveEnable;
	int mAutosaveCount;
	int mAutosaveInterval;
};

extern AtanuaConfig gConfig;
extern ACFont fn, fn14;
extern int gKeyState[SDLK_LAST];
extern int gSelectKeyMask;
extern int gCloneKeyMask;
extern int gBlackBackground;

char *mystrdup(const char *data);