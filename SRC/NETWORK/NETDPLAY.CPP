/*=============================================================================
	NetDPlay.cpp: Unreal Windows DirectPlay networking

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

#pragma warning (disable : 4201) /* nonstandard extension used : nameless struct/union */
#include <windows.h>
#include <windowsx.h>
#include "DPlay.h"

#include "Net.h"
#include "NetPrv.h"
#include "NetDPlay.h"
#include "Resource.h"

/*------------------------------------------------------------------------------
	Globals
------------------------------------------------------------------------------*/

//
// Unreal's DirectPlay GUID:
// {A05B9163-9EF7-11cf-94FD-0000C028B992}
//
static const GUID GUnrealDirectPlayGUID = 
{ 0xa05b9163, 0x9ef7, 0x11cf, { 0x94, 0xfd, 0x0, 0x0, 0xc0, 0x28, 0xb9, 0x92 } };

/*------------------------------------------------------------------------------
	NDirectPlayDriver Implementation
------------------------------------------------------------------------------*/

//
// Initialize the driver, parsing any non-default parameters from ParamBuffer.
// If successful, returns 1.  If failure, returns 0 and sets Error to
// an appropriate description.
//
int NDirectPlayDriver::Init(char *ParamBuffer,char *Error)
	{
	guard(NDirectPlayDriver::Init);
	debug(LOG_Init,"DirectPlay Init");
	//
	// Try to initialize generic properties:
	//
	if (!NDriver::Init(ParamBuffer,Error)) return 0;
	DriverDescription = "DirectPlay";
	//
	// Attempt to load DPlay.dll:
	//
	HMODULE hModule = LoadLibrary("DPlay.dll");
	strcpy(Error,"Can't find DPlay.dll");
	if (!hModule) return 0;
	//
	// Find address of DirectPlayEnumerate function:
	//
	DirectPlayEnumerate = (pDirectPlayEnumerate)GetProcAddress(hModule,"DirectPlayEnumerate");
	strcpy(Error,"Can't find DirectPlayEnumerate");
	if (!DirectPlayEnumerate) return 0; // Not present
	//
	// Find address of DirectPlayCreate function:
	//
	DirectPlayCreate = (pDirectPlayCreate)GetProcAddress(hModule,"DirectPlayCreate");
	strcpy(Error,"Can't find DirectPlayCreate");
	if (!DirectPlayCreate) return 0; // Not present
	//
	// Set DirectPlay interface pointer to NULL, since a particular DirectPlay
	// service provider hasn't been selected.
	//
	DirectPlay = NULL;
	NetManager.RegisterDriver(this);
	//
	return 1; // Success
	//
	unguard;
	};

//
// Shut down the driver, closing any connections that may be open.
//
void NDirectPlayDriver::Exit()
	{
	guard(NDirectPlayDriver::Exit);
	debug(LOG_Exit,"DirectPlay Exit");
	AssertInitialized();
	//
	// Shut down generic info, and any remaining sockets:
	//
	NDriver::Exit();
	//
	// Shut down DirectPlay driver, if it exists:
	//
	if (DirectPlay)
		{
		debugf(LOG_Exit,"Releasing DirectPlay driver");
		DirectPlay->Release();
		DirectPlay = NULL;
		};
	unguard;
	};

//
// Decide whether this driver can handle the specified full URL, based
// on the URL's contents.  Returns 1 if this driver can handle it, 0 if not.
//
int NDirectPlayDriver::CanHandleURL(char *ServerURL)
	{
	guard(NDirectPlayDriver::CanHandleURL);
	AssertInitialized();
	//
	// DirectPlay doesn't have the intelligence to handle URL's now.
	// Could later expand for such things are remembering phone numbers for dialing.
	//
	return 0;
	//
	unguard;
	};

//
// See if there's a connection waiting to come in for the server
// identified by ServerID.  If so, creates a new server socket associated
// with the connection and returns it.  If no connections are waiting to
// come in, returns NULL.
//
NSocket *NDirectPlayDriver::ServerAcceptConnection(int ServerID)
	{
	guard(NDirectPlayDriver::ServerAcceptConnection);
	AssertInitialized();
	//
	return NULL;
	//
	unguard;
	};

