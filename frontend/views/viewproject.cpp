// Include files needed for general document and application purposes.

#include <stdafx.h>

#include <frontend\rosetta.h>
#include <frontend\rosettadoc.h>

#include <resource.h>

// Include files needed to create and manage views.
#include <frontend\views\viewproject.h>
#include <frontend\views\projecttreectrl.h>
#include <frontend\views\viewdecisiontable.h>
#include <frontend\views\viewreducts.h>
#include <frontend\views\viewrules.h>
#include <frontend\views\viewpatterns.h>
#include <frontend\views\viewbatchclassification.h>
#include <frontend\views\viewpartition.h>
#include <frontend\views\viewapproximation.h>
#include <frontend\views\viewtextfile.h>
#include <frontend\views\hint.h>

// Include files needed to create dialogs.
#include <frontend\dialogs\dialoggenericlist.h>

//Ewa Makosa 03.06.2005
#include <frontend\dialogs\structuredialogs\dialoggeneralrulesstatistics.h>

#include <frontend\dialogs\structuredialogs\dialoginformation.h>
#include <frontend\dialogs\structuredialogs\dialogreductsstatistics.h>
#include <frontend\dialogs\structuredialogs\dialogrulesstatistics.h>
#include <frontend\dialogs\structuredialogs\dialogattributestatistics.h>
#include <frontend\dialogs\structuredialogs\dialogapproximationstatistics.h>

#include <frontend\dialogs\algorithmdialogs\dialogalgorithm.h>

/*
  #ifdef _CLASSIFIERGUIDE
  #include <frontend\dialogs\classifierdialogs\dialogclassify.h>
  #endif
*/

// Include ODBC stuff.
#include <frontend\algorithms\odbcdecisiontableimporter.h>

// Include files from the structures in the kernel.
#include <kernel\structures\decisiontable.h>
#include <kernel\structures\decisiontables.h>
#include <kernel\structures\reducts.h>
#include <kernel\structures\rules.h>
#include <kernel\structures\batchclassification.h>
#include <kernel\structures\dictionary.h>
#include <kernel\structures\equivalenceclasses.h>
#include <kernel\structures\approximation.h>
#include <kernel\structures\textfile.h>

// For debugging purposes only.
#ifdef _DEBUG
//#include <kernel\utilities\hanleymcneilcomparator.h>
//#include <kernel\utilities\mcnemarcomparator.h>
//#include <kernel\utilities\mathkit.h>
//#include <kernel\structures\graph.h>
//#include <kernel\structures\discernibilitymatrix.h>
//#include <kernel\utilities\discerner.h>
//#include <kernel\basic\array.h>
#endif


// Include files from the algorithms in the kernel.
#include <kernel\algorithms\algorithm.h>
#include <kernel\algorithms\reducer.h>
#include <kernel\algorithms\splitter.h>
#include <kernel\algorithms\rulegenerator.h>
#include <kernel\algorithms\scaler.h>
#include <kernel\algorithms\reductshortener.h>
#include <kernel\algorithms\reductfilter.h>
#include <kernel\algorithms\rulefilter.h>
#include <kernel\algorithms\batchclassifier.h>
#include <kernel\algorithms\decisiontableimporter.h>
#include <kernel\algorithms\dictionaryimporter.h>
#include <kernel\algorithms\reductimporter.h>
#include <kernel\algorithms\ruleexporter.h>
#include <kernel\algorithms\reductexporter.h>
#include <kernel\algorithms\decisiontableexporter.h>
#include <kernel\algorithms\dictionaryexporter.h>
#include <kernel\algorithms\reporter.h>
#include <kernel\algorithms\partitioner.h>
#include <kernel\algorithms\approximator.h>
#include <kernel\algorithms\completer.h>
#include <kernel\algorithms\executor.h>
#include <kernel\algorithms\scriptalgorithm.h>
// Ewa Makosa 03.06.2005
#include <kernel\algorithms\ruletuner.h>


// Include general kernel stuff.
#include <kernel\basic\undefined.h>

#include <kernel\utilities\iokit.h>
#include <kernel\utilities\creator.h>

#include <kernel\system\fstream.h>

// Include files needed to use the object manager and display messages.
#include <common\objectmanager.h>
#include <common\modemanager.h>
#include <common\messagehelper.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewProject

IMPLEMENT_DYNCREATE(CViewProject,CView)


	BEGIN_MESSAGE_MAP(CViewProject, CView)
	ON_WM_CONTEXTMENU()
//{{AFX_MSG_MAP(CViewProject)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

//-------------------------------------------------------------------
// Static helpers (file scope)
//===================================================================

static bool
StaticContainsId(const Vector(Id) &ids, Id id) {

	int i;

	for (i = 0; i < ids.size(); i++) {
		if (ids[i] == id)
			return true;
	}

	return false;

}


//-------------------------------------------------------------------
// Constructor and destructor
//===================================================================

//-------------------------------------------------------------------
// Method  ......: Constructor
// Author........: Daniel Remmem
// Date..........: 9606
// Description...: 	
// Comments......: 
// Revisions.....:
//===================================================================

CViewProject::CViewProject()
{
	//{{AFX_DATA_INIT(CViewProject)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here

	m_menuflag        = MF_STRING | MF_ENABLED;
	m_menugrayedflag  = MF_STRING | MF_GRAYED;
	m_popupflag       = MF_STRING | MF_POPUP | MF_ENABLED;
	m_popupgrayedflag = MF_STRING | MF_POPUP | MF_GRAYED;

	m_hLastModified   = NULL;	

	m_Masked          = FALSE;

}

//-------------------------------------------------------------------
// Method  ......: Destructor
// Author........: Daniel Remmem
// Date..........: 9606
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

CViewProject::~CViewProject()
{

	// Make sure temp. stuff is deleted.
	DeleteTempMenus();
	DeleteImageList();
}


//-------------------------------------------------------------------
// Methods inherited from CView
//===================================================================

//-------------------------------------------------------------------
// Method  ......: PreCreateWindow
// Author........: J—rn E. Nygjerd
// Date..........: 960412
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

BOOL 
CViewProject::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	return CView::PreCreateWindow(cs);
}

//-------------------------------------------------------------------
// Method  ......: AssertValid
// Author........: J—rn E. Nygjerd
// Date..........: 960412
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

//-------------------------------------------------------------------
// Method  ......: OnSize
// Author........: Knut Magne Risvik
// Date..........: 970315
// Description...: Resizes the Tree control to fit entirely into the view
//									on WM_SIZE messages
// Comments......: 
// Revisions.....: 
//===================================================================

void 
CViewProject::OnSize(UINT nType, int cx, int cy) 
{

	CView::OnSize(nType, cx, cy);

	CRect rect;

	// Retrieve all client rectangle
	GetClientRect(rect);

	// Resize the child control
	m_Tree.MoveWindow(rect);
	
}

//-------------------------------------------------------------------
// Method  ......: OnCreate
// Author........: Knut Magne Risvik
// Date..........: 970315
// Description...: Creates the Tree control upon creation of the view
// Comments......: 
// Revisions.....: 
//===================================================================


int 
CViewProject::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Tree.Create(WS_CHILD | WS_VISIBLE |
				  TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS,
				  CRect(0, 0, 0, 0), // Will be resized on WM_SIZE anyway.
				  this,
				  100);              // Use dummy ID.
	
	return 0;

}

//-------------------------------------------------------------------
// Method  ......: OnDraw
// Author........: Knut Magne Risvik
// Date..........: 970315
// Description...: Does nothing, but need to be implemented to avoid
//                 CView from being an abstract base class
// Comments......: 
// Revisions.....: 
//===================================================================

void 
CViewProject::OnDraw(CDC* /*pDC*/) 
{
}


//-------------------------------------------------------------------
// Method  ......: OnInitialUpdate
// Author........: J—rn E. Nygjerd
// Date..........: 960412
// Description...:
// Comments......: 
// Revisions.....: 970315 Modification for converting to CView, KMR
//===================================================================

void 
CViewProject::OnInitialUpdate() 
{
		
	CView::OnInitialUpdate();
	
	InitialUpdateTree();
	UpdateTree();

	GetParentFrame()->SetWindowText("Project");

}


//-------------------------------------------------------------------
// Method  ......: PreTranslateMessage
// Author........: J—rn E. Nygjerd
// Date..........: 960412
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

BOOL 
CViewProject::PreTranslateMessage(MSG* pMsg)
{
	{
		// Shift+F10: show pop-up menu.
		if ((((pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) && // If we hit a key and
			  (pMsg->wParam == VK_F10) && (GetKeyState(VK_SHIFT) & ~1)) != 0) ||	// it's Shift+F10 OR
			(pMsg->message == WM_CONTEXTMENU))									                // Natural keyboard key
		{
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);

			CPoint point = rect.TopLeft();
			point.Offset(5, 5);
			OnContextMenu(NULL, point);

			return TRUE;
		}
	}

	return CView::PreTranslateMessage(pMsg);
}

//-------------------------------------------------------------------
// Method  ......: OnUpdate
// Author........: J—rn E. Nygjerd
// Date..........: 960412
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void 
CViewProject::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/) 
{
	// UpdateTree();	
}

//-------------------------------------------------------------------
// Method  ......: OmCmdMsg
// Author........: J—rn E. Nygjerd/Aleksander šhrn
// Date..........: 960412
// Description...: This is where much of the work in the tree is done.
//                 
//
//                 TODO: Grid menu (upper left corner)
//                 When the user opens for example a decisiontable view
//                 he should be able to choose a menu from the upper
//                 left corner of the table (NOT IMPLEMENTED). To avoid
//                 duplicating code the tableview should send a message,
//                 (SendMessage or PostMessage, probably) to this view
//                 and let this view handle the menu selection.
//                 
// Comments......: It is checked to see if the message comes from a menu
//                 which belongs to the tree.  The menu ID's are defined
//                 in the header file and ranges from 0 to m_NumberOfItems - 1.
//                 In addition, it is checked to see if nID represents
//                 an algorithm in Objectmanager. 
// Revisions.....: DR - Changed the organisation
//===================================================================
                                           

