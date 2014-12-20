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
#include "atanua_internal.h" // for TITLE

char * gFilename = NULL;
char * gAltFilename = NULL;


FILE * openfileinsamedir(const char * aFname)
{
	char temp[1024];
	temp[0] = 0;
	if (gFilename)
	{
		strcat(temp,gFilename);
		int i = strlen(temp);
		while (i > 0 && temp[i] != '\\' && temp[i] != '/') i--;
		temp[i+1] = 0;
	}
	else
	if (gAltFilename)
	{
		strcat(temp, gAltFilename);
		int i = strlen(temp);
		while (i > 0 && temp[i] != '\\' && temp[i] != '/') i--;
		temp[i+1] = 0;
	}
	strcat(temp,aFname);
	
	return fopen(temp, "rb");
}

void resetfilename()
{
	delete[] gFilename;
	gFilename = NULL;
    char temp[256];
    sprintf(temp, "%s - %s", TITLE, gConfig.mUserInfo);
    SDL_WM_SetCaption(temp, NULL);  
}

void storefilename(const char *fn)
{	
	if (!fn)
	{
		resetfilename();
		return;
	}

    if (gFilename)
    {
        delete[] gFilename;
    }    

	if (gAltFilename)
    {
        delete[] gAltFilename;
    }    
    
    gFilename = mystrdup(fn);
    gAltFilename = mystrdup(fn);

	const char * pt = strrchr(fn,'\\');
    
    if (pt == NULL) 
        pt = strrchr(fn,'/');

    if (pt == NULL) 
    {
        pt = fn; 
    }
    else
	{
		pt++;
	}

    char temp[1024];
    sprintf(temp, "%s - " TITLE " - %s", pt, gConfig.mUserInfo);
    SDL_WM_SetCaption(temp, NULL);  
	
}

#ifdef WINDOWS_VERSION

#include <windows.h> // dialogs
#include <direct.h> // directory handling
#include "SDL_syswm.h" // to get current window handle

FILE * openfiledialog(const char *title)
{
    // save app dir
    int curdrive = _getdrive();
    char path[1024];
    _getdcwd(curdrive, path, 1024);

    SDL_SysWMinfo wmInfo;
    memset(&wmInfo, 0, sizeof(wmInfo));
    SDL_GetWMInfo(&wmInfo);
    HWND hWnd = wmInfo.window;
    OPENFILENAME ofn;
    char szFileName[1024] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    if (title == NULL)
        ofn.lpstrFilter = "Atanua Design Files (*.atanua)\0*.atanua\0All Files (*.*)\0*.*\0\0";
    else
        ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
    ofn.nMaxFile = 1024;

    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrFile = szFileName;

    if (title == NULL)
    {
        ofn.lpstrDefExt = "atanua";

        if (gFilename)
		{
            strcpy(szFileName, gFilename);
			int i = 0;
			while (szFileName[i])
			{
				if (szFileName[i] == '/')
					szFileName[i] = '\\';
				i++;
			}
		}
        
        ofn.lpstrTitle = "Open Atanua design file";
    }
    else
        ofn.lpstrTitle = title;


    FILE * f = NULL;

    if(GetOpenFileName(&ofn))
    {
        f = fopen(szFileName, "rb");
        if (title == NULL)
            storefilename(szFileName);
    }

    // return to app dir
    _chdir(path);
    return f;
}

FILE * savefiledialog(const char *title)
{
    // save app dir
    int curdrive = _getdrive();
    char path[1024];
    _getdcwd(curdrive, path, 1024);

    SDL_SysWMinfo wmInfo;
    memset(&wmInfo, 0, sizeof(wmInfo));
    SDL_GetWMInfo(&wmInfo);
    HWND hWnd = wmInfo.window;

    OPENFILENAME ofn;
    char szFileName[1024] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFileName;

    if (title == NULL)
    {
        ofn.lpstrFilter = "Atanua Design Files (*.atanua)\0*.atanua\0All Files (*.*)\0*.*\0\0";
        ofn.lpstrDefExt = "atanua";
        ofn.lpstrTitle = "Save Atanua design file";
    }
    else
    {
        ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
        ofn.lpstrTitle = title;
    }
    ofn.nMaxFile = 1024;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    if (gFilename)
	{
        strcpy(szFileName, gFilename);
		int i = 0;
		while (szFileName[i])
		{
			if (szFileName[i] == '/')
				szFileName[i] = '\\';
			i++;
		}
	}

    FILE * f = NULL;

    if(GetSaveFileName(&ofn))
    {
        f = fopen(szFileName, "wb");
        storefilename(szFileName);
    }

    // return to app dir
    _chdir(path);

    return f;
}

