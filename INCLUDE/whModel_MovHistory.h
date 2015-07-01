#ifndef __WHMODEL_MOVHISTORY_H
#define __WHMODEL_MOVHISTORY_H

#include "_pch.h"
#include "whDB.h"
#include "whTableMgr.h"
#include "ResManager.h" 

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class whModel_MovHistory: public wxDataViewVirtualListModel, public ctrlWithResMgr
{
protected:
	whLogQueryData*		m_QData;
	whTable*			m_Table;
	bool				m_ViewCompact;
public:
	whModel_MovHistory();
	~whModel_MovHistory();

	// implementation of base class virtuals to define model
	/*virtual*/ unsigned int	GetColumnCount() const;
    /*virtual*/ wxString		GetColumnType( unsigned int col ) const;
    
	/*virtual*/ unsigned int	GetRowCount()const;
	//unsigned int	GetRowCount(){return 0;}

    /*virtual*/ void			GetValueByRow( wxVariant &variant,unsigned int row, unsigned int col ) const;
    /*virtual*/ bool			GetAttrByRow( unsigned int row, unsigned int col,wxDataViewItemAttr &attr ) const;
    /*virtual*/ bool			SetValueByRow( const wxVariant &variant,unsigned int row, unsigned int col );

	int			Refresh();
	wxString	GetColumnName(unsigned int nCol);

	void SetDB(whDB* db)				{	m_Table->SetDB(db);	}


	void SetLogQueryData(whLogQueryData* qdata)
	{
		m_QData = qdata;
	}

	DEFINE_GETACESSOR(bool,ViewCompact,m_ViewCompact); 
	DEFINE_SETACESSOR(bool,ViewCompact,m_ViewCompact); 

	
};






#endif // __WHMODEL_MOVHISTORY_H