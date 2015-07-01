#ifndef __WHVIEWCTRL_SORTER_H
#define __WHVIEWCTRL_SORTER_H

#include "_pch.h"
#include "ResManager.h" 
#include "whTableMgr.h"
#include "wxDataViewRenderer.h"
//---------------------------------------------------------------------------
class whViewCtrl_Sorter: public wxDataViewCtrl, public ctrlWithResMgr
{
private:
//	whModelGui_Sorter* m_Model;

	void OnBeginDrag	( wxDataViewEvent &event );
	void OnDropPossible	( wxDataViewEvent &event );
	void OnDrop			( wxDataViewEvent &event );
	void OnActivated	( wxDataViewEvent &event );
public:

	whViewCtrl_Sorter (wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, 
		const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator);
	~whViewCtrl_Sorter();

	void SetLogQueryData(whLogQueryData* qdata);

};



#endif // __WHVIEWCTRL_SORTER_H