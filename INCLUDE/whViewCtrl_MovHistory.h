#ifndef __WHVIEWCTRL_MOVHISTORY_H
#define __WHVIEWCTRL_MOVHISTORY_H

#include "_pch.h"
#include "whModel_MovHistory.h" 



class whViewCtrl_MovHistory: public wxDataViewCtrl, public ctrlWithResMgr
{
private:
	whModel_MovHistory*		m_Model;
	whLogQueryData*			m_QData;
public:

	whViewCtrl_MovHistory (wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, 
		const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator);

	// virtual_destr 
	~whViewCtrl_MovHistory();

	int ConnectDB(whDB *db);

	void UpdateData();
	void UpdateView();

	
	void SetLogQueryData(whLogQueryData* qdata)
	{
		m_QData = qdata;
		m_Model->SetLogQueryData(m_QData);
	}

	inline void SetViewCompact(bool data)	{	m_Model->SetViewCompact(data);	}
	inline bool GetViewCompact()			{	return m_Model->GetViewCompact();	}


};

#endif // __WHVIEWCTRL_MOVHISTORY_H
