#ifndef __DCLSACTEDITOR_H
#define __DCLSACTEDITOR_H

#include "MTypeNode.h"
#include "MAct.h"
#include "MGroup.h"
#include "MClsAct.h"
#include "TPresenter.h"
#include "TViewCtrlPanel.h"
#include "PathPatternEditor.h"

namespace wh{
namespace view{
//-----------------------------------------------------------------------------
/// Редактор для свойства действия
class DClsActEditor
	: public wxDialog
	, public T_View
{
public:
	DClsActEditor(wxWindow* parent = nullptr,
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
	void GetData(rec::ClsActAccess& rec) const;
	void SetData(const rec::ClsActAccess& rec);

	void OnChangeModel(const IModel* model, const MClsAct::T_Data* data);

	// графические компоненты свеху вниз
	PathPatternEditor*		mPathEditor;
	wxPropertyGrid*			mPropGrid;
	wxButton*				m_btnOK;
	wxButton*				m_btnCancel;
	wxStdDialogButtonSizer*	m_sdbSizer;

	// модели
	/// Модель свойства класса
	std::shared_ptr<MClsAct>		mModel;
	/// Модель для массива шаблона-пути
	std::shared_ptr<temppath::model::Array>	mPatternPath;
	
	// сигналы
	sig::scoped_connection			mChangeConnection;
	
	// TODO: заменить на диалоги групп и действий
	std::shared_ptr<MActArray>		mActArray;// каталог действий
	std::shared_ptr<MGroupArray>	mGroupArray;// каталог групп

};
//-----------------------------------------------------------------------------	



class DActSelector
	: public view::DlgBaseOkCancel
{
public:
	DActSelector(wxWindow* parent = nullptr)
		:DlgBaseOkCancel(parent)
	{
		mArrayView = new TViewTable(this);
		GetSizer()->Insert(0, mArrayView, 1, wxALL | wxEXPAND, 0);
		Layout();

		Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &DActSelector::OnActivate, this);
	}

	void SetSrcVec(std::shared_ptr<MActArray>& propArray)
	{
		mActArray = propArray;
		if (mActArray)
		{
			mArrayView->SetModel(std::dynamic_pointer_cast<IModel>(mActArray));
		}

	}
	void			GetSelections(wxDataViewItemArray& array)
	{
		mArrayView->GetSelections(array);
	}
	unsigned int	GetRow(const wxDataViewItem &item) const
	{
		return mArrayView->GetRow(item);
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

	std::shared_ptr<MActArray>		mActArray;
	TViewTable*						mArrayView;
};


//-----------------------------------------------------------------------------	



class DGroupSelector
	: public view::DlgBaseOkCancel
{
public:
	DGroupSelector(wxWindow* parent = nullptr)
		:DlgBaseOkCancel(parent)
	{
		mArrayView = new TViewTable(this);
		GetSizer()->Insert(0, mArrayView, 1, wxALL | wxEXPAND, 0);
		Layout();

		Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &DGroupSelector::OnActivate, this);
	}

	void SetSrcVec(std::shared_ptr<MGroupArray>& groupArray)
	{
		mGroupArray = groupArray;
		if (groupArray)
		{
			mArrayView->SetModel(std::dynamic_pointer_cast<IModel>(mGroupArray));
		}

	}
	void			GetSelections(wxDataViewItemArray& array)
	{
		mArrayView->GetSelections(array);
	}
	unsigned int	GetRow(const wxDataViewItem &item) const
	{
		return mArrayView->GetRow(item);
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

	std::shared_ptr<MGroupArray>	mGroupArray;
	TViewTable*						mArrayView;
};





}//namespace view{
}//namespace wh{
#endif // __****_H