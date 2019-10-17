//-------------------------------------------------------------------
// Author........: Aleksander Ûhrn
// Date..........: 980423
// Description...: Used for sorting CListCtrl objects.
//                 Found on the 'net.
//                 Made some modifications, generalized.
// Revisions.....:
//===================================================================

#ifndef __SORTCLASS_H__
#define __SORTCLASS_H__

//-------------------------------------------------------------------
// Class.........: CSortClass
// Author........: Aleksander Ûhrn
// Date..........: 
// Description...: 
// Comments......: http://www.codeguru.com/listview/sort_generic_callback.shtml
// Revisions.....:
//===================================================================

class CSortClass {
protected:

	//- Initialization.................................................
	void Initialize(CListCtrl *_pWnd, const int _iCol, const bool _bIsNumeric, const bool _bIsInteger);

public:

	//- Public members.................................................
	int        iCol;
	CListCtrl *pWnd;
	bool       bIsNumeric;
	bool       bIsInteger;

public:

	//- Constructors/destructor........................................
	CSortClass(CListCtrl *_pWnd, const int _iCol);
	CSortClass(CListCtrl *_pWnd, const int _iCol, const bool _bIsNumeric, const bool _bIsInteger = true);
	virtual ~CSortClass();

	//- Sorting method.................................................
	void Sort(const bool bAscending);
        
	//- Callback functions.............................................
	static int CALLBACK CompareAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareDes(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
        
	static int CALLBACK CompareAscI(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareDesI(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	static int CALLBACK CompareAscD(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareDesD(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

public:

	//- Helper class...................................................
	class CSortItem {
	public:

		//- Destructor...................................................
		virtual  ~CSortItem();

		//- Sorting method...............................................
		CSortItem(const DWORD _dw, const CString &_txt);

		//- Public members...............................................
		CString txt;
		DWORD   dw;

	};

	//- Helper class...................................................
	class CSortItemInt {
	public:

		//- Sorting method...............................................
		CSortItemInt(const DWORD _dw, const CString &_txt);

		//- Public members...............................................
		int   iInt;
		DWORD dw;


	};

	//- Helper class...................................................
	class CSortItemDouble {
	public:

		//- Sorting method...............................................
		CSortItemDouble(const DWORD _dw, const CString &_txt);

		//- Public members...............................................
		double dDouble;
		DWORD  dw;

	};

};

#endif
 
