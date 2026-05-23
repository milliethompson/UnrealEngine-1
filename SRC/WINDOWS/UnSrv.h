/*=============================================================================
	UnWnEdSv.h: Header file for CUnrealServer Ole class

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/////////////////////////////////////////////////////////////////////////////
// CUnrealServer
/////////////////////////////////////////////////////////////////////////////

class CUnrealServer : public CWnd
	{
	DECLARE_DYNCREATE(CUnrealServer)
	//
	// Construction
	//
	public:
	CUnrealServer();
	//
	// Attributes
	//
	public:
	//
	// Operations
	//
	public:
	//
	// Overrides
	//
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnrealServer)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL
	//
	// Implementation
	//
	public:
	virtual ~CUnrealServer();
	//
	// Generated message map functions
	//
	protected:
	//{{AFX_MSG(CUnrealServer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CUnrealServer)
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CUnrealServer)
	afx_msg void Exec(LPCTSTR Cmd);
	afx_msg BSTR GetProp(LPCTSTR Topic, LPCTSTR Item);
	afx_msg void Init(long hWndMain, long hWndCallback);
	afx_msg void SetProp(LPCTSTR Topic, LPCTSTR Item, LPCTSTR NewValue);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	};

/////////////////////////////////////////////////////////////////////////////
