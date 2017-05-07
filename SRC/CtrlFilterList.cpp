#include "_pch.h"
#include "CtrlFilterList.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CtrlFilterList::CtrlFilterList(const std::shared_ptr<IViewFilterList>& view
	, const std::shared_ptr<ModelFilterList>& model)
	: CtrlWindowBase(view, model)
{
	namespace ph = std::placeholders;
	connViewCmd_Update = mView->sigUpdate
		.connect(std::bind(&CtrlFilterList::Update, this, ph::_1));

	connViewCmd_Update = mView->sigUpdateAll
		.connect(std::bind(&CtrlFilterList::UpdateAll, this));

	connViewCmd_Apply = mView->sigApply
		.connect(std::bind(&CtrlFilterList::Apply, this));

	connViewCmd_ViewToModel = mView->sigUpdateFilter
		.connect(std::bind(&CtrlFilterList::UpdateFilter, this
		, ph::_1, ph::_2, ph::_3, ph::_4, ph::_5));


	connModel_Update = mModel->sigUpdate.connect
		([this](const std::vector<NotyfyItem>& data)
	{
		mView->Update(data);
	});



}

//---------------------------------------------------------------------------
void CtrlFilterList::Update(const std::vector<NotyfyItem>& data)
{
	mModel->sigUpdate(data);
}
//---------------------------------------------------------------------------
void CtrlFilterList::UpdateAll()
{
	std::vector<NotyfyItem> list;
	mModel->sigUpdate(list);
	for (size_t i = 0; i < mModel->size(); ++i)
	{
		const std::shared_ptr<const ModelFilter>& new_item = mModel->at(i);
		std::shared_ptr<const ModelFilter> old_item;
		NotyfyItem ni = NotyfyItem(old_item, new_item);
		list.emplace_back(ni);
	}
	if (mModel->size())
		mModel->sigUpdate(list);

}
//---------------------------------------------------------------------------
void CtrlFilterList::UpdateFilter(const wxString& title, const wxString& sys_title
	, FilterOp op, FieldType type
	, const std::vector<wxString>& val)
{
	mModel->UpdateFilter(title, sys_title, op, type, val);
}
//---------------------------------------------------------------------------
void CtrlFilterList::Apply()
{
	mModel->Apply();
}