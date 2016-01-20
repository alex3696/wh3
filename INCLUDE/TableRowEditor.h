#ifndef __TABLEROWEDITOR_H
#define __TABLEROWEDITOR_H

#include "MTable.h"

namespace wh{
//-----------------------------------------------------------------------------
class TableRowEditor
{
public:
	virtual int ShowModal() = 0;
	virtual void SetModel(std::shared_ptr<ITableRow>& model) = 0;
	virtual void DataFromWindow() = 0;
	virtual void DataToWindow() = 0;

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


	virtual void SetModel(std::shared_ptr<ITableRow>& model)override;
	virtual void DataFromWindow() override;
	virtual void DataToWindow() override;
	virtual int ShowModal()override { return wxDialog::ShowModal(); }

	void SetData(const TableRowData& data);
	void GetData(TableRowData& data)const;
private:
	void OnChangeModel(const IModel* model, const TableRowData* data);

	wxPropertyGrid*				mPropGrid = nullptr;
	std::shared_ptr<ITableRow>	mModel;
	sig::scoped_connection		mChangeConnection;

};//-----------------------------------------------------------------------------


}//namespace wh
#endif //__*_H