int okcancel(const char *prompt)
{
    SDL_SysWMinfo wmInfo;
    memset(&wmInfo, 0, sizeof(wmInfo));
    SDL_GetWMInfo(&wmInfo);
    HWND hWnd = wmInfo.window;
    if (MessageBox(hWnd,prompt,TITLE,MB_OKCANCEL | MB_ICONWARNING) == IDOK)
        return 1;
    return 0;
}

void gotoappdirectory(int parc, char ** pars)
{
    char buf[1024];
    GetModuleFileName(GetModuleHandle(0), &buf[0], 1024);
    char *rch = strrchr(buf, '\\');
    if (rch == NULL) return;
    *rch = 0;
    _chdir(buf);
}

int opendll(const char *dllfilename)
{
    HMODULE dllh = LoadLibrary(dllfilename);
    return (int)dllh;
}

void *getdllproc(int dllhandle, const char *procname)
{
    HMODULE dllh = (HMODULE)dllhandle;
    return GetProcAddress(dllh, procname);
}

#endif

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#ifdef OSX_VERSION

#include <dlfcn.h> // dll functions

#define OSX_COCOA // comment out for carbon

#ifdef OSX_COCOA

// from cocoadialogs.m
extern "C" int doOpenDialog(char *buf, const char *title);
extern "C" int doSaveDialog(char *buf, const char *title);
extern "C" int showWarning(const char *str);

FILE * openfiledialog(const char *title)
{
  char temp[1024];

  if (doOpenDialog(temp, (title == NULL)?"Open Atanua Design File":title))
  {
      if (title==NULL)
        storefilename(temp);
    return fopen(temp, "rb");
  }
  return NULL;
}

FILE * savefiledialog(const char *title)
{
  char temp[1024];

  if (doSaveDialog(temp, (title == NULL)?"Save Atanua Design File":title))
  {
    if (title == NULL)
      storefilename(temp);
    return fopen(temp, "wb");
  }
  return NULL;
}

int okcancel(const char *prompt)
{
  return showWarning(prompt);
}

#else // carbon

#include <SDL/SDL_syswm.h>
#include <Carbon/Carbon.h>

static void 
FSMakeFSRef(
  FSVolumeRefNum volRefNum,
  SInt32 dirID,
  ConstStr255Param name,
  FSRef *ref)
{
  FSRefParam  pb;
  
  pb.ioVRefNum = volRefNum;
  pb.ioDirID = dirID;
  pb.ioNamePtr = (const StringPtr)name;
  pb.newRef = ref;
  PBMakeFSRefSync(&pb);
}

static void
FSMakePath(
  SInt16 volRefNum,
  SInt32 dirID,
  ConstStr255Param name,
  UInt8 *path,
  UInt32 maxPathSize)
{
  FSRef    ref;
    
  /* convert the inputs to an FSRef */
  FSMakeFSRef(volRefNum, dirID, name, &ref);
  
  /* and then convert the FSRef to a path */
  FSRefMakePath(&ref, path, maxPathSize);
}

