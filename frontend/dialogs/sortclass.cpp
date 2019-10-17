//-------------------------------------------------------------------
// Author........: Aleksander šhrn
// Date..........: 980423
// Description...: Found on the 'net, made some modifications.
// Revisions.....:
//===================================================================

#include <stdafx.h> // CHANGE

#include <frontend/dialogs/sortclass.h>
#include <kernel/basic/string.h>

//-------------------------------------------------------------------
// Static stuff (file scope).
//===================================================================

//-------------------------------------------------------------------
// Method........: StaticDetectType
// Author........: Aleksander šhrn
// Date..........: 
// Description...: Runs through the specifiec columns of the list
//                 control and returns (in-place) type flags.
// Comments......: 
// Revisions.....:
//===================================================================

static void
StaticDetectType(const CListCtrl &pWnd, int iCol, bool &bIsNumeric, bool &bIsInteger) {

	bIsNumeric = false;
	bIsInteger = true;

  String text;

	int no_items = pWnd.GetItemCount();
	int i;

	for (i = 0; i < no_items; i++) {
    text = pWnd.GetItemText(i, iCol).GetBuffer(0);
		if (!text.IsFloat()) {
			bIsNumeric = false;
			bIsInteger = false;
			return;
		}
		if (!text.IsInteger())
			bIsInteger = false;
  }

	bIsNumeric = true;

}

//-------------------------------------------------------------------
// Methods for class CSortClass.
//===================================================================

//-------------------------------------------------------------------
// Method........: Initialize
// Author........: 
// Date..........: 
// Description...: Called by constructors.
// Comments......: 
// Revisions.....:
//===================================================================

void
CSortClass::Initialize(CListCtrl *_pWnd, const int _iCol, const bool _bIsNumeric, const bool _bIsInteger) {

	iCol       = _iCol;
	pWnd       = _pWnd;
	bIsNumeric = _bIsNumeric;
	bIsInteger = _bIsInteger;
        
	ASSERT(pWnd);

	int no_items = pWnd->GetItemCount();

	DWORD   dw;
	CString txt;
	int     i;

	for (i = 0; i < no_items; i++) {
		dw  = pWnd->GetItemData(i);
		txt = pWnd->GetItemText(i, iCol);
		if (bIsNumeric) {
			if (bIsInteger)
				pWnd->SetItemData(i, (DWORD) new CSortItemInt(dw, txt));
			else
				pWnd->SetItemData(i, (DWORD) new CSortItemDouble(dw, txt));
		}
		else {
			pWnd->SetItemData(i, (DWORD) new CSortItem(dw, txt));
		}
	}

}

//-------------------------------------------------------------------
// Method........: Constructors
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

CSortClass::CSortClass(CListCtrl *_pWnd, const int _iCol) {
	ASSERT(pWnd);
	StaticDetectType(*_pWnd, _iCol, bIsNumeric, bIsInteger);
	Initialize(_pWnd, _iCol, bIsNumeric, bIsInteger);
}

CSortClass::CSortClass(CListCtrl * _pWnd, const int _iCol, const bool _bIsNumeric, const bool _bIsInteger) {
	Initialize(_pWnd, _iCol, _bIsNumeric, _bIsInteger);
}

//-------------------------------------------------------------------
// Method........: Destructor
// Author........: 
// Date..........: 
// Description...: Inverse of Initialize.
// Comments......: 
// Revisions.....:
//===================================================================

CSortClass::~CSortClass() {

	ASSERT(pWnd);

	int no_items = pWnd->GetItemCount();
	int i;

  if (bIsNumeric) {
		if (bIsInteger) {
			CSortItemInt *pItem;
			for (i = 0; i < no_items; i++) {
				pItem = (CSortItemInt *) pWnd->GetItemData(i);
				ASSERT(pItem);
				pWnd->SetItemData(i, pItem->dw);
				delete pItem;
			}
		}
		else {
			CSortItemDouble *pItem;
			for (i = 0; i < no_items; i++) {
				pItem = (CSortItemDouble *) pWnd->GetItemData(i);
				ASSERT(pItem);
				pWnd->SetItemData(i, pItem->dw);
				delete pItem;
			}
		}
	}
	else {
		CSortItem *pItem;
		for (i = 0; i < no_items; i++) {
			pItem = (CSortItem *) pWnd->GetItemData(i);
			ASSERT(pItem);
			pWnd->SetItemData(i, pItem->dw);
			delete pItem;
		}
	}

}

