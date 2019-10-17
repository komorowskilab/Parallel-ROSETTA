#if !defined(AFX_DIALOGODBCDECISIONTABLEIMPORTER_H__086B0409_1639_11D2_954C_006008A066CE__INCLUDED_)
#define AFX_DIALOGODBCDECISIONTABLEIMPORTER_H__086B0409_1639_11D2_954C_006008A066CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dialogodbcdecisiontableimporter.h : header file
//

#include <afxdb.h>
#include <frontend\dialogs\mychecklistbox.h>
#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>
#include <kernel\structures\attribute.h>

/////////////////////////////////////////////////////////////////////////////
// CDialogODBCDecisionTableImporter dialog

class CDialogODBCDecisionTableImporter : public CDialogAlgorithm
{
// Construction
public:
	CDialogODBCDecisionTableImporter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDialogODBCDecisionTableImporter)
	enum { IDD = IDD_ODBC_IMPORTER };
	CListBox	m_ListTable;
	CString	m_TableInfo;
	//}}AFX_DATA

	CMyCheckListBox	         m_ListAttributes;

	CDatabase                m_Database; // The DB itself.
	Vector(String)           m_Tables;   // Names of tables in DB.
	Vector(String)           m_Names;    // Names of columns in current DB table.
	Vector(Attribute::Type)  m_Types;    // Types of columns in current DB table.
	Vector(int)              m_Scales;   // Scales of (float) columns in current DB table.

	int                      m_CurrentTable;     // Index into m_Tables.
	int                      m_CurrentAttribute; // Index into m_Names/m_Types/m_Scales.

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogODBCDecisionTableImporter)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void FillTableList();
	void FillAttributeList();

	// Generated message map functions
	//{{AFX_MSG(CDialogODBCDecisionTableImporter)
	afx_msg void OnButtonDatabase();
	afx_msg void OnButtonType();
	afx_msg void OnSelchangeListTables();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListAttributes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIALOGODBCDECISIONTABLEIMPORTER_H__086B0409_1639_11D2_954C_006008A066CE__INCLUDED_)
 
