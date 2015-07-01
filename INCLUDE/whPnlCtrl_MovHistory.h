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
	///данные о запросе к таблице истории
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

	// создаём гуй для существующей группы фильтров
	void CreateFilterGroupGUI(unsigned int gid);
	/// Убрать все группы фильтров
	void ClearFilterGroup();
	// добавляем группу фильтров фильтр
	void OnCreateFilterGroup(wxCommandEvent& evt=wxCommandEvent() );
	// закрываем группу фильтров и одновременно её удаляем
	void OnPaneClose(wxAuiManagerEvent& evt);
	// обновляем данные в табличке истории
	void OnUpdateData(wxCommandEvent& evt=wxCommandEvent() );
	// Компактный вид ВКЛ/ВЫКЛ
	void OnMore(wxCommandEvent& evt=wxCommandEvent() );
public:
	

	whPnlCtrl_MovHistory( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, 
							const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
	~whPnlCtrl_MovHistory();


	whLogQueryData* GetLogQueryData()	{return &m_LogQueryData;}
	
	void SetLogQueryData(const whLogQueryData& data);
	whLogQueryData* SetLogQueryDataDefault();

	// подключаем табличку истории к базе данных
	int ConnectDB(whDB *db)	{	return m_View->ConnectDB(db);	}
	
	// обновляем данные в табличке истории
	void UpdateData();
	void UpdateView();

	DECLARE_EVENT_TABLE()
};




#endif // __WHPNLCTRL_MOVHISTORY_H