/*
 * CallerLookup.c
 */
 
#include "CallerLookup.h"

// Prototypes
static void 			ReplaceDateTimeTokens( Char* str, UInt16 strLen );
static void 			ReplaceContactsTokens( AddrDBRecordPtr addrRecordP, Char* str, UInt16 strLen );
static Boolean 			AddrDBGetNote( UInt16 pIndex, UInt32 pUniqueID, Char* txtStr, UInt16 txtStrLen );
static Boolean 			CallerLookupCallbackFunc( void* ref );

/*
 * ReplaceDateTimeTokens()
 */
static void ReplaceDateTimeTokens( Char* str, UInt16 strLen )
{
	Char				dateStr[dateStringLength];
	Char				timeStr[timeStringLength];
	
	if ( !str ) return;
	
	MakeTimeDateStr( TimGetSeconds(), NULL, timeStr, dateStr, false );
				
	TxtReplaceStr( str, strLen, dateStr, 0 ); // ^0
	TxtReplaceStr( str, strLen, timeStr, 1 ); // ^1

	return;
	
} // ReplaceDateTimeTokens()

/*
 * ReplaceContactsTokens()
 */
static void ReplaceContactsTokens( AddrDBRecordPtr addrRecordP, Char* str, UInt16 strLen )
{
	if ( ( addrRecordP ) || ( !str ) ) return;

	if ( addrRecordP )
	{
		if ( addrRecordP->fields[company] )
			TxtReplaceStr( str, strLen, addrRecordP->fields[company], 0 ); // ^0
		else
			TxtReplaceStr( str, strLen, " ", 0);
	
		if ( addrRecordP->fields[title] )	
			TxtReplaceStr( str, strLen, addrRecordP->fields[title], 1 ); // ^1
		else
			TxtReplaceStr( str, strLen, " ", 1 );
		
		if ( addrRecordP->fields[address] )
			TxtReplaceStr( str, strLen, addrRecordP->fields[address], 2 );
		else
			TxtReplaceStr( str, strLen, " ", 2 );
		
		if ( addrRecordP->fields[city] )
			TxtReplaceStr( str, strLen, addrRecordP->fields[city], 3 );
		else
			TxtReplaceStr( str, strLen, " ", 3 );
		
		if ( addrRecordP->fields[state] )
			TxtReplaceStr( str, strLen, addrRecordP->fields[state], 4 );
		else
			TxtReplaceStr( str, strLen, " ", 4 );
			
		if ( addrRecordP->fields[country] )
			TxtReplaceStr( str, strLen, addrRecordP->fields[country], 5 );
		else			TxtReplaceStr( str, strLen, " ", 5 );
			
		if ( addrRecordP->fields[custom1] )	
			TxtReplaceStr( str, strLen, addrRecordP->fields[custom1], 6 );
		else
			TxtReplaceStr( str, strLen, " ", 6 );
		
		if ( addrRecordP->fields[custom2] )
			TxtReplaceStr( str, strLen, addrRecordP->fields[custom2], 7 );  
		else
			TxtReplaceStr( str, strLen, " ", 7 );
			
		if ( addrRecordP->fields[custom3] )
			TxtReplaceStr( str, strLen, addrRecordP->fields[custom3], 8 );
		else
			TxtReplaceStr( str, strLen, " ", 8 );
		
		if ( addrRecordP->fields[custom4] )
			TxtReplaceStr( str, strLen, addrRecordP->fields[custom4], 9 );
		else
			TxtReplaceStr( str, strLen, " ", 9 );
	}
	
	return;
	
} // ReplaceContactsTokens()

/*
 * AddrDBGetNote()
 */
