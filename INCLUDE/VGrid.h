#ifndef __VGRID_H
#define __VGRID_H


#include "BaseOkCancelDialog.h"
#include "BaseRefControlPanel.h"
#include "BaseControlPanel.h"
#include "BaseTable.h"

#include "rdbs.h"

namespace wh{
namespace view{



//-----------------------------------------------------------------------------
//template < typename T_VecModel >
class VGrid
	: public BaseTable
{
public:

	VGrid(wxWindow*		parent,
		wxWindowID		id = wxID_ANY,
		const wxPoint&	pos = wxDefaultPosition,
		const wxSize &	size = wxDefaultSize,
		long			style = 0 | wxDV_MULTIPLE)
		: BaseTable(parent, id, pos, size, style)
	{}

	void SetModel(std::shared_ptr<rdbs::Table> model);


	void OnInsert(const rdbs::Table* table,
		rdbs::RecordSp& newItem, rdbs::RecordSp& oldItem, const rdbs::TgOp tg_op);
	void OnDelete(const rdbs::Table* table,
		rdbs::RecordSp& newItem, rdbs::RecordSp& oldItem, const rdbs::TgOp tg_op);
	void OnUpdate(const rdbs::Table* table,
		rdbs::RecordSp& newItem, rdbs::RecordSp& oldItem, const rdbs::TgOp tg_op);


	void GetSelected(std::vector<unsigned int>& selected);

	void OnChangeVecState(ModelState state);
protected:
	std::shared_ptr<rdbs::Table> mModel;
	sig::scoped_connection		mConnAppend;
	sig::scoped_connection		mConnRemove;
	sig::scoped_connection		mConnChange;



	virtual bool GetAttrByRow(unsigned int row, unsigned int WXUNUSED(col),
		wxDataViewItemAttr &attr) const override;
	virtual void GetValueByRow(wxVariant& val,
		unsigned int row, unsigned int col) override final;

};
//-----------------------------------------------------------------------------






}//namespace view
}//namespace wh
#endif //__*_H