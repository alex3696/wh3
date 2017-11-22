#ifndef __MODEL_UNDO_H
#define __MODEL_UNDO_H

#include "ModelHistoryData.h"
#include "IModelWindow.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelUndo
{
	wxString mOid;
	wxString mParentOid;

	std::shared_ptr<ModelHistoryRecord> mRecord;

	void LoadPropertyDetails(PropTable& prop_table);
	void LoadActAndPrepareProp();

public:
	ModelUndo();
	ModelUndo(const wxString& oid, const wxString& parent_oid);
	void Set(const wxString& oid, const wxString& parent_oid);
	void Load();
	const ModelHistoryRecord& GetHistoryRecord()const;

	sig::signal<void(const ModelHistoryRecord&)>	sigHistoryRecordLoaded;
	sig::signal<void(const ModelHistoryRecord&)>	sigHistoryRecordRemoved;

	void ExecuteUndo();

};
//---------------------------------------------------------------------------

class ModelUndoWindow : public IModelWindow
{
	sig::scoped_connection connModelUndo_RecordLoaded;
public:
	std::shared_ptr<ModelUndo>		mModelUndo;
	
	ModelUndoWindow(const wxString& oid, const wxString& parent_oid);
	ModelUndoWindow(const rec::ObjInfo& oi);

	// IModelWindow
	virtual void UpdateTitle()override;
	virtual void Show()override { sigShow(); }
	virtual void Init()override;
	virtual void Load(const boost::property_tree::wptree& page_val)override;
	virtual void Save(boost::property_tree::wptree& page_val)override;

};
//---------------------------------------------------------------------------





} //namespace wh{
#endif // __*_H