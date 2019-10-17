#include <stdafx.h>

#include <frontend\rosetta.h>
#include <frontend\rosettadoc.h>

#include <frontend\views\viewtextfile.h>
#include <frontend\views\hint.h>

#include <kernel\structures\textfile.h>

#include <kernel\utilities\iokit.h>

#include <kernel\system\fstream.h>

#include <kernel\basic\message.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Static helpers (file scope).

static void
StaticTabToSpace(char *buffer) {
	while (*buffer != '\0') {
		if (*buffer == '\t')
			*buffer = ' ';
		buffer++;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CViewTextFile

IMPLEMENT_DYNCREATE(CViewTextFile, CViewGrid)

//-------------------------------------------------------------------
// Method........: Constructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewTextFile::CViewTextFile()
{
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CViewTextFile::~CViewTextFile()
{
}

BEGIN_MESSAGE_MAP(CViewTextFile, CViewGrid)
	//{{AFX_MSG_MAP(CViewTextFile)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//-------------------------------------------------------------------
// Method........: OnDraw
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewTextFile::OnDraw(CDC* pDC)
{
  CViewGrid::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CViewTextFile diagnostics

#ifdef _DEBUG
void CViewTextFile::AssertValid() const
{
	CViewGrid::AssertValid();
}

void CViewTextFile::Dump(CDumpContext& dc) const
{
	CViewGrid::Dump(dc);
}
#endif //_DEBUG

//-------------------------------------------------------------------
// Method........: OnInitialUpdate
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewTextFile::OnInitialUpdate() 
{
	CViewGrid::OnInitialUpdate();
}

//-------------------------------------------------------------------
// Method........: SetStructure
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewTextFile::SetStructure(Structure *structure)
{

	m_File = dynamic_cast(TextFile *, structure);
	
	CViewGrid::SetStructure(structure);
	
	SetReadOnly(TRUE);
	
}

//-------------------------------------------------------------------
// Method........: InitializeGridAppearance
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewTextFile::InitializeGridAppearance()
{

	CViewGrid::InitializeGridAppearance();

	// Is a partition attached to the view?
  if (m_File.IsNULL())
		return;

	// Get grid control parameters.
	CGXGridParam *params = GetParam();

	// Disable some stuff.
  params->EnableMoveRows(FALSE);
  params->EnableMoveCols(FALSE);

	params->GetProperties()->SetDisplayHorzLines(FALSE);
	params->GetProperties()->SetPrintHorzLines(FALSE);

	// Set window text.
  GetParentFrame()->SetWindowText((m_File->GetFilename()).GetBuffer());

	BOOL bLockOld = LockUpdate(TRUE);

	// Set the number of rows and columns, prior to loading.
  SetColCount(1);
  SetRowCount(0);

	// Set column headers.
  SetStyleRange(CGXRange(0, 1), CGXStyle().SetValue((m_File->GetFilename()).GetBuffer()));

	// Set column width to a few times the default width.
	// Not necessary since we call ResizeColWidthsToFit in LoadDataIntoGrid() anyway...
	// SetColWidth(1, 1, 10 * GetColWidth(1));

	// Set font face.
	SetStyleRange(CGXRange().SetCols(1), 
			          CGXStyle().SetFont(CGXFont().SetFaceName("Courier New")));

  LockUpdate(bLockOld);
	
}

//-------------------------------------------------------------------
// Method........: LoadDataIntoGrid
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....: 
//===================================================================

void CViewTextFile::LoadDataIntoGrid()
{

	if (m_File.IsNULL())
		return;

	ifstream stream;

	if (!IOKit::Open(stream, m_File->GetFilename())) {
		Message::Error("Error opening " + m_File->GetFilename() + ".\nDoes the file really exist?");
		return;
	}

	BOOL bLockOld = LockUpdate(TRUE);

	CWaitCursor wait;
	Message     message;

	message.Notify("Reading file for display...");

  int no_lines = 0;

	const int maxlength = 2048;
  char      buffer[maxlength];

	// Read all lines, or until current grid is filled.
	while (!stream.eof()) {
		stream.getline(buffer, maxlength);
		StaticTabToSpace(buffer);
		no_lines++;
		if (no_lines >= GetRowCount()) // Resize in chunks
			SetRowCount(GetRowCount() + 500);
    SetValueRange(CGXRange(no_lines, 1), buffer);
	}

	IOKit::Close(stream);

	SetRowCount(no_lines);

	if (no_lines <= 1000) // Set limit, 'cause it's tine-consuming...
		ResizeColWidthsToFit(CGXRange(0, 1, GetRowCount(), GetColCount()));
			
  LockUpdate(bLockOld);
	
	Redraw();

}

//-------------------------------------------------------------------
// Method........: OnCmdMsg
// Author........: 
// Date..........: 
// Description...: 
// Comments......:
// Revisions.....:
//===================================================================

BOOL CViewTextFile::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{

	if (pHandlerInfo != NULL)
		return CViewGrid::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    
  if ((nID > CVIEWTEXTFILE_FIRSTVALUE) && (nID < CVIEWTEXTFILE_LASTVALUE)) {
		if (nCode == CN_COMMAND) {
      switch (nID) {
      }
    }
    else if (nCode == CN_UPDATE_COMMAND_UI) {
    }
  }

	// The command message may be handled higher up in the hierarchy.
	return CViewGrid::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

}

//-------------------------------------------------------------------
// Method........: OnUpdate
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

void CViewTextFile::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CViewGrid::OnUpdate(pSender, lHint, pHint);
}

//-------------------------------------------------------------------
// Method........: CreateGridMenu
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CMenu *CViewTextFile::CreateGridMenu(CMenu *menu)
{

	if (menu == NULL)
		return NULL;

  menu->CreatePopupMenu();

  menu->AppendMenu(MF_STRING | MF_ENABLED,              CVIEWGRID_GRID_RESIZETOFIT,     "Resize to fit");
  menu->AppendMenu(MF_STRING | MF_ENABLED,              CVIEWGRID_GRID_DISPLAYSETTINGS, "Properties...");

	return menu;
 
}

//-------------------------------------------------------------------
// Method........: CreateColumnMenu
// Author........: 
// Date..........: 
// Description...: 
// Revisions.....:
//===================================================================

CMenu *CViewTextFile::CreateColumnMenu(CMenu *menu, ROWCOL nCol)
{

	if (menu == NULL)
		return NULL;

	if (nCol == 1) {
		//menu->CreatePopupMenu();
		//menu->AppendMenu(MF_STRING | MF_ENABLED, WHATEVER, "Whatever");
		//return menu;
	}

	return NULL;
 
}

 