BOOL 
CViewProject::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	
	Handle<Algorithm>    algorithm;
	Handle<Structure>    structure;
	Handle<Identifier>   identifier;
	long                 item_data;
  
	// The id of the algorithm is LASTVALUE lower than the nID.
	int algID = nID - LASTVALUE;
	
	if (pHandlerInfo != NULL) 
		return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

	if (((nID > FIRSTVALUE) && (nID < LASTVALUE)) || (ObjectManager::IsInstalled(algID))) {
    
		if (nID == NO_ACTION)
			return FALSE;
			
		if (nCode == CN_COMMAND)
		{
			// Get the pointer to the object.
			item_data  = m_Tree.GetItemData(m_Tree.GetSelectedItem());
			identifier = reinterpret_cast(Identifier *, item_data);
			if (identifier.IsNULL()) {
				Message::Debug("Identifier is not valid in CViewProject::OnCmdMsg().");
				return FALSE;
			}
			if (identifier->IsA(STRUCTURE)) {
				structure = dynamic_cast(Structure *, identifier.GetPointer());
			}
			else if (identifier->IsA(ALGORITHM)) {
				algorithm = dynamic_cast(Algorithm *, identifier.GetPointer());
			}
			else {
				Message::Debug("Identifier is not recognized in CViewProject::OnCmdMsg().");
				return FALSE;
			}
          
			switch (nID) 
			{
			case TESTSTUFFDEBUG:       OnTestStuffDebug(structure.GetPointer());              break;
			case SAVE:                 OnSave(structure.GetPointer());                        break;          
			case SAVE_AS:              OnSaveAs(structure.GetPointer());                      break;          
			case LOAD_CHILDREN:        OnLoadChildren(structure.GetPointer());                break;          
			case VIEW:                 OnView(structure.GetPointer());		                    break;
			case REMOVE:               OnRemove(structure.GetPointer());                      break;
			case DUPLICATE:            OnDuplicate(structure.GetPointer());                   break;
			case STATISTICS:           OnStatistics(structure.GetPointer());                  break;
			case INFORMATION:          OnInformation(structure.GetPointer());                 break;
			case RULES_CLASSIFY:       OnRulesClassify(structure.GetPointer());               break;
			case ODBC_IMPORT:          OnODBCImport(structure.GetPointer());                  break;
			case DICTIONARY_EXPORT:    OnDictionaryExport(structure.GetPointer());            break;
			case DICTIONARY_IMPORT:    OnDictionaryImport(structure.GetPointer());            break;
			case APPLY:                OnApply(algorithm.GetPointer());                       break;

				// Dynamic function management.  The messages sent
				// from these ID's are messages from installed algorithms
				// not known at compile time.
			default:

				// Get algorithm.
				algorithm = ObjectManager::GetIdentifiedAlgorithm(algID);
						
				if (algorithm.IsNULL()) {
					Message::Error("Could not get the algorithm."); 
					return FALSE;
				}

				// Get document.
				CRosettaDoc *document = GetDocument();

				if (document == NULL) {
					Message::Error("Could not get the document."); 
					return FALSE;
				}

				// Use common Apply-code.
				if (document->ApplyAlgorithm(algorithm.GetPointer(), structure.GetPointer()) != NULL) {
					document->SetModifiedFlag(TRUE);
					UpdateTree();
				}

				return TRUE;
						
				break;

			}  // End switch
		
		} // End if (nCode = OnCommand)
		
		else if (nCode == CN_UPDATE_COMMAND_UI)
		{
			// Update UI element state					
		}
		return TRUE;
	
	} // end if (nID >= 0........)
		
	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

}


//-------------------------------------------------------------------
// Debug assertion and dump functions
//===================================================================

#ifdef _DEBUG
void 
CViewProject::AssertValid() const
{
	CView::AssertValid();
}

//-------------------------------------------------------------------
// Method  ......: Dump
// Author........: J—rn E. Nygjerd
// Date..........: 960412
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void 
CViewProject::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

//-------------------------------------------------------------------
// Method  ......: GetDocument
// Author........: J—rn E. Nygjerd
// Date..........: 960412
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

CRosettaDoc* 
CViewProject::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRosettaDoc)));
	return (CRosettaDoc*)m_pDocument;
}
#endif //_DEBUG



//-------------------------------------------------------------------
// Menu creation functions
//===================================================================

//-------------------------------------------------------------------
// Method  ......: MakeMenu
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Makes the menu when the user right-clicks on an
//                 object.

// Comments......: 
// Revisions.....: 
//===================================================================

int
CViewProject::MakeMenu(CMenu &menu, const Identifier *identifier) {

	if (identifier == NULL)
		return 0;

	// Creates the pop-up menu.
	menu.CreatePopupMenu();
	
	// Generates the menuitems that are common to some types (on top of the menu).
 	int count = MakeCommonTop(&menu, identifier);
		
	// Dispatch on type.
	if (identifier->IsA(DECISIONTABLE))
		count += MakeDecisionTableMenu(menu, *dynamic_cast(DecisionTable *, const_cast(Identifier *, identifier)));
	else if (identifier->IsA(REDUCTS)) 
		count += MakeReductsMenu(menu, *dynamic_cast(Reducts *, const_cast(Identifier *, identifier)));
	else if (identifier->IsA(RULES))
		count += MakeRulesMenu(menu, *dynamic_cast(Rules *, const_cast(Identifier *, identifier)));
	else if (identifier->IsA(ALGORITHM))
		count += MakeAlgorithmMenu(menu, *dynamic_cast(Algorithm *, const_cast(Identifier *, identifier)));
		
	// Generates the menuitems that are common to some types (on bottom of the menu).
	// Remove spurious separator if needed.
	if (count > 0) {
		menu.AppendMenu(MF_SEPARATOR);
		count++;
	}
 	if (MakeCommonBottom(&menu, identifier) == 0) {
		menu.RemoveMenu(menu.GetMenuItemCount() - 1, MF_BYPOSITION);
	}

	return count;

}

//-------------------------------------------------------------------
// Method  ......: MakeCommonTop
// Author........: Daniel Remmem
// Date..........: 960625
// Description...: Generates the menuitems that are common to some types 
//                 (on top of the menu).
// Comments......: Add 'if(structure->IsA(XXX)' if some menuitems
//                 are wanted for only one or some structures.
// Revisions.....: Aš: Returns the number of menu items added,
//                     separators included.
//===================================================================

int 
CViewProject::MakeCommonTop(CMenu *menu, const Identifier *identifier)
{
	
	int no_added = 0;

	// All algorithms.
	if (identifier->IsA(ALGORITHM)) {
		return 0;
	}

	// Debugging stuff.
#ifdef _DEBUG
	/*
	if (identifier->IsA(DECISIONTABLE)) {
		menu->AppendMenu(m_menuflag, TESTSTUFFDEBUG,   "Test stuff");        no_added++;
		menu->AppendMenu(MF_SEPARATOR);                                      no_added++;
	}
	*/
#endif

	// All viewable objects.
	if (identifier->IsA(DECISIONTABLE) ||
	    identifier->IsA(DECISIONTABLES) || 
	    identifier->IsA(RULES) || 
	    identifier->IsA(REDUCTS) || 
	    identifier->IsA(BATCHCLASSIFICATION) || 
	    identifier->IsA(APPROXIMATION) || 
	    identifier->IsA(TEXTFILE) || 
	    identifier->IsA(EQUIVALENCECLASSES)) {
		menu->AppendMenu(m_menuflag, VIEW,            "View...");            no_added++;
	}

	// Project tree management.
	if (identifier->IsA(STRUCTURE) && (identifier != GetTopProject())) {
		menu->AppendMenu(m_menuflag, REMOVE,          "Remove");             no_added++;
		menu->AppendMenu(m_menuflag, DUPLICATE,       "Duplicate");          no_added++;
	}

	// I/O, dictionary.
	if (identifier->IsA(DECISIONTABLE)) {
		if (no_added > 0)
			menu->AppendMenu(MF_SEPARATOR);                                    no_added++;
		CMenu *submenu = new CMenu;
		m_temporaryMenu.AddTail(submenu);
		submenu->CreatePopupMenu();
		UINT menuflag = m_popupflag;
		UINT menuid   = reinterpret_cast(UINT, submenu->GetSafeHmenu());
		menu->AppendMenu(menuflag, menuid, "Dictionary");                    no_added++;
		submenu->AppendMenu(m_menuflag, DICTIONARY_EXPORT, "Export...");
		submenu->AppendMenu(m_menuflag, DICTIONARY_IMPORT, "Import...");
	}

	if (no_added > 0 && !identifier->IsA(TEXTFILE)) {
		menu->AppendMenu(MF_SEPARATOR);                                      no_added++;
	}

	// I/O, save.
	if (identifier->IsA(PERSISTENT) && !identifier->IsA(TEXTFILE)) {
		menu->AppendMenu(m_menuflag, SAVE,            "Save");               no_added++;
		menu->AppendMenu(m_menuflag, SAVE_AS,         "Save as...");         no_added++;		
	}

	// I/O, load.
	if (identifier->IsA(PARENTSTRUCTURE)) {
		menu->AppendMenu(m_menuflag, LOAD_CHILDREN,    "Load...");           no_added++;
	}

	// ODBC.
	if (identifier == GetTopProject()) {
		menu->AppendMenu(m_menuflag, ODBC_IMPORT,      "ODBC...");           no_added++;
	}

	return no_added;

}

//-------------------------------------------------------------------
// Method  ......: MakeCommonBottom
// Author........: Daniel Remmem
// Date..........: 960625
// Description...: Generates the menuitems that are common to some types 
//                 (on bottom of the menu).
// Comments......: 
// Revisions.....: Aš: Returns number of added menu items, seps. included.
//===================================================================

int 
CViewProject::MakeCommonBottom(CMenu *menu, const Identifier *identifier)
{

	int no_added = 0;

	// Execute.
	if (identifier->IsA(STRUCTURE) && !identifier->IsA(TEXTFILE)) {
		no_added += MakeDynamicMenu(*menu, "Execute", EXECUTOR, *((Structure *) identifier), true, NULL);
		menu->AppendMenu(MF_SEPARATOR);
		no_added++;
	}

	// Report generation.
	if (identifier == GetTopProject()) {
		no_added += MakeDynamicMenu(*menu, "Report", REPORTER, *dynamic_cast(const Structure *, identifier), true, NULL);
	}

	// Statistics.
	if (identifier->IsA(DECISIONTABLE) || 
		identifier->IsA(REDUCTS) ||
		identifier->IsA(RULES) || 
		identifier->IsA(APPROXIMATION)) {
		menu->AppendMenu(m_menuflag, STATISTICS,  "Statistics...");  no_added++;
	}

	// Information.
	if (identifier->IsA(ANNOTATEDSTRUCTURE)) {
		menu->AppendMenu(m_menuflag, INFORMATION, "Annotations..."); no_added++;
	}

	// All algorithms.
	if (identifier->IsA(ALGORITHM)) {
	}

	return no_added;

}

//-------------------------------------------------------------------
// Method  ......: MakeDynamicMenu
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

int 
CViewProject::MakeDynamicMenu(CMenu &menu,                  // Menu to append entries to.
							  const Vector(String) &labels, // Labels for each entry category.
							  const Vector(Id) &ids,        // Algorithm categories.
							  const Structure &structure,   // The object the menu is for.
							  bool  verify,                 // Verify applicability?
							  const Vector(Id) *exceptions) // Exception categories.
{

	// Dimension mismatch?
	if (labels.size() != ids.size())
		return 0;

	int no_added = 0;
	int i;

	// Append entries/submenus to input menu.
	for (i = 0; i < labels.size(); i++)
		no_added += MakeDynamicMenu(menu, labels[i], ids[i], structure, verify, exceptions);

	return no_added;

}

//-------------------------------------------------------------------
// Method  ......: MakeDynamicMenu
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

int 
CViewProject::MakeDynamicMenu(CMenu &menu,                  // Menu to append entries to.
							  const Vector(String) &labels, // Labels for each entry category.
							  const Vector(Id) &ids,        // Algorithm categories.
							  const Structure &structure,   // The object the menu is for.
							  bool verify,                  // Verify applicability?
							  Id exception)                 // Exception category.
{

	Vector(Id) exceptions;
	exceptions.push_back(exception);

	return MakeDynamicMenu(menu, labels, ids, structure, verify, &exceptions);

}

//-------------------------------------------------------------------
// Method  ......: MakeDynamicMenu
// Author........: Aleksander šhrn
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

CViewProject::MakeDynamicMenu(CMenu &menu,                       // Menu to append entries to.
							  const String &label,               // Label for category entry.
							  const Id id,                       // Algorithm category.
							  const Structure &structure,        // The object the menu is for.
							  bool verify,                       // Verify applicability?
							  Id exception)                      // Exception category.
{

	Vector(Id) exceptions;
	exceptions.push_back(exception);

	return MakeDynamicMenu(menu, label, id, structure, verify, &exceptions);

}

