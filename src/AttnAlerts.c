/*
 * AttnAlerts.c
 */
 
#include "AttnAlerts.h"

// Prototypes
static void 			DrawDetail( RectangleType rect );
static void 			DrawList( RectangleType rect );
static void 			DrawStrLines( Char* str, FontID fontID, IndexedColorType StrColor, UInt16 maxDrawLines, UInt16 strPixelWidth, Coord xPos, Coord yPos );
static UInt16 			WrapNoteLines( Char* str, UInt16 linePixelWidth, MemHandle* memHP );

/*
 * CancelAlert()
 */
void CancelAlert( UInt32 userData )
{
	UInt16 				cardNo = 0;
	LocalID				dbID = 0;
	
	if ( !SysCurAppDatabase( &cardNo, &dbID ) )
	{
		AttnForgetIt( cardNo, dbID, userData );
	}
	
	return;
	
} // CancelAlert()

/*
 * DoAlert()
 */
void DoAlert( AttnLevelType level )
{
	UInt16 				cardNo = 0;
	LocalID				dbID = 0;
	
	if ( !SysCurAppDatabase( &cardNo, &dbID ) )
	{
		AttnForgetIt( cardNo, dbID, 0 );
		
		if ( !AttnUpdate( cardNo, dbID, 0, NULL, NULL, NULL, NULL ) )
		{
			AttnGetAttention( cardNo, dbID, 0, NULL, 
									level, kAttnFlagsUseUserSettings, 10, 15 );
		}
	}
		
	return;
	
} // DoAlert()

/*
 * ProcessAttention
 */
void ProcessAttention( AttnLaunchCodeArgsType* paramP )
{
	AttnCommandArgsType* 		argsP = (AttnCommandArgsType *)paramP->commandArgsP;
	GoToParamsType*				cmdPBP = NULL;	
	UInt16 						cardNo = 0;
	LocalID						dbID = 0;
			
	switch ( paramP->command ) 
	{
		case kAttnCommandDrawDetail:
			
			DrawDetail( (RectangleType) argsP->drawDetail.bounds );

			break;
			
		case kAttnCommandDrawList:
			
			DrawList( (RectangleType) argsP->drawDetail.bounds );
			
			break;
		
		case kAttnCommandPlaySound:
			
			SndPlaySystemSound( sndInfo /* sndClick */ /* sndInfo */ /* sndAlarm */ );
			
			break;  
	
		case kAttnCommandCustomEffect:
		
			// SndPlaySystemSound( sndClick );
			vibrate( 10 );
			
			break;
		
		case kAttnCommandGoThere:

			DoAlert( kAttnLevelInsistent );
			
			break; 
		
		case kAttnCommandGotIt:
			
			if ( argsP->gotIt.dismissedByUser )
			{
				CancelAlert( (UInt32) paramP->userData );
				// DoAlert( kAttnLevelSubtle );			
			}
			
			break;   
		
		case kAttnCommandIterate:

			CancelAlert( (UInt32) paramP->userData );	
			
			break;
			
		case kAttnCommandSnooze:
		
			break;
		
		default:
		
			break;
	}

} // ProcessAttention

/*
 * DrawDetail() // Large alert
 */
