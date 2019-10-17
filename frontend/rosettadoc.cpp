// RosettaDoc.cpp : implementation of the CRosettaDoc class
//

#include <stdafx.h>

#include <frontend\rosettadoc.h>
#include <frontend\rosetta.h>
#include <frontend\mainfrm.h>

#include <kernel\system\fstream.h>  
#include <kernel\system\time.h>

#include <frontend\views\viewgrid.h>
#include <frontend\views\hint.h>

#include <frontend\dialogs\dialoggenericlist.h>
#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

#include <common\messagehelper.h>
#include <common\configuration.h>

#include <kernel\basic\message.h>

#include <kernel\structures\projectmanager.h>
#include <kernel\structures\textfile.h>

#include <kernel\algorithms\scriptalgorithm.h>
#include <kernel\algorithms\reductimporter.h>

#include <kernel\utilities\creator.h>
#include <kernel\utilities\iokit.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRosettaDoc

IMPLEMENT_DYNCREATE(CRosettaDoc, CDocument)

BEGIN_MESSAGE_MAP(CRosettaDoc, CDocument)
	//{{AFX_MSG_MAP(CRosettaDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRosettaDoc construction/destruction

//-------------------------------------------------------------------
// Method........: 
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

CRosettaDoc::CRosettaDoc()
{
	InitializeNewProject();
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

CRosettaDoc::~CRosettaDoc()
{

	// Remove current project from "global" project pool.
	ProjectManager::RemoveProject(GetProject());

}

//-------------------------------------------------------------------
// Method........: InitializeNewProject
// Author........: 
// Date..........: 
// Description...:
// Comments......: The handles should be smart enough to handle deletion
//                 of any old project.
// Revisions.....:
//===================================================================

void CRosettaDoc::InitializeNewProject()
{

	// Remove current project from "global" project pool.
	ProjectManager::RemoveProject(GetProject());

	// Instatiate top level project structure.
	m_Project = Creator::Project();

	// Set a default name.
	m_Project->SetName("Structures");

	// Insert new project into "global" project pool.
	ProjectManager::InsertProject(GetProject());

}

//-------------------------------------------------------------------
// Method........: RemoveViewsForStructure
// Author........: Knut Magne Risvik
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....: Aš - handles children as well.
//===================================================================

void CRosettaDoc::RemoveViewsForStructure(const Handle<Structure> &structure, bool including_children)
{

	// Acquire the first view's position.
	POSITION pos = GetFirstViewPosition();

	// Check all views.
	while (pos != NULL) {

		// Acquire the next view.
		CView *pView = GetNextView(pos);

		// Check its type before casting.
		if (!pView->IsKindOf(RUNTIME_CLASS(CViewGrid)))
			continue;

		// Cast to verified type.
		CViewGrid *pGrid = dynamic_cast(CViewGrid *, pView);

		bool remove_view = false;

		// Determine if the view should be closed.
		if (structure == pGrid->GetStructure())
			remove_view = true;

		if (!remove_view && including_children) {
			if (structure->IsParent(pGrid->GetStructure(), true))
				remove_view = true;
		}

		if (!remove_view)
			continue;

		// Remove/close the view.
		ASSERT_VALID(pView);
		RemoveView(pView);
		CFrameWnd *pFrame = pView->GetParentFrame();
		ASSERT_VALID(pFrame);
		PreCloseFrame(pFrame);
		pFrame->DestroyWindow();
	}

}


/////////////////////////////////////////////////////////////////////////////
// CRosettaDoc diagnostics

#ifdef _DEBUG
void CRosettaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRosettaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRosettaDoc commands

//-------------------------------------------------------------------
// Method........: OnCmdMsg
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CRosettaDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//-------------------------------------------------------------------
// Method........: DeleteContents
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void CRosettaDoc::DeleteContents() 
{

	Message::Debug("Deleting document contents...");

	InitializeNewProject();
	
	CDocument::DeleteContents();

}

//-------------------------------------------------------------------
// Method........: Serialize
// Author........: 
// Date..........: 
// Description...:
// Comments......: Obsolete, now handled by OnOpenDocument and 
//                 OnSaveDocument.
// Revisions.....:
//===================================================================

void CRosettaDoc::Serialize(CArchive& ar)
{
}

//-------------------------------------------------------------------
// Method........: GetProject
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....: 
//===================================================================

Project *
CRosettaDoc::GetProject()
{
  return m_Project.GetPointer();
}

//-------------------------------------------------------------------
// Methods inherited from CDocument.
//===================================================================

//-------------------------------------------------------------------
// Method........: OnNewDocument
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CRosettaDoc::OnNewDocument() 
{
	return CDocument::OnNewDocument();
}

//-------------------------------------------------------------------
// Method........: OnOpenDocument
// Author........: Knut Magne Risvik
// Date..........: 970316
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL 
CRosettaDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{

	DeleteContents();

	// Select topmost structure for target of objects
	// Handle<ParentStructure> project = dynamic_cast(ParentStructure *, m_Project.GetPointer());

	BOOL ok = LoadStructures(lpszPathName, Handle<ParentStructure>(NULL));

	// Override the Modified flag.
	SetModifiedFlag(FALSE);

	return ok;

}

//-------------------------------------------------------------------
// Method........: OnSaveDocument
// Author........: Knut Magne Risvik
// Date..........: 970316
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL 
CRosettaDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	
	// Select topmost structure for target of objects
	Handle<Structure> project = dynamic_cast(Structure *,m_Project.GetPointer()) ;
	
	if (!SaveStructures(lpszPathName, project))
		return FALSE;

	SetModifiedFlag(FALSE);

	return TRUE;

}