//-------------------------------------------------------------------
// Method  ......: MakeDynamicMenu
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Appends menu options to the input menu, each
//                 main option corresponding to an algorithm
//                 category. Scans the prototype pool.
//
//                 Undefined Ids are interpreted as separators.
//
// Comments......: 
//                 algorithm's id becomes the same as one of the static
//                 menu id's, we add a special number (LASTVALUE) to the
//                 id. (Remember to subtract again in OnCmdMsg so that
//                 the correct id is used.)
// Revisions.....: 
//                 RA 040720: Optimized, will iterate over allready
//                 retrieved algorithms instead of generating them again.
//===================================================================

int 
CViewProject::MakeDynamicMenu(CMenu &menu,                  // Menu to append entries to.
							  const String &label,          // Label for category entry.
							  const Id id,                  // Algorithm category.
							  const Structure &structure,   // The object the menu is for.
							  bool verify,                  // Verify applicability?
							  const Vector(Id) *exceptions) // Exception categories.
{

	// Append a separator only?
	if (id == Undefined::Id()) {
		menu.AppendMenu(MF_SEPARATOR);
		return 1;
	}

	// Create pop-up menu. Add submenu to temporary-list so we can delete it later.
	CMenu *submenu = new CMenu;
	m_temporaryMenu.AddTail(submenu);
	submenu->CreatePopupMenu();

	Algorithm::Handles algorithms;

	int i;

	// Collect candidate algorithms to append to menu.
	for (i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {
		Handle<Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(i);
		if (algorithm->IsA(id)) {
			if (exceptions == NULL || 
				//RA 040720: Added: && !StaticContainsId(*exceptions, algorithm->GetId())
				(!StaticContainsId(*exceptions, id) && !StaticContainsId(*exceptions, algorithm->GetId())))
				algorithms.push_back(algorithm);
		}
	}

	String text     = label;
	UINT   menuflag = 0;
	UINT   menuid   = 0;

	// Determine menu appearance.
	switch (algorithms.size()) {
	case 0:  menuflag  = m_menugrayedflag;
		menuid    = NO_ACTION;
		break;
	case 1:  menuflag  = m_menuflag;
		if (verify && !algorithms[0]->IsApplicable(structure, false)) {
			menuflag  = m_menugrayedflag;
			menuid    = NO_ACTION;
		}
		else {
			menuflag  = m_menuflag;
			menuid    = algorithms[0]->GetId() + LASTVALUE;
		}
		break;
	default: menuflag  = m_popupflag;
		menuid    = reinterpret_cast(UINT, submenu->GetSafeHmenu());
	}

	// Determine if the label should have trailing ellipsis.
	if (algorithms.size() == 1 && ObjectManager::GetIdentifiedDialog(algorithms[0]->GetId()) != NULL)
		text += "...";

	// Append the entry/submenu to the input menu.
	menu.AppendMenu(menuflag, menuid, text.GetBuffer());

	// No submenu?
	if (algorithms.size() <= 1)
		return 1;

	int j;

	// Append dynamic menu entries to submenu, if appropriate.
	// RA 040720: loop over algorithms instead...
	/*
	for (j = 0; j < ObjectManager::GetNoAlgorithms(); j++) {
		Handle <Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(j);
		text = IdHolder::GetDescription(algorithm->GetId());
		if (ObjectManager::HasEnumeratedDialog(j))
			text += "...";
		if (algorithm->IsA(id)) {
	*/
	for (j = 0; j < algorithms.size(); j++) {
	
		Handle<Algorithm> algorithm = algorithms[j];
		text = IdHolder::GetDescription(algorithm->GetId());
		if (ObjectManager::HasIdentifiedDialog(algorithm->GetId()))
			text += "...";

		if (verify && !algorithm->IsApplicable(structure, false))
			submenu->AppendMenu(m_menugrayedflag, NO_ACTION, text.GetBuffer());
		else
			submenu->AppendMenu(m_menuflag, algorithm->GetId() + LASTVALUE, text.GetBuffer());
	}

	return 1;

}

//-------------------------------------------------------------------
// Method  ......: MakeDynamicMenu
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Appends menu options to the input menu under
//                 a specified label name. Scans the prototype pool
//                 for algorithms that are applicable and not
//                 mentioned in an exception list. Typically
//                 used to generate a 'catch-all' menu.
//
//                 Returns the number of entries added to the
//                 menu's top level, 0 or 1.
//
// Comments......: See comments for MakeDynamicMenu regarding
//                 dynamic menu appending.
// Revisions.....: 
//===================================================================

int 
CViewProject::MakeDynamicMenu(CMenu &menu,                  // Menu to append entries to.
							  const String &label,          // Label for main menu entry.
							  const Vector(Id) &exceptions, // Categories to not consider.
							  const Structure &structure,   // The object the menu is for.
							  bool verify)                  // Verify applicability?
{

	Algorithm::Handles algorithms;

	int i, j;

	// Scan pool of installed prototypes.
	for (i = 0; i < ObjectManager::GetNoAlgorithms(); i++) {

		// Get current candidate.
		Handle<Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(i);

		// Candidate not applicable?
		if (verify && !algorithm->IsApplicable(structure, false))
			continue;

		// Candidate listed in exceptions?
		for (j = 0; j < exceptions.size(); j++) {
			if (algorithm->IsA(exceptions[j])) {
				algorithm = NULL;
				break;
			}
		}

		// Add candidate to list of applicable algorithms?
		if (algorithm != NULL)
			algorithms.push_back(algorithm);

	}

	// No applicable algorithms?
	if (algorithms.empty())
		return 0;

	// Create pop-up menu. Add submenu to temporary-list so we can delete it later.
	CMenu *submenu = new CMenu;
	m_temporaryMenu.AddTail(submenu);
	submenu->CreatePopupMenu();
 
	menu.AppendMenu(m_popupflag, reinterpret_cast(UINT, submenu->GetSafeHmenu()), label.GetBuffer());

	// Append applicable dynamic menu entries to submenu.
	for (i = 0; i < algorithms.size(); i++) {
		String text = IdHolder::GetDescription(algorithms[i]->GetId());
		if (ObjectManager::GetIdentifiedDialog(algorithms[i]->GetId()) != NULL)
			text += "...";
		if (!algorithms[i]->IsApplicable(structure, false))
			submenu->AppendMenu(m_menugrayedflag, NO_ACTION, text.GetBuffer());
		else
			submenu->AppendMenu(m_menuflag, algorithms[i]->GetId() + LASTVALUE, text.GetBuffer());
	}

	return 1;

}

//-------------------------------------------------------------------
// Method  ......: MakeInverseModeMenu
// Author........: Robin Andersson
// Date..........: 040617
// Description...: Appends menu options to the input menu under
//                 the inverse mode label (i.e. if the current mode
//                 is ROSETTA, then add algorithms for the GENOMIC
//                 mode and vice versa)
//
// Comments......: 
// Revisions.....: RA: 040921: Don't display genomic algorithms in
//                 Rosetta inverse mode menu
//===================================================================
int 
CViewProject::MakeInverseModeMenu(CMenu &menu,                  // Menu to append entries to.
								  const Structure &structure,   // The object the menu is for.
								  bool verify)                  // Verify applicability?
{

	//Add menues for Genomic Rosetta mode:
	if (ModeManager::IsMode(ModeManager::ROSETTA)) {
		String label = "Genomic Rosetta algorithms";
		// Create pop-up menu. Add submenu to temporary-list so we can delete it later.
		CMenu *submenu = new CMenu;
		m_temporaryMenu.AddTail(submenu);
		submenu->CreatePopupMenu();
 
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(m_popupflag, reinterpret_cast(UINT, submenu->GetSafeHmenu()), label.GetBuffer());

		int tmp = MakeGenomicModeMenues((*submenu), ModeManager::GetNodes(), structure, verify, false);
		if (tmp > 0)
			return 1;
		else 
			return 0;
	}
	//Add menues for standard Rosetta mode:
	else if (ModeManager::IsMode(ModeManager::GENOMIC)) {
		String label = "Standard Rosetta algorithms";
		// Create pop-up menu. Add submenu to temporary-list so we can delete it later.
		CMenu *submenu = new CMenu;
		m_temporaryMenu.AddTail(submenu);
		submenu->CreatePopupMenu();
 
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(m_popupflag, reinterpret_cast(UINT, submenu->GetSafeHmenu()), label.GetBuffer());
		
		Vector(String) labels;            Vector(Id) ids;
		labels.push_back("Complete");     ids.push_back(COMPLETER);
		labels.push_back("Discretize");   ids.push_back(SCALER);
		labels.push_back("Reduce");       ids.push_back(REDUCER);
		labels.push_back("Classify");     ids.push_back(BATCHCLASSIFIER);
		
		Vector(Id) exceptions = ids;
	
		exceptions.push_back(DECISIONTABLEEXPORTER);
		exceptions.push_back(SCRIPTALGORITHM);
		exceptions.push_back(DECISIONTABLEIMPORTER);
		exceptions.push_back(DICTIONARYEXPORTER);
		exceptions.push_back(DICTIONARYIMPORTER);
		exceptions.push_back(REPORTER);
		exceptions.push_back(EXECUTOR);

		//RA 040921: Collect the genomic algorithm ids
		Vector(Id) genids = ModeManager::GetGenomicIds();
		for (int i = 0; i < genids.size(); i++)
			exceptions.push_back(genids[i]);

		int tmp = MakeDynamicMenu((*submenu), labels, ids, structure, false, &genids);
		tmp += MakeDynamicMenu((*submenu), "Other", exceptions, structure, true);				
		if (tmp > 0)
			return 1;
		else 
			return 0;
	}
	
	return 0;
}


//-------------------------------------------------------------------
// Method  ......: MakeGenomicModeMenues
// Author........: Robin Andersson
// Date..........: 040617
// Description...: Appends menu options to the input nodes defined
//                 in the GENOMIC mode
//
// Comments......: 
// Revisions.....: 
//===================================================================
int 
CViewProject::MakeGenomicModeMenues(CMenu &menu,                    // Menu to append entries to.
								    const Vector(char*) genlabels,  // The genomic labels
									const Structure &structure,     // The object the menu is for.
								    bool verify,                    // Verify applicability?
									bool sep)                       // Add menu separator?
{

	//Check if there are any labels
	if (genlabels.size() == 0) 
		return 0;
	
	int i, j, n = 0;

	Vector(Id) ids;

	Algorithm::Handles algorithms;

	//Generate all menu items in the genlabel vector
	for (i = 0; i < genlabels.size(); i++) {

		ids = ModeManager::GetIdsForNode(genlabels[i]);

		for (j = 0; j < ids.size(); j++) {
		
			// Get current candidate.
			Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(ids[j]);

			// Candidate not applicable?
			if (verify && !algorithm->IsApplicable(structure, false))
				continue;
		
			// Add candidate to list of applicable algorithms
			algorithms.push_back(algorithm);
		}

	
		// No applicable algorithms?
		if (algorithms.empty()) {
			ids.clear();
			continue;
		}
		
		// Create pop-up menu. Add submenu to temporary-list so we can delete it later.
		CMenu *submenu = new CMenu;
		m_temporaryMenu.AddTail(submenu);
		submenu->CreatePopupMenu();
 
		menu.AppendMenu(m_popupflag, reinterpret_cast(UINT, submenu->GetSafeHmenu()), genlabels[i]);

		// Append applicable dynamic menu entries to submenu.
		for (j = 0; j < algorithms.size(); j++) {
			String text = IdHolder::GetDescription(algorithms[j]->GetId());
			if (ObjectManager::GetIdentifiedDialog(algorithms[j]->GetId()) != NULL)
				text += "...";
			if (algorithms[j]->IsApplicable(structure, false))
				submenu->AppendMenu(m_menuflag, algorithms[j]->GetId() + LASTVALUE, text.GetBuffer());
			else
				submenu->AppendMenu(m_menugrayedflag, NO_ACTION, text.GetBuffer());
		}

		n++;
		
		//Release the algorithm handles
		for (j = 0; j < algorithms.size(); j++) {
			algorithms[j].Release();
		}
		algorithms.clear();
	}

	if (n > 0 && sep)
		menu.AppendMenu(MF_SEPARATOR);
	
	return n;
}

//-------------------------------------------------------------------
// Method  ......: MakeDecisionTableMenu
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....: RA 040617: Generate menu according to application mode
//                 RA 040921: Corrected
//===================================================================

int 
CViewProject::MakeDecisionTableMenu(CMenu &menu, const DecisionTable &table) {

	Vector(String) labels;            Vector(Id) ids;
	Vector(char*) genlabels(ModeManager::GetNodes());

	labels.push_back("Export");       ids.push_back(DECISIONTABLEEXPORTER);
	labels.push_back("---");          ids.push_back(Undefined::Id());

	//RA 0406: Add labels if standard Rosetta mode
	if (ModeManager::IsMode(ModeManager::ROSETTA)) {
		labels.push_back("Complete");     ids.push_back(COMPLETER);
		labels.push_back("Discretize");   ids.push_back(SCALER);
		labels.push_back("Reduce");       ids.push_back(REDUCER);
		labels.push_back("Classify");     ids.push_back(BATCHCLASSIFIER);
	}

	Vector(Id) exceptions = ids;

	exceptions.push_back(SCRIPTALGORITHM);
	exceptions.push_back(DECISIONTABLEIMPORTER);
	exceptions.push_back(DICTIONARYEXPORTER);
	exceptions.push_back(DICTIONARYIMPORTER);
	exceptions.push_back(REPORTER);
	exceptions.push_back(EXECUTOR);

	//RA 040921: Collect the genomic algorithm ids
	Vector(Id) genids = ModeManager::GetGenomicIds();
	for (int i = 0; i < genids.size(); i++)
		exceptions.push_back(genids[i]);

	int n = 0;

	//RA 040617: Build menu according to application mode
	n += MakeDynamicMenu(menu, labels, ids, table, false, &genids);
	if (ModeManager::IsMode(ModeManager::GENOMIC)) 
		n += MakeGenomicModeMenues(menu, genlabels, table, false, false);
	else if (ModeManager::IsMode(ModeManager::ROSETTA))
		n += MakeDynamicMenu(menu, "Other", exceptions, table, true);
	n += MakeInverseModeMenu(menu, table, false);

	return n;
  
}

//-------------------------------------------------------------------
// Method  ......: MakeReductsMenu
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....: 
//===================================================================

int 
CViewProject::MakeReductsMenu(CMenu &menu, const Reducts &reducts) {

	Vector(String) labels;              Vector(Id) ids;

	labels.push_back("Export");         ids.push_back(REDUCTEXPORTER);
	labels.push_back("---");            ids.push_back(Undefined::Id());
	labels.push_back("Filter");         ids.push_back(REDUCTFILTER);
	labels.push_back("Generate rules"); ids.push_back(RULEGENERATOR);

	Vector(Id) exceptions = ids;

	exceptions.push_back(SCRIPTALGORITHM);
	exceptions.push_back(REPORTER);
	exceptions.push_back(EXECUTOR);

	int n = 0;

	n += MakeDynamicMenu(menu, labels, ids, reducts, true, NULL);
	n += MakeDynamicMenu(menu, "Other", exceptions, reducts, true);

	return n;

}

//-------------------------------------------------------------------
// Method  ......: MakeRulesMenu
// Author........: 
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....: 
//===================================================================

int 
CViewProject::MakeRulesMenu(CMenu &menu, const Rules &rules) {


	Vector(String) labels;              Vector(Id) ids;

	labels.push_back("Export");         ids.push_back(RULEEXPORTER);
	labels.push_back("---");            ids.push_back(Undefined::Id());
	labels.push_back("Filter");         ids.push_back(RULEFILTER);
	
	//Ewa Makosa 31.01.2005----------------------------------------------
	labels.push_back("Tune");			ids.push_back(RULETUNER);
	//Ewa Makosa 31.01.2005----------------------------------------------
	Vector(Id) exceptions = ids;

	exceptions.push_back(SCRIPTALGORITHM);
	exceptions.push_back(REPORTER);
	exceptions.push_back(EXECUTOR);

	int n = 0;

	n += MakeDynamicMenu(menu, labels, ids, rules, true, NULL);
	n += MakeDynamicMenu(menu, "Other", exceptions, rules, true);

	return n;

	/*
	  #ifdef _CLASSIFIERGUIDE
	  // Add things the hacky way... (cf. method header).
	  UINT menuflag = 0;
	  UINT menuid   = 0;

	
	  //menu->AppendMenu(MF_SEPARATOR);
	  //count++;
	
	  // Is the native rule-based classifier installed?
	  if (ObjectManager::IsInstalled(MAJORITYVOTER)) {
	  menuflag = m_menuflag;
	  menuid   = RULES_CLASSIFY;
	  }
	  else {
	  menuflag = m_menugrayedflag;
	  menuid   = NO_ACTION;
	  }

	  String label = "Guide";

	  if (ObjectManager::GetIdentifiedDialog(MAJORITYVOTER) != NULL)
	  label += "...";
    
	  menu.AppendMenu(menuflag, menuid, label.GetBuffer());
	  count++;

	  return count;
	  #endif
	*/
 	  
}	

	
//-------------------------------------------------------------------                            
// Method  ......: MakeAlgorithmMenu
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....: 
//===================================================================

int 
CViewProject::MakeAlgorithmMenu(CMenu &menu, const Algorithm &algorithm) {

	String label = "Apply";

	if (ObjectManager::GetIdentifiedDialog(algorithm.GetId()) != NULL)
		label += "...";
    
	menu.AppendMenu(m_menuflag, APPLY, label.GetBuffer());

	return 1;

}

//-------------------------------------------------------------------
// Method  ......: InitialUpdateTree
// Author........: J—rn E. Nygjerd
// Date..........: 960412
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void 
CViewProject::InitialUpdateTree()
{

	CBitmap bitmap;

	m_imagelist.Create(14, 14, FALSE, 0, 0);

	/*
	  bitmap.LoadBitmap(IDB_FOLDER_CLOSED);
	  m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	  bitmap.DeleteObject();
	*/

	bitmap.LoadBitmap(IDB_PROJECT);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_TABLE);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_REDUCTS);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_RULES);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_ALGORITHMS);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_UNKNOWN);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_CONFUSIONMATRIX);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_TABLES);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_PARTITION);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_APPROXIMATION);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_PATTERNS);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	bitmap.LoadBitmap(IDB_TEXTFILE);
	m_imagelist.Add(&bitmap, (CBitmap *) NULL);
	bitmap.DeleteObject();

	// TODO: Bitmaps in the tree
	// INSERT NEW BITMAPS HERE (The order of installation is important)
  
	m_Tree.SetImageList(&m_imagelist, TVSIL_NORMAL);

}