static void DrawDetail( RectangleType rect )
{
	MemHandle 				BmpH = NULL;
	BitmapType* 			BmpP = NULL;
	Coord					BmpSizeX = 0;
	Coord					BmpSizeY = 0;
	RGBColorType 			rgb;
    IndexedColorType		StrColor;
	UInt16					strHeight = 0;
	UInt16					strWidth = 0;
	chPrefs_t				chPrefs;
	Char					strCallType[STRLEN_CALLTYPE] = "\0";
	
	ReadChPrefs( &chPrefs );
	
	WinPushDrawState();
	
	BmpP = GetBitmap( BMP_ICON_LARGE, &BmpSizeX, &BmpSizeY, &BmpH );
	WinDrawBitmap( BmpP, rect.topLeft.x + DETAIL_X_POS - BmpSizeX - 5, rect.topLeft.y + DETAIL_Y_POS - 4 );
	ReleaseBitmap( &BmpH );	
	BmpH = NULL;
	BmpP = NULL;
			
	FntSetFont( largeBoldFont );
	
	strHeight = FntLineHeight();
	rgb.r=0; rgb.g=51; rgb.b=200; // Blue
	StrColor = WinRGBToIndex( &rgb );	
	WinSetTextColor( StrColor );
	strWidth = FntLineWidth( APP_NAME, StrLen( APP_NAME ) );		
	WinDrawChars( APP_NAME, StrLen( APP_NAME ), rect.topLeft.x + DETAIL_X_POS, rect.topLeft.y + DETAIL_Y_POS );

	if ( chPrefs.bDoReminder )
	{
		rgb.r=250; rgb.g=0; rgb.b=51; // Red
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );
	    StrCopy( strCallType, ( chPrefs.bOutgoingCall ) ? "Outgoing" : "Incoming" );
		WinDrawChars( strCallType, StrLen( strCallType ), rect.topLeft.x + DETAIL_X_POS + strWidth + 5, rect.topLeft.y + DETAIL_Y_POS );
		
		rgb.r=0; rgb.g=0; rgb.b=0; // Black
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );
	    WinDrawTruncChars( chPrefs.CallerID, StrLen( chPrefs.CallerID ), 
	    						rect.topLeft.x + DETAIL_X_POS, 
	    						rect.topLeft.y + ( strHeight ) + ( 2 * DETAIL_Y_POS ), 
	    						rect.extent.x - DETAIL_X_POS );
		FntSetFont( boldFont );	
		WinDrawTruncChars( chPrefs.Address, StrLen( chPrefs.Address ), 
								rect.topLeft.x + DETAIL_X_POS, 
								rect.topLeft.y + ( 2 * strHeight ) + ( 2 * DETAIL_Y_POS ),
								rect.extent.x - DETAIL_X_POS );
		
		rgb.r=250; rgb.g=0; rgb.b=51; // Red
		StrColor = WinRGBToIndex( &rgb );	
		DrawStrLines( chPrefs.Note, boldFont, StrColor, 3, rect.extent.x - DETAIL_X_POS - 5, 
						rect.topLeft.x + DETAIL_X_POS, 
						rect.topLeft.y + 56 + DETAIL_Y_POS );	
	}
	
	WinPopDrawState();
		
	return;
	
} // DrawDetail()

/*
 * DrawList() // Small alert.
 */
static void DrawList( RectangleType rect )
{
	MemHandle 				BmpH = NULL;
	BitmapType* 			BmpP = NULL;
	Coord					BmpSizeX = 0;
	Coord					BmpSizeY = 0;
	RGBColorType 			rgb;
    IndexedColorType		StrColor;
	UInt16					strHeight = 0;
	UInt16					strWidth = 0;
	chPrefs_t				chPrefs;
	Char					strCallType[STRLEN_CALLTYPE] = "\0";

	ReadChPrefs( &chPrefs );
	
	WinPushDrawState();
	
	BmpP = GetBitmap( BMP_ICON_SMALL, &BmpSizeX, &BmpSizeY, &BmpH );
	WinDrawBitmap( BmpP, rect.topLeft.x + LIST_X_POS - BmpSizeX - 3, rect.topLeft.y + LIST_Y_POS + 1 );
	ReleaseBitmap( &BmpH );	
	BmpH = NULL;
	BmpP = NULL;
	
	FntSetFont( stdFont );
	strHeight = FntLineHeight();
		
	if ( chPrefs.bDoReminder )
	{
		rgb.r=0; rgb.g=0; rgb.b=0; // Black
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );
	    
	    StrCopy( strCallType, ( chPrefs.bOutgoingCall ) ? "Out:" : "In:" );
	    strWidth = FntLineWidth( strCallType, StrLen( strCallType ) );
		WinDrawChars( strCallType, StrLen( strCallType ), rect.topLeft.x + LIST_X_POS, rect.topLeft.y + LIST_Y_POS );
					
		rgb.r=0; rgb.g=51; rgb.b=200; // Blue
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );
		
		WinDrawTruncChars( chPrefs.CallerID, StrLen( chPrefs.CallerID ),
								rect.topLeft.x + LIST_X_POS + strWidth + 3, rect.topLeft.y,
								rect.extent.x - ( LIST_X_POS + strWidth + 3 ) );
	
		rgb.r=250; rgb.g=0; rgb.b=51; // Red
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );
		
		WinDrawTruncChars( chPrefs.Note, StrLen( chPrefs.Note ), 
								rect.topLeft.x + LIST_X_POS, 
								rect.topLeft.y + strHeight,
								rect.extent.x - LIST_X_POS );
	}
	else
	{
		rgb.r=0; rgb.g=51; rgb.b=200; // Blue
		StrColor = WinRGBToIndex( &rgb );	
	    WinSetTextColor( StrColor );
		
		strWidth = FntLineWidth( APP_NAME, StrLen( APP_NAME ) );	
		WinDrawChars( APP_NAME, StrLen( APP_NAME ), rect.topLeft.x + 19, rect.topLeft.y );
	}
	
		
	WinPopDrawState();

	return;
	
} // DrawList()