//
// Try to open a client connection based on a URL.  Returns the socket if successful.
// If failed, returns NULL and sets Error to an appropriate error description.
//
NSocket *NDirectPlayDriver::ClientOpenServer(char *ServerURL,char *Error)
	{
	guard(NDirectPlayDriver::ClientOpenServer);
	AssertInitialized();
	//
	// DirectPlay can only be opened by ClientOpenServerByUI.
	//
	strcpy(Error,"ClientOpenServer not supported by DirectPlay");
	return NULL;
	//
	unguard;
	};

//
// Make a particular server available on this driver.
//
void NDirectPlayDriver::BeginAdvertising(NServerAd *Ad)
	{
	guard(NDirectPlayDriver::BeginAdvertising);
	AssertInitialized();
	//
	// Does nothing, by design.
	//
	unguard;
	};

//
// Retract a particular server on this driver.
//
void NDirectPlayDriver::EndAdvertising(NServerAd *Ad)
	{
	guard(NDirectPlayDriver::EndAdvertising);
	AssertInitialized();
	//
	// Does nothing, by design.
	//
	unguard;
	};

//
// Command line.
//
int NDirectPlayDriver::Exec(const char *Cmd,FOutputDevice *Out)
	{
	guard(NDirectPlayDriver::Exec);
	const char *Str = Cmd;
	//
	if (NetGetCMD(&Str,"DPLAY"))
		{
		return 0;
		}
	else return 0;
	//
	unguard;
	};

//
// DirectPlay tick.
//
void NDirectPlayDriver::Tick()
	{
	guard(NDirectPlayDriver::Tick);
	//
	unguard;
	};

/*------------------------------------------------------------------------------
	NDirectPlaySocket implementation
------------------------------------------------------------------------------*/

//
// See if a packet is waiting to come in on this socket.
// If there is, returns 1, sets PacketToGet to the contents
// of the packet, and removes the packet from the incoming
// queue.  If no packets are waiting, returns 0.
//
int NDirectPlaySocket::GetPacket(NPacket *PacketToGet)
	{
	guard(NDirectPlaySocket::GetPacket);
	AssertInitialized();
	//
	return 0;
	//
	unguard;
	};

//
// Send a packet on this socket.
//
void NDirectPlaySocket::SendPacket(NPacket *PacketToSend)
	{
	guard(NDirectPlaySocket::SendPacket);
	AssertInitialized();
	//
	//
	unguard;
	};

//
// Find the maximum allowable packet size that can be sent on 
// this socket, depending on the type of driver that owns
// the socket.  The result is always guaranteed to be less
// than or equal to the global maximum, MAX_PACKET_SIZE.
//
//
int NDirectPlaySocket::MaxPacketSize()
	{
	guard(NDirectPlaySocket::MaxPacketSize);
	AssertInitialized();
	//
	return MAX_PACKET_SIZE;
	//
	unguard;
	};

//
// Process a timer tick for this socket:
//
void NDirectPlaySocket::Tick()
	{
	guard(NDirectPlaySocket::Tick);
	AssertInitialized();
	//
	// Call parent tick routine to handle standard work (file transfer, timeouts, etc).
	// 
	NSocket::Tick();
	//
	unguard;
	};

//
// Initialize this socket:
//
void NDirectPlaySocket::Init(NDriver *CreatingDriver)
	{
	guard(NDirectPlaySocket::Init);
	//
	// Call parent init routine to init all standard info:
	//
	NSocket::Init(CreatingDriver);
	//
	// Init items specific to this socket type:
	//
	unguard;
	};

//
// Shut down this socket.  Removes socket from driver's socket list.
//
void NDirectPlaySocket::Delete()
	{
	guard(NDirectPlaySocket::Delete);
	AssertInitialized();
	//
	// Do custom close code:
	//
	/**/
	//
	// Call generic exit routine for standard exit work.  Removes
	// socket from driver's socket list.
	//
	NSocket::Delete();
	//
	unguard;
	};

