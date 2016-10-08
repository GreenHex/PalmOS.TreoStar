/*
 * PhnEventHandler.c
 */
 
#include "PhnEventHandler.h"

// Prototypes 
typedef Char* 			PhnLibGetFieldF( UInt16 refNum, PhnAddressHandle address, PhnAddressField field ); 
static Char*			GetAddressGSM( UInt16 phnLibRefNum, PhnAddressHandle address, PhnAddressField field );
static Char*			GetAddressCDMA( UInt16 phnLibRefNum, PhnAddressHandle address, PhnAddressField field );

/*
 * HandlePhnEvent()
 */
Err HandlePhnEvent( PhnEventType* phnEventP )
{
	Err							error = errNone;
	appPrefs_t					prefs;
	chPrefs_t					chPrefs;
	SysNotifyParamType* 		chNotifyParamP = NULL;
	UInt16						phnLibRefNum = sysInvalidRefNum;
	char* 						strPhnNumP = NULL;
	PhnLibGetFieldF*			funcPhnLibGetFieldP = NULL;
	
	switch ( phnEventP->eventType )
	{
		case phnEvtStartDial: // 0x0027 // Outgoing call
		case phnEvtSubscriber: // 0x000A, we use it now ONLY to capture the Caller ID.
			
			ReadAppPrefs( &prefs );
			
			if ( prefs.bEnabled )
			{
				if ( ( ( prefs.bShowOutgoing ) && ( phnEventP->eventType == phnEvtStartDial ) )
						||	( ( prefs.bShowIncoming )  && ( phnEventP->eventType == phnEvtSubscriber ) ) )
				{					
					if ( !HsGetPhoneLibrary( &phnLibRefNum ) )
					{
						if ( !PhnLibOpen( phnLibRefNum ) )
						{	
							char* 		callerNum = NULL;
						
							funcPhnLibGetFieldP = ( IsPhoneGSM() ) ? &GetAddressGSM : &GetAddressCDMA;
							
							strPhnNumP = (*funcPhnLibGetFieldP)( phnLibRefNum, (PhnAddressHandle) phnEventP->data.info.caller, phnAddrFldPhone );
																
							InitializeChPrefs( &chPrefs );
							
							if ( ( strPhnNumP ) && StrLen( strPhnNumP ) )
							{
								StrCopy( chPrefs.Address, strPhnNumP );
								MakeNum( (char *) &( chPrefs.Address ) ); // remove unwanted characters
								
								if ( StrLen( chPrefs.Address ) > 0 )
								{
									if ( chPrefs.bDoReminder = CallerLookup( chPrefs.Address, STR_LOOKUP_CALLER_NAME_FMT, 
														chPrefs.CallerID, STRLEN_CALLERID,
														chPrefs.Note, STRLEN_NOTE ) )
									{
										chPrefs.bOutgoingCall = ( phnEventP->eventType == phnEvtStartDial );
										
										if ( chPrefs.bDoReminder )
										{
											chPrefs.attnLevel = ( prefs.bMinimizeOutgoing ) ? kAttnLevelSubtle : kAttnLevelInsistent;
										}
										else
										{
											chPrefs.attnLevel = ( prefs.bMinimizeIncoming ) ? kAttnLevelSubtle : kAttnLevelInsistent;
										}
									
										RegisterForChNotifications( chPrefs.bDoReminder );
									}
								}
							}
														
							WriteChPrefs( &chPrefs );
							
							PhnLibClose( phnLibRefNum );
							
							if ( chPrefs.bDoReminder )
							{
								DoAlert( chPrefs.attnLevel );
								SetCustomEffectAlarm( true, prefs.nagRateIdx );
							}
						}	
					}
				}		
			}
			
			phnEventP->acknowledge = true;
			
			break;
		
		case phnEvtConnectConf: // 0x0009 // call goes into connected state
		
			ReadAppPrefs( &prefs );
			
			if ( prefs.bWarnOnCallAccept )
			{
				vibrate( 30 );
			}
			
			// NO break;
			
		case phnEvtProgress: // 0x0006 // outgoing call progress dialog		
		case phnEvtConnectInd: // 0x0008 // Incoming call queued...
		case phnEvtUpdate: 	// 0x000E // Accept an incoming call on the 650	// Status of incoming call changed, whatever that means...
		case phnEvtConference: // 0x000F // This notification occurs when a call goes into conferrence (or 3-way calling) mode.
		case phnEvtAlertingPreConnected: // 0x0029 // call is alerting or preconnected
			
			ReadChPrefs( &chPrefs );

			if ( chPrefs.bDoReminder )
			{
				DoAlert( chPrefs.attnLevel );
			}
			
			break;
	
		// case phnEvtError: // 0x0002 // error scenarios
		case phnEvtDisconnectInd: // 0x000B // disconnected at the other end
		case phnEvtDisconnectConf: // 0x000C // conference call disconnected (one of remote party disconnects) Amazing!
		case phnEvtBusy: // 0x000D // phone disconnect (Ignore, red-button) sends this notification, don't know why!

			CancelAlert( 0 );
			SetCustomEffectAlarm( false, 0 );
			
			RegisterForChNotifications( false );
			
			InitializeChPrefs( &chPrefs );
			
			WriteChPrefs( &chPrefs );
						
			break;
	
		default:		
			break;
	}
	
	return ( error );
	
} // HandlePhnEvent()

/*
 * HandleChEvent()
 */
Err HandleChEvent( SysNotifyParamType* notifyParamP, appPrefs_t* pPrefs )
{
	Err							error = errNone;
	WChar						wChar = 0;
	UInt16						keyCode = 0;
	UInt16						modifier = 0;
	
	switch ( notifyParamP->notifyType )
	{
		case chDeferredNotifyEvent:
			
			notifyParamP->handled = true;
				
			break;
			
		case sysNotifyVirtualCharHandlingEvent:
			
			{
				wChar = ( ( SysNotifyVirtualCharHandlingType *) notifyParamP->notifyDetailsP )->keyDown.chr;
				keyCode = ( ( SysNotifyVirtualCharHandlingType *) notifyParamP->notifyDetailsP )->keyDown.keyCode;
				modifier = ( ( SysNotifyVirtualCharHandlingType *) notifyParamP->notifyDetailsP )->keyDown.modifiers;
				
				if ( ( wChar == hsKeySymbol ) && ( modifier & commandKeyMask ) )
				{
					DoAlert( kAttnLevelInsistent );
					notifyParamP->handled = true;
				}
			}
				
			break;
			
		default:
			break;
	}
	
	return ( error );
	
} // HandleChEvent()

/*
 * GetAddressGSM()
 */
static Char* GetAddressGSM( UInt16 phnLibRefNum, PhnAddressHandle address, PhnAddressField field )
{
	return ( PhnLibGetField( phnLibRefNum, address, field ) );

} // GetAddressGSM()

/*
 * GetAddressCDMA()
 */
static Char* GetAddressCDMA( UInt16 phnLibRefNum, PhnAddressHandle address, PhnAddressField field )
{
	return ( PhnLibAPGetField( phnLibRefNum, address, field ) );	

} // GetAddressCDMA()

/*
 * PhnEventHandler.c
 */