//-------------------------------------------------------------------
// Method........: OnCloseDocument
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

void CRosettaDoc::OnCloseDocument() 
{

	// Causes an MFC crash...
	/*
	// Close all views associated with this document.
	if (m_Project != NULL)
	  RemoveViewsForStructure(m_Project.GetPointer(), true);
	*/

	CDocument::OnCloseDocument();

}

//-------------------------------------------------------------------
// Method........: CanCloseFrame
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL CRosettaDoc::CanCloseFrame(CFrameWnd* pFrame) 
{

	return CDocument::CanCloseFrame(pFrame);

}

//-------------------------------------------------------------------
// Method........: PreCloseFrame
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Cannot class OnCloseDocument, since
//                 CDocument::OnCloseDocument() calls PreCloseFrame,
//                 and this would result in an infinite recursion.
// Revisions.....:
//===================================================================

void CRosettaDoc::PreCloseFrame(CFrameWnd* pFrame) 
{

	// Causes an MFC crash...
	/*
	// Close all views associated with this document.
	if (m_Project != NULL)
	  RemoveViewsForStructure(m_Project.GetPointer(), true);
	*/

	CDocument::PreCloseFrame(pFrame);

}

//-------------------------------------------------------------------
// File operations helper functions
//===================================================================

//-------------------------------------------------------------------
// Method........: LoadStructures
// Author........: Knut Magne Risvik
// Date..........: 970316
// Description...: 
// Comments......: 
// Revisions.....: Aš - fixed some bugs.
//                 Aš - moved code to Creator::Load
//                 Aš - calls to ProjectManager
//                 Aš 9708 - Added tree argument, so as to be able to
//                           reuse apply-code in case loading involves
//                           an installed algorithm (i.e. we are really
//                           doing an import.)
//===================================================================

BOOL 
CRosettaDoc::LoadStructures(const String &pathname, Handle<ParentStructure> &parent) 
{

	CWaitCursor wait;

	// Create and load child from file.
	Handle<Structure> child = Creator::Load(pathname, parent.GetPointer(), false);

	// If loading failed, try import routines.
	if (child == NULL) {
		child = ImportStructure(DECISIONTABLE, pathname); // GENERALIZE!
		if (child == NULL) {
			Message::Error("Unable to load or import file.");
			return FALSE ;
		}
	}

	// If parent is empty or child is a project then we load a new project.
	if (parent.IsNULL() && child->IsA(PROJECT)) {
		ProjectManager::RemoveProject(GetProject());             // Remove old project from "global" pool.
		m_Project = dynamic_cast(Project *, child.GetPointer()); // Set new project.
		ProjectManager::InsertProject(GetProject());             // Add new project to "global" pool.
	}
	else {
		// If we don't have a parent, use the top-level project structure.
		if (parent.IsNULL())
			parent = dynamic_cast(ParentStructure *, m_Project.GetPointer());
		// Append child to parent.
		if (!parent.IsNULL() && !parent->AppendChild(child.GetPointer())) {
		  Message::Error("Appending child structure to parent structure failed.");
		  return FALSE;
		}
	}

	SetModifiedFlag(TRUE);

	UpdateAllViews(NULL);
	
	return TRUE;	

}

//-------------------------------------------------------------------
// Method........: SaveStructures
// Author........: Knut Magne Risvik
// Date..........: 970316
// Description...: 
// Comments......: 
// Revisions.....: Aš 9708 - Added tree argument, so as to be able to
//                           reuse apply-code in case saving involves
//                           an installed algorithm (i.e. we are really
//                           doing an export.)
//===================================================================

