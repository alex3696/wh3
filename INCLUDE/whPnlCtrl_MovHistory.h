#ifndef __WHPNLCTRL_MOVHISTORY_H
#define __WHPNLCTRL_MOVHISTORY_H

#include "whViewCtrl_Sorter.h"
#include "whViewCtrl_Filter.h"
#include "whViewCtrl_MovHistory.h"
//--------------------------------------------------------------------------
class whPnlCtrl_MovHistory
	: public wxPanel
	, public ctrlWithResMgr
{
private:
	///������ � ������� � ������� �������
	whLogQueryData	m_LogQueryData;

	
	wxAuiManager			m_AuiMgr;
	wxAuiToolBar*			m_ToolBar;

	whViewCtrl_Sorter*		m_ctrlSorter;
	whViewCtrl_MovHistory*	m_View;
	
	//wxBoxSizer*		szrFilterMain;
	//wxPanel*		pnlFilter;

	void InitToolBar();
	void InitFilterBar();
	void InitDataView();

	// ������ ��� ��� ������������ ������ ��������
	void CreateFilterGroupGUI(unsigned int gid);
	/// ������ ��� ������ ��������
	void ClearFilterGroup();
	// ��������� ������ �������� ������
	void OnCreateFilterGroup(wxCommandEvent& evt=wxCommandEvent() );
	// ��������� ������ �������� � ������������ � �������
	void OnPaneClose(wxAuiManagerEvent& evt);
	// ��������� ������ � �������� �������
	void OnUpdateData(wxCommandEvent& evt=wxCommandEvent() );
	// ���������� ��� ���/����
	void OnMore(wxCommandEvent& evt=wxCommandEvent() );
public:
	

	whPnlCtrl_MovHistory( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, 
							const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
	~whPnlCtrl_MovHistory();


	whLogQueryData* GetLogQueryData()	{return &m_LogQueryData;}
	
	void SetLogQueryData(const whLogQueryData& data);
	whLogQueryData* SetLogQueryDataDefault();

	// ���������� �������� ������� � ���� ������
	int ConnectDB(whDB *db)	{	return m_View->ConnectDB(db);	}
	
	// ��������� ������ � �������� �������
	void UpdateData();
	void UpdateView();

	DECLARE_EVENT_TABLE()
};




#endif // __WHPNLCTRL_MOVHISTORY_H