FILE * openfiledialog(const char *title)
{
    NavDialogRef fod;
    NavDialogCreationOptions opts;
    NavReplyRecord reply;
    AEKeyword keyword;
    FSSpec filespec;
    DescType actualtype;
    Size actualsize;
    
    NavGetDefaultDialogCreationOptions(&opts);
    
    NavCreateGetFileDialog (
     &opts, //const NavDialogCreationOptions *inOptions,
     NULL, //NavTypeListHandle inTypeList,
     NULL, //NavEventUPP inEventProc,
     NULL, //NavPreviewUPP inPreviewProc,
     NULL, //NavObjectFilterUPP inFilterProc,
     NULL, //void *inClientData,
     &fod //NavDialogRef *outDialog
    );
    
    FILE * f = NULL;
    
    NavDialogRun(fod);
    NavDialogGetReply(fod, &reply);
    // check
    if (reply.validRecord)
    {
        AEGetNthPtr(&(reply.selection), 1,
                                   typeFSS, &keyword,
                                   &actualtype, &filespec,
                                   sizeof(FSSpec),
                                   &actualsize);  
        char temp[256];
        FSMakePath(filespec.vRefNum, filespec.parID, filespec.name, (UInt8*)temp, 256);
        f = fopen(temp, "rb");
        if (title == NULL)
            storefilename(temp);        
    }
    NavDisposeReply(&reply);  
    NavDialogDispose(fod);  
    return f;
}

FILE * savefiledialog(const char* title)
{
    NavDialogRef fod;
    NavDialogCreationOptions opts;
    NavReplyRecord reply;
    AEKeyword keyword;
    FSSpec filespec;
    DescType actualtype;
    Size actualsize;
    
    NavGetDefaultDialogCreationOptions(&opts);
    
    NavCreatePutFileDialog (
    &opts, //const NavDialogCreationOptions *inOptions,
    'Atea', //OSType inFileType,
    kNavGenericSignature, //OSType inFileCreator,
    NULL, //NavEventUPP inEventProc,
    NULL, //void *inClientData,
    &fod //NavDialogRef *outDialog
    );
    
    
    FILE * f = NULL;
    
    NavDialogRun(fod);
    NavDialogGetReply(fod, &reply);
    
    // check
    if (reply.validRecord)
    {
        AEGetNthPtr(&(reply.selection), 1,
                                   typeFSS, &keyword,
                                   &actualtype, &filespec,
                                   sizeof(FSSpec),
                                   &actualsize);  
        char temp[256];
        char fn[256];
        FSMakePath(filespec.vRefNum, filespec.parID, filespec.name, (UInt8*)temp, 256);
        CFStringGetCString(reply.saveFileName,fn,256,kCFStringEncodingNonLossyASCII);
        strcat(temp, "/");
        strcat(temp, fn);
        if (strstr(temp, ".atanua") == NULL)
            strcat(temp,".atanua");
        f = fopen(temp, "wb");
        if (title == NULL)
            storefilename(temp);
    }
    NavDisposeReply(&reply);  
    NavDialogDispose(fod);  
    return f;
}

int okcancel(const char *prompt)
{
  unsigned char temp[257];
  temp[0] = 0;
  while (prompt[temp[0]])
  {
    temp[temp[0]+1] = (unsigned char)prompt[temp[0]];
    temp[0]++;
    if (temp[0] == 0xff) break;
  } 
  
  // By default, the StandardAlert only shows the 'OK' button. In order to get the
  // 'Cancel' to show as well, we need to fill out this structure. Fun.
  SInt16 result;
  AlertStdAlertParamRec alertparams;
  alertparams.movable = true;
  alertparams.helpButton = false;
  alertparams.filterProc = NULL;
  alertparams.defaultText = (const unsigned char*)-1;
  alertparams.cancelText = (const unsigned char*)-1;
  alertparams.otherText = NULL;
  alertparams.defaultButton = kAlertStdAlertCancelButton;
  alertparams.cancelButton = kAlertStdAlertOKButton;
  alertparams.position = kWindowDefaultPosition;
  
  StandardAlert (
     kAlertCautionAlert, //AlertType inAlertType,
     (ConstStr255Param)temp, //ConstStr255Param inError,
     NULL, //ConstStr255Param inExplanation,
     &alertparams, //const AlertStdAlertParamRec *inAlertParam,
     &result//SInt16 *outItemHit
  );  
  if (result == kAlertStdAlertOKButton)
    return 1;
  return 0;
}

#endif

void gotoappdirectory(int parc, char ** pars)
{
  // Handled in sdl's objective-c "main" source
}

int opendll(const char *dllfilename)
{
    void* library = dlopen(dllfilename, RTLD_LAZY);
    return (int)library;
}

