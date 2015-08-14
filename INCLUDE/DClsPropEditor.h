#ifndef __DCLSPROPEDITOR_H
#define __DCLSPROPEDITOR_H

#include "MProp.h"
#include "MClsProp.h"
#include "TPresenter.h"
#include "TViewCtrlPanel.h"

namespace wh{
namespace view{
//-----------------------------------------------------------------------------
/// Редактор для свойства действия
class DClsPropEditor
	: public view::DlgBaseOkCancel
	, public T_View
{
public:
	DClsPropEditor(wxWindow* parent = nullptr,
		wxWindowID id = wxID_ANY,
		const wxString& title = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxSize(400, 300),//wxDefaultSize,
		long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
		const wxString& name = wxDialogNameStr);

	virtual void SetModel(std::shared_ptr<IModel>& model)override;
	virtual void UpdateModel()const override;
	virtual int  ShowModal() override;
private:
	void GetData(rec::ClsProp& rec) const;
	void SetData(const rec::ClsProp& rec);

	virtual void OnChangeModel(const IModel& model) override;

	wxPropertyGrid*					mPropGrid;

	// каталог свойств
	std::shared_ptr<MPropArray>		mPropArray;

	/// Модель свойства класса
	std::shared_ptr<MClsProp>		mModel; // std::shared_ptr<MActProp>
	scoped_connection				mChangeConnection;
};
//-----------------------------------------------------------------------------	


class DPropSelector
	: public view::DlgBaseOkCancel
{
public:
	DPropSelector(wxWindow* parent = nullptr)
		:DlgBaseOkCancel(parent)
	{
		mPropArrayView = new TViewTable(this);
		GetSizer()->Insert(0, mPropArrayView, 1, wxALL | wxEXPAND, 0);
		Layout();

		Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &DPropSelector::OnActivate,this);
	}

	void SetSrcVec(std::shared_ptr<MPropArray>& propArray)
	{
		mPropArray = propArray;
		if(mPropArray)
		{ 
			mPropArrayView->SetModel(std::dynamic_pointer_cast<IModel>(mPropArray));
		}
		
	}
	void			GetSelections(wxDataViewItemArray& array)
	{
		mPropArrayView->GetSelections(array);
	}
	unsigned int	GetRow(const wxDataViewItem &item) const
	{
		return mPropArrayView->GetRow(item);
	}

	void OnActivate(wxCommandEvent& evt)
	{
		if (!IsModal())
		{
			SetReturnCode(wxID_OK);
			this->Show(false);
		}
		else
			EndModal(wxID_OK);
	}
private:

	typedef view::TViewTable	PropTable;

	std::shared_ptr<MPropArray>		mPropArray;
	TViewTable*						mPropArrayView;
};




}//namespace view{
}//namespace wh{
#endif // __****_H