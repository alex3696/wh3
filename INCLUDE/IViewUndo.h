#ifndef __IVIEW_UNDO_H
#define __IVIEW_UNDO_H
//-----------------------------------------------------------------------------

#include "IViewWindow.h"
#include "ModelHistoryData.h"

namespace wh{
//-----------------------------------------------------------------------------
class IViewUndoWindow : public IViewWindow
{
public:
	virtual void SetHistoryRecord(const ModelHistoryRecord&) = 0;

	sig::signal<void()>		sigExecuteUndo;
	sig::signal<void()>		sigLoad;

};



//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H