/*
 * DrawStrLines()
 */
static void DrawStrLines( Char* str, FontID fontID, IndexedColorType StrColor, UInt16 maxDrawLines, UInt16 strPixelWidth, Coord xPos, Coord yPos )
{
	MemHandle				memH = NULL;
	UInt16					numLines = 0;
	Char**          		linesP = NULL;
	UInt16					idx = 0;
	Char*					strP = NULL;
	UInt16					yPosInc = 0;
	UInt16					drawLines = 0;
	UInt16					strHeight = 0;
	UInt16					strWidth = 0;

	if ( ( !str ) || ( !StrLen( str ) ) ) return;
	
	FntSetFont( fontID );
	WinSetTextColor( StrColor );
	strHeight = FntLineHeight();
	
	strP = MemPtrNew( StrLen( str ) + 1 );
	MemSet( strP, StrLen( str ) + 1, 0 );
	StrCopy( strP, str );
			
	numLines = WrapNoteLines( strP, strPixelWidth, &memH );
	
	linesP = MemHandleLock( memH );

	drawLines = ( numLines > maxDrawLines ) ? maxDrawLines : numLines;
	
	for ( idx = 0 ; idx < drawLines ; idx++ )
	{
		WinDrawChars( *(linesP + idx), StrLen( *(linesP + idx) ), xPos, yPos + yPosInc );
	
		yPosInc += strHeight;
	}
	
	if ( numLines > maxDrawLines )
	{
		--idx;
		yPosInc -=strHeight;
		
		strWidth = FntLineWidth( *(linesP + idx), StrLen( *(linesP + idx) ) );	
		WinDrawChar( 0x0085, xPos + strWidth, yPos + yPosInc );	
	}
	
	if ( memH ) // unlock... // don't want problems
	{
        MemHandleUnlock( memH );
        MemHandleFree( memH );
        memH = NULL;
    }
    
    if ( strP ) MemPtrFree( strP );
    
    return;
    
} // DrawStrLines()

/*
 * WrapNoteLines()
 */
static UInt16 WrapNoteLines( Char* str, UInt16 linePixelWidth, MemHandle* memHP )
{
	UInt16		retVal = 0;
	Char*		startP = str;
	UInt16		endLen = 0;
	UInt16		strLen = StrLen( str );
	
 	if ( ( !str) || ( !strLen ) ) return ( retVal );
	
	while ( ( startP - str ) <= strLen )
	{
		endLen = FntWordWrap( startP, linePixelWidth );
		
		startP += endLen;
		
		if ( startP != ( str + strLen ) ) 
		{
			*(startP - 1) = chrNull;
		}
				
		++startP;
			
		++retVal;
	} 
	
	if ( memHP )
	{
		*memHP = SysFormPointerArrayToStrings( str, retVal );
	}
	
	return ( retVal );

} // WrapNoteLines()

/*
 * vibrate()
 */
void vibrate( Int32 duration )
{
 	UInt16 		count = 1;
	UInt16		state = kIndicatorForceOn;
	
	HsIndicatorState( count, kIndicatorTypeVibrator, &state );
	SysTaskDelay( duration );
	state = kIndicatorForceOff;
	HsIndicatorState( count, kIndicatorTypeVibrator, &state );
	
	return;	

} // vibrate

/*
 * SetCustomEffectAlarm()
 */
void SetCustomEffectAlarm( Boolean set, UInt8 nagRateIdx )
{
	UInt16 				cardNo = 0;
	LocalID				dbID = 0;
	UInt8				nagRateSecs[4] = { 0, 5, 10, 15 };
	
	if ( !SysCurAppDatabase( &cardNo, &dbID ) )
	{
		if ( ( set ) && ( nagRateIdx ) )
		{
			AlmSetAlarm( cardNo, dbID, 0, TimGetSeconds() + nagRateSecs[nagRateIdx], false );
		}
		else
		{
			AlmSetAlarm( cardNo, dbID, 0, 0, false );
		}
	}

	return;	

} // SetCustomEffectAlarm()

/*
 * AttnAlerts.c
 */