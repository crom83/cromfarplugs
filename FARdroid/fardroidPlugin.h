BOOL	APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);
void	WINAPI GetGlobalInfoW(struct GlobalInfo *Info);
void	WINAPI  SetStartupInfoW(const struct PluginStartupInfo *Info);
void	WINAPI  GetPluginInfoW(struct PluginInfo *Info);
HANDLE	WINAPI OpenW(const struct OpenInfo *Info);
void	WINAPI ClosePanelW(const struct ClosePanelInfo *Info);
intptr_t WINAPI ConfigDlgProc(HANDLE hDlg, intptr_t Msg, intptr_t Param1, void *Param2);
intptr_t WINAPI ConfigureW(const struct ConfigureInfo *Info);
void	WINAPI GetOpenPanelInfoW(struct OpenPanelInfo *Info);
intptr_t WINAPI GetFindDataW(struct GetFindDataInfo *Info);
void	WINAPI FreeFindDataW(const struct FreeFindDataInfo *Info);
intptr_t WINAPI ProcessPanelInputW(const struct ProcessPanelInputInfo *Info);
intptr_t WINAPI ProcessPanelEventW(const struct ProcessPanelEventInfo *Info);
intptr_t WINAPI GetFilesW(struct GetFilesInfo *Info);
intptr_t WINAPI SetDirectoryW(const struct SetDirectoryInfo *Info);
intptr_t WINAPI PutFilesW(const struct PutFilesInfo *Info);
intptr_t WINAPI DeleteFilesW(const struct DeleteFilesInfo *Info);
intptr_t WINAPI MakeDirectoryW(struct MakeDirectoryInfo *Info);