//-------------------------------------------------------------------
// Method  ......: GetTopProject
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Returns the toplevel project that this view covers.
// Comments......: 
// Revisions.....: 
//===================================================================

Project *
CViewProject::GetTopProject()
{

	if (GetDocument() == NULL)
		return NULL;

	return GetDocument()->GetProject();

}

//-------------------------------------------------------------------
// Method  ......: UpdateTree
// Author........: J—rn E. Nygjerd/Daniel Remmem/Aleksander šhrn
// Date..........: 960412
// Description...: Every item in the tree have a 32 bit variable
//                 associated with it (lParam).  This is used by the
//                 application to keep track of which object from the 
//                 kernel the treenode represents.  Some casting was
//                 neccessary to make this happen.  All objects pointers
//                 are casted to long values.  Of course, you have to
//                 cast the long values back again to access the objects.
// Comments......: Now, every time the tree is updated (new item is inserted)
//                 the tree is deleted and then rebuilt.  Before deletion,
//                 the tree state is attempted reconstructed (visibility-wise)
//                 after rebuilding.  This whole approach is a bit awkward, but
//                 goes so fast that it can be done repeatedly on the fly
//                 without being noticeable.
// Revisions.....: DR/Aš 960618: Build algorithm branch.
//                 Aš    960821: Attempt visibility reconstruction.
//                 RA    040617: Build tree according to current application mode
//===================================================================

