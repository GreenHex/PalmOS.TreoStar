/*
 * AttnAlerts.h
 */
 
#ifndef __TFS_ATTNALERTS_H__
#define __TFS_ATTNALERTS_H__

#include <Hs.h>
#include <SysUtils.h>
#include <Font.h>

#include "Global.h"
#include "AppResources.h"
#include "Prefs.h"
#include "../../TreoFlex2/src/Global.h"
#include "../../Centroid/src/Bitmaps.h"
#include "../../TreoFlex2/src/Utils.h"

#define STRLEN_CALLTYPE		32
#define DETAIL_X_POS		32
#define DETAIL_Y_POS		8
#define LIST_X_POS			19
#define LIST_Y_POS			0

typedef enum {
	incomingCall			= 0,
	outgoingCall			= 1
} attnType_e;

// Prototypes
extern void 			CancelAlert( UInt32 userData );
extern void 			DoAlert( AttnLevelType level );
extern void 			vibrate( Int32 duration );
extern void 			SetCustomEffectAlarm( Boolean set, UInt8 nagRateIdx );

/* AttnLevelType
		kAttnLevelInsistent 
		kAttnLevelSubtle 
*/

/*
typedef enum SndSysBeepTag {
  sndInfo = 1,
  sndWarning,
  sndError,
  sndStartUp,
  sndAlarm,
  sndConfirmation,
  sndClick
}
*/

#endif /* __TFS_ATTNALERTS_H__ */

/*
 * AttnAlerts.h
 */