/*------------------------------------------------------------------------------
	DirectPlay service provider dialog
------------------------------------------------------------------------------*/

//
// Helper class for remembering DirectPlay drivers returned by enumeration.
//
class FDirectPlayEnumInfo
	{
	public:
	enum{MAX_DRIVERS=64};
	LPGUID DriverGuids[MAX_DRIVERS];
	HWND hWndListBox;
	int Num;
	};

//
// DirectPlay provider enumeration callback for DirectPlayProviderDialogProc
//
BOOL CALLBACK DirectPlayServicesCallback(LPGUID lpSPGuid, LPSTR lpFriendlyName,
    DWORD  dwMajorVersion, DWORD dwMinorVersion, LPVOID lpContext)
	{
	guard(DirectPlayServicesCallback);
	//
	FDirectPlayEnumInfo *Info = (FDirectPlayEnumInfo *)lpContext;
	//
	ListBox_AddString(Info->hWndListBox,lpFriendlyName);
	Info->DriverGuids[Info->Num++] = lpSPGuid;
	//
	return TRUE; // Continue enumeration
	//
	unguard;
	};

//
// DirectPlay provider dialog procedure.
// Assumes: Driver is initialized and ready for EnumDrivers.
// This is modal, called with DialogBox().
//
BOOL CALLBACK DirectPlayProviderDialogProc
	(
    HWND	hDlg,
    UINT	uMsg,
    WPARAM  wParam,
    LPARAM  lParam
	)
	{
	guard(DirectPlayProviderDialogProc);
	static HWND hWndReturn;
	static FDirectPlayEnumInfo Info;
	HWND hWndListBox = GetDlgItem(hDlg,IDC_PROVIDERS);
	char Error[256];
	static int Success=0;
	//
	if (uMsg==WM_COMMAND) switch(LOWORD(wParam))
		{
		case ID_SUCCESS:
			Success = 1; // Prevent DirectPlay driver from being destroyed on WM_DESTROY
			SendMessage((HWND)NetManager.hWndFakeWizard,WM_COMMAND,ID_SUCCESS,0);
			DestroyWindow(hDlg);
			return TRUE;
		case IDCANCEL:
			GotCancel:
			SendMessage((HWND)NetManager.hWndFakeWizard,WM_COMMAND,IDCANCEL,0);
			DestroyWindow(hDlg);
			return TRUE;
		case ID_BACK:
			SendMessage(hWndReturn,WM_USER_REFRESH,0,0);
			DestroyWindow(hDlg);
			return TRUE;
		case ID_NEXT:
			GotNext:
			{
			int Index = ListBox_GetCurSel(hWndListBox);
			char Name[256];
			//
			ListBox_GetText(hWndListBox,Index,Name);
			debugf(LOG_Info,"Creating DirectPlay driver %i: %s",Index,Name);
			//
			if (NetManager.DirectPlayDriver->DirectPlayCreate
				(
				Info.DriverGuids[Index],
				&NetManager.DirectPlayDriver->DirectPlay,NULL
				)!=DP_OK)
				{
				sprintf(Error,"Failed to create driver %i: %s",Index,Name);
				MessageBox(hWndReturn,Error,"Could not initialize DirectPlay",MB_ICONHAND|MB_APPLMODAL);
				goto GotCancel;
				}
			else // Success, go on to select a DirectPlay service provider
				{
				CreateDialogParam // Go to join-a-game dialog:
					(
					(HINSTANCE)NetManager.hInstance,
					MAKEINTRESOURCE(IDD_DPLAYSESSIONS),
					(HWND)NetManager.hWndFakeWizard,
					(DLGPROC)DirectPlaySessionDialogProc,
					(LPARAM)hDlg
					);
				}EndDialog(hDlg,1); 
			};
			return TRUE;
		case IDC_PROVIDERS:
			if (HIWORD(wParam)==LBN_DBLCLK) goto GotNext;
			return TRUE;
		default:
			return FALSE;
		}
	else switch(uMsg)
		{
		case WM_DESTROY:
			if (!Success)
				{
				NetManager.UnregisterDriver(NetManager.DirectPlayDriver);
				};
			return 0;
		case WM_INITDIALOG:
			//
			Success = 0;
			hWndReturn = (HWND)lParam;
			//
			// Initialize DirectPlay:
			//
			if (!NetManager.RegisterDriver(NetManager.DirectPlayDriver))
				{
				DestroyWindow(hDlg);
				MessageBox(hWndReturn,Error,"Could not initialize DirectPlay",MB_ICONHAND|MB_APPLMODAL);
				return TRUE;
				}
			else
				{
				//
				// Display this dialog:
				//
				SetParent(hDlg,(HWND)NetManager.hWndFakeWizard);
				ShowWindow(hWndReturn,SW_HIDE);
				SendMessage(hDlg,WM_USER_REFRESH,0,0);
				//
				// Enumerate available DirectPlay drivers and add them to IDC_PROVIDERS:
				//
				Info.Num			= 0;
				Info.hWndListBox	= hWndListBox;
				debug(LOG_Info,"Enumerating DirectPlay drivers");
				//NetManager.DirectPlayDriver->DirectPlayEnumerate(DirectPlayServicesCallback,(LPVOID)&Info);
				//
				ListBox_SetCurSel(hWndListBox,0);
				//
				SendMessage(hDlg,WM_USER_REFRESH,0,0);
				return FALSE;
				};
		case WM_USER_REFRESH:
			SetWindowText((HWND)NetManager.hWndFakeWizard,"Join a DirectPlay game");
			ShowWindow(hDlg,SW_SHOW);
			SetFocus(GetDlgItem(hDlg,ID_NEXT));
			return TRUE;
		default:
			return FALSE;
		};
	unguard;
	};

