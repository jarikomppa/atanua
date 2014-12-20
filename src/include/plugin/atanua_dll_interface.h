#ifndef ATANUADLL_H
#define ATANUADLL_H

#ifdef _MSC_VER
#ifdef  __cplusplus
#define ATANUADLLEXPORT extern "C" __declspec(dllexport)
#else
#define ATANUADLLEXPORT __declspec(dllexport)
#endif
#define ATANUACALL __cdecl
#else
#define ATANUACALL
#endif

#define ATANUA_PLUGIN_DLL_VERSION 1

#ifndef NO_ATANUA_ENUMS
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
    NETSTATE_NC,      /* not connected - zero inputs */
    NETSTATE_INVALID, /* invalid state - mode than one input */
    NETSTATE_HIGH,
    NETSTATE_LOW
};
#endif

enum ATANUA_ASYNC_CALL 
{
    ATANUA_ASYNC_CALL_NONE = 0, /* No call active */
    ATANUA_ASYNC_CALL_OKCANCEL = 1, /* Show message box with ok/cancel, return as ok = nonzeo */
    ATANUA_ASYNC_CALL_OPENFILE = 2, /* Show open file dialog. Returns FILE pointer. Plugin must close the file handle. */
    ATANUA_ASYNC_CALL_SAVEFILE = 3, /* Show save file dialog. Returns FILE pointer. Plugin must close the file handle. */
    ATANUA_ASYNC_CALL_STORE_UNDO = 4, /* Causes Atanua to store a undo point */
};

typedef struct chipinfostruct_
{
  int mPinCount;     /* Number of pins for the structure */
  int *mPinOutput;   /* Pin output data states. See pinmodes enum. */
  int *mPinInput;    /* Pin input data states. See netstates enum. */
  float mWidth;      /* Chip width */
  float mHeight;     /* Chip height */
  float *mPinCoordinates; /* Pin coordinates in relation to the chip. Two floats per pin. */
  const char *mTextureFilename; /* Texture filename. Can be NULL. */
  const char *mChipName;        /* Chip name rendered at the center of the chip */
  const char *mTooltip;         /* Chip tooltip. Can be NULL. */
  const char **mPinTooltips;    /* Tooltips for the pins. Can be NULL. */
  int mPersistentDataCount;     /* Number of bytes in persistent data store. Can be 0. */
  char *mPersistentDataPtr; /* Pointer to the persistent data store. Can be NULL. */
  void *mUserPtr;    /* User pointer for storing other chip-related data. */

  /* Set to a ATANUA_ASYNC_CALL value to perform an asynchronous call */
  /* If zero, normal operation continues. If non-zero, the requested operation is */
  /* performed and the same function is called again */
  /* mAsyncCall is reset to zero by Atanua before the new call. */
  int mAsyncCall;         
  void *mAsyncCallParam;  /* Async call parameter to Atanua */
  void *mAsyncCallReturn; /* Async call return value from Atanua */
} 
chipinfostruct;

typedef struct atanuadllinfo_
{
  int mDllVersion;   /* DLL interface version. Set to 0. Set to -1 if in error. */
  int mChipCount;    /* Number of chips provided by this DLL */
  const char **mChipName; /* Pointers to chip names. These may not point to stack! */
  int *mChipCategory; /* Category numbers for chips. May be NULL. Categories are 0=dont care, 1=base, 2=chip, 3=I/O */
}
atanuadllinfo;

#ifndef NO_ATANUA_DLL_PROTOTYPES

/* Get the number of chips this DLL/interface supports. */
ATANUADLLEXPORT void ATANUACALL getatanuadllinfo(atanuadllinfo *aDllInfo);

/* create a chip. Return 0 if unable. */
ATANUADLLEXPORT int ATANUACALL create(chipinfostruct *aChipInfo, const char *aChipname);

/* Update a chip */
ATANUADLLEXPORT void ATANUACALL update(chipinfostruct *aChipInfo, float aTick);

/* Render and/or handle key input. Return 0 to let Atanua render the chip. */
ATANUADLLEXPORT int ATANUACALL render(chipinfostruct *aChipInfo, int aKey);

/* Called when chip is being destroyed. */
ATANUADLLEXPORT void ATANUACALL cleanup(chipinfostruct *aChipInfo);
#endif

#endif