//-------------------------------------------------------------------
// Method........: Sort
// Author........: 
// Date..........: 
// Description...: Calls CListCtrl::SortItems with suitable
//                 comparator function.
// Comments......: 
// Revisions.....:
//===================================================================

void
CSortClass::Sort(const bool bAscending) {

	if (bIsNumeric)	{
		if (bIsInteger) {
			if (bAscending)
				pWnd->SortItems(CompareAscI, 0L);
			else
				pWnd->SortItems(CompareDesI, 0L);
		}
		else {
			if (bAscending)
				pWnd->SortItems(CompareAscD, 0L);
			else
				pWnd->SortItems(CompareDesD, 0L);
		}
	}
	else {
		if (bAscending)
			pWnd->SortItems(CompareAsc, 0L);
		else
			pWnd->SortItems(CompareDes, 0L);
	}

}

//-------------------------------------------------------------------
// Method........: CompareAsc etc.
// Author........: 
// Date..........: 
// Description...: Comparator functions.
// Comments......: 
// Revisions.....:
//===================================================================

int CALLBACK CSortClass::CompareAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	CSortItem *i1 = (CSortItem *) lParam1;
	CSortItem *i2 = (CSortItem *) lParam2;
	ASSERT(i1 && i2);
	return i1->txt.CompareNoCase(i2->txt);
}

int CALLBACK CSortClass::CompareDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	CSortItem *i1 = (CSortItem *) lParam1;
	CSortItem *i2 = (CSortItem *) lParam2;
	ASSERT(i1 && i2);
	return i2->txt.CompareNoCase(i1->txt);
}

int CALLBACK CSortClass::CompareAscI(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	CSortItemInt *i1 = (CSortItemInt *) lParam1;
	CSortItemInt *i2 = (CSortItemInt *) lParam2;
	ASSERT(i1 && i2);
	if (i1->iInt == i2->iInt) 
		return 0;
	return i1->iInt > i2->iInt ? 1 : -1;
}

int CALLBACK CSortClass::CompareDesI(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	CSortItemInt *i1 = (CSortItemInt *) lParam1;
	CSortItemInt *i2 = (CSortItemInt *) lParam2;
	ASSERT(i1 && i2);
	if (i1->iInt == i2->iInt) 
		return 0;
	return i1->iInt < i2->iInt ? 1 : -1;
}

int CALLBACK CSortClass::CompareAscD(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	CSortItemDouble *i1 = (CSortItemDouble *) lParam1;
	CSortItemDouble *i2 = (CSortItemDouble *) lParam2;
	ASSERT(i1 && i2);
	if (i1->dDouble == i2->dDouble) 
		return 0;
	return i1->dDouble > i2->dDouble ? 1 : -1;
}

int CALLBACK CSortClass::CompareDesD(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	CSortItemDouble *i1 = (CSortItemDouble *) lParam1;
	CSortItemDouble *i2 = (CSortItemDouble *) lParam2;
	ASSERT(i1 && i2);
	if (i1->dDouble == i2->dDouble) 
		return 0;
	return i1->dDouble < i2->dDouble ? 1 : -1;
}

//-------------------------------------------------------------------
// Method........: Helper class constructors/destructors.
// Author........: 
// Date..........: 
// Description...: 
// Comments......: 
// Revisions.....:
//===================================================================

CSortClass::CSortItem::CSortItem(const DWORD _dw, const CString &_txt) {
	dw  = _dw;
	txt = _txt;
}

CSortClass::CSortItem::~CSortItem() {
}

CSortClass::CSortItemInt::CSortItemInt(const DWORD _dw, const CString &_txt) {
	iInt = atoi(_txt);
	dw   = _dw;
}

CSortClass::CSortItemDouble::CSortItemDouble(const DWORD _dw, const CString &_txt) {
	dDouble = atof(_txt);
	dw      = _dw;
}
 
