//===================================================================
// Author........: Daniel Remmem
// Date..........: 960703
// Description...: 
// Revisions.....:
//===================================================================

// viewbatchclassification.cpp : implementation file
//

#include <stdafx.h>

#include <frontend\rosetta.h>

#include <frontend\views\viewbatchclassification.h>
#include <frontend\views\hint.h>

#include <kernel\structures\batchclassification.h>
#include <kernel\structures\decisiontable.h>
#include <kernel\structures\dictionary.h>
#include <kernel\structures\project.h>

#include <kernel\basic\message.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewBatchClassification

IMPLEMENT_DYNCREATE(CViewBatchClassification, CViewGrid)

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: Daniel Remmem
// Date..........: 960703
// Description...: 
// Revisions.....:
//===================================================================

CViewBatchClassification::CViewBatchClassification()
{
  m_BatchClassification = NULL;
	m_Masked          = TRUE;
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: Daniel Remmem
// Date..........: 960703
// Description...: 
// Revisions.....:
//===================================================================

CViewBatchClassification::~CViewBatchClassification()
{
}

BEGIN_MESSAGE_MAP(CViewBatchClassification, CViewGrid)
	//{{AFX_MSG_MAP(CViewBatchClassification)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewBatchClassification diagnostics

#ifdef _DEBUG
void CViewBatchClassification::AssertValid() const
{
	CViewGrid::AssertValid();
}

void CViewBatchClassification::Dump(CDumpContext& dc) const
{
	CViewGrid::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewBatchClassification message handlers

//-------------------------------------------------------------------
// Method........: OnDraw
// Author........: Daniel Remmem
// Date..........: 960703
// Description...: 
// Revisions.....:
//===================================================================

void CViewBatchClassification::OnDraw(CDC* pDC)
{
	CViewGrid::OnDraw(pDC);
}

//-------------------------------------------------------------------
// Method........: OnInitialUpdate
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewBatchClassification::OnInitialUpdate() 
{
	CViewGrid::OnInitialUpdate();
}

//-------------------------------------------------------------------
// Method........: OnCmdMsg
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

BOOL CViewBatchClassification::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	return CViewGrid::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//-------------------------------------------------------------------
// Method........: SetStructure
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewBatchClassification::SetStructure(Structure *structure) 
{

	m_BatchClassification = dynamic_cast(BatchClassification *, structure);

	CViewGrid::SetStructure(m_BatchClassification.GetPointer());

}

//-------------------------------------------------------------------
// Method........: InitializeGridAppearance
// Author........: Daniel Remmem/Aleksander šhrn
// Date..........: 960704
// Description...: 
// Revisions.....:
//===================================================================

void CViewBatchClassification::InitializeGridAppearance()
{

	CViewGrid::InitializeGridAppearance();
		
	// Any confusionmatrix present?
	if (m_BatchClassification.IsNULL())
		return;
	
  BOOL bLockOld = LockUpdate(TRUE);

	// Get the dimension of the confusionmatrix.
	int dimension = m_BatchClassification->GetConfusionMatrix().GetDimension();
	
  // Set the number of rows and columns in the front-end grid.
  SetColCount(dimension + 2);
	SetRowCount(dimension + 2 + 5);

	// Get grid control parameters.
	CGXGridParam *params = GetParam();

	// Disable some stuff.
  params->EnableMoveRows(FALSE);
  params->EnableMoveCols(FALSE);

  LockUpdate(bLockOld);
	
}

//-------------------------------------------------------------------
// Method........: LoadDataIntoGrid
// Author........: Daniel Remmem/Aleksander šhrn
// Date..........: 960704
// Description...: 
// Revisions.....:
//===================================================================

void CViewBatchClassification::LoadDataIntoGrid()
{
	
	int         i, j;
	float       ratio;
	const char *undefined = Undefined::String().GetBuffer();

	// Any confusionmatrix present?
  if (m_BatchClassification.IsNULL())
		return;

	// Get parent decision table.
	m_DecisionTable = dynamic_cast(DecisionTable *, m_BatchClassification->FindParent(DECISIONTABLE, *GetTopProject()));

	if (m_DecisionTable.IsNULL()) {
		Message::Error("No parent decision table found.");
		return;
	}
  
  BOOL bLockOld = LockUpdate(TRUE);
	
  // Allows the programmer to change the values in the table.
	GetParam()->SetLockReadOnly(FALSE);
	
	// Initializes the different styles needed for the matrix.
	SetGridStyles();

	int width = 70;
			
	SetColWidth(1, 1, width);
	GetParam()->EnableTrackColWidth(FALSE);
	GetParam()->EnableTrackRowHeight(FALSE);

	int dimension = m_BatchClassification->GetConfusionMatrix().GetDimension();

	// Initializes the cells that cover the table. 
	SetCoveredCellsRowCol (0, 1, 0, dimension + 2, GX_UPDATENOW, gxDo);
	SetCoveredCellsRowCol (1, 0, dimension + 2, 0, GX_UPDATENOW, gxDo);
	SetValueRange(CGXRange(1, 0), "Actual");
	SetValueRange(CGXRange(0, 1), "Predicted");
	
	SetValueRange(CGXRange(1, 1), "" /*"Values"*/);

	// Load ROC stuff into grid.
	SetCoveredCellsRowCol(dimension + 2 + 1, 0, dimension + 2 + 1 + 4, 0, GX_UPDATENOW, gxDo);
	SetValueRange(CGXRange(dimension + 2 + 1, 0), "ROC");

	SetCoveredCellsRowCol(dimension + 2 + 1, 2, dimension + 2 + 1, 2 + dimension, GX_UPDATENOW, gxDo);
	SetCoveredCellsRowCol(dimension + 2 + 2, 2, dimension + 2 + 2, 2 + dimension, GX_UPDATENOW, gxDo);
	SetCoveredCellsRowCol(dimension + 2 + 3, 2, dimension + 2 + 3, 2 + dimension, GX_UPDATENOW, gxDo);
	SetCoveredCellsRowCol(dimension + 2 + 4, 2, dimension + 2 + 4, 2 + dimension, GX_UPDATENOW, gxDo);
	SetCoveredCellsRowCol(dimension + 2 + 5, 2, dimension + 2 + 5, 2 + dimension, GX_UPDATENOW, gxDo);

	SetValueRange(CGXRange(dimension + 2 + 1, 1), "Class");
	SetValueRange(CGXRange(dimension + 2 + 2, 1), "Area");
	SetValueRange(CGXRange(dimension + 2 + 3, 1), "Std. error");
	SetValueRange(CGXRange(dimension + 2 + 4, 1), "Thr. (0, 1)");
	SetValueRange(CGXRange(dimension + 2 + 5, 1), "Thr. acc.");

	if (m_BatchClassification->GetROCClass() == Undefined::Integer()) {
		SetValueRange(CGXRange(dimension + 2 + 1, 2), Undefined::String().GetBuffer());
	}
	else {
		if (m_UseDictionary)
			SetValueRange(CGXRange(dimension + 2 + 1, 2), m_DecisionTable->GetDictionaryEntry(m_BatchClassification->GetDecisionAttribute(), m_BatchClassification->GetROCClass(), m_Masked).GetBuffer());
		else
			SetValueRange(CGXRange(dimension + 2 + 1, 2), static_cast(long, m_BatchClassification->GetROCClass()));
	}

	if (m_BatchClassification->GetROCArea() == Undefined::Float())
		SetValueRange(CGXRange(dimension + 2 + 2, 2), Undefined::String().GetBuffer());
	else
		SetValueRange(CGXRange(dimension + 2 + 2, 2), String::Format(m_BatchClassification->GetROCArea()).GetBuffer());

	if (m_BatchClassification->GetROCStandardError() == Undefined::Float())
		SetValueRange(CGXRange(dimension + 2 + 3, 2), Undefined::String().GetBuffer());
	else
		SetValueRange(CGXRange(dimension + 2 + 3, 2), String::Format(m_BatchClassification->GetROCStandardError()).GetBuffer());

	if (m_BatchClassification->GetROCNWThreshold() == Undefined::Float())
		SetValueRange(CGXRange(dimension + 2 + 4, 2), Undefined::String().GetBuffer());
	else
		SetValueRange(CGXRange(dimension + 2 + 4, 2), String::Format(m_BatchClassification->GetROCNWThreshold()).GetBuffer());

	if (m_BatchClassification->GetROCAccuracyThreshold() == Undefined::Float())
		SetValueRange(CGXRange(dimension + 2 + 5, 2), Undefined::String().GetBuffer());
	else
		SetValueRange(CGXRange(dimension + 2 + 5, 2), String::Format(m_BatchClassification->GetROCAccuracyThreshold()).GetBuffer());
	
	// Load the front-end grid with data from the kernel.
	for (i = 0; i < dimension; i++) {				
		for (j = 0; j < dimension; j++) {
			long value = m_BatchClassification->GetConfusionMatrix().GetEntry(i, j);
			SetValueRange(CGXRange(i + 2, j + 2), value);			
		}
	}
	
	// Load the rowratios and set header
	SetValueRange(CGXRange(1, dimension + 2), "" /*"Row ratios"*/);
	for (i = 0; i < dimension; i++) {
		ratio = m_BatchClassification->GetConfusionMatrix().GetRowRatio(i);
		if (ratio == Undefined::Float()) 
			SetValueRange(CGXRange(i + 2, dimension + 2), undefined);
		else	
			SetValueRange(CGXRange(i + 2, dimension + 2), String::Format(ratio).GetBuffer());
	}
	
	// Load the columnratios and set header
	SetValueRange(CGXRange(dimension + 2, 1), "" /*"Column ratios"*/);
	for (j = 0; j < dimension; j++) {
		ratio = m_BatchClassification->GetConfusionMatrix().GetColumnRatio(j);
	  if (ratio == Undefined::Float()) 
			SetValueRange(CGXRange(dimension + 2, j + 2), undefined);
		else	
			SetValueRange(CGXRange(dimension + 2, j + 2), String::Format(ratio).GetBuffer());
	}
	
	// Load the diagonalratio.
	ratio = m_BatchClassification->GetConfusionMatrix().GetDiagonalRatio();
	if (ratio == Undefined::Float()) 
		SetValueRange(CGXRange(dimension + 2, dimension + 2), undefined);
	else	
		SetValueRange(CGXRange(dimension + 2, dimension + 2), String::Format(ratio).GetBuffer());	
	
	// Determine decision attribute.
	int decision_attribute = m_BatchClassification->GetDecisionAttribute();
	
	// Load the unique decision values into the confusionmatrix' headers.
	for (i = 0; i < dimension; i++) {
		int value = m_BatchClassification->GetConfusionMatrix().GetValue(i);		
		if (value == Undefined::Integer()) {
			SetValueRange(CGXRange(1, i + 2), undefined);
			SetValueRange(CGXRange(i + 2, 1), undefined);
		}
		else if (m_UseDictionary) {
			String realworld = m_DecisionTable->GetDictionaryEntry(decision_attribute, value, m_Masked);
			SetValueRange(CGXRange(1, i + 2), realworld.GetBuffer());
			SetValueRange(CGXRange(i + 2, 1), realworld.GetBuffer());
		}
		else {
			long avoid_cast = value;
			SetValueRange(CGXRange(1, i + 2), avoid_cast);
			SetValueRange(CGXRange(i + 2, 1), avoid_cast);
		}
	}
			
	// Relock the table.
	GetParam()->SetLockReadOnly(TRUE);
	LockUpdate(bLockOld);	

	// Redraw the grid.
  RedrawRowCol(0, 0, GetRowCount(), GetColCount());  

}

//-------------------------------------------------------------------
// Method........: SetGridStyles
// Author........: Daniel Remmem
// Date..........: 960704
// Description...: 
// Revisions.....:
//===================================================================

void CViewBatchClassification::SetGridStyles(void)
{

	CGXStyle allstyle;
	CGXStyle diagonalstyle;
	CGXStyle edgestyle;
	CGXStyle headerstyle;
	CGXStyle valuestyle;
	CGXStyle rocstyle;

	if (m_BatchClassification.IsNULL())
		return;

	int dimension = m_BatchClassification->GetConfusionMatrix().GetDimension();
	
	// Set the different styles:

	// Set the styles which apply to the whole table.
	allstyle.SetHorizontalAlignment(DT_CENTER);
	allstyle.SetVerticalAlignment(DT_VCENTER);
	allstyle.SetFont(CGXFont().SetBold(FALSE));
	allstyle.SetReadOnly(TRUE);

	// Set style for ROC cells.
	rocstyle.SetHorizontalAlignment(DT_LEFT);
	
	// Set the styles for the values.
		
 	// Set the style for the diagonal cells.
	diagonalstyle.SetInterior(CGXBrush().SetColor(RGB(200, 200, 200)));

  // Set the style for the edges (the ratios).
	edgestyle.SetInterior(CGXBrush().SetColor(RGB(230, 230, 230)));	        
			
	// Set the style for the headers.
	headerstyle.SetInterior(CGXBrush().SetColor(RGB(180, 180, 180)));	        
	

	// Using the styles to define the look of different ranges:

	// Sets the style for the whole table.
	SetStyleRange(CGXRange(0, 0, dimension + 2 + 3, dimension + 2), allstyle); 

	// Sets the style of the cells that display ROC stuff.
	SetStyleRange(CGXRange(dimension + 2 + 1, 1, dimension + 2 + 3, dimension + 2), rocstyle); 
  

	// Sets the style of the cells that display the values in the matrix. 
	SetStyleRange(CGXRange(2, 2, dimension + 1, dimension + 1), valuestyle); 
            		        
	// Sets the style of the cells on the edge (on the right and bottom).
	SetStyleRange(CGXRange(2, dimension + 2, dimension + 1, dimension + 2), edgestyle); 
  SetStyleRange(CGXRange(dimension + 2, 2, dimension + 2, dimension + 2), edgestyle); 
  

	// Sets the style of the row header (left header).
	SetStyleRange(CGXRange(1, 1, dimension + 2, 1), headerstyle); 

	// Sets the style of the column header (top header).
	SetStyleRange(CGXRange(1, 2, 1, dimension + 2), headerstyle); 

	// Sets the style of the value header (topleft cell).
	SetStyleRange(CGXRange(1, 1), headerstyle); 

	int i;
	
	// Sets the style of the diagonal cells.
	for (i = 0; i < dimension; i++)
		SetStyleRange(CGXRange(i + 2, i + 2), diagonalstyle);   

}

//-------------------------------------------------------------------
// Method........: OnBeginPrinting
// Author........: Daniel Remmem
// Date..........: 960717
// Description...: 
// Revisions.....:
//===================================================================

void CViewBatchClassification::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CViewGrid::OnBeginPrinting(pDC, pInfo);
}

//-------------------------------------------------------------------
// Method........: OnEndPrinting
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewBatchClassification::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CViewGrid::OnEndPrinting(pDC, pInfo);
}

//-------------------------------------------------------------------
// Method........: OnPreparePrinting
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

BOOL CViewBatchClassification::OnPreparePrinting(CPrintInfo* pInfo) 
{
	return CViewGrid::OnPreparePrinting(pInfo);
}

//-------------------------------------------------------------------
// Method........: PreCreateWindow
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

BOOL CViewBatchClassification::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CViewGrid::PreCreateWindow(cs);
}
	
//-------------------------------------------------------------------
// Method........: OnUpdate
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewBatchClassification::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CViewGrid::OnUpdate(pSender, lHint, pHint);
}
 