void 
CViewProject::UpdateTree()
{

	UINT            flags, states;
	HTREEITEM       hStructures, hAlgorithms, hCompleters, hScalers, hReducers;
	HTREEITEM       /*hRuleGenerators,*/ hIO, hFilters, /* Ewa Makosa 31.01.2005 */ hTuners, /* Ewa Makosa 31.01.2005 */ hClassifiers, hExecutors, hScriptCommands, hOther, hTmp;
	HTREEITEM       hFound = NULL; 
	TV_INSERTSTRUCT TreeCtrlItem;	

	// Quick return?
	if (!m_Tree.m_hWnd)
		return;

	// Compile an index of all currently visible items in the tree.
	typedef Map(LPARAM, bool) VisibilityMap;
	VisibilityMap vmap;
	HTREEITEM hVisible = m_Tree.GetFirstVisibleItem();
	while (hVisible != NULL) {
		LPARAM data = m_Tree.GetItemData(hVisible);
		vmap.insert(Pair(const LPARAM, bool)(data, true));
		hVisible = m_Tree.GetNextVisibleItem(hVisible);
	}
   		
	// Clear the current tree.
	m_Tree.DeleteAllItems();

	// Get the top level structure.
	Handle<Project> project = GetTopProject();

	// Initialize flags and states.
	// TODO: The bitmaps should change when the item is expanded.
	
	flags	 = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	states = TVIS_EXPANDED;
	
	// Build static part of tree containing structures.
	TreeCtrlItem.hParent             = TVI_ROOT;
	TreeCtrlItem.hInsertAfter        = TVI_LAST;
	TreeCtrlItem.item.mask           = flags; 
	TreeCtrlItem.item.pszText        = const_cast(char *, (project->GetName()).GetBuffer());
	TreeCtrlItem.item.lParam         = reinterpret_cast(LPARAM, project.GetPointer());
	TreeCtrlItem.item.iImage         = 0;
	TreeCtrlItem.item.iSelectedImage = 0;

	hStructures                      = m_Tree.InsertItem(&TreeCtrlItem);
	
	// Build static part of tree containing algorithms.
	TreeCtrlItem.hParent             = TVI_ROOT;
	TreeCtrlItem.hInsertAfter        = TVI_LAST;
	TreeCtrlItem.item.mask           = flags;
	
	//RA 0406: Check if the current mode is genomic rosetta
	if (ModeManager::IsMode(ModeManager::GENOMIC))
		TreeCtrlItem.item.pszText        = "Genomic Algorithms";
	else
		TreeCtrlItem.item.pszText        = "Algorithms";
	TreeCtrlItem.item.lParam         = NULL;
	TreeCtrlItem.item.iImage         = 4;
	TreeCtrlItem.item.iSelectedImage = 4;

	hAlgorithms                      = m_Tree.InsertItem(&TreeCtrlItem);

	
	//RA 0406: Build the tree as usual if the current application
	//mode is standard Rosetta mode.
	if (ModeManager::IsMode(ModeManager::ROSETTA)){

		TreeCtrlItem.hParent             = hAlgorithms;
		TreeCtrlItem.hInsertAfter        = TVI_LAST;
		TreeCtrlItem.item.mask           = flags;
		TreeCtrlItem.item.pszText        = "I/O";
		TreeCtrlItem.item.lParam         = NULL;
		TreeCtrlItem.item.iImage         = 4;
		TreeCtrlItem.item.iSelectedImage = 4;
		
		hIO     	                       = m_Tree.InsertItem(&TreeCtrlItem);     
		
		TreeCtrlItem.hParent             = hAlgorithms;
		TreeCtrlItem.hInsertAfter        = TVI_LAST;
		TreeCtrlItem.item.mask           = flags;
		TreeCtrlItem.item.pszText        = "Completion";
		TreeCtrlItem.item.lParam         = NULL;
		TreeCtrlItem.item.iImage         = 4;
		TreeCtrlItem.item.iSelectedImage = 4;
		
		hCompleters                      = m_Tree.InsertItem(&TreeCtrlItem);     
		
		TreeCtrlItem.hParent             = hAlgorithms;
		TreeCtrlItem.hInsertAfter        = TVI_LAST;
		TreeCtrlItem.item.mask           = flags;
		TreeCtrlItem.item.pszText        = "Discretization";
		TreeCtrlItem.item.lParam         = NULL;
		TreeCtrlItem.item.iImage         = 4;
		TreeCtrlItem.item.iSelectedImage = 4;
		
		hScalers	                       = m_Tree.InsertItem(&TreeCtrlItem);     
		
		TreeCtrlItem.hParent             = hAlgorithms;
		TreeCtrlItem.hInsertAfter        = TVI_LAST;
		TreeCtrlItem.item.mask           = flags;
		TreeCtrlItem.item.pszText        = "Reduction";
		TreeCtrlItem.item.lParam         = NULL;
		TreeCtrlItem.item.iImage         = 4;
		TreeCtrlItem.item.iSelectedImage = 4;
		
		hReducers		                     = m_Tree.InsertItem(&TreeCtrlItem);
		
		TreeCtrlItem.hParent             = hAlgorithms;
		TreeCtrlItem.hInsertAfter        = TVI_LAST;
		TreeCtrlItem.item.mask           = flags;
		TreeCtrlItem.item.pszText        = "Filtering";
		TreeCtrlItem.item.lParam         = NULL;
		TreeCtrlItem.item.iImage         = 4;
		TreeCtrlItem.item.iSelectedImage = 4;

		hFilters                         = m_Tree.InsertItem(&TreeCtrlItem);
// Ewa Makosa 31.01.2005		
		TreeCtrlItem.hParent             = hAlgorithms;
		TreeCtrlItem.hInsertAfter        = TVI_LAST;
		TreeCtrlItem.item.mask           = flags;
		TreeCtrlItem.item.pszText        = "Tuning";
		TreeCtrlItem.item.lParam         = NULL;
		TreeCtrlItem.item.iImage         = 4;
		TreeCtrlItem.item.iSelectedImage = 4;
		
		hTuners                         = m_Tree.InsertItem(&TreeCtrlItem);
// Ewa Makosa 31.01.2005
		/*
		  TreeCtrlItem.hParent             = hAlgorithms;
		  TreeCtrlItem.hInsertAfter        = TVI_LAST;
		  TreeCtrlItem.item.mask           = flags;
		  TreeCtrlItem.item.pszText        = "Rule generation";
		  TreeCtrlItem.item.lParam         = NULL;
		  TreeCtrlItem.item.iImage         = 4;
		  TreeCtrlItem.item.iSelectedImage = 4;
		  
		  hRuleGenerators                  = m_Tree.InsertItem(&TreeCtrlItem);     
		*/
		
		TreeCtrlItem.hParent             = hAlgorithms;
		TreeCtrlItem.hInsertAfter        = TVI_LAST;
		TreeCtrlItem.item.mask           = flags;
		TreeCtrlItem.item.pszText        = "Classification";
		TreeCtrlItem.item.lParam         = NULL;
		TreeCtrlItem.item.iImage         = 4;
		TreeCtrlItem.item.iSelectedImage = 4;
		
		hClassifiers                     = m_Tree.InsertItem(&TreeCtrlItem);
		
		TreeCtrlItem.hParent             = hAlgorithms;
		TreeCtrlItem.hInsertAfter        = TVI_LAST;
		TreeCtrlItem.item.mask           = flags;
		TreeCtrlItem.item.pszText        = "Command execution";
		TreeCtrlItem.item.lParam         = NULL;
		TreeCtrlItem.item.iImage         = 4;
		TreeCtrlItem.item.iSelectedImage = 4;
		
		hExecutors                       = m_Tree.InsertItem(&TreeCtrlItem);     
		
		TreeCtrlItem.hParent             = hAlgorithms;
		TreeCtrlItem.hInsertAfter        = TVI_LAST;
		TreeCtrlItem.item.mask           = flags;
		TreeCtrlItem.item.pszText        = "Script algorithms";
		TreeCtrlItem.item.lParam         = NULL;
		TreeCtrlItem.item.iImage         = 4;
		TreeCtrlItem.item.iSelectedImage = 4;
		
		hScriptCommands			             = m_Tree.InsertItem(&TreeCtrlItem);
		
		TreeCtrlItem.hParent             = hAlgorithms;
		TreeCtrlItem.hInsertAfter        = TVI_LAST;
		TreeCtrlItem.item.mask           = flags;
		TreeCtrlItem.item.pszText        = "Other";
		TreeCtrlItem.item.lParam         = NULL;
		TreeCtrlItem.item.iImage         = 4;
		TreeCtrlItem.item.iSelectedImage = 4;
		
		hOther				                   = m_Tree.InsertItem(&TreeCtrlItem);     
		
		int i, no_algorithms = ObjectManager::GetNoAlgorithms();
		
		// Build dynamic part of tree (leaf-nodes) containing installed algorithms.
		for (i = 0; i < no_algorithms; i++) {
			
			// Get the installed algorithm.
			Handle<Algorithm> algorithm = ObjectManager::GetEnumeratedAlgorithm(i);
			
			//RA 0406: continue with next algorithm if the curent is a
			//genomic algorithm
			if (!ModeManager::IsRosettaAlgorithm(algorithm->GetId()))
				continue;
			
			// Determine which branch to attach the algorithm to. 
			if (algorithm->IsA(IMPORTER) || algorithm->IsA(EXPORTER))
				TreeCtrlItem.hParent = hIO;	
			else if (algorithm->IsA(COMPLETER))
				TreeCtrlItem.hParent = hCompleters;	
			else if (algorithm->IsA(SCALER))
				TreeCtrlItem.hParent = hScalers;	
			else if (algorithm->IsA(REDUCER))
				TreeCtrlItem.hParent = hReducers;
			/*
			  else if (algorithm->IsA(RULEGENERATOR))
			  TreeCtrlItem.hParent = hRuleGenerators;
			*/
			else if (algorithm->IsA(FILTER))
				TreeCtrlItem.hParent = hFilters;
// Ewa Makosa 31.01.2005
			else if (algorithm->IsA(RULETUNER))
				TreeCtrlItem.hParent = hTuners;
			else if (algorithm->IsA(RULEGENERALIZER))
				TreeCtrlItem.hParent = hTuners;
			else if (algorithm->IsA(RULEPAIRWISEGENERALIZER))
				TreeCtrlItem.hParent = hTuners;
// Ewa Makosa 31.01.2005
			else if (algorithm->IsA(BATCHCLASSIFIER) || algorithm->IsA(CLASSIFIER))
				TreeCtrlItem.hParent = hClassifiers;
			else if (algorithm->IsA(EXECUTOR))
				TreeCtrlItem.hParent = hExecutors;
			else if (algorithm->IsA(SCRIPTALGORITHM))
				TreeCtrlItem.hParent = hScriptCommands;
			else
				TreeCtrlItem.hParent = hOther;
			
			// Determine a suitable label.
			String classname   = IdHolder::GetClassname(algorithm->GetId());
			String description = IdHolder::GetDescription(algorithm->GetId());
			String label       = classname + " (" + description + ")";
			if (ObjectManager::HasEnumeratedDialog(i))
				label += "...";
			
			// Construct the rest of the tree control item.
			TreeCtrlItem.item.pszText        = const_cast(char *, label.GetBuffer());
			TreeCtrlItem.item.lParam         = reinterpret_cast(LPARAM, algorithm.GetPointer());
			TreeCtrlItem.item.iImage         = 4;
			TreeCtrlItem.item.iSelectedImage = 4;
			
			// Insert the algorithm into the project tree.	  
			m_Tree.InsertItem(&TreeCtrlItem);                      
		}
		
		
	}
	//RA 0406: If the current application mode is Genomic Rosetta
	//then build the tree according to the node settings in ObjectManager
	else if (ModeManager::IsMode(ModeManager::GENOMIC)){
		
		//Get the node names
		std::vector<char*> nodenames(ModeManager::GetNodes());
		
		
		//Build all nodes with associated genomic algorithms
		for (int i = 0; i < nodenames.size(); i++){
			
			TreeCtrlItem.hParent             = hAlgorithms;
			TreeCtrlItem.hInsertAfter        = TVI_LAST;
			TreeCtrlItem.item.mask           = flags;
			TreeCtrlItem.item.pszText        = nodenames[i];
			TreeCtrlItem.item.lParam         = NULL;
			TreeCtrlItem.item.iImage         = 4;
			TreeCtrlItem.item.iSelectedImage = 4;

			hTmp    	                     = m_Tree.InsertItem(&TreeCtrlItem);
			
			//Get associated algorithms
			std::vector<Id> ids(ModeManager::GetIdsForNode(nodenames[i])); 
			
			//Add all associated algorithms to the node
			for (int j = 0; j < ids.size(); j++){
			
				TreeCtrlItem.hParent             = hTmp;
				TreeCtrlItem.hInsertAfter        = TVI_LAST;
				TreeCtrlItem.item.mask           = flags;
				
				// Determine a suitable label.
				String classname   = IdHolder::GetClassname(ids[j]);
				String description = IdHolder::GetDescription(ids[j]);
				String label       = classname + " (" + description + ")";
				if (ObjectManager::HasEnumeratedDialog(i))
					label += "...";
				
				// Get the installed algorithm.
				Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(ids[j]);

				// Construct the rest of the tree control item.
				TreeCtrlItem.item.pszText        = const_cast(char *, label.GetBuffer());
				TreeCtrlItem.item.lParam         = reinterpret_cast(LPARAM, algorithm.GetPointer());
				TreeCtrlItem.item.iImage         = 4;
				TreeCtrlItem.item.iSelectedImage = 4;
	  
				// Insert the algorithm into the project tree.	  
				m_Tree.InsertItem(&TreeCtrlItem);    
				
			}
		}
	
	}
	// Was the top level structure accessible?
	if (project == NULL) {
		Message::Error("Could not get top-level project structure.");
		return;
	}

	// Any structures present?
	if (!project->HasChildren()) {
		Message::Debug("No structures are currently present."); 
		return;
	}	

	// Build the structural part of the tree recursively.
	for (int i = 0; i < project->GetNoChildren(); i++) {
		BuildStructuresInTree(project->GetChild(i), hStructures);
	}

	// This should be a const_iterator, but VC++ 6.0 won't let me...
	VisibilityMap::iterator it = vmap.begin();

	// All items that used to be visible, should still be so.
	while (!(it == vmap.end())) {
		LPARAM data = (*it).first;
		if (data != NULL)
			EnsureVisible(m_Tree.GetRootItem(), data);
		it++;
	}

}

//-------------------------------------------------------------------
// Tree control functions
//===================================================================

//-------------------------------------------------------------------
// Method  ......: BuildStructuresInTree
// Author........: Daniel Remmem
// Date..........: 960620
// Description...: This function builds the structures in the tree 
//								 recursively
// Comments......: 
// Revisions.....:
//===================================================================

