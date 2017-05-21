#ifndef __CTRL_UNDO_H
#define __CTRL_UNDO_H

#include "CtrlWindowBase.h"
#include "IViewUndo.h"
#include "ModelUndo.h"

namespace wh{

//-----------------------------------------------------------------------------
class CtrlUndoWindow final : public CtrlWindowBase<IViewUndoWindow, ModelUndoWindow>
{
	sig::scoped_connection connViewCmd_ExecuteUndo;
	sig::scoped_connection connViewCmd_Load;

	sig::scoped_connection connModel_HistoryRecordLoaded;

public:
	CtrlUndoWindow(const std::shared_ptr<IViewUndoWindow>& view
		, const  std::shared_ptr<ModelUndoWindow>& model);

	void ExecuteUndo();
	void Load();


};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __*_H