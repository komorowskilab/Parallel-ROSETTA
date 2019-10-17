#ifndef __ROSETTA_H__
#define __ROSETTA_H__

// Rosetta.h : main header file for the Rosetta application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class String;

/////////////////////////////////////////////////////////////////////////////
// CRosettaApp:
// See Rosetta.cpp for the implementation of this class
//

class CRosettaApp : public CWinApp
{
private:

  void InstallAlgorithms();
  void InstallStructures();
	void ProcessLICFile() const;
	void ProcessCFGFile() const;

public:

 virtual BOOL PreTranslateMessage(MSG* pMsg);

 CMultiDocTemplate *m_TreeTemplate;
 CMultiDocTemplate *m_DecisionTableTemplate;
 CMultiDocTemplate *m_ProgressTemplate;
 CMultiDocTemplate *m_ReductsTemplate;
 CMultiDocTemplate *m_RulesTemplate;
 CMultiDocTemplate *m_PatternsTemplate;
 CMultiDocTemplate *m_BatchClassificationTemplate;
 CMultiDocTemplate *m_PartitionTemplate;
 CMultiDocTemplate *m_ApproximationTemplate;
 CMultiDocTemplate *m_TextFileTemplate;

public:
	CRosettaApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRosettaApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRosettaApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


extern CRosettaApp theApp;
#endif
/////////////////////////////////////////////////////////////////////////////
 