void 
CViewProject::BuildStructuresInTree(Structure *structure, HTREEITEM parent)
{
	
	UINT            flags;
	HTREEITEM       ChildItem;
	TV_INSERTSTRUCT TreeCtrlItem;

	// Set flags.
	flags = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;

	// Construct (partial) tree control item.	
	TreeCtrlItem.hParent			= parent;	
	TreeCtrlItem.item.mask		= flags;
	TreeCtrlItem.item.lParam	= reinterpret_cast(LPARAM, structure);
	TreeCtrlItem.item.pszText	= const_cast(char *, (structure->GetName()).GetBuffer());
		
	// Determine which image to use.
	if (structure->IsA(PROJECT)) {
		TreeCtrlItem.item.iImage         = 0;
		TreeCtrlItem.item.iSelectedImage = 0;
	}
	else if (structure->IsA(DECISIONTABLE)) {
		TreeCtrlItem.item.iImage         = 1;
		TreeCtrlItem.item.iSelectedImage = 1;
	}
	else if (structure->IsA(REDUCTS)) {
		TreeCtrlItem.item.iImage         = 2;
		TreeCtrlItem.item.iSelectedImage = 2;
	}
	else if (structure->IsA(RULES)) {
		if (dynamic_cast(Rules *, structure)->HasRHS(true)) {
			TreeCtrlItem.item.iImage         = 3;
			TreeCtrlItem.item.iSelectedImage = 3;
		}
		else {
			TreeCtrlItem.item.iImage         = 10;
			TreeCtrlItem.item.iSelectedImage = 10;
		}
	}
	else if (structure->IsA(BATCHCLASSIFICATION)) {
		TreeCtrlItem.item.iImage         = 6;
		TreeCtrlItem.item.iSelectedImage = 6;
	}
	else if (structure->IsA(DECISIONTABLES)) {
		TreeCtrlItem.item.iImage         = 7;
		TreeCtrlItem.item.iSelectedImage = 7;
	}
	else if (structure->IsA(EQUIVALENCECLASSES)) {
		TreeCtrlItem.item.iImage         = 8;
		TreeCtrlItem.item.iSelectedImage = 8;
	}
	else if (structure->IsA(APPROXIMATION)) {
		TreeCtrlItem.item.iImage         = 9;
		TreeCtrlItem.item.iSelectedImage = 9;
	}
	else if (structure->IsA(TEXTFILE)) {
		TreeCtrlItem.item.iImage         = 11;
		TreeCtrlItem.item.iSelectedImage = 11;
	}
	else {
		TreeCtrlItem.item.iImage         = 5;
		TreeCtrlItem.item.iSelectedImage = 5;
	}

	// Insert the structure in the tree.
	ChildItem = m_Tree.InsertItem(&TreeCtrlItem);

	// Does the structure have children?
	if (!structure->HasChildren())
		return;

	int i;
		
	// Insert the children in the tree recursively.
	for (i = 0; i < structure->GetNoChildren(); i++) {
		BuildStructuresInTree(structure->GetChild(i), ChildItem);
	}
	
}

//-------------------------------------------------------------------
// Method  ......: EnsureVisible
// Author........: Aleksander šhrn
// Date..........: 960620
// Description...: This function runs through the tree recursively, and
//                 ensures that the item with the given data is visible.
// Comments......: 
// Revisions.....:
//===================================================================

void 
CViewProject::EnsureVisible(HTREEITEM parent, LPARAM data)
{
	
	if (parent == NULL)
		return;

	HTREEITEM item = parent;
		
	while (item != NULL) {
		if (m_Tree.GetItemData(item) == data) {
			m_Tree.EnsureVisible(item);
			return;
		}
		EnsureVisible(m_Tree.GetChildItem(item), data);
		item = m_Tree.GetNextSiblingItem(item);
	}
	
}

//-------------------------------------------------------------------
// Method  ......: DeleteTempMenus
// Author........: Knut Magne Risvik
// Date..........: 970317
// Description...: Deletes temporary menus used for ContextMenus
// Comments......: If all temp-menu stuff is ignored, pop-up menus
//                 don't function properly under Win95 (under NT things
//                 seem to go OK).  All the temporary pop-up menu stuff
//                 is hence a workaround.
// Revisions.....:
//===================================================================

void
CViewProject::DeleteTempMenus() {

	if (m_temporaryMenu.IsEmpty())
		return;

	do {
		CMenu *pMenu = dynamic_cast(CMenu *, m_temporaryMenu.RemoveTail());
		if (pMenu)
			delete pMenu;
	} while (!m_temporaryMenu.IsEmpty());
		
}

//-------------------------------------------------------------------
// Method  ......: DeleteImageList
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Deletes list of icon bitmaps.
// Comments......: 
// Revisions.....:
//===================================================================

void
CViewProject::DeleteImageList() {

	m_imagelist.DeleteImageList();
		
}

//-------------------------------------------------------------------
// Handler functions for context menus
//===================================================================

//-------------------------------------------------------------------
// Method  ......: OnContextMenu
// Author........: J—rn E. Nygjerd/Aleksander šhrn
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void 
CViewProject::OnContextMenu(CWnd *, CPoint point) {

	CPoint ptAction;
	UINT	 nFlags;
	CMenu  menu;

	// Where are we?
	GetCursorPos(&ptAction);
	m_Tree.ScreenToClient(&ptAction);

	// What was hit?
	HTREEITEM hitItem = m_Tree.HitTest(ptAction, &nFlags);

	// Anything hit?
	if (hitItem) {

		// Select hit item.
		m_Tree.SelectItem(hitItem);

		// Make a menu appropriate for the hit item, and display it.
		if (0 < MakeMenu(menu, reinterpret_cast(Identifier *, m_Tree.GetItemData(hitItem))))
			menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

	}
	
	// Clean up.
	DeleteTempMenus();

}


//-------------------------------------------------------------------
// Method  ......: OnView
// Author........: J—rn E. Nygjerd/Daniel Remmem
// Date..........: 960628
// Description...: Does the work needed when the user chooses the menuitem
//                 'View...'. 
// Comments......: This function is called from OnCmdMsg. 
// Revisions.....:
//===================================================================

void 
CViewProject::OnView(Structure *structure)
{

	if (structure == NULL)
		return;

	int i;

	if (structure->IsA(DECISIONTABLES)) {
		for (i = 0; i < dynamic_cast(DecisionTables *, structure)->GetNoDecisionTables(); i++)
			OnView(dynamic_cast(DecisionTables *, structure)->GetDecisionTable(i));
	}

	else if (structure->IsA(DECISIONTABLE)) {
		CFrameWnd* pNewFrame = theApp.m_DecisionTableTemplate->CreateNewFrame(GetDocument(), NULL);            
		theApp.m_DecisionTableTemplate->InitialUpdateFrame(pNewFrame, GetDocument());
		static_cast(CViewDecisionTable *, pNewFrame->GetActiveView())->SetStructure(structure);
	}
	
	else if (structure->IsA(REDUCTS)) {
		CFrameWnd* pNewFrame = theApp.m_ReductsTemplate->CreateNewFrame(GetDocument(), NULL);            
		theApp.m_ReductsTemplate->InitialUpdateFrame(pNewFrame, GetDocument());
		static_cast(CViewReducts *, pNewFrame->GetActiveView())->SetStructure(structure);
	}

	else if (structure->IsA(RULES)) {
		Handle<Rules> rules = dynamic_cast(Rules *, structure);
		if (rules->HasRHS(false)) {
			CFrameWnd* pNewFrame = theApp.m_RulesTemplate->CreateNewFrame(GetDocument(), NULL);            
			theApp.m_RulesTemplate->InitialUpdateFrame(pNewFrame, GetDocument());
			static_cast(CViewRules *, pNewFrame->GetActiveView())->SetStructure(structure);
		}
		else {
			CFrameWnd* pNewFrame = theApp.m_PatternsTemplate->CreateNewFrame(GetDocument(), NULL);            
			theApp.m_PatternsTemplate->InitialUpdateFrame(pNewFrame, GetDocument());
			static_cast(CViewPatterns *, pNewFrame->GetActiveView())->SetStructure(structure);
		}
	}
	
	else if (structure->IsA(BATCHCLASSIFICATION)) {
		CFrameWnd* pNewFrame = theApp.m_BatchClassificationTemplate->CreateNewFrame(GetDocument(), NULL);            
		theApp.m_BatchClassificationTemplate->InitialUpdateFrame(pNewFrame, GetDocument());
		static_cast(CViewBatchClassification *, pNewFrame->GetActiveView())->SetStructure(structure);
	}
	
	else if (structure->IsA(EQUIVALENCECLASSES)) {
		CFrameWnd* pNewFrame = theApp.m_PartitionTemplate->CreateNewFrame(GetDocument(), NULL);            
		theApp.m_PartitionTemplate->InitialUpdateFrame(pNewFrame, GetDocument());
		static_cast(CViewPartition *, pNewFrame->GetActiveView())->SetStructure(structure);
	}

	else if (structure->IsA(APPROXIMATION)) {
		CFrameWnd* pNewFrame = theApp.m_ApproximationTemplate->CreateNewFrame(GetDocument(), NULL);            
		theApp.m_ApproximationTemplate->InitialUpdateFrame(pNewFrame, GetDocument());
		static_cast(CViewApproximation *, pNewFrame->GetActiveView())->SetStructure(structure);
	}

	else if (structure->IsA(TEXTFILE)) {
		CFrameWnd* pNewFrame = theApp.m_TextFileTemplate->CreateNewFrame(GetDocument(), NULL);            
		theApp.m_TextFileTemplate->InitialUpdateFrame(pNewFrame, GetDocument());
		static_cast(CViewTextFile *, pNewFrame->GetActiveView())->SetStructure(structure);
	}

	else {
		Message::Debug("Cannot view a " + IdHolder::GetClassname(structure->GetId()) + ".");
	}
	
}

//-------------------------------------------------------------------
// Method  ......: OnRemove
// Author........: J—rn E. Nygjerd/Daniel Remmem
// Date..........: 960628
// Description...: Does the work needed when the user chooses the menuitem
//                 'Remove'. 
// Comments......: This function is called from OnCmdMsg. 
// Revisions.....: Aš 960718 Added querying and associated logic.
//===================================================================

