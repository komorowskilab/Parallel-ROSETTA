// HMXDataset.cpp: implementation of the CHMXDataset class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// #include "TestChart.h"
#include "HMXDataset.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHMXDataset::CHMXDataset()
{
	m_nSize  = 2;
	m_nStyle = HMX_DATASET_STYLE_LINE;
	m_clr    = RGB( 0, 0, 0);
}

CHMXDataset::~CHMXDataset()
{

}

bool CHMXDataset::SetData( double nData )
{
	m_data.Add( nData );

	return true;
}

bool CHMXDataset::SetData( int nIndex, double nData )
{
	if( nIndex < 0 || nIndex > GetDatasetSize() )
		return false;
	
	m_data.SetAt( nIndex, nData );

	return true;
}


int CHMXDataset::GetDatasetSize()
{
	return m_data.GetSize();
}

bool CHMXDataset::GetData(int nCount, double &nSample)
{
	int nDatasetSize = GetDatasetSize();
	
	if( nCount < 0 || nCount >= nDatasetSize )
		return false;
	
	nSample = m_data.GetAt(nCount);
	
	return true;
}

bool CHMXDataset::SetStyle(int nStyle)
{
	m_nStyle = nStyle;
	
	return true;
}

bool CHMXDataset::SetColor(COLORREF clr)
{
	m_clr = clr;

	return true;
}

bool CHMXDataset::SetMarker(int nMarker)
{
	m_nMarker = nMarker;

	return true;
}

int CHMXDataset::GetMarker()
{
	return m_nMarker;
}

bool CHMXDataset::SetSize(int nSize)
{
	
	// between 1 an 10	
	m_nSize = min( nSize, 10 );
	m_nSize = max( m_nSize,  1 );

	return true;
}

bool CHMXDataset::GetMinMax(double& nMin, double& nMax)
{
	double min, max, temp;

	if( GetDatasetSize() > 0 ) {
		GetData( 0, temp);
		// following lines help me to solve some problems with invalid values
		if( temp == HMX_DATASET_VALUE_INVALID ) {
			min = HMX_DATASET_VALUE_INVALID;
			max = -HMX_DATASET_VALUE_INVALID;
		}
		for( int f=0; f<GetDatasetSize(); f++ ) {
			GetData( f, temp );
			if( temp != HMX_DATASET_VALUE_INVALID ) {
				if( temp < min )
					min = temp;
				if( temp > max )
					max = temp;
			}
		}
		nMin = min;
		nMax = max;
		return true;
	} else
		return false;
}

int CHMXDataset::GetSize()
{
	return m_nSize;
}

COLORREF CHMXDataset::GetColor()
{
	return m_clr;
}

int CHMXDataset::GetStyle()
{
	return m_nStyle;
}
 