BOOL CRosettaDoc::SaveStructures(const String &pathname, const Handle<Structure> &structure) const 
{

	CWaitCursor wait;

	// Do the actual saving.
	if (!structure->Save(pathname)) {
		Message::Error("Saving to " + pathname + " failed.");
		return FALSE;
	}

	return TRUE;

}

//-------------------------------------------------------------------
// Method  ......: ImportStructure
// Author........: Aleksander šhrn
// Date..........: 
// Description...:
// Comments......: Move to Creator::Load...?
// Revisions.....: 970316 KMR - Method moved to CRosettaDoc
//                 970825 Aš  - Generalized, present list of importers
//                 041005 RA  - Acquire importer from id not description,
//                              descriptions may conflict
//===================================================================

Structure *
CRosettaDoc::ImportStructure(Id id, const String &filename)
{

	// Create structure to "fill".
	Handle<Structure> structure = Creator::Create(id);

	if (structure == NULL) {
		Message::Error("Failed to create structure to fill.");
		return NULL;
	}

	Vector(String) names;
	int            i;
	
	Map(String,Id) algorithm_ids;

	// Build vector of applicable import routines.
	for (i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {

		if (!ObjectManager::GetEnumeratedAlgorithm(i)->IsA(IMPORTER))
			continue;
		if (!ObjectManager::GetEnumeratedAlgorithm(i)->IsApplicable(*structure, false))
			continue;

		// Hack: Don't add algorithms intended for use in scripts.
		if (ObjectManager::GetEnumeratedAlgorithm(i)->IsA(SCRIPTALGORITHM))
			continue;

		// Hack: Reduct importers take an already filled table as input, and does not fill
		// the input table.
		if (id == DECISIONTABLE && ObjectManager::GetEnumeratedAlgorithm(i)->IsA(REDUCTIMPORTER))
			continue;
		
		// RA 041005: Hack, keep id as well, so selected algorithm can be acquired correctly,
		// descriptions may be non-unique...
		Id id = ObjectManager::GetEnumeratedAlgorithm(i)->GetId();
		String name = IdHolder::GetDescription(id);
		names.push_back(name);
		Pair(const String,Id) entry(name,id);
		algorithm_ids.insert(entry);

	}

	CDialogGenericList dialog;

	// Set dialog "input".
	dialog.m_Title     = "Import structure";
	dialog.m_Header    = "Select an installed import routine:";
	dialog.m_Entries   = names;
	dialog.m_CanEdit   = false;
	dialog.m_CanSelect = true;

	// Pop dialog.
	if (dialog.DoModal() != IDOK)
		return NULL;

	// Check dialog "output".
	if (dialog.m_Selection == Undefined::Integer()) {
		Message::Error("No importer selected.");
		return NULL;
	}

	String name = names[dialog.m_Selection];

	// Acquire the selected importer.
	// RA 041005: Acquire algorithm from id, not name.
	//Handle<Importer> importer = dynamic_cast(Importer *, ObjectManager::GetIdentifiedAlgorithm(IdHolder::GetId(name)));
	Handle<Importer> importer = dynamic_cast(Importer *, ObjectManager::GetIdentifiedAlgorithm(algorithm_ids[name]));

	if (importer == NULL) {
		Message::Error("Failed to acquire selected import routine.");
		return NULL;
	}

	// Set algorithm parameter.
	if (!importer->SetFilename(filename)) {
		Message::Error("Failed to set filename parameter.");
		return NULL;
	}

	// Do the importing. Reuse Apply-code.
	structure = ApplyAlgorithm(importer.GetPointer(), structure.GetPointer());

	return structure.Release();

}

//-------------------------------------------------------------------
// Method........: ApplyAlgorithm
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Applies the given algorithm to the given structure.
//                 If the application goes well, the result is returned.
//                 If the application fails, NULL is returned.
//
// Comments......: Moved from CProjectTreeCtrl.
// Revisions.....:
//===================================================================

Structure *
CRosettaDoc::ApplyAlgorithm(Algorithm *algorithm, Structure *structure)
{

	// Verify input.
	if ((algorithm == NULL) || (structure == NULL)) {
		Message::Error("Cannot apply algorithm to structure, one or both are invalid objects.");
		return NULL;
	}

	String algorithmname = IdHolder::GetClassname(algorithm->GetId());
	String structurename = structure->GetName();

	// Check applicability.
	if (!algorithm->IsApplicable(*structure)) {
		Message::Error(algorithmname + " is not applicable to " + structurename + ".");
		return NULL;
	}

	// Get the algorithm's input parameter dialog.
	CDialogAlgorithm *dialog = ObjectManager::GetIdentifiedDialog(algorithm->GetId());

	// Pop the input parameter dialog (if it exists) and acquire/set algorithm parameters.
	if (dialog != NULL) {
		dialog->SetAlgorithm(algorithm, structure, GetProject());
		if (dialog->DoModal() != IDOK)
			return NULL;
	}
	else {
		String question = "Are you sure you want to apply " + algorithmname + " to " + structurename + "?";
		if (MessageHelper::Query(question, "Apply", MessageHelper::BUTTONS_OKCANCEL) == MessageHelper::RESPONSE_CANCEL)
			return NULL;
	}

	// Display a wait cursor.
	CWaitCursor wait;

	// Get current time.
	CTime time_start = CTime::GetCurrentTime();

	Message message;

	message.Notify("Applying " + algorithmname + " to " + structurename + "...");

	// Apply the algorithm to the structure.
	Handle<Structure> result = structure->Apply(*algorithm);

	message.Notify("Done applying " + algorithmname + " to " + structurename + ".");

	// Compute duration.
	CTimeSpan duration = CTime::GetCurrentTime() - time_start;

	// Output time.
	message.Notify((duration.Format("Application took %H:%M:%S")).GetBuffer(0));

	// Did the application fail?
	if (result == NULL) {
		Message::Error("Application of algorithm to structure failed.");
		return NULL;
	}

	// Touch the result structure if it has an annotation.
	if (result->IsA(ANNOTATEDSTRUCTURE)) {
		String action;
		if (result == structure)
			action = "Applied " + algorithmname;
		else
			action = "Result of applying " + algorithmname + " to " + structurename;
		action += " with parameters {" + algorithm->GetParameters() + "}.";
		dynamic_cast(AnnotatedStructure *, result.GetPointer())->Touch(action);
	}

	// Was a new structure created?  If so, append it to the input structure.
	if (result != structure) {
		if (structure->IsA(PARENTSTRUCTURE)) {
			if (!dynamic_cast(ParentStructure *, structure)->AppendChild(result.GetPointer())) {
				Message::Error("Error appending the result structure as a child of " + structurename + ".");
				return NULL;
			}
		}
		else {
			Message::Error("Nowhere to place the result structure.");
			return NULL;
		}
	}

	// Baptize result structure.
	if (!NameResult(algorithm, structure, result.GetPointer()))
		Message::Warning("Failed to name result structure(s).");

	// Append any textfile wrappers.
	if (Configuration::GUI::AppendOutputFiles()) {
		if (!AppendOutputFiles(algorithm, structure))
			Message::Warning("File objects not appended.");
	}

	// Flag that the document has been modified.
	SetModifiedFlag(TRUE);

	CHint pHint;

	// Prepare update hint.
	if (result == structure)
		pHint.m_Hint = CHint::HINT_MODIFIED;
	else
		pHint.m_Hint = CHint::HINT_NEW;

	LPARAM lHint = reinterpret_cast(LPARAM, structure);

	Message::Debug("Updating views...");

	// Update all views that take the hint.
	UpdateAllViews(NULL, lHint, &pHint);

	// Return the result of the Apply call.
	return result.Release();

}

//-------------------------------------------------------------------
// Method........: AppendOutputFiles
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

BOOL
CRosettaDoc::AppendOutputFiles(Algorithm *algorithm, Structure *structure)
{

	if (algorithm == NULL || structure == NULL)
		return FALSE;

	Vector(String) filenames;

	// No output files?
	if (!algorithm->GetOutputFilenames(filenames))
		return FALSE;

	if (filenames.empty())
		return TRUE;

	if (!structure->IsA(PARENTSTRUCTURE))
		return FALSE;

	Handle<ParentStructure> pstructure = dynamic_cast(ParentStructure *, structure);

	int i;

	// Create, touch and append a wrapper for each output file.
	for (i = 0; i < filenames.size(); i++) {
		Handle<TextFile> file = new TextFile();
		file->SetName(filenames[i]);
		file->SetFilename(filenames[i]);
		if (file->IsAnnotated())
			file->GetAnnotation()->SetFilename(filenames[i]);
		file->Touch("Result of applying " + IdHolder::GetClassname(algorithm->GetId()) + " to " + structure->GetName() + " with parameters {" + algorithm->GetParameters() + "}.");
		pstructure->AppendChild(file.GetPointer());
	}

	return TRUE;

}


//-------------------------------------------------------------------
// Method........: NameResult
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: Place-holder for smart auto-naming.
// Revisions.....:
//===================================================================

BOOL
CRosettaDoc::NameResult(Algorithm *algorithm, Structure *structure, Structure *result)
{
	return TRUE;
}
 