static Boolean AddrDBGetNote( UInt16 pIndex, UInt32 pUniqueID, Char* txtStr, UInt16 txtStrLen )
{
	Boolean					retVal = false;
	DmOpenRef				AddrDBRef;
	MemHandle				recordH = NULL;
	AddrDBRecordType		record;
	
	if (!txtStr) return ( retVal );
		
	if ( AddrDBGetDatabase( &AddrDBRef, dmModeReadOnly | dmModeShowSecret ) ) return ( retVal );
	
	MemSet( &record, sizeof( AddrDBRecordType ), 0 );
	
	if( !AddrDBGetRecord( AddrDBRef, pIndex, &record, &recordH ) )
	{	
		if ( ( record.fields[note] ) && ( StrLen( record.fields[note] ) ) )
		{
			Char		tmpTxtStr[txtStrLen];
			Char*		pStartLocation = NULL;
			Char*		pEndLocation = NULL;
			
			StrNCopy( tmpTxtStr, record.fields[note], txtStrLen - 1 );
			tmpTxtStr[txtStrLen - 1] = chrNull;
			
			pStartLocation = StrStr( tmpTxtStr, STR_TOKEN_START );
			pEndLocation = StrStr( tmpTxtStr, STR_TOKEN_END );
			
			if ( !pEndLocation )
			{
				pEndLocation = tmpTxtStr + ( txtStrLen - 1 );
			}
			
			if ( ( pStartLocation ) && ( pEndLocation ) )
			{
				StrNCopy( txtStr, pStartLocation + StrLen( STR_TOKEN_START ),
						( pEndLocation - pStartLocation ) - StrLen( STR_TOKEN_START ) );
		
				txtStr[ ( pEndLocation - pStartLocation ) - StrLen( STR_TOKEN_START ) ] = chrNull;
		
				ReplaceContactsTokens( &record, txtStr, txtStrLen );
				
				// ReplaceDateTimeTokens( txtStr, txtStrLen );
				
				retVal = ( StrLen( txtStr ) > 0 );
			}    
		}
		
		MemHandleUnlock( recordH );
	}
	
	DmCloseDatabase( AddrDBRef );

	return ( retVal );
	
} // AddrDBGetNote()

/*
 * CallerLookupCallbackFunc()
 */
static Boolean CallerLookupCallbackFunc( void* ref )
{
    return ( false );
    
} // CallerLookupCallbackFunc()

/*
 * CallerLookup()
 */
Boolean CallerLookup( char* number, char* formatStr, char* nameStr, UInt16 nameStrSize,
						char* contactsNote, UInt16 contactsNoteSize )
{
    Boolean 					retVal = false;
    SysNotifyParamType* 		pNotifyParams;
    AddrCallerIDParamsType* 	pLookupParams;
    
    if ( !nameStr ) return ( retVal );
    
    pNotifyParams = MemPtrNew( sizeof( SysNotifyParamType ) );
    pLookupParams = MemPtrNew( sizeof( AddrCallerIDParamsType ) );
    
    if ( pNotifyParams && pLookupParams )
	{
		Err						error = errNone;
		UInt32					result;
		UInt16					cardNo;
		LocalID					dbID;
		DmSearchStateType 		stateInfo;	
		UInt32 					plainVal = 0;
	    
	    MemSet( pNotifyParams, sizeof( SysNotifyParamType ), 0 );
	    MemSet( pLookupParams, sizeof( AddrCallerIDParamsType ), 0 );

	    pNotifyParams->notifyType = addrAppNotificationCmdCallerID;
	    pNotifyParams->notifyDetailsP = pLookupParams;
	
	    pLookupParams->lookupString = number;
	    pLookupParams->formatString = formatStr;
	    pLookupParams->callback = CallerLookupCallbackFunc;
	    pLookupParams->ref = (void *)&plainVal;
	
	 	if( DmGetNextDatabaseByTypeCreator( true, &stateInfo, sysFileTApplication, kPalmOneCreatorIDContacts, true, &cardNo, &dbID ) )
	 		return retVal;
		
		error = SysAppLaunch( cardNo, dbID, 0, sysAppLaunchCmdNotify, pNotifyParams, &result );	    
	    
	    if( ( !error ) && ( pLookupParams->resultString ) && ( StrLen( pLookupParams->resultString ) ) )
	    {
	    	StrNCopy( nameStr, pLookupParams->resultString, nameStrSize - 1 );
	    	nameStr[nameStrSize-1] = chrNull;
	    	
	    	if ( contactsNote )
	    	{
				retVal = AddrDBGetNote( pLookupParams->recordNum, pLookupParams->uniqueID, contactsNote, contactsNoteSize );    
	    	}	    	
	    }
	}

	if ( pLookupParams ) MemPtrFree( pLookupParams );
	if ( pNotifyParams ) MemPtrFree( pNotifyParams );

    return ( retVal );

} // CallerLookup()

/*
 * CallerLookup.c
 */
