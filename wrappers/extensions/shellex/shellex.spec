2   stdcall SHChangeNotifyRegister(long long long long long ptr)
3   stdcall SHDefExtractIconA(str long long ptr ptr long)
4   stdcall SHChangeNotifyDeregister(long)
#5   stdcall -noname SHChangeNotifyUpdateEntryList(long long long long) #Windows Vista has only this follow to SHUNIMPL.DLL 
6   stdcall SHDefExtractIconW(wstr long long ptr ptr long)
#7   stdcall -noname SHLookupIconIndexA(str long long) #Windows Vista has only this follow to SHUNIMPL.DLL
#8   stdcall -noname SHLookupIconIndexW(wstr long long) #Windows Vista has only this follow to SHUNIMPL.DLL
9   stdcall PifMgr_OpenProperties(wstr wstr long long)
10  stdcall PifMgr_GetProperties(ptr wstr ptr long long)
11  stdcall PifMgr_SetProperties(ptr wstr ptr long long)
12  stdcall -noname SHStartNetConnectionDialogA(ptr str long)
13  stdcall PifMgr_CloseProperties(ptr long)
14  stdcall SHStartNetConnectionDialogW(ptr wstr long)
15  stdcall -noname ILGetDisplayName(ptr ptr)
16  stdcall ILFindLastID(ptr)
17  stdcall ILRemoveLastID(ptr)
18  stdcall ILClone(ptr)
19  stdcall ILCloneFirst(ptr)
20  stdcall -noname ILGlobalClone(ptr)
21  stdcall ILIsEqual(ptr ptr)
22  stdcall DAD_DragEnterEx2(ptr long long ptr)
23  stdcall ILIsParent(ptr ptr long)
24  stdcall ILFindChild(ptr ptr)
25  stdcall ILCombine(ptr ptr)
26  stdcall ILLoadFromStream(ptr ptr) shell32.ILLoadFromStream
27  stdcall ILSaveToStream(ptr ptr)
28  stdcall SHILCreateFromPath(ptr ptr ptr) 
29  stdcall -noname PathIsRoot(ptr) 
30  stdcall -noname PathBuildRoot(ptr long) 
31  stdcall -noname PathFindExtension(wstr) 
32  stdcall -noname PathAddBackslash(wstr) 
33  stdcall -noname PathRemoveBlanks(wstr) 
34  stdcall -noname PathFindFileName(wstr) 
35  stdcall -noname PathRemoveFileSpec(ptr) 
36  stdcall -noname PathAppend(ptr ptr) 
37  stdcall -noname PathCombine(wstr wstr wstr) 
38  stdcall -noname PathStripPath(wstr) 
39  stdcall -noname PathIsUNC(wstr) 
40  stdcall -noname PathIsRelative(wstr) 
41  stdcall IsLFNDriveA(str)
42  stdcall IsLFNDriveW(wstr)
43  stdcall PathIsExe(ptr) 
45  stdcall -noname PathFileExists(ptr) 
46  stdcall -noname PathMatchSpec(wstr wstr) 
47  stdcall PathMakeUniqueName(ptr long ptr ptr ptr) 
48  stdcall -noname PathSetDlgItemPath(long long wstr) 
49  stdcall PathQualify(ptr) 
50  stdcall -noname PathStripToRoot(wstr) 
51  stdcall PathResolve(str long long) 
52  stdcall -noname PathGetArgs(wstr) 
53  stdcall -noname IsSuspendAllowed() 
54  stdcall -noname LogoffWindowsDialog(ptr)
55  stdcall -noname PathQuoteSpaces(wstr) 
56  stdcall -noname PathUnquoteSpaces(wstr) 
57  stdcall -noname PathGetDriveNumber(wstr) 
58  stdcall -noname ParseField(str long ptr long) 
59  stdcall RestartDialog(long wstr long)
60  stdcall -noname ExitWindowsDialog(long) 
61  stdcall -noname RunFileDlg(long long long wstr wstr long) 
62  stdcall PickIconDlg(long long long long)
63  stdcall GetFileNameFromBrowse(long long long long wstr wstr wstr)
64  stdcall DriveType(long)
65  stdcall -noname InvalidateDriveType(long)
66  stdcall IsNetDrive(long)
67  stdcall Shell_MergeMenus(long long long long long long)
68  stdcall SHGetSetSettings(ptr long long)
69  stdcall -noname SHGetNetResource(ptr long ptr long)
70  stdcall -noname SHCreateDefClassObject(long long long long long)
71  stdcall Shell_GetImageLists(ptr ptr)
72  stdcall Shell_GetCachedImageIndex(ptr ptr long) 
73  stdcall SHShellFolderView_Message(long long long)
74  stdcall SHCreateStdEnumFmtEtc(long ptr ptr)
75  stdcall PathYetAnotherMakeUniqueName(ptr wstr wstr wstr)
76  stdcall -noname DragQueryInfo(ptr ptr)
77  stdcall SHMapPIDLToSystemImageListIndex(ptr ptr ptr)
#78  stdcall -noname OleStrToStrN(str long wstr long) #Windows Vista has only this follow to SHUNIMPL.DLL
#79  stdcall -noname StrToOleStrN(wstr long str long) #Windows Vista has only this follow to SHUNIMPL.DLL
80  stdcall SHOpenPropSheetW(wstr ptr long ptr ptr ptr wstr)
#82  stdcall -noname DDECreatePostNotify(ptr) #Windows Vista has only this follow to SHUNIMPL.DLL
83  stdcall CIDLData_CreateFromIDArray(ptr long ptr ptr)
#84  stdcall -noname SHIsBadInterfacePtr(ptr long) #Windows Vista has only this follow to SHUNIMPL.DLL
85  stdcall OpenRegStream(long str str long)
#86  stdcall -noname SHRegisterDragDrop(long ptr) #Windows Vista has only this follow to SHUNIMPL.DLL
#87  stdcall -noname SHRevokeDragDrop(long) #Windows Vista has only this follow to SHUNIMPL.DLL
88  stdcall SHDoDragDrop(long ptr ptr long ptr)
89  stdcall SHCloneSpecialIDList(long long long)
90  stdcall SHFindFiles(ptr ptr)
91  stdcall -noname SHFindComputer(ptr ptr)
92  stdcall PathGetShortPath(ptr) 
93  stdcall -noname Win32CreateDirectory(wstr ptr) 
94  stdcall -noname Win32RemoveDirectory(wstr) 
95  stdcall -noname SHLogILFromFSIL(ptr)
96  stdcall -noname StrRetToStrN(ptr long ptr ptr) 
97  stdcall -noname SHWaitForFileToOpen(long long long)
98  stdcall SHGetRealIDL(ptr ptr ptr)
99  stdcall -noname SetAppStartingCursor(long long)
100 stdcall SHRestricted(long)
102 stdcall SHCoCreateInstance(wstr ptr long ptr ptr)
103 stdcall SignalFileOpen(ptr)
119 stdcall IsLFNDrive(ptr) 
121 stdcall SHFlushClipboard() shell32.SHFlushClipboard
#122 stdcall -noname RunDll_CallEntry16(long long long str long) #Windows Vista has only this follow to SHUNIMPL.DLL
#123 stdcall -noname SHFreeUnusedLibraries() #Windows Vista has only this follow to SHUNIMPL.DLL
#126 stdcall -noname SHOutOfMemoryMessageBox(long long long) #Windows Vista has only this follow to SHUNIMPL.DLL # Fixme
127 stdcall -noname SHWinHelp(long long long long)
128 stdcall -noname SHDllGetClassObject(ptr ptr ptr)
129 stdcall DAD_AutoScroll(long ptr ptr)
#130 stdcall -noname DAD_DragEnter(long) #Windows Vista has only this follow to SHUNIMPL.DLL
131 stdcall DAD_DragEnterEx(long double)
132 stdcall DAD_DragLeave()
134 stdcall DAD_DragMove(double)
136 stdcall DAD_SetDragImage(long long)
137 stdcall DAD_ShowDragImage(long)
145 stdcall -noname PathFindOnPath(wstr wstr)
#146 stdcall -noname RLBuildListOfPaths() #Windows Vista has only this follow to SHUNIMPL.DLL
147 stdcall SHCLSIDFromString(long long) 
148 stdcall SHMapIDListToImageListIndexAsync(ptr ptr ptr long ptr ptr ptr ptr ptr) shell32.SHMapIDListToImageListIndexAsync
149 stdcall SHFind_InitMenuPopup(long long long long)
151 stdcall SHLoadOLE(long) shell32.SHLoadOLE
152 stdcall ILGetSize(ptr)
153 stdcall ILGetNext(ptr)
154 stdcall ILAppendID(long long long)
155 stdcall ILFree(ptr)
156 stdcall -noname ILGlobalFree(ptr)
157 stdcall ILCreateFromPath(ptr) 
158 stdcall -noname PathGetExtension(wstr long long) 
159 stdcall -noname PathIsDirectory(wstr) 
160 stdcall -noname SHNetConnectionDialog(ptr wstr long) 
161 stdcall SHRunControlPanel(long long) shell32.SHRunControlPanel
162 stdcall SHSimpleIDListFromPath(ptr) 
#163 stdcall -noname StrToOleStr(wstr str) #Windows Vista has only this follow to SHUNIMPL.DLL
164 stdcall Win32DeleteFile(wstr)
165 stdcall SHCreateDirectory(long ptr)
166 stdcall CallCPLEntry16(long long long long long long) shell32.CallCPLEntry16
167 stdcall SHAddFromPropSheetExtArray(long long long)
168 stdcall SHCreatePropSheetExtArray(long wstr long)
169 stdcall SHDestroyPropSheetExtArray(long)
170 stdcall SHReplaceFromPropSheetExtArray(long long long long)
171 stdcall PathCleanupSpec(ptr ptr)
172 stdcall -noname SHCreateLinks(long str ptr long ptr)
173 stdcall SHValidateUNC(long long long)
174 stdcall SHCreateShellFolderViewEx(ptr ptr)
175 stdcall -noname SHGetSpecialFolderPath(long long long long) 
176 stdcall SHSetInstanceExplorer(long)
#177 stdcall -noname DAD_SetDragImageFromListView(ptr long long) #Windows Vista has only this follow to SHUNIMPL.DLL
178 stdcall SHObjectProperties(long long wstr wstr)
179 stdcall SHGetNewLinkInfoA(str str ptr long long)
180 stdcall SHGetNewLinkInfoW(wstr wstr ptr long long)
181 stdcall -noname RegisterShellHook(long long)
182 varargs ShellMessageBoxW(long long wstr wstr long)
183 varargs ShellMessageBoxA(long long str str long)
184 stdcall -noname ArrangeWindows(long long long long long)
185 stdcall -noname SHHandleDiskFull(ptr long) # Fixme
186 stdcall -noname ILGetDisplayNameEx(ptr ptr ptr long)
187 stdcall -noname ILGetPseudoNameW(ptr ptr wstr long)
188 stdcall -noname ShellDDEInit(long)
189 stdcall ILCreateFromPathA(str)
190 stdcall ILCreateFromPathW(wstr)
191 stdcall SHUpdateImageA(str long long long)
192 stdcall SHUpdateImageW(wstr long long long)
193 stdcall SHHandleUpdateImage(ptr)
194 stdcall -noname SHCreatePropSheetExtArrayEx(long wstr long ptr)
195 stdcall SHFree(ptr)
196 stdcall SHAlloc(long)
#197 stdcall -noname SHGlobalDefect(long) #Windows Vista has only this follow to SHUNIMPL.DLL
#198 stdcall -noname SHAbortInvokeCommand() #Windows Vista has only this follow to SHUNIMPL.DLL
200 stdcall -noname SHCreateDesktop(ptr)
201 stdcall -noname SHDesktopMessageLoop(ptr)
#202 stdcall -noname DDEHandleViewFolderNotify(ptr ptr ptr) #Windows Vista has only this follow to SHUNIMPL.DLL
203 stdcall -noname AddCommasW(long wstr)
204 stdcall -noname ShortSizeFormatW(double)
205 stdcall -noname Printer_LoadIconsW(wstr ptr ptr)
209 stdcall -noname Int64ToString(double wstr long long ptr long) 
210 stdcall -noname LargeIntegerToString(ptr wstr long long ptr long) 
211 stdcall -noname Printers_GetPidl(ptr str) # Fixme
212 stdcall -noname Printers_AddPrinterPropPages(ptr ptr)
213 stdcall -noname Printers_RegisterWindowW(wstr long ptr ptr)
214 stdcall -noname Printers_UnregisterWindow(long long)
215 stdcall -noname SHStartNetConnectionDialog(long str long)
230 stdcall -noname FirstUserLogon(wstr wstr)
231 stdcall SHSetFolderPathA(long ptr long str)
232 stdcall SHSetFolderPathW(long ptr long wstr)
#233 stdcall -noname SHGetUserPicturePathW(wstr long ptr) #Windows Vista has only this follow to SHUNIMPL.DLL
#234 stdcall -noname SHSetUserPicturePathW(wstr long ptr) #Windows Vista has only this follow to SHUNIMPL.DLL
#235 stdcall -noname SHOpenEffectiveToken(ptr) #Windows Vista has only this follow to SHUNIMPL.DLL
236 stdcall -noname SHTestTokenPrivilegeW(ptr ptr)
237 stdcall -noname SHShouldShowWizards(ptr)
239 stdcall PathIsSlowW(wstr long)
240 stdcall PathIsSlowA(str long)
241 stdcall -noname SHGetUserDisplayName(wstr ptr)
242 stdcall -noname SHGetProcessDword(long long)
243 stdcall -noname SHSetShellWindowEx(ptr ptr) 
244 stdcall -noname SHSettingsChanged(ptr ptr)
245 stdcall SHTestTokenMembership(ptr ptr)
246 stdcall -noname SHInvokePrivilegedFunctionW(wstr ptr ptr)
247 stdcall -noname SHGetActiveConsoleSessionId()
248 stdcall -noname SHGetUserSessionId(ptr)
249 stdcall -noname PathParseIconLocation(wstr) 
250 stdcall -noname PathRemoveExtension(wstr) 
251 stdcall -noname PathRemoveArgs(wstr) 
252 stdcall -noname PathIsURL(wstr) 
253 stdcall -noname SHIsCurrentProcessConsoleSession()
254 stdcall -noname DisconnectWindowsDialog(ptr)
256 stdcall SHCreateShellFolderView(ptr ptr)
257 stdcall -noname SHGetShellFolderViewCB(ptr)
258 stdcall -noname LinkWindow_RegisterClass()
259 stdcall -noname LinkWindow_UnregisterClass()
520 stdcall SHAllocShared(ptr long long) shell32.SHAllocShared
521 stdcall SHLockShared(long long) shell32.SHLockShared
522 stdcall SHUnlockShared(ptr) shell32.SHUnlockShared
523 stdcall SHFreeShared(long long) shell32.SHFreeShared
524 stdcall RealDriveType(long long)
525 stdcall -noname RealDriveTypeFlags(long long)
526 stdcall SHFlushSFCache()
#640 stdcall -noname NTSHChangeNotifyRegister(long long long long long long) #Windows Vista has only this follow to SHUNIMPL.DLL
#641 stdcall -noname NTSHChangeNotifyDeregister(long) #Windows Vista has only this follow to SHUNIMPL.DLL
#643 stdcall -noname SHChangeNotifyReceive(long long ptr ptr) #Windows Vista has only this follow to SHUNIMPL.DLL
644 stdcall SHChangeNotification_Lock(long long ptr ptr)
645 stdcall SHChangeNotification_Unlock(long)
#646 stdcall -noname SHChangeRegistrationReceive(ptr long) #Windows Vista has only this follow to SHUNIMPL.DLL
648 stdcall -noname SHWaitOp_Operate(ptr long)
650 stdcall -noname PathIsSameRoot(ptr ptr) 
651 stdcall -noname OldReadCabinetState(long long) 
652 stdcall WriteCabinetState(long)
653 stdcall PathProcessCommand(long long long long) shell32.PathProcessCommand
654 stdcall ReadCabinetState(long long)
660 stdcall -noname FileIconInit(long)
680 stdcall IsUserAnAdmin()
681 stdcall -noname SHGetAppCompatFlags(long) shlwapi.SHGetAppCompatFlags
683 stdcall -noname SHStgOpenStorageW(wstr ptr long ptr long ptr)
684 stdcall -noname SHStgOpenStorageA(str ptr long ptr long ptr)
685 stdcall SHPropStgCreate(ptr ptr ptr long long long ptr ptr)
688 stdcall SHPropStgReadMultiple(ptr long long ptr ptr)
689 stdcall SHPropStgWriteMultiple(ptr ptr long ptr ptr long)
690 stdcall -noname SHIsLegacyAnsiProperty(ptr long long)
#691 stdcall -noname SHFileSysBindToStorage(wstr long long long long long) #Windows Vista has only this follow to SHUNIMPL.DLL
700 stdcall CDefFolderMenu_Create(ptr ptr long ptr ptr ptr ptr ptr ptr)
701 stdcall CDefFolderMenu_Create2(ptr ptr long ptr ptr ptr long ptr ptr)
702 stdcall -noname CDefFolderMenu_MergeMenu(ptr long long ptr)
703 stdcall -noname GUIDFromStringA(str ptr)
704 stdcall -noname GUIDFromStringW(wstr ptr)
#707 stdcall -noname SHOpenPropSheetA(str ptr long ptr ptr ptr str) #Windows Vista has only this follow to SHUNIMPL.DLL
#708 stdcall -noname SHGetSetFolderCustomSettingsA(ptr str long) #Windows Vista has only this follow to SHUNIMPL.DLL
709 stdcall SHGetSetFolderCustomSettingsW(ptr wstr long) shell32.SHGetSetFolderCustomSettingsW
711 stdcall -noname CheckWinIniForAssocs()
#712 stdcall -noname SHCopyMonikerToTemp(ptr wstr wstr long) #Windows Vista has only this follow to SHUNIMPL.DLL
713 stdcall -noname PathIsTemporaryA(str)
714 stdcall -noname PathIsTemporaryW(wstr)
#715 stdcall -noname SHCreatePropertyBag(ptr ptr) #Windows Vista has only this follow to SHUNIMPL.DLL
716 stdcall SHMultiFileProperties(ptr long)
719 stdcall -noname SHParseDarwinIDFromCacheW(wstr wstr)
#720 stdcall -noname MakeShellURLFromPathA(str str long) #Windows Vista has only this follow to SHUNIMPL.DLL
#721 stdcall -noname MakeShellURLFromPathW(wstr wstr long) #Windows Vista has only this follow to SHUNIMPL.DLL
722 stdcall -noname SHCreateInstance(long long long)
723 stdcall -noname SHCreateSessionKey(long ptr)
724 stdcall -noname SHIsTempDisplayMode()
#725 stdcall -noname GetFileDescriptor(ptr long long wstr) #Windows Vista has only this follow to SHUNIMPL.DLL
#726 stdcall -noname CopyStreamUI(ptr ptr ptr) #Windows Vista has only this follow to SHUNIMPL.DLL
730 stdcall RestartDialogEx(long wstr long long)
731 stdcall -noname SHRegisterDarwinLink(ptr wstr long)
732 stdcall -noname SHReValidateDarwinCache()
733 stdcall -noname CheckDiskSpace()
740 stdcall -noname SHCreateFileDataObject(ptr long long long long)
743 stdcall SHCreateFileExtractIconW(wstr long ptr ptr)
744 stdcall -noname Create_IEnumUICommand(ptr ptr long ptr)
745 stdcall -noname Create_IUIElement(long long)
747 stdcall SHLimitInputEdit(ptr ptr)
748 stdcall -noname SHLimitInputCombo(ptr ptr)
749 stdcall SHGetShellStyleHInstance() shell32.SHGetShellStyleHInstance
750 stdcall SHGetAttributesFromDataObject(ptr long ptr ptr)
751 stdcall -noname SHSimulateDropOnClsid(ptr ptr long)
752 stdcall -noname SHGetComputerDisplayNameW(ptr long wstr long)
753 stdcall -noname CheckStagingArea()
754 stdcall -noname SHLimitInputEditWithFlags(ptr long)
755 stdcall -noname PathIsEqualOrSubFolder(wstr wstr)
#756 stdcall -noname DeleteFileThumbnail(ptr) #Windows Vista has only this follow to SHUNIMPL.DLL