void 
CViewProject::OnRemove(Structure *structure)
{
	
	CRosettaDoc	*pDoc = GetDocument();

	// Valid input?
	if (structure == NULL) {
		Message::Error("Cannot remove a NULL structure.");
		return;
	}

	// Does a parent exist?
	if (structure->FindParent(*GetTopProject()) == NULL) {
		Message::Error("Structure to remove has no parent.");
		return;
	}

	// Check parent type.
	if (!structure->FindParent(*GetTopProject())->IsA(PARENTSTRUCTURE)) {
		Message::Error("Parent of structure to remove cannot manage children.");
		return;
	}

	// Query before proceeding.
	if (MessageHelper::Query("Are you sure you want to remove " + structure->GetName() + "?", "Remove", MessageHelper::BUTTONS_OKCANCEL) == MessageHelper::RESPONSE_CANCEL)
		return;

	bool remove_children;

	// Query further if the structure has children.
	if (structure->HasChildren()) {
		String s = "'s";
		if ((structure->GetName()).Last() == 's')
			s = "'";
		remove_children = (MessageHelper::Query("Do you want to remove all " + structure->GetName() + s + " children recursively?", "Remove", MessageHelper::BUTTONS_YESNO) == MessageHelper::RESPONSE_YES);
	}
	else {
		remove_children = false;
	}
		
	// Cast parent to verified subclass.
	Handle<ParentStructure> parent = dynamic_cast(ParentStructure *, structure->FindParent(*GetTopProject()));					

	bool reassign_ok = true;
	int  i, j;

	// Reassign parent/children pointers if specified.
	if (!remove_children) {
		for (i = 0; i < structure->GetNoChildren(); i++) {
			if (!parent->AppendChild(structure->GetChild(i))) {                              // Shit, undo all previous appends and break.
				Message::Error("An error occurred while reassigning/appending the children."); // Notify the user.
				reassign_ok = false;                                                           // Flag that an reassignment error occurred.
				for (j = 0; j < i; j++) {                                                      // Undo.
					if (!parent->RemoveChild(parent->GetNoChildren() - 1)) {                     // Shit, undo failed.
						Message::Error("Undo failed!");
						break;
					}
				}
				break;                                                                         // Break reassignment loop.
			}
		}
	}

	// Remove views connected to the structure(s) to remove.
	pDoc->RemoveViewsForStructure(structure, remove_children);

	// If reassignment failed, update the tree to reflect the current state, and return.
	if (!reassign_ok) {
		UpdateTree();
		return;
	}

	// Remove children if specified.
	if (remove_children && structure->IsA(PARENTSTRUCTURE)) {
		Handle<ParentStructure> casted = dynamic_cast(ParentStructure *, structure);
		if (!casted->RemoveAllChildren()) {
			Message::Error("An error occurred while removing the children.");
		}
	}

	// Remove the structure.  Handles take care of the physical deletion if needed.
	if (!parent->RemoveChild(parent->FindChild(structure))) {
		Message::Error("An error occurred while removing the structure from its parent.");
	}

	// Things have happened, update the tree.
	UpdateTree();

}

//-------------------------------------------------------------------
// Method  ......: OnDuplicate
// Author........: Daniel Remmem
// Date..........: 960628
// Description...: Does the work needed when the user chooses the menuitem
//                 'Duplicate'. 
// Comments......: This function is called from OnCmdMsg. 
// Revisions.....:
//===================================================================

void 
CViewProject::OnDuplicate(Structure *structure)
{

	// Valid input?
	if (structure == NULL) {
		Message::Error("Cannot duplicate a NULL structure.");
		return;
	}

	// Does a parent exist?
	if (structure->FindParent(*GetTopProject()) == NULL) {
		Message::Error("Structure to duplicate has no parent.");
		return;
	}

	// Check parent type.
	if (!structure->FindParent(*GetTopProject())->IsA(PARENTSTRUCTURE)) {
		Message::Error("Parent of structure to duplicate cannot manage children.");
		return;
	}

	CWaitCursor wait;
	Message     message;

	message.Notify("Duplicating " + structure->GetName() + "...");

	// Cast parent to verified subclass.
	Handle<ParentStructure> parent = dynamic_cast(ParentStructure *, structure->FindParent(*GetTopProject()));

	// Do the duplication.
	Handle<Structure> duplicate = structure->Duplicate();

	// Verify result.
	if (duplicate == NULL) {
		Message::Error("Duplication failed.");
		return;
	}

	// Append the duplicate to the parent.
	parent->AppendChild(duplicate.GetPointer());

	// Update tree.
	UpdateTree();

}

//-------------------------------------------------------------------
// Method  ......: OnStatistics
// Author........: J—rn E. Nygjerd/Daniel Remmem
// Date..........: 960628
// Description...: Does the work needed when the user chooses the menuitem
//                 'Statistics'. 
// Comments......: This function is called from OnCmdMsg. 
// Revisions.....:
//===================================================================

void 
CViewProject::OnStatistics(Structure *structure)
{

	if (structure == NULL)
		return;

	CWaitCursor wait;

	if (structure->IsA(DECISIONTABLE)) {
		CDialogAttributeStatistics dialog;
		dialog.SetDecisionTable(dynamic_cast(DecisionTable *, structure));
		dialog.m_NoAttribute = 0;
		dialog.DoModal();
	}
	
	else if (structure->IsA(REDUCTS)) {
		CDialogReductsStatistics dialog;
		dialog.SetReducts(dynamic_cast(Reducts *, structure));
		dialog.DoModal();	
	}

	//Ewa Makosa
	else if (structure->IsA(GENERALRULES)) {
		CDialogGeneralRulesStatistics dialog;
		dialog.SetRules(dynamic_cast(GeneralRules *, structure));
		dialog.DoModal();	
	}
	
	else if (structure->IsA(RULES)) {
		CDialogRulesStatistics dialog;
		dialog.SetRules(dynamic_cast(Rules *, structure));
		dialog.DoModal();	
	}

	else if (structure->IsA(APPROXIMATION)) {
		CDialogApproximationStatistics dialog;
		dialog.SetApproximation(dynamic_cast(Approximation *, structure));
		dialog.DoModal();	
	}
	
	else {
		Message::Error("Statistics cannot be viewed for this structure ");
	}

}

//-------------------------------------------------------------------
// Method  ......: OnODBCImport
// Author........: Aleksander šhrn
// Date..........: 0797
// Description...: 
// Comments......: This function is called from OnCmdMsg. 
// Revisions.....:
//===================================================================

void 
CViewProject::OnODBCImport(Structure *structure)
{

	if (structure == NULL)
		return;

	// Create an empty decision table to fill.
	Handle<DecisionTable> table = Creator::DecisionTable();

	// Append it to the input structure.
	structure->AppendChild(table.GetPointer());

	// Acquire ODBC import routine.
	Handle<ODBCDecisionTableImporter> odbcimporter = dynamic_cast(ODBCDecisionTableImporter *, ObjectManager::GetIdentifiedAlgorithm(ODBCDECISIONTABLEIMPORTER));

	if (odbcimporter == NULL) {
		Message::Error("Unable to acquire ODBC import routine. Make sure it is installed.");
		return;
	}

	// Use common Apply-code. If application failed, clean up and return.
	if (GetDocument()->ApplyAlgorithm(odbcimporter.GetPointer(), table.GetPointer()) == NULL) {
		structure->RemoveChild(structure->FindChild(table.GetPointer()));
		return;
	}

	GetDocument()->SetModifiedFlag(TRUE);

	// Make sure the project tree is redrawn.
	UpdateTree();

}

//-------------------------------------------------------------------
// Method  ......: OnInformation
// Author........: J—rn E. Nygjerd/Daniel Remmem
// Date..........: 960628
// Description...: Does the work needed when the user chooses the menuitem
//                 'Information'. 
// Comments......: This function is called from OnCmdMsg. 
// Revisions.....:
//===================================================================

void 
CViewProject::OnInformation(Structure *structure)
{

	if (structure == NULL)
		return;
	
	if (structure->IsA(ANNOTATEDSTRUCTURE)) {
		CDialogInformation dialog;
		dialog.SetAnnotatedStructure(dynamic_cast(AnnotatedStructure *, structure));
		dialog.DoModal();
	}
	else {
		Message::Error("Information is only available for annotated structures.");
	}

}

//-------------------------------------------------------------------
// Method  ......: OnRulesClassify
// Author........: J—rn E. Nygjerd/Daniel Remmem
// Date..........: 960628
// Description...: Does the work needed when the user chooses the menuitem
//                 'classify new objects'.
// Comments......: This function is called from OnCmdMsg. 
// Revisions.....:
//===================================================================

void 
CViewProject::OnRulesClassify(Structure *structure)
{
#ifdef _CLASSIFIERGUIDE
	if (structure == NULL)
		return;

	if (!structure->IsA(RULES))
		return;

	Message::Information("The environment for interactive guidance\n"
						 "is only meant as a crude prototype.");

	Handle<Algorithm> algorithm = ObjectManager::GetIdentifiedAlgorithm(MAJORITYVOTER);
	Handle<Project>   project   = dynamic_cast(Project *, structure->FindParent(PROJECT, *GetTopProject()));
  
	CDialogClassify dialog;

	dialog.SetAlgorithm(algorithm.GetPointer(), structure, project.GetPointer());

	dialog.DoModal();
#endif       
}

//-------------------------------------------------------------------
// Method  ......: OnDictionaryExport
// Author........: Aleksander
// Date..........: 
// Description...: 
// Comments......: This function is called from OnCmdMsg. 
// Revisions.....: Aš 000310: Input table instead of dictionary to
//                            exporter algorithm. Call UpdateTree on
//                            success.
//===================================================================

void 
CViewProject::OnDictionaryExport(Structure *structure)
{

	if (structure == NULL)
		return;

	if (!structure->IsA(DECISIONTABLE))
		return;

	// Get table.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, structure);

	if (!table->HasDictionary()) {
		Message::Error("Decision table has no dictionary.");
		return;
	}

	// Get exporter.
	Handle<DictionaryExporter> exporter = dynamic_cast(DictionaryExporter *, ObjectManager::GetIdentifiedAlgorithm(DICTIONARYEXPORTER));

	if (exporter == NULL) {
		Message::Error("No dictionary export algorithm installed.");
		return;
	}

	// Use common Apply-code.
	if (GetDocument()->ApplyAlgorithm(exporter.GetPointer(), table.GetPointer()) != NULL) {
		GetDocument()->SetModifiedFlag(TRUE);
		UpdateTree();
	}
	
}

//-------------------------------------------------------------------
// Method  ......: OnDictionaryImport
// Author........: Aleksander
// Date..........: 
// Description...: 
// Comments......: This function is called from OnCmdMsg. 
// Revisions.....: Aš 000310: Input table instead of dictionary to
//                            importer algorithm.
//===================================================================

void 
CViewProject::OnDictionaryImport(Structure *structure)
{

	if (structure == NULL)
		return;

	if (!structure->IsA(DECISIONTABLE))
		return;

	// Get table.
	Handle<DecisionTable> table = dynamic_cast(DecisionTable *, structure);

	// Get importer.
	Handle<DictionaryImporter> importer = dynamic_cast(DictionaryImporter *, ObjectManager::GetIdentifiedAlgorithm(DICTIONARYIMPORTER));

	if (importer == NULL) {
		Message::Error("No dictionary import algorithm installed.");
		return;
	}

	// Use common Apply-code.
	if (GetDocument()->ApplyAlgorithm(importer.GetPointer(), table.GetPointer()) != NULL) {

		// Set modification flag to trigger possible saving.
		GetDocument()->SetModifiedFlag(TRUE);

		// Update views.
		LPARAM lHint = reinterpret_cast(LPARAM, table.GetPointer());
		CHint  pHint(CHint::HINT_MODIFIED, 0, 0);

		GetDocument()->UpdateAllViews(this, lHint, &pHint);

	}
 
}

//-------------------------------------------------------------------
// Method  ......: OnApply
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Does the work needed when the user chooses the menuitem
//                 'Apply' from an algorithm.
// Comments......: This function is called from OnCmdMsg. 
// Revisions.....:
//===================================================================

void 
CViewProject::OnApply(Algorithm *algorithm)
{

	if (algorithm == NULL)
		return;

	// Get the top level project.
	Handle<Project> project = GetTopProject();

	if (project == NULL)
		return;

	Identifier::Handles structures;

	// Get all structures in project.
	if (!project->GetAllIdentifiers(STRUCTURE, structures))
		return;

	Structure::Handles applicable;
	Vector(String)     names;

	int i;

	// Get all applicable structures.
	for (i = 0; i < structures.size(); i++) {
		Handle<Structure> s = dynamic_cast(Structure *, structures[i].GetPointer());
		if (algorithm->IsApplicable(*s, false)) {
			applicable.push_back(s);
			names.push_back(s->GetName());
		}
	}

	if (applicable.size() == 0) {
		Message::Error("The algorithm isn't applicable to any structures in the project.");
		return;
	}

	Handle<Structure> structure;

	// Determine structure to apply algorithm to.
	if (applicable.size() == 1) {

		structure = applicable[0];

	}
	else {

		CDialogGenericList dialog;

		// Set dialog "input".
		dialog.m_Title   = "Apply";
		dialog.m_Header  = "Select a structure:";
		dialog.m_Entries = names;
			
		// Pop dialog.
		if (dialog.DoModal() != IDOK)
			return;

		// Check dialog "output".
		if (dialog.m_Selection == Undefined::Integer()) {
			Message::Error("No structure selected.");
			return;
		}

		structure = applicable[dialog.m_Selection];

	}

	// Use common Apply-code.
	if (GetDocument()->ApplyAlgorithm(algorithm, structure.GetPointer()) != NULL) {
		GetDocument()->SetModifiedFlag(TRUE);
		UpdateTree();
	}

}

