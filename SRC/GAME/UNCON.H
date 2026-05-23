/*=============================================================================
	UnCon.h: FCameraConsole game-specific definition

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Contains routines for: Messages, menus, status bar
=============================================================================*/

#ifndef _INC_UNCON
#define _INC_UNCON

/*------------------------------------------------------------------------------
	FCameraConsole definition.
------------------------------------------------------------------------------*/

//
// Camera console. Overrides the virtual base class FCameraConsoleBase.
// The Unreal engine only has access to the public members in
// FCameraConsoleBase.
//
class FCameraConsole : public FCameraConsoleBase
{
public:
	enum {TEXTMSG_LENGTH=255};

	// FCameraConsoleBase interface.
	void	Init 		(UCamera *Camera);
	void	Exit		();
	int		Key 		(int Key);
	int		Process		(EInputKey iKey, EInputState State, FLOAT Delta=0.0 );
	int		IsTyping	();
	void	PreRender	(UCamera *Camera);
	void	PostRender	(UCamera *Camera,int XLeft);
	void	Write		(const void *Data, int Length, ELogType MsgType=LOG_None);
	int		Exec		(const char *Cmd,FOutputDevice *Out=GApp);
	void	NoteResize	();
	void	PostReadInput(PPlayerTick &Move,FLOAT DeltaSeconds,FOutputDevice *Out);

private:
	// Game-specific implementation.
	#define CON_SHOW     0.60 /* Fraction of playfield occupied by console when shown */
	#define MESSAGE_TIME 2.00 /* Two seconds */
	enum {MAX_BORDER    = 6};
	enum {MAX_LINES		= 64};
	enum {MAX_HISTORY	= 16};
	typedef char TEXTMSG[TEXTMSG_LENGTH];

	UCamera		*Camera;				// Camera owning this console.
	FCameraConsole *Old;
	int StatusRefreshPages;

	int  		KeyState;				// Typing state.
	int			HistoryTop;				// Top of history list.
	int			HistoryBot;				// Bottom of history list.
	int			HistoryCur;				// Current entry in history list.
	TEXTMSG		TypedStr;				// Message the player is typing.
	TEXTMSG		History[MAX_HISTORY];	// Message the player is typing.

	int			Scrollback;				// How many lines the console is scrolled back.
	int			NumLines;				// Number of valid lines in buffer.
	int			TopLine;				// Current message (loop buffer).
	int  		MsgType;				// Type of most recent message.
	FLOAT		MsgStart;				// Timer tick when the most recent message begin, seconds.
	FLOAT		MsgDuration;			// Expiration time of most recent message, seconds.
	TEXTMSG 	MsgText[MAX_LINES];		// Current console message.

	int			BorderSize,Redraw;
	int			SXR,SYR;
	int			ConsoleLines,StatusBarLines;
	int			BorderLines,BorderPixels;

	int			QuickStats,AllStats;
	FLOAT		LastTimeSeconds;
	FLOAT		Fade;
	FLOAT		ConsolePos,ConsoleDest;
	UTexture	*StatusBar;				// Status bar picture
	UTexture	*StatusSmallBar;		// Smaller version of StatusBar.
	UTexture	*ConBackground;			// Player console background texture
	UTexture	*Border;				// Screen border when sized down

	void	ShowStat	(UCamera *Camera,int *StatYL,const char *Str);
	void	DrawStats 	(UCamera *Camera);
	void	Tick		(UCamera *Camera,int TicksPassed);

friend class FGame;
};

/*------------------------------------------------------------------------------
	The End.
------------------------------------------------------------------------------*/
#endif // _INC_UNCON