@ stdcall Activate_RunDLL(long ptr ptr ptr) shell32.Activate_RunDLL
@ stdcall AppCompat_RunDLLW(ptr ptr wstr long)
@ stdcall CheckEscapesA(str long) shell32.CheckEscapesA
@ stdcall CheckEscapesW(wstr long)
@ stdcall Control_FillCache_RunDLL(long long long long) shell32.Control_FillCache_RunDLL
@ stdcall Control_FillCache_RunDLLA(long long long long) shell32.Control_FillCache_RunDLLA
@ stdcall Control_FillCache_RunDLLW(long long long long) shell32.Control_FillCache_RunDLLW
@ stdcall Control_RunDLL(ptr ptr str long) Control_RunDLLA
@ stdcall Control_RunDLLA(ptr ptr str long)
@ stdcall Control_RunDLLAsUserW(ptr ptr wstr long)
@ stdcall Control_RunDLLW(ptr ptr wstr long)
@ stdcall -private DllCanUnloadNow()
@ stdcall -private DllGetVersion(ptr)
#@ stdcall -private DllInstall(long wstr)
@ stdcall -private DllRegisterServer()
@ stdcall -private DllUnregisterServer()
@ stdcall DoEnvironmentSubstA(str str)
@ stdcall DoEnvironmentSubstW(wstr wstr)
@ stdcall DragAcceptFiles(long long)
@ stdcall DragFinish(long)
@ stdcall DragQueryFile(long long ptr long) 
@ stdcall DragQueryFileA(long long ptr long)
@ stdcall DragQueryFileAorW(ptr long wstr long long long)
@ stdcall DragQueryFileW(long long ptr long)
@ stdcall DragQueryPoint(long ptr)
@ stdcall DuplicateIcon(long long)
@ stdcall ExtractAssociatedIconA(long str ptr)
@ stdcall ExtractAssociatedIconExA(long str long long)
@ stdcall ExtractAssociatedIconExW(long wstr long long)
@ stdcall ExtractAssociatedIconW(long wstr ptr)
@ stdcall ExtractIconA(long str long)
@ stdcall ExtractIconEx(ptr long ptr ptr long)
@ stdcall ExtractIconExA(str long ptr ptr long)
@ stdcall ExtractIconExW(wstr long ptr ptr long)
@ stdcall ExtractIconResInfoA(ptr str long ptr ptr) shell32.ExtractIconResInfoA
@ stdcall ExtractIconResInfoW(ptr wstr long ptr ptr) shell32.ExtractIconResInfoW
@ stdcall ExtractIconW(long wstr long)
@ stdcall ExtractVersionResource16W(wstr ptr) shell32.ExtractVersionResource16W
@ stdcall FindExecutableA(str str ptr)
@ stdcall FindExecutableW(wstr wstr ptr)
@ stdcall FindExeDlgProc(ptr long ptr ptr) shell32.FindExeDlgProc
@ stdcall FreeIconList(long)
@ stdcall InternalExtractIconListA(ptr str ptr)
@ stdcall InternalExtractIconListW(ptr wstr ptr)
@ stdcall OpenAs_RunDLL(long long str long)
@ stdcall OpenAs_RunDLLA(long long str long)
@ stdcall OpenAs_RunDLLW(long long wstr long)
@ stdcall Options_RunDLL(ptr ptr str long)
@ stdcall Options_RunDLLA(ptr ptr str long)
@ stdcall Options_RunDLLW(ptr ptr wstr long)
@ stdcall PrintersGetCommand_RunDLL(ptr ptr wstr long)
@ stdcall PrintersGetCommand_RunDLLA(ptr ptr str long)
@ stdcall PrintersGetCommand_RunDLLW(ptr ptr wstr long)
@ stdcall RealShellExecuteA(ptr str str str str str str str long ptr)
@ stdcall RealShellExecuteExA(ptr str str str str str str str long ptr long)
@ stdcall RealShellExecuteExW(ptr str str str str str str str long ptr long)
@ stdcall RealShellExecuteW(ptr wstr wstr wstr wstr wstr wstr wstr long ptr)
@ stdcall RegenerateUserEnvironment(ptr long)
@ stdcall SHAddToRecentDocs(long ptr)
@ stdcall SHAppBarMessage(long ptr)
@ stdcall SHBindToParent(ptr ptr ptr ptr)
@ stdcall SHBrowseForFolder(ptr) 
@ stdcall SHBrowseForFolderA(ptr)
@ stdcall SHBrowseForFolderW(ptr)
@ stdcall SHChangeNotify(long long ptr ptr)
@ stdcall SHChangeNotifySuspendResume(long ptr long long)
@ stdcall SHCreateDirectoryExA(long str ptr)
@ stdcall SHCreateDirectoryExW(long wstr ptr)
@ stdcall SHCreateLocalServerRunDll(long long str long)
@ stdcall SHCreateProcessAsUserW(ptr)
@ stdcall SHCreateQueryCancelAutoPlayMoniker(ptr)
@ stdcall SHCreateShellItem(ptr ptr ptr ptr)
@ stdcall SHEmptyRecycleBinA(long str long)
@ stdcall SHEmptyRecycleBinW(long wstr long)
@ stdcall SHEnableServiceObject(ptr long)
@ stdcall SHEnumerateUnreadMailAccountsW(ptr long wstr long)
@ stdcall SHExtractIconsW(wstr long long long ptr ptr long long)
@ stdcall SHFileOperation(ptr) 
@ stdcall SHFileOperationA(ptr)
@ stdcall SHFileOperationW(ptr)
@ stdcall SHFormatDrive(long long long long)
@ stdcall SHFreeNameMappings(ptr)
@ stdcall SHGetDataFromIDListA(ptr ptr long ptr long)
@ stdcall SHGetDataFromIDListW(ptr ptr long ptr long)
@ stdcall SHGetDesktopFolder(ptr)
@ stdcall SHGetDiskFreeSpaceA(str ptr ptr ptr) 
@ stdcall SHGetDiskFreeSpaceExA(str ptr ptr ptr) 
@ stdcall SHGetDiskFreeSpaceExW(wstr ptr ptr ptr)
@ stdcall SHGetFileInfo(ptr long ptr long long) 
@ stdcall SHGetFileInfoA(ptr long ptr long long)
@ stdcall SHGetFileInfoW(ptr long ptr long long)
@ stdcall SHGetFolderLocation(long long long long ptr)
@ stdcall SHGetFolderPathA(long long long long ptr)
@ stdcall SHGetFolderPathAndSubDirA(long long long long str ptr)
@ stdcall SHGetFolderPathAndSubDirW(long long long long wstr ptr)
@ stdcall SHGetFolderPathW(long long long long ptr)
@ stdcall SHGetIconOverlayIndexA(str long)
@ stdcall SHGetIconOverlayIndexW(wstr long)
@ stdcall SHGetInstanceExplorer(long)
@ stdcall SHGetMalloc(ptr)
@ stdcall SHGetNewLinkInfo(str str ptr long long) 
@ stdcall SHGetPathFromIDList(ptr ptr) 
@ stdcall SHGetPathFromIDListA(ptr ptr)
@ stdcall SHGetPathFromIDListW(ptr ptr)
@ stdcall SHGetSettings(ptr long)
@ stdcall SHGetSpecialFolderLocation(long long ptr)
@ stdcall SHGetSpecialFolderPathA(long ptr long long)
@ stdcall SHGetSpecialFolderPathW(long ptr long long)
@ stdcall SHGetUnreadMailCountW (long wstr long ptr wstr long)
@ stdcall SHHelpShortcuts_RunDLL(long long long long) 
@ stdcall SHHelpShortcuts_RunDLLA(long long long long)
@ stdcall SHHelpShortcuts_RunDLLW(long long long long)
@ stdcall SHInvokePrinterCommandA(ptr long str str long)
@ stdcall SHInvokePrinterCommandW(ptr long wstr wstr long)
@ stdcall SHIsFileAvailableOffline(wstr ptr)
@ stdcall SHLoadInProc(long)
@ stdcall SHLoadNonloadedIconOverlayIdentifiers()
@ stdcall SHOpenFolderAndSelectItems(ptr long ptr long)
@ stdcall SHParseDisplayName(wstr ptr ptr long ptr)
@ stdcall SHPathPrepareForWriteA(long ptr str long)
@ stdcall SHPathPrepareForWriteW(long ptr wstr long)
@ stdcall SHQueryRecycleBinA(str ptr)
@ stdcall SHQueryRecycleBinW(wstr ptr)
@ stdcall SHSetLocalizedName(wstr wstr long)
@ stdcall SHSetUnreadMailCountW (wstr long wstr)
@ stdcall SHUpdateRecycleBinIcon()
@ stdcall SheChangeDirA(str)
@ stdcall SheChangeDirExA(str) shell32.SheChangeDirExA
@ stdcall SheChangeDirExW(wstr)
@ stdcall SheChangeDirW(wstr) shell32.SheChangeDirW
@ stdcall SheConvertPathW(wstr wstr long) shell32.SheConvertPathW
@ stdcall SheFullPathA(str long str) shell32.SheFullPathA
@ stdcall SheFullPathW(wstr long wstr) shell32.SheFullPathW
@ stdcall SheGetCurDrive() shell32.SheGetCurDrive
@ stdcall SheGetDirA(long long)
@ stdcall SheGetDirExW(wstr ptr wstr) shell32.SheGetDirExW
@ stdcall SheGetDirW(long long) shell32.SheGetDirW
@ stdcall SheGetPathOffsetW(wstr) shell32.SheGetPathOffsetW
@ stdcall SheRemoveQuotesA(str) shell32.SheRemoveQuotesA
@ stdcall SheRemoveQuotesW(wstr) shell32.SheRemoveQuotesW
@ stdcall SheSetCurDrive(long)
@ stdcall SheShortenPathA(str long) shell32.SheShortenPathA
@ stdcall SheShortenPathW(wstr long) shell32.SheShortenPathW
@ stdcall ShellAboutA(long str str long)
@ stdcall ShellAboutW(long wstr wstr long)
@ stdcall ShellExec_RunDLL(ptr ptr wstr long)
@ stdcall ShellExec_RunDLLA(ptr ptr str long)
@ stdcall ShellExec_RunDLLW(ptr ptr wstr long)
@ stdcall ShellHookProc(long ptr ptr)
@ stdcall StrChrA(str long) 
@ stdcall StrChrIA(str long) 
@ stdcall StrChrIW(wstr long) 
@ stdcall StrChrW(wstr long) 
@ stdcall StrCmpNA(str str long) 
@ stdcall StrCmpNIA(str str long) 
@ stdcall StrCmpNIW(wstr wstr long) 
@ stdcall StrCmpNW(wstr wstr long) 
@ stdcall StrCpyNA (ptr str long) shell32.StrCpyNA
@ stdcall StrCpyNW(wstr wstr long) shell32.StrCpyNW
@ stdcall StrNCmpA(str str long) 
@ stdcall StrNCmpIA(str str long) 
@ stdcall StrNCmpIW(wstr wstr long) 
@ stdcall StrNCmpW(wstr wstr long)
@ stdcall StrNCpyA (ptr str long) shell32.StrNCpyA
@ stdcall StrNCpyW(wstr wstr long) shell32.StrNCpyW
@ stdcall StrRChrA(str str long) 
@ stdcall StrRChrIA(str str long) 
@ stdcall StrRChrIW(wstr wstr long) 
@ stdcall StrRChrW(wstr wstr long) 
@ stdcall StrRStrA(str str str)
@ stdcall StrRStrIA(str str str) 
@ stdcall StrRStrIW(wstr wstr wstr) 
@ stdcall StrRStrW(wstr wstr wstr)
@ stdcall StrStrA(str str) 
@ stdcall StrStrIA(str str)
@ stdcall StrStrIW(wstr wstr)
@ stdcall StrStrW(wstr wstr) 
@ stdcall WOWShellExecute(ptr str str str str long ptr)

