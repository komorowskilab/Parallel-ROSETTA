// dialogpca.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\dialogs\algorithmdialogs\dialogpca.h>

#include <kernel\algorithms\keyword.h>

#include <kernel\structures\decisiontable.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDialogPrincipalComponentAnalysis dialog


CDialogPrincipalComponentAnalysis::CDialogPrincipalComponentAnalysis(CWnd* pParent /*=NULL*/)
	: CDialogAlgorithm(CDialogPrincipalComponentAnalysis::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogPrincipalComponentAnalysis)
	//}}AFX_DATA_INIT
}

void CDialogPrincipalComponentAnalysis::DoDataExchange(CDataExchange* pDX)
{
	CDialogAlgorithm::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogPrincipalComponentAnalysis)
	DDX_Control(pDX, IDC_MAXCOMP, m_EditMaxComp);	
	DDX_Control(pDX, IDC_STATICGRAPH, m_Chart);
	DDX_Text(pDX, IDC_MAXCOMP, m_MaxComp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogPrincipalComponentAnalysis, CDialogAlgorithm)
	//{{AFX_MSG_MAP(CDialogPrincipalComponentAnalysis)


	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogManualReducer message handlers

//-------------------------------------------------------------------
// Method........: OnOK
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CDialogPrincipalComponentAnalysis::OnOK() 
{
	UpdateData(TRUE);

	// attributes += '{';

	// Build description of selected attributes.
		
	
	if(!(m_alg->SetAttributes(m_MaxComp)))
		{
			Message::Error("Could not set maximum number of components");
			return;
		
		}


	
	CDialogAlgorithm::OnOK();

}

//-------------------------------------------------------------------
// Method........: OnInitDialog
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CDialogPrincipalComponentAnalysis::OnInitDialog() 
{

	int i;
	CString sBuffer;
	int f;
	m_MaxComp = 1;
	m_Chart.ClearData();
	CDialogAlgorithm::OnInitDialog();
	
	if (!m_Algorithm->IsA(PRINCIPALCOMPONENTANALYSIS)) { 
		Message::Error("Algorithm was not pricipal component analysis.");
		CDialog::OnCancel();
		return FALSE;
  }

Vector (float) eigenvalues;

	Handle<DecisionTable> hej = dynamic_cast(DecisionTable *, m_Structure.GetPointer());

	bool masked=true;
int size= hej->GetNoAttributes(masked)-1;

eigenvalues.reserve(size);

for (i=0;i<size;i++)
{

	eigenvalues.push_back(0.0f);
}
		if(!m_alg->PreCalculate(*m_Structure, eigenvalues))
	{
			Message::Error("Pre-calulation failed!");
			return false;
	}
	
	for (int k=0;k<eigenvalues.size() && k<20;k++)
	{
		m_Chart.SetData(0,eigenvalues[size-1-k]);
	
	}
  m_Chart.SetDatasetStyle( 0, HMX_DATASET_STYLE_LINE );


m_Chart.SetDatasetPenColor(0, RGB( 255, 50, 50 ));	
	for(f=0; f<eigenvalues.size() && f<20; f++ )
	{
		sBuffer.Format("%d", f +1);
		m_Chart.SetXScaleLabel( f, sBuffer );
	}
m_Chart.SetDatasetMarker( 0, HMX_DATASET_MARKER_TRI );
//  m_Chart.SetXLabelStep( int nStep );
	 m_Chart.SetYText("Eigenvalue" );
	m_Chart.SetXText( "Component" );
	m_Chart.SetTitle("Eigenvalues");
		m_Chart.SetYTicks(5 );
	m_Chart.SetRoundY( 1 );
	m_Chart.SetDatasetStyle(0, 1);


	m_Chart.SetDatasetPenSize( 0,1 );
	// Cast to a t teste r.
	m_alg = dynamic_cast(PrincipalComponentAnalysis *, m_Algorithm.GetPointer());
	
	


	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



 
