#ifndef __PATHPATTERNEDITOR_H
#define __PATHPATTERNEDITOR_H

#include "PathPattern_model.h"
#include "globaldata.h"

namespace wh{

class TmpStrPathItem : public wxStaticText
{
	sig::scoped_connection connChange;
	std::shared_ptr<temppath::model::Item> mModel;

	void OnEnter(wxMouseEvent& event);
	void OnLeave(wxMouseEvent& event);
	
public:
	TmpStrPathItem(wxWindow *parent,
		wxWindowID id,
		const wxString& label = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxString& name = wxStaticTextNameStr);

	void SetModel(std::shared_ptr<temppath::model::Item>& newModel);
	void OnChange(const IModel*, const temppath::model::Item::DataType*);

};


typedef TmpStrPathItem TmpPathItem;
//using TmpPathItem = TmpPathChoiceItem;



class PathPatternEditor
	:public wxScrolledWindow
{

	using CtrlStore =
		boost::multi_index_container
		<
			TmpPathItem*,
			indexed_by
			<
			random_access<>
			, hashed_unique< identity<TmpPathItem*> >
			>
		>;
	using TIdxRnd = nth_index< CtrlStore, 0>::type;
	using TIdxPtr = nth_index< CtrlStore, 1>::type;
	using RndIdx = TIdxRnd;
	using PtrIdx = TIdxPtr;
	using RndIterator = RndIdx::iterator;
	using CRndIterator = RndIdx::const_iterator;

	using PtrIterator = PtrIdx::iterator;
	using CPtrIterator = PtrIdx::const_iterator;

	CtrlStore								mPathChoice;
	std::shared_ptr<temppath::model::Array> mModel;

	sig::scoped_connection connAdd;
	sig::scoped_connection connDel;

	sig::scoped_connection connAfterInsert;

	void ResetGui();
	void OnAfterInsert(const IModel&
		, const std::vector<SptrIModel>&, const SptrIModel&);
	void OnAddNode(const IModel&, const std::vector<unsigned int>&);
	void OnDelNode(const IModel&, const std::vector<unsigned int>&);

	void MakeGuiItem(unsigned int pos);

	bool GetGiuItemIndex(TmpPathItem*, size_t& model_idx);
	wxMenu mMenu;

	wxMenuItem* mMnuAddToLeft;
	wxMenuItem* mMnuAddToRight;
	wxMenuItem* mMnuRemove;
	wxMenuItem* mMnuSetAny;
	wxMenuItem* mMnuSetCls;
	wxMenuItem* mMnuSetClsObj;
	wxMenuItem* mMnuSetFixObj;
	wxMenuItem* mMnuSetFixAny;
	

	enum MenuID
	{
		miAddToLeft = 6000
		, miAddToRight
		, miRemove
		, miSetAny
		, miSetCls
		, miSetClsObj
		, miSetFixObj
		, miSetFixAny
	};
	TmpPathItem* mSelectedItem = nullptr;

	void OnCmdAddToLeft(wxCommandEvent& evt);
	void OnCmdAddToRight(wxCommandEvent& evt);
	void OnCmdRemove(wxCommandEvent& evt);
	void OnCmdSetAny(wxCommandEvent& evt);
	void OnCmdSetCls(wxCommandEvent& evt);
	void OnCmdSetClsObj(wxCommandEvent& evt);
	void OnCmdSetFixObj(wxCommandEvent& evt);
	void OnCmdSetFixAny(wxCommandEvent& evt);

public:
	PathPatternEditor(wxWindow *parent,
		wxWindowID winid = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxHSCROLL,
		const wxString& name = wxPanelNameStr);

	void SetModel(std::shared_ptr<temppath::model::Array>& newModel);

	enum Mode			// ограничения
	{
		FixOne_ReqCls=0,	// только один элемент // обязательно хотя бы класс [в последнем]
		ReqOne_ReqCls=1,	// хотя бы один элемент // обязательно хотя бы класс в последнем
		ReqOne_FixCls=2		// хотя бы один элемент // фиксированный класс в последнем
	};
	
	void SetMode(Mode mode)
	{
		mMode = mode;
	}
	
protected:
	Mode mMode = ReqOne_FixCls;
};










}//namespace wh{
#endif // __****_H