#Missing on Windows XP SP1
727 stdcall -noname SHGetImageList(long ptr ptr)

#Functions exported by Longhorn shell32.dll
@ stdcall SHInitializeNamespace() ;version 3718
@ stdcall SHUninitializeNamespace() ;version 3718
@ stdcall SHEvaluateSystemCommandTemplate(wstr wstr wstr wstr)  ;Redirected to Longhorn shell
@ stdcall SHFormatForDisplay(ptr ptr long wstr long) 
@ stdcall WaitForExplorerRestartW(long long wstr long)
757 stdcall DisplayNameOfW(ptr ptr long ) ;Redirected to Longhorn shell
758 stdcall SHCreateThreadUndoManager(ptr ptr) ;Redirected to Longhorn shell
759 stdcall SHGetThreadUndoManager(ptr ptr) ;Redirected to Longhorn shell
761 stdcall SHChangeNotifyDeregisterWindow(ptr) ;Redirected to Longhorn shell
787 stdcall SHMapIDListToSystemImageListIndexAsync(long ptr ptr ptr ptr ptr long ptr)
810 stdcall SHGetUserPicturePathEx(wstr long wstr wstr long wstr long)
813 stdcall SHUserSetPasswordHint(wstr wstr)
846 stdcall ILLoadFromStreamEx(ptr ptr) ;Redirected to Longhorn shell
850 stdcall PathComparePaths(wstr wstr) ;Redirected to Longhorn shell
859 stdcall WPC_InstallState(ptr)
864 stdcall StampIconForElevation(ptr long long)
865 stdcall IsElevationRequired(wstr)
881 stdcall SHEnumClassesOfCategories(long long long long long)
882 stdcall SHWriteClassesOfCategories(long long long long long long long)