/*------------------------------------------------------------------------------
	DirectPlay sessions dialog
------------------------------------------------------------------------------*/

//
// Callback for enumerating DirectPlay sessions:
//
BOOL CALLBACK EnumSessionsCallback(LPDPSESSIONDESC lpDPSGameDesc,
    LPVOID lpContext, LPDWORD lpdwTimeOut, DWORD dwFlags)
	{
	guard(EnumSessionsCallback);
	if (!(dwFlags & DPESC_TIMEDOUT))
		{
		FDirectPlayEnumInfo *Info = (FDirectPlayEnumInfo *)lpContext;
		//
		char Descr[256];
		if (lpDPSGameDesc->dwCurrentPlayers) sprintf
			(
			Descr,"%s (%i/%i)",
			lpDPSGameDesc->szSessionName,
			lpDPSGameDesc->dwCurrentPlayers,
			lpDPSGameDesc->dwMaxPlayers
			);
		else sprintf
			(
			Descr,"%s",
			lpDPSGameDesc->szSessionName
			);
		ListBox_AddString(Info->hWndListBox,Descr);
		//
		Info->Num++;
		//
		return TRUE; // Continue enumeration
		}
	return FALSE; // Timed out, stop the enumeration
	unguard;
	};
//
// DirectPlay session dialog procedure.
// Assumes: Driver is passed in lParam, and is ready to query sessions.
// This is modal, called with DialogBox().
//
BOOL CALLBACK DirectPlaySessionDialogProc
	(
    HWND	hDlg,
    UINT	uMsg,
    WPARAM  wParam,
    LPARAM  lParam
	)
	{
	guard(DirectPlaySessionDialogProc);
	static FDirectPlayEnumInfo Info;
	static HWND hWndReturn;
	static int Success = 0;
	char Name[256];
	HWND hWndListBox = GetDlgItem(hDlg,IDC_SESSIONS);
	int Index;
	//
	if (uMsg==WM_COMMAND) switch(LOWORD(wParam))
		{
		case IDCANCEL:
			SendMessage((HWND)NetManager.hWndFakeWizard,WM_COMMAND,IDCANCEL,0);
			DestroyWindow(hDlg);
			return TRUE;
		case ID_SUCCESS:
			Success = 1; // Prevent DirectPlay driver from unloading on WM_DESTROY
			SendMessage((HWND)NetManager.hWndFakeWizard,WM_COMMAND,ID_SUCCESS,0);
			DestroyWindow(hDlg);
			return TRUE;
		case ID_BACK:
			SendMessage(hWndReturn,WM_USER_REFRESH,0,0);
			DestroyWindow(hDlg);
			return TRUE;
		case ID_NEXT:
			GotNext:
			Index = ListBox_GetCurSel(hWndListBox);
			ListBox_GetText(hWndListBox,Index,Name);
			//
			NetManager.LaunchDriver = NetManager.DirectPlayDriver;
			strcpy(NetManager.LaunchURL,Name);
			//
			return TRUE;
		case IDC_SESSIONS:
			if (HIWORD(wParam)==LBN_DBLCLK) goto GotNext;
			return TRUE;
		default:
			return FALSE;
		}
	else switch(uMsg)
		{
		case WM_INITDIALOG:
			Success    = 0;
			hWndReturn = (HWND)lParam;
			//
			// Display the window, so that something is visible for the 1 second
			// it takes to query the network for servers:
			//
			ListBox_AddString (hWndListBox,"Searching for UnrealServers...");
			ListBox_Enable    (hWndListBox,0);
			Button_Enable	  (GetDlgItem(hDlg,ID_NEXT),0);
			//
			SendMessage(hDlg,WM_USER_REFRESH,0,0);
			UpdateWindow(hDlg);
			//
			// Enumerate available DirectPlay drivers and add them to IDC_PROVIDERS:
			//
			Info.Num						= 0;
			Info.hWndListBox				= hWndListBox;
			//
			DPSESSIONDESC SessionDesc;
			SessionDesc.dwSize				= sizeof(DPSESSIONDESC);
			SessionDesc.guidSession			= GUnrealDirectPlayGUID;
			SessionDesc.dwSession			= 0;
			SessionDesc.dwMaxPlayers		= 0;
			SessionDesc.dwCurrentPlayers	= 0;
			SessionDesc.dwFlags				= 0;
			SessionDesc.szSessionName[0]	= 0;
			SessionDesc.szUserField[0]		= 0;
			SessionDesc.dwReserved1			= 0;
			SessionDesc.szPassword[0]		= 0;
			SessionDesc.dwReserved2			= 0;
			SessionDesc.dwUser1				= 0;
			SessionDesc.dwUser2				= 0;
			SessionDesc.dwUser3				= 0;
			SessionDesc.dwUser4				= 0;
			//
			debug(LOG_Info,"Enumerating DirectPlay sessions");
			//
			NetManager.DirectPlayDriver->DirectPlay->EnumSessions
				(
				&SessionDesc,1000,EnumSessionsCallback,
				(LPVOID)&Info,DPENUMSESSIONS_AVAILABLE
				);
			ListBox_DeleteString(hWndListBox,0);
			if (Info.Num==0)
				{
				ListBox_AddString (hWndListBox,"(No UnrealServers were found)");
				}
			else
				{
				ListBox_Enable    (hWndListBox,1);
				Button_Enable	  (GetDlgItem(hDlg,ID_NEXT),1);
				ListBox_SetCurSel (hWndListBox,0);
				};
			return FALSE;
		case WM_DESTROY:
			if (!Success) // Must unload the DirectPlay driver
				{
				NetManager.DirectPlayDriver->DirectPlay->Release();
				NetManager.DirectPlayDriver->DirectPlay = NULL;
				};
		case WM_USER_REFRESH:
			SetWindowText((HWND)NetManager.hWndFakeWizard,"Join a DirectPlay game");
			ShowWindow(hDlg,SW_SHOW);
			SetFocus(GetDlgItem(hDlg,ID_NEXT));
			return TRUE;
		default:
			return FALSE;
		};
	unguard;
	};

/*------------------------------------------------------------------------------
	The End
------------------------------------------------------------------------------*/
