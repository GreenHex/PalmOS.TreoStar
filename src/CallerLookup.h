/*
 * CallerLookup.h
 */
 
#ifndef __TS_CALLERLOOKUP_H__
#define __TS_CALLERLOOKUP_H__

#include <Hs.h>
#include <HsExt.h>
#include <palmOneResources.h>
#include <PalmTypes.h>
#include <HsAppLaunchCmd.h>
#include <palmOneCreators.h>

#include "Global.h"
#include "../../TreoFlex2/src/Utils.h"
#include "../../TreoFlex2/src/AddrDB.h"

#define STR_LOOKUP_CALLER_NAME_FMT			"^friendlyname (^label)" // "^first ^name (^label)"

#define STR_TOKEN_START						"<ts>"
#define STR_TOKEN_END						"</ts>"

// Prototypes
extern Boolean 			CallerLookup( char* number, char* formatStr, char* nameStr, UInt16 nameStrSize, char* contactsNote, UInt16 contactsNoteSize );

#endif /* __TS_CALLERLOOKUP_H__ */
 
/*
 * CallerLookup.h
 */