# Functions exported by the WinVista shell32.dll
@ stdcall Shell_GetCachedImageIndexA(ptr ptr long)  ;Redirected to Longhorn shell
@ stdcall Shell_GetCachedImageIndexW(ptr ptr long) Shell_GetCachedImageIndex
@ stdcall SHBindToObject(ptr ptr ptr ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHCreateItemFromIDList(ptr ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHCreateItemFromParsingName(wstr ptr ptr ptr)  ;Redirected to Longhorn shell
@ stdcall SHCreateItemFromRelativeName(ptr wstr ptr long ptr) ;Redirected to Longhorn shell
@ stdcall SHCreateItemWithParent(ptr ptr ptr long ptr) ;Redirected to Longhorn shell
@ stdcall SHCreateShellItemArray(ptr ptr long ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHCreateShellItemArrayFromDataObject(ptr ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHCreateShellItemArrayFromShellItem(ptr ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHCreateShellItemArrayFromIDLists(long ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHGetKnownFolderIDList(ptr long ptr ptr)
@ stdcall SHGetKnownFolderPath(ptr long ptr ptr)
@ stdcall AssocCreateForClasses(ptr long long ptr) ;Redirected to Longhorn shell
@ stdcall AssocGetDetailsOfPropKey(ptr ptr ptr ptr ptr) ;Redirected to Longhorn shell
@ stdcall InitNetworkAddressControl() ;Redirected to Longhorn shell
@ stdcall SHCreateDefaultContextMenu(ptr ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHCreateDefaultExtractIcon(ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHOpenWithDialog(ptr wstr)
@ stdcall SHCreateDataObject(ptr long ptr ptr ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHBindToFolderIDListParent(ptr ptr ptr ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHBindToFolderIDListParentEx(ptr ptr ptr long ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHGetFolderPathEx(ptr long ptr ptr long)
@ stdcall SHGetIDListFromObject(ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHSetTemporaryPropertyForItem(ptr ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHGetLocalizedName(wstr wstr long ptr) ;Redirected to Longhorn shell
@ stdcall SHCreateItemInKnownFolder(ptr long wstr ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHGetPathFromIDListEx(ptr wstr long long) ;Redirected to Longhorn shell
@ stdcall SHSetKnownFolderPath(long long ptr wstr)
@ stdcall SHGetTemporaryPropertyForItem(ptr ptr ptr) ;Redirected to Longhorn shell
@ stdcall SHGetPropertyStoreFromIDList(ptr long long ptr) ;Redirected to Longhorn shell
@ stdcall SHRemoveLocalizedName(wstr) ;Redirected to Longhorn shell
@ stdcall SHQueryUserNotificationState(ptr) ;Redirected to Longhorn shell
@ stdcall SHGetDriveMedia(wstr ptr) ;Redirected to Longhorn shell
@ stdcall SHGetNameFromIDList(ptr long ptr) ;Redirected to Longhorn shell
@ stdcall SHCreateAssociationRegistration(long ptr) ;Redirected to Longhorn shell
@ stdcall SHChangeNotifyRegisterThread(long) ;Redirected to Longhorn shell
@ stdcall SHAddDefaultPropertiesByExt(wstr ptr) ;Redirected to Longhorn shell
@ stdcall SHGetPropertyStoreFromParsingName(wstr ptr long long ptr) ;Redirected to Longhorn shell
@ stdcall SHGetStockIconInfo(long long ptr)
@ stdcall SHAssocEnumHandlers(wstr long ptr) ;Redirected to Longhorn shell
@ stdcall SHSetDefaultProperties(ptr ptr long ptr) ;Redirected to Longhorn shell
@ stdcall SHCreateItemFromFolderID(ptr ptr ptr) SHCreateItemFromIDList;Redirected to Longhorn shell

#Hooks
@ stdcall CommandLineToArgvW(wstr ptr) CommandLineToArgvWInternal
@ stdcall DllGetClassObject(ptr ptr ptr) DllGetClassObjectInternal
@ stdcall ShellExecuteA(long str str str str long) ;ShellExecuteAInternal
@ stdcall ShellExecuteEx(ptr) ShellExecuteExA #ShellExecuteExAInternal
@ stdcall ShellExecuteExA(ptr) #ShellExecuteExAInternal
@ stdcall ShellExecuteExW(ptr)
@ stdcall ShellExecuteW(long wstr wstr wstr wstr long) ;ShellExecuteWInternal
@ stdcall Shell_NotifyIcon(long ptr) ;Shell_NotifyIconAInternal ;Redirected to Longhorn shell
@ stdcall Shell_NotifyIconA(long ptr) ;Shell_NotifyIconAInternal ;Redirected to Longhorn shell
@ stdcall Shell_NotifyIconW(long ptr) ;Shell_NotifyIconWInternal ;Redirected to Longhorn shell

#Win7
@ stdcall GetCurrentProcessExplicitAppUserModelID(ptr)
@ stdcall Shell_NotifyIconGetRect(ptr ptr)
@ stdcall SetCurrentProcessExplicitAppUserModelID(wstr)
@ stdcall SHGetKnownFolderItem(ptr long long ptr ptr)
@ stdcall SHGetItemFromObject(ptr long ptr)
@ stdcall SHGetItemFromDataObject(ptr long ptr ptr)
@ stdcall SHGetPropertyStoreForWindow(ptr long ptr)
@ stdcall SHAssocEnumHandlersForProtocolByApplication(wstr long ptr)
682 stdcall -noname StgMakeUniqueName(ptr wstr long long ptr)