//-------------------------------------------------------------------
// Method  ......: OnSave
// Author........: Aleksander šhrn
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....: 970316 KMR Actual saving moved to CRosettaDoc
//									This is done for transparency between context
//									menu saving, and CDocument IO-management.
//===================================================================

void 
CViewProject::OnSave(Structure *structure) {

	CRosettaDoc *pDoc = GetDocument () ;

	// Valid input?
	if (structure == NULL) {
		Message::Error("Cannot save an invalid structure.");
		return;
	}

	// Is the structure persistent?
	if (!structure->IsA(PERSISTENT)) {
		Message::Error("The structure is not persistent.");
		return;
	}

	// Does the structure have an associated filename?
	if (!structure->IsA(ANNOTATEDSTRUCTURE)) {
		OnSaveAs(structure);
		return;
	}

	Handle<AnnotatedStructure> annotated = dynamic_cast(AnnotatedStructure *, structure);

	// Is the associated filename valid?
	if (annotated->GetAnnotation()->GetFilename() == Undefined::String()) {
		OnSaveAs(structure);
		return;
	}

	CWaitCursor wait;
	Message     message;

	message.Notify("Saving " + structure->GetName() + " to " + annotated->GetAnnotation()->GetFilename() + "...");

	// Do the actual saving.
	pDoc->SaveStructures(annotated->GetAnnotation ()->GetFilename (),structure);

}

//-------------------------------------------------------------------
// Method  ......: OnSaveAs
// Author........: Aleksander šhrn
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....: 970316 KMR Actual saving moved to CRosettaDoc
//									This is done for transparency between context
//									menu saving, and CDocument IO-management.
//===================================================================

void 
CViewProject::OnSaveAs(Structure *structure) {
	
	CRosettaDoc		*pDoc = GetDocument () ;

	// Valid input?
	if (structure == NULL) {
		Message::Error("Cannot save an invalid structure.");
		return;
	}

	// Is the structure persistent?
	if (!structure->IsA(PERSISTENT)) {
		Message::Error("The structure is not persistent.");
		return;
	}

	// Create and display a "File save as..." dialog.
	CFileDialog filedialog(FALSE, NULL, "*.ros");  
	if (filedialog.DoModal() != IDOK)
		return;

	String fullpath = filedialog.GetPathName().GetBuffer (0);

	CWaitCursor wait;
	Message     message;

	message.Notify("Saving " + structure->GetName() + " to " + fullpath + "...");

	// Do the actual saving.
	pDoc->SaveStructures(fullpath, structure);

}

//-------------------------------------------------------------------
// Method  ......: OnLoadChildren
// Author........: Aleksander šhrn
// Date..........: 
// Description...:
// Comments......: Extend to allow multiple file selections.
// Revisions.....: 970316 KMR Actual loading moved to CRosettaDoc
//									This is done for transparency between context
//									menu saving, and CDocument IO-management.
//===================================================================

void 
CViewProject::OnLoadChildren(Structure *structure)
{	

	CRosettaDoc *pDoc = GetDocument();

	// Valid input?
	if (structure == NULL) {
		Message::Error("Cannot load and append children to an invalid structure.");
		return;
	}

	// Can the structure manage children?
	if (!structure->IsA(PARENTSTRUCTURE)) {
		Message::Error("The structure cannot manage children structures.");
		return;
	}

	Handle<ParentStructure> parent = dynamic_cast(ParentStructure *, structure);

	// Create and display a "File open" dialog.
	CFileDialog filedialog(TRUE, NULL, "*.ros");  
	if (filedialog.DoModal() != IDOK)
		return;

	// Determine the name of the associated header file.
	String fullpath = filedialog.GetPathName().GetBuffer(0);

	if (!pDoc->LoadStructures(fullpath, parent))
		return;

	UpdateTree();

}

//-------------------------------------------------------------------
// Method  ......: OnTestStuffDebug
// Author........: Aleksander šhrn
// Date..........: 
// Description...:
// Comments......: 
// Revisions.....:
//===================================================================

void 
CViewProject::OnTestStuffDebug(Structure *structure)
{
#ifdef _DEBUG

	/*
	  Graph g;

	  g.AddNode(1);
	  g.AddNode(2);
	  g.AddNode(3);
	  g.AddNode(4);
	  g.AddNode(5);
	  g.AddNode(6);
	  g.AddNode(7);

	  g.MakeAdjacencyMatrix();

	  g.SetEdgeByValue(1, 2, true);
	  g.SetEdgeByValue(1, 3, true);
	  g.SetEdgeByValue(1, 6, true);
	  g.SetEdgeByValue(5, 6, true);
	  g.SetEdgeByValue(3, 5, true);
	  g.SetEdgeByValue(2, 4, true);
	  g.SetEdgeByValue(4, 7, true);

	  g.MakeReflexive();
	  g.MakeSymmetric();

	  Graph::APSPMatrix distances;

	  g.Floyd(distances);

	  ofstream stream("c:/temp/apsp.txt");

	  int i, j;

	  stream << "APSPMatrix by Floyd:" << endl;
	  stream << endl << "   ";
	  for (i = 0; i < distances.size(); i++)
	  stream << i + 1 << ' ';
	  stream << endl;

	  for (i = 0; i < distances.size(); i++) {
	  stream << i + 1 << ": ";
	  for (j = 0; j < distances.size(); j++) {
	  if (distances[i][j] == Undefined::Integer())
	  stream << "* ";
	  else
	  stream << distances[i][j] << ' ';
	  }
	  stream << endl;
	  }
	  stream << endl;

	  Graph::SPVector dist;

	  for (i = 0; i < distances.size(); i++) {
	  g.Dijkstra(i, dist);
	  stream << "SPVector(" << i + 1 << ") by Dijkstra: ";
	  for (j = 0; j < dist.size(); j++) {
	  if (dist[j] == Undefined::Integer())
	  stream << "* ";
	  else
	  stream << dist[j] << ' ';
	  }
	  stream << endl;
	  }
	*/

	/*
	  Discerner discerner;

	  if (discerner.LoadIDG("s:/datasets/toltest/idg.txt", *dynamic_cast(const DecisionTable *, structure), false)) {
	  for (int i = 0; i < dynamic_cast(const DecisionTable *, structure)->GetNoAttributes(false); i++) {
	  if (discerner.HasIDG(i)) {
	  discerner.GetIDG(i)->Save(String("s:/datasets/toltest/graph") + String::Format(i) + ".txt");
	  }
	  }
	  }
	*/

	/*
	  ofstream stream("c:/temp/nemo.txt");

	  char *alder[]    = {"under30", "over30"};
	  char *kjonn[]    = {"mann", "kvinne"};
	  char *landsdel[] = {"nordnorge", "midtnorge", "vestlandet", "sorostlandet"};
	  char *urban[]    = {"by", "land"};
	  char *inntekt[]  = {"hoy", "middels", "lav"};
	  char *favoritt[] = {"kultur", "innenriks", "utenriks"};

	  int f = 0;

	  for (int a = 0; a < 2; a++) {
	  for (int k = 0; k < 2; k++) {
	  for (int l = 0; l < 4; l++) {
	  for (int u = 0; u < 2; u++) {
	  for (int i = 0; i < 3; i++) {
	  stream << alder[a] << ' ' << kjonn[k] << ' ' << landsdel[l] << ' ' << urban[u] << ' ' << inntekt[i] << ' ';
	  if ((a == 0 && k == 0 && u == 0) || (a == 1 && k == 1 && l == 0))
	  stream << "sport" << endl;
	  else {
	  stream << favoritt[f % 3] << endl;
	  f++;
	  }
	  }
	  }
	  }
	  }
	  }
	*/

	/*
	//Array<char> v(3, 'a');
	Array<int> v;

	//v.Reserve(10);
	v.reserve(10);

	Message::Information("After constructor: v = [" + v.Format() + "]");

	//v.Append('b');
	//v.Append('c');
	//v.Append('d');
	//v.Append('e');
	//v.Append('f');

	//v.push_back('b');
	//v.push_back('c');
	//v.push_back('d');
	//v.push_back('e');
	//v.push_back('f');

	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	v.push_back(5);

	Message::Information("After several Appends: v = [" + v.Format() + "]");

	//v.Remove(1, 3);
	v.erase(v.begin() + 1, v.begin() + 3);

	Message::Information("After Remove(1, 3): v = [" + v.Format() + "]");

	//v.Insert(0, 'q');
	//v.insert(v.begin(), 'q');
	v.insert(v.begin(), 1970);

	Message::Information("After Insert(0, 'q'): v = [" + v.Format() + "]");

	Array<int> w(5, 70);

	Message::Information("After constructor: w = [" + w.Format() + "]");

	v.insert(v.end(), w.begin(), w.end());

	Message::Information("After Insert(w): v = [" + v.Format() + "]");
	*/

	/*
	  Vector(String) filenames;
	  filenames.push_back("s:/tmp/sorted_big.txt");
	  filenames.push_back("s:/tmp/sorted_small.txt");

	  HanleyMcNeilComparator hmc(filenames);
	  hmc.Summarize();

	  McNemarComparator mnc(filenames, 0.25, 0.30);
	  mnc.Summarize();
	*/

	/*
	  Bits b(String("0110001"));
	  Bits a(b);
	  a.Invert();
	  Bits c(b);
	  c.Reverse();

	  Message::Information("b  = " + b.Format() + ", b.count(1) = " + String::Format(b.Count(true)) + ", b.count(0) = " + String::Format(b.Count(false)) + ".\n" +
	  "a  = " + a.Format() + ", a.count(1) = " + String::Format(a.Count(true)) + ", a.count(0) = " + String::Format(a.Count(false)) + ".\n" +
	  "c  = " + c.Format() + ", c.count(1) = " + String::Format(c.Count(true)) + ", c.count(0) = " + String::Format(c.Count(false)) + ".\n");
	*/

	/*
	  DiscernibilityMatrix matrix;
	  Discerner discerner;

	  if (!matrix.Create(*dynamic_cast(const DecisionTable *, structure), true, true, discerner))
	  Message::Error("Matrix construction failed.");
	  ofstream stream1("c:/temp/matrix.modulo.txt");
	  if (!matrix.Save(stream1))
	  Message::Error("Matrix saving failed.");

	  if (!matrix.Create(*dynamic_cast(const DecisionTable *, structure), true, false, discerner))
	  Message::Error("Matrix construction failed.");
	  ofstream stream2("c:/temp/matrix.txt");
	  if (!matrix.Save(stream2))
	  Message::Error("Matrix saving failed.");
	*/

	/*
	  Graph graph;

	  if (!graph.Load("c:/temp/graph.in.txt", dynamic_cast(const DecisionTable *, structure)))
	  Message::Error("Graph load failed.");
	
	  if (!graph.Save("c:/temp/graph.out.txt", dynamic_cast(const DecisionTable *, structure)))
	  Message::Error("Graph save failed.");
	*/

#endif
}


 
