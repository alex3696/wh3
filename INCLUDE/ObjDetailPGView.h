#ifndef __DETAIL_PG_VIEW_H
#define __DETAIL_PG_VIEW_H

#include "detail_model.h"


namespace wh{
namespace detail{
namespace view{
//-----------------------------------------------------------------------------
class ObjDetailPGView
	: public wxPropertyGrid
{
public:
	ObjDetailPGView(wxWindow *parent, wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxPG_DEFAULT_STYLE,
		const wxString& name = wxPropertyGridNameStr);


	void SetModel(std::shared_ptr<detail::model::Obj> model);

	void UpdatePGColour();
	
protected:
	void OnCmdReload(wxCommandEvent& evt);

	void OnChangeMainDetail(const IModel* model, const model::Obj::T_Data* data);

	void OnClsPropAfterInsert(const IModel& vec
		, const std::vector<SptrIModel>& newItems
		, const SptrIModel& itemBefore);
	void OnClsPropBeforeRemove(const IModel&, const std::vector<SptrIModel>&);
	void OnClsPropChange(const IModel&, const std::vector<unsigned int>&);
	void OnObjPropAfterInsert(const IModel& vec
		, const std::vector<SptrIModel>& newItems
		, const SptrIModel& itemBefore);
	void OnObjPropBeforeRemove(const IModel&, const std::vector<SptrIModel>&);
	void OnObjPropChange(const IModel&, const std::vector<unsigned int>&);

	std::shared_ptr<model::Obj> mObj;

	sig::scoped_connection		mChangeMainDetail;

	sig::scoped_connection		mConnClsPropAppend;
	sig::scoped_connection		mConnClsPropRemove;
	sig::scoped_connection		mConnClsPropChange;
	sig::scoped_connection		mConnObjPropAppend;
	sig::scoped_connection		mConnObjPropRemove;
	sig::scoped_connection		mConnObjPropChange;

};
//-----------------------------------------------------------------------------
}//namespace view {
}//namespace detail {
}//namespace wh{
#endif // __****_H