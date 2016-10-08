/*
 * Prefs.c
 */

#include "Prefs.h"

// Prototypes
static Boolean 			PrefsFormHandleMenu( UInt16 menuID );
static void 			InitializePrefsForm( FormType* pForm, appPrefs_t* pPrefs );
static void 			CopyPrefsFromForm( FormType* pForm, appPrefs_t* pPrefs );
static void 			InitializeAppPrefs( appPrefs_t* pPrefs );

/*
 * PrefsFormHandleMenu()
 */
Boolean PrefsFormHandleMenu( UInt16 menuID )
{
	Boolean 			handled = false;
	EventType			newEvent;

	switch ( menuID )
	{
		case MAIN_MENU_EXIT:
		
			newEvent.eType = appStopEvent;
									
			EvtAddEventToQueue( &newEvent );	
					
			handled = true;
		
			break;
		
		case MAIN_MENU_HELP:
		
			FrmHelp( MAIN_HELP_STRING );
					
			handled = true;
			
			break;
			
		case MAIN_MENU_ABOUT:
		
			FrmPopupForm( ABOUT_FORM );
			
			handled = true;
			
			break;
						
		default:
			break;
	}

	return ( handled );

} // PrefsFormHandleMenu()

/*
 * PrefsFormHandleEvent()
 */
Boolean PrefsFormHandleEvent( EventType* pEvent )
{
	Boolean 				handled = false;
	FormType* 				pForm = FrmGetActiveForm();
	appPrefs_t				prefs;
	EventType				newEvent;
		
	switch ( pEvent->eType )
	{		
		case frmOpenEvent:
		
			ReadAppPrefs( &prefs );
		
			InitializePrefsForm( pForm, &prefs );
			
			FrmDrawForm( pForm );
			
			FrmNavObjectTakeFocus( pForm, MAIN_OK_BTN );
			
			handled = true;
						
			break;
			
		case frmCloseEvent:
			
			CopyPrefsFromForm( pForm, &prefs );
								
			WriteAppPrefs( &prefs );
			
			RegisterForPhnNotifications( prefs.bEnabled );
						
			handled = false;
			
			break;
			
		case ctlSelectEvent:
		
			switch ( pEvent->data.ctlSelect.controlID )
			{
				case MAIN_OK_BTN:
					
					newEvent.eType = appStopEvent;
									
					EvtAddEventToQueue( &newEvent );
					
					handled = true;

					break;
						
				default:
					break;
			}
		
			break;
			
		case menuEvent:
										
			handled = PrefsFormHandleMenu( pEvent->data.menu.itemID );
			
			break;
			
		default:
								
			break;
	}
	
	return ( handled );
	
} // PrefsFormHandleEvent()

/*
 * InitializePrefsForm()
 */
static void InitializePrefsForm( FormType* pForm, appPrefs_t* pPrefs )
{		
	if ( ( !pForm ) || ( !pPrefs ) ) return;
	
	SetCtlBoolValue( pForm, MAIN_ENABLE_CBX, pPrefs->bEnabled );
	SetCtlBoolValue( pForm, MAIN_SHOW_INCOMING_CBX, pPrefs->bShowIncoming );
	SetCtlBoolValue( pForm, MAIN_SHOW_OUTGOING_CBX, pPrefs->bShowOutgoing );
	SetCtlBoolValue( pForm, MAIN_SHOW_INCOMING_MINIMIZED_CBX, pPrefs->bMinimizeIncoming );
	SetCtlBoolValue( pForm, MAIN_SHOW_OUTGOING_MINIMIZED_CBX, pPrefs->bMinimizeOutgoing );
	SetCtlBoolValue( pForm, MAIN_WARN_ON_CALL_ACCEPT_CBX, pPrefs->bWarnOnCallAccept );
	SetPopLabelFromList( pForm, MAIN_NAG_RATE_POP, MAIN_NAG_RATE_LST, pPrefs->nagRateIdx );
	
	return;
	
} // InitializePrefsForm()

/*
 * CopyPrefsFromForm()
 */
