/*
 * AppMain.c
 */
 
#include <Hs.h>
#include <HsPhone.h>
#include <HsNav.h>
#include <HsExt.h>
#include <palmOneResources.h>
#include <PalmTypes.h>
#include <Window.h>
#include <Form.h>
#include <AlarmMgr.h>
#include <TextMgr.h>

#include "Global.h"
#include "AppResources.h"
#include "Prefs.h"
#include "PhnEventHandler.h"
#include "AttnAlerts.h"
#include "../../TreoFlex2/src/Utils.h"

// Prototypes
static Err 					AppStart( void );
static void 				AppStop( void );
static Boolean 				AppHandleEvent( EventType* pEvent );
static void 				AppEventLoop( void );
UInt32 						PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags );

/*
 * AppStart
 */
static Err AppStart( void )
{
	FrmGotoForm( MAIN_FORM );

	return ( errNone );
}

/*
 * AppStop()
 */
static void AppStop( void )
{
	FrmCloseAllForms();	
} // AppStop()

/*
 * AppHandleEvent()
 */
static Boolean AppHandleEvent( EventType* pEvent )
{
	FormType* 	pForm;
	Boolean		handled = false;

	if ( pEvent->eType == frmLoadEvent )
	{		
		pForm = FrmInitForm( pEvent->data.frmLoad.formID );

		FrmSetActiveForm( pForm );

		switch ( pEvent->data.frmLoad.formID )
		{
			case MAIN_FORM:
			
				FrmSetEventHandler( pForm, PrefsFormHandleEvent );
				
				handled = true;
				
				break;
			
			case ABOUT_FORM:
			
				FrmSetEventHandler( pForm, AboutFormHandleEvent );
				
				handled = true;
				
				break;
				
			default:
				break;
		}
	}
	
	return ( handled );
	
} // AppHandleEvent()

/*
 * AppEventLoop()
 */
static void AppEventLoop( void )
{
	Err			error = errNone;
	EventType	event;

	do {
		EvtGetEvent( &event, evtWaitForever );

		if ( SysHandleEvent( &event ) )
			continue;
		
		if ( MenuHandleEvent( 0, &event, &error ) )
			continue;
						
		if ( AppHandleEvent( &event ) )
			continue;

		FrmDispatchEvent( &event );

	} while ( event.eType != appStopEvent );

	return;
	
} // AppEventLoop()

/*
 * PilotMain()
 */
UInt32 PilotMain( UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags )
{
	Err 			error = errNone;
	appPrefs_t 		prefs;
	
	switch ( cmd )
	{
		case phnLibLaunchCmdEvent:
		
			ReadAppPrefs( &prefs );
			
			if ( prefs.bEnabled ) 	
			{
				error = HandlePhnEvent( (PhnEventType *) cmdPBP );
			}
			
			break;
			
		case sysAppLaunchCmdNotify:
			
			ReadAppPrefs( &prefs );
			
			error = HandleChEvent( (SysNotifyParamType *) cmdPBP, &prefs );
			
			break;
			
		case sysAppLaunchCmdAttention:
		
			ProcessAttention( ( AttnLaunchCodeArgsType *) cmdPBP );
			
			break;
			
		case sysAppLaunchCmdGoTo:
						
			break;
			
		case sysAppLaunchCmdAlarmTriggered:
		
			((SysAlarmTriggeredParamType *) cmdPBP)->purgeAlarm = ( ((SysAlarmTriggeredParamType *)cmdPBP)->ref != 0 );
						
			break;
			
		case sysAppLaunchCmdDisplayAlarm:
		
			if ( ((SysDisplayAlarmParamType *)cmdPBP)->ref != 0 ) break;
			
			++( ((SysDisplayAlarmParamType *)cmdPBP)->ref );
			
			vibrate( 5 );
			SndPlaySystemSound( sndClick );
			
			ReadAppPrefs( &prefs );
			SetCustomEffectAlarm( true, prefs.nagRateIdx );
			
			break;
			
		case sysAppLaunchCmdSystemReset:

			if ( ( error = RomVersionCompatible(MIN_VERSION, launchFlags) ) == errNone )
			{
				ReadAppPrefs( &prefs );
				RegisterForPhnNotifications( prefs.bEnabled );
			}		
			break;

		case sysAppLaunchCmdNormalLaunch:
	    case sysAppLaunchCmdPanelCalledFromApp:
	    case sysAppLaunchCmdReturnFromPanel:
			
			if ( ( error = RomVersionCompatible(MIN_VERSION, launchFlags) ) )
			{
				FrmCustomAlert( ErrOKAlert, "Incompatible Device", "", "" );
				return ( error );
			} 
			
			if ( ( error = AppStart() ) == 0 )
			{
				AppEventLoop();
				AppStop();			 	
			}
					
			break;

		default:
			break;
	}

	return ( error );

} // PilotMain()