void *getdllproc(int dllhandle, const char *procname)
{
    void *library = (void*)dllhandle;
    return dlsym(library,procname);
}
#endif

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#ifdef LINUX_VERSION

#include "SDL/SDL_syswm.h" // to get current window handle
#include <gtk/gtk.h>
#include <dlfcn.h> // dll functions
#include <unistd.h>


static char *gtkfilename;
static bool selected = false; // is a file selected?

static void
selected_file(GtkWidget * widget, gint arg1, gpointer data)
{
  if(arg1==GTK_RESPONSE_ACCEPT)
  {
  	const gchar* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(data));  	
  	if (!g_file_test(filename, G_FILE_TEST_IS_DIR))
  	{
  		strncpy(gtkfilename, filename, 255);
  		gtkfilename[255] = 0;
  		selected = true;
  	}
  }
  gtk_widget_destroy(GTK_WIDGET(data));
  gtk_main_quit();

}

static void
selected_dlg(GtkWidget * widget, gint arg1, gpointer data)
{
  if(arg1==GTK_RESPONSE_OK)
  {
      selected = true;
  }

  gtk_widget_destroy(GTK_WIDGET(data));
  gtk_main_quit();
}

static bool select_file (char *buf, int mode, const char*title)
{
	gtkfilename = buf;
	selected = false;
	GtkWidget *dialog;
	if (mode == 0)
    {
        dialog = gtk_file_chooser_dialog_new (title,
    				      NULL, 
    				      GTK_FILE_CHOOSER_ACTION_OPEN,
    				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
    				      (char*)0);//NULL);
    }
    else
    {
        dialog = gtk_file_chooser_dialog_new (title,
    				      NULL, 
    				      GTK_FILE_CHOOSER_ACTION_SAVE,
    				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
    				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
    				      (char*)0);//NULL);
    if (gFilename)
        gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), gFilename);
    }
	
	g_signal_connect(G_OBJECT(dialog), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(GTK_FILE_CHOOSER_DIALOG(dialog)), "response", G_CALLBACK(selected_file), G_OBJECT(dialog));
	gtk_widget_show(GTK_WIDGET(dialog));
	gtk_main();
	return selected;
}


FILE * openfiledialog(const char *title)
{ 
  char temp[256];

  select_file(temp, 0, (title == NULL)?"Open Atanua design file":title);
  FILE * f = NULL;
  if (selected)
  {
    f = fopen(temp, "rb");
    if (title == NULL)
        storefilename(temp);    
  }
  return f;
}

FILE * savefiledialog(const char *title)
{
  char temp[256];

  select_file(temp, 1, (title == NULL)?"Save Atanua design file":title);
  FILE * f = NULL;
  if (selected)
  {
    if (strstr(temp, ".atanua") == NULL)
        strcat(temp, ".atanua");
    f = fopen(temp, "wb");
    if (title == NULL)
        storefilename(temp);    
  }
  return f;
}

int okcancel(const char *prompt)
{
	selected = false;
	GtkWidget *dialog;
    dialog = gtk_message_dialog_new(NULL,
                                  GTK_DIALOG_MODAL,
                                  GTK_MESSAGE_WARNING,
                                  GTK_BUTTONS_OK_CANCEL,
                                  "%s", prompt);
	g_signal_connect(G_OBJECT(dialog), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(GTK_MESSAGE_DIALOG(dialog)), "response", G_CALLBACK(selected_dlg), G_OBJECT(dialog));
	gtk_widget_show(GTK_WIDGET(dialog));
	gtk_main();
    return selected;
}

void gotoappdirectory(int parc, char ** pars)
{
    // as good place as any..
    gtk_init(&parc, &pars);
    
    // only works if pars[0] points to application and not to a link. 
    char *rch = strrchr(pars[0], '/');
    if (rch != NULL) 
    {
      *rch = 0;
      chdir(pars[0]);
    }  
}

void* opendll(const char *dllfilename)
{
    void* library = dlopen(dllfilename, RTLD_LAZY);
    return library;
}

void *getdllproc(void* dllhandle, const char *procname)
{
    void* library = dllhandle;
    return dlsym(library,procname);
}
#endif
