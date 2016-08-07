#ifndef __TABLEROWEDITOR_H
#define __TABLEROWEDITOR_H

#include "MTable.h"

namespace wh{
//-----------------------------------------------------------------------------
class TableRowEditor
{
public:
	virtual int ShowModal() = 0;
	virtual void SetModel(std::shared_ptr<IModel>& model) = 0;
};
//-----------------------------------------------------------------------------
class TableRowPGDefaultEditor
	: public wxDialog
	, public TableRowEditor
{
public:
	TableRowPGDefaultEditor(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);


	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual int ShowModal()override 
	{ 
		return wxDialog::ShowModal(); 
	}

	void SetData(const TableRowData& data);
	void GetData(TableRowData& data)const;
private:
	void OnChangeModel(const IModel* model, const TableRowData* data);

	void OnCmdCancel(wxCommandEvent& WXUNUSED(evt));
	void OnCmdOk(wxCommandEvent& WXUNUSED(evt));

	wxPropertyGrid*				mPropGrid = nullptr;
	std::shared_ptr<ITableRow>	mModel;
	sig::scoped_connection		mChangeConnection;

};//-----------------------------------------------------------------------------

wxArrayString Sql2ArrayString(const wxString& sql_value);
wxString ArrayString2Sql(const wxArrayString& array_string_value);

bool Sql2Bool(const wxString& sql_string);
wxString Bool2Sql(bool bool_value);


}//namespace wh
#endif //__*_H