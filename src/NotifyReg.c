/*
 * NotifyReg.c
 */
 
#include "NotifyReg.h"

/*
 * RegisterForPhnNotifications()
 */
Err RegisterForPhnNotifications( Boolean bRegister )
{
	Err					error = errNone;
	UInt16 				PhoneLibRefNum = sysInvalidRefNum;

	if ( ( error = HsGetPhoneLibrary( &PhoneLibRefNum ) ) != errNone )
		return ( error );
		
	if ( ( error = PhnLibOpen( PhoneLibRefNum ) ) != errNone )
		return ( error );
		
	if ( bRegister )
	{
		error = PhnLibRegister( PhoneLibRefNum, appFileCreator, phnServiceVoice );
	}
	else
	{
		error = PhnLibRegister( PhoneLibRefNum, appFileCreator, 0 );
	}
			
	PhnLibClose( PhoneLibRefNum );

	return ( error );
		
} // RegisterForPhnNotifications()

/*
 * RegisterForChNotifications()
 */
Err RegisterForChNotifications( Boolean bRegister )
{
	Err					error = errNone;
	UInt16 				cardNo = 0; 
	LocalID  			dbID = 0;	
	
	if ( ( error = SysCurAppDatabase( &cardNo, &dbID ) ) != errNone ) 
		return ( error );
	
	if ( bRegister )
	{
		error = SysNotifyRegister( cardNo, dbID, sysNotifyVirtualCharHandlingEvent, NULL, sysNotifyNormalPriority, NULL );
		// error = SysNotifyRegister( cardNo, dbID, chDeferredNotifyEvent, NULL, sysNotifyNormalPriority, NULL );
	}
	else
	{
		error = SysNotifyUnregister( cardNo, dbID, sysNotifyVirtualCharHandlingEvent, sysNotifyNormalPriority );
		// error = SysNotifyUnregister( cardNo, dbID, chDeferredNotifyEvent, sysNotifyNormalPriority );
	}
	
	return ( error );
	
} // RegisterForChNotifications()

/*
 * NotifyReg.c
 */