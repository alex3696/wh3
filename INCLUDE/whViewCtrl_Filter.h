#ifndef __WHVIEWCTRL_FILTER_H
#define __WHVIEWCTRL_FILTER_H

#include "_pch.h"
#include "ResManager.h" 
#include "whTableMgr.h"
#include "wxDataViewRenderer.h"

//---------------------------------------------------------------------------
class whViewCtrl_Filter: public wxDataViewCtrl, public ctrlWithResMgr
{
	unsigned int		m_GroupID;
	whLogQueryData*		m_LogQueryData;
	
	whListBoxSelectTextDialog* m_FieldDlg;

	void OnActivated	( wxDataViewEvent &event );
	
	
public:

	whViewCtrl_Filter (wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, 
		const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator);
	~whViewCtrl_Filter();

	void SetLogQueryData(whLogQueryData* qdata,unsigned int groupid);
	void UpdateView();
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// whPanel_Filter
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** ������ �������*/
class  whPanel_Filter: public wxPanel,public ctrlWithResMgr
{
protected:
	/// ��������� ���������
	enum
	{
		whID_TBTN_ADDFILTER= 1000,
		whID_TBTN_DELFILTER,
	};

	/// ������ ��������
	unsigned int		m_GroupID;
	/// ������ ��������
	whLogQueryData*		m_LogQueryData;
	
	/// ������� �� ������� ��������
	whViewCtrl_Filter*	m_FilterList;
	/// ������ � �������� ���������� � �������� �������
	wxToolBar*			m_ToolBar;
	
	virtual void OnAddFilter( wxCommandEvent& event );
	virtual void OnDelFilter( wxCommandEvent& event );

public:
	whPanel_Filter( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
	~whPanel_Filter();

	void SetLogQueryData(whLogQueryData* qdata,unsigned int groupid);

};



#endif //__WHVIEWCTRL_FILTER_H