static void CopyPrefsFromForm( FormType* pForm, appPrefs_t* pPrefs )
{
	if ( ( !pForm ) || ( !pPrefs ) ) return;

	pPrefs->bEnabled = GetCtlBoolValue( pForm, MAIN_ENABLE_CBX );
	pPrefs->bShowIncoming = GetCtlBoolValue( pForm, MAIN_SHOW_INCOMING_CBX );
	pPrefs->bShowOutgoing = GetCtlBoolValue( pForm, MAIN_SHOW_OUTGOING_CBX );
	pPrefs->bMinimizeIncoming = GetCtlBoolValue( pForm, MAIN_SHOW_INCOMING_MINIMIZED_CBX );
	pPrefs->bMinimizeOutgoing = GetCtlBoolValue( pForm, MAIN_SHOW_OUTGOING_MINIMIZED_CBX );
	pPrefs->bWarnOnCallAccept = GetCtlBoolValue( pForm, MAIN_WARN_ON_CALL_ACCEPT_CBX );
	pPrefs->nagRateIdx = LstGetSelection( FrmGetObjectPtr( pForm, FrmGetObjectIndex( pForm, MAIN_NAG_RATE_LST ) ) );
	
	return;	

} // CopyPrefsFromForm()

/*
 * WriteAppPrefs()
 */
void WriteAppPrefs( appPrefs_t* pPrefs )
{
	if ( !pPrefs ) return;
	
	PrefSetAppPreferences( appPrefCreator, appPrefID, appPrefVersionNum, pPrefs, sizeof( appPrefs_t ), appPrefType );
	
	return;
	
} // WriteAppPrefs()

/*
 * ReadAppPrefs()
 */
void ReadAppPrefs( appPrefs_t* pPrefs )
{
	UInt16 			prefSize = sizeof( appPrefs_t );
	UInt16			prefsVersion = 0;

	if ( !pPrefs ) return;
		
	prefsVersion = PrefGetAppPreferences( appPrefCreator, appPrefID, pPrefs, &prefSize, appPrefType );
	
	if ( prefsVersion != appPrefVersionNum ) // remove old prefs...
	{
		PrefSetAppPreferences( appPrefCreator, appPrefID, prefsVersion, NULL, 0, appPrefType );				
	
		InitializeAppPrefs( pPrefs );
		
		WriteAppPrefs( pPrefs );
	}
	
	return;
	
} // ReadAppPrefs()

/*
 * InitializeAppPrefs()
 */
static void InitializeAppPrefs( appPrefs_t* pPrefs )
{
	if ( !pPrefs ) return;
	
	pPrefs->bEnabled = true;
	pPrefs->bShowIncoming = true;
	pPrefs->bShowOutgoing = true;
	pPrefs->bMinimizeIncoming = false;
	pPrefs->bMinimizeOutgoing = false;
	pPrefs->bReenableOnAppSwitch = true;
	pPrefs->bWarnOnCallAccept = false;
	pPrefs->nagRateIdx = 0;
		
	return;
	
} // InitializeAppPrefs

/*
 * WriteChPrefs()
 */
void WriteChPrefs( chPrefs_t* pChPrefs )
{
	if ( !pChPrefs ) return;
	
	PrefSetAppPreferences( chPrefCreator, chPrefID, chPrefVersionNum, pChPrefs, sizeof( chPrefs_t ), chPrefType );
	
	return;
	
} // WriteChPrefs()

/*
 * ReadChPrefs()
 */
void ReadChPrefs( chPrefs_t* pChPrefs )
{
	UInt16 			prefSize = sizeof( chPrefs_t );
	UInt16			prefsVersion = 0;

	if ( !pChPrefs ) return;
		
	prefsVersion = PrefGetAppPreferences( chPrefCreator, chPrefID, pChPrefs, &prefSize, chPrefType );
	
	if ( prefsVersion != chPrefVersionNum ) // remove old prefs...
	{
		PrefSetAppPreferences( chPrefCreator, chPrefID, prefsVersion, NULL, 0, chPrefType );				
	
		InitializeChPrefs( pChPrefs );
		
		WriteChPrefs( chPrefType );
	}
	
	return;
	
} // ReadAppPrefs()

/*
 * InitializeChPrefs()
 */
void InitializeChPrefs( chPrefs_t* pChPrefs )
{
	if ( !pChPrefs ) return;
	
	MemSet( pChPrefs, sizeof( chPrefs_t ), 0 );
	
	return;

} // InitializeChPrefs()

/*
 * Prefs.c
 */