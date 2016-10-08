/*
 * Prefs.h
 */
 
#ifndef __TS_PREFS_H__
#define __TS_PREFS_H__

#include <Hs.h>
#include <Preferences.h>
#include <StringMgr.h>

#include "Global.h"
#include "AppResources.h"
#include "NotifyReg.h"
#include "../../TreoFlex2/src/Utils.h"
#include "../../TreoFlex2/src/FormUtils.h"

typedef enum {
	prefUnsaved							= false,
	prefSaved							= true
} prefType_e;

#define appPrefCreator					appFileCreator
#define appPrefID						0x00
#define appPrefVersionNum				0x01
#define appPrefType						prefSaved						
//
#define chPrefCreator					appFileCreator
#define chPrefID						appPrefID
#define chPrefVersionNum				0x01
#define chPrefType						prefUnsaved

typedef struct { 
	Boolean					bEnabled; // 1
	Boolean					bShowIncoming; // 1
	Boolean					bShowOutgoing; // 1
	Boolean					bMinimizeIncoming; // 1
	Boolean					bMinimizeOutgoing; // 1
	Boolean					bReenableOnAppSwitch; // 1
	Boolean					bWarnOnCallAccept; // 1
	UInt8					nagRateIdx;
} appPrefs_t;

/*
 * Miscellaneous stuff
 */
typedef struct {
	Boolean					bDoReminder;
	AttnLevelType 			attnLevel;
	Boolean					bOutgoingCall;
	Char					Address[STRLEN_PHN_NUMBER];
	Char					CallerID[STRLEN_CALLERID];
	Char					Note[STRLEN_NOTE];
} chPrefs_t;

// Prototypes
extern Boolean 				PrefsFormHandleEvent( EventType* pEvent );
extern void 				WriteAppPrefs( appPrefs_t* pPrefs );
extern void 				ReadAppPrefs( appPrefs_t* pPrefs );
extern void					WriteChPrefs( chPrefs_t* pChPrefs );
extern void					ReadChPrefs( chPrefs_t* pChPrefs );
extern void 				InitializeChPrefs( chPrefs_t* pChPrefs );

#endif /* __TS_PREFS_H__ */
 
/*
 * Prefs.h
 */