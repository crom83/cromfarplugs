#ifndef lang_hpp
#define lang_hpp

enum
{
  MTitle,

	MOk,
  MCancel,
	MYes,
	MNo,
	MAlwaysYes,
	MAlwaysNo,
	MRetry,

  MConfTitle,
	MConfAddToDisk,
	MConfAddToDiskLabel,
  MConfPrefix,
	MConfSafeMode,
	MConfNative,
	MConfBusybox,
	MConfShowLinksAsDirs,
	MConfShowAllFilesystems,
	MConfUseSU,
	MConfUseExtendedAccess,
	MConfRemountSystem,
	MConfTimeout,
	MConfADBPath,

	MError,
	MDeviceNotFound,

	MADBExecError,

	MProcessed,
	MFrom,
	MTo,

	MGetFile,
	MPutFile,
	MDelFile,
	MCreateDir,

	MBreakWarn,

	MDirName,
	MCopyTitle,
	MCopyDest,
	MCopyDestName,
	MCopyWarnIfExists,
	MCopyError,
	MDeleteTitle,
	MDeleteWarn,
	MCopyDeleteError,

	MMemoryInfo,
	MPartitionsInfo,

	MWarningTitle,
	MOnlyNative,
	MSetPermFail,
	MPermTitle,
	MPermFileName,
	MPermFileAttr,

	MNeedFolderExePerm,
	MNeedSuperuserPerm, 
	MNeedNativeSuperuserPerm,
};

#endif