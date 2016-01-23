#include "_pch.h"
#include "VTable.h"

using namespace wh;
//-----------------------------------------------------------------------------
VTable::VTable(wxWindow*		parent,
		wxWindowID		id ,
		const wxPoint&	pos ,
		const wxSize &	size ,
		long			style )
		: BaseTable(parent, id, pos, size, style)
{

}
//-----------------------------------------------------------------------------
void VTable::SetModel(std::shared_ptr<ITable> model)
{
	if (model && mModel != model)
	{
		mConnAppend.disconnect();
		mConnRemove.disconnect();
		mConnChange.disconnect();
		this->mDataViewModel->Reset(0);
		this->ClearColumns();
		mModel = model;
		const auto& fvec = mModel->mFieldVec;
			
		for (unsigned int i = 0; i < fvec->size(); ++i)
		{
			const auto& field = fvec->at(i)->GetData();
			if (field.mGuiShow)
			{
				wxString name = field.mTitle;
				auto width = GetColumnWidthBy(field.mType);

				if (ftText == field.mType)
					EnableAutosizeColumn(i);

				AppendTextColumn(name, i, wxDATAVIEW_CELL_INERT, width,
					wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
			}
		}

		namespace ph = std::placeholders;

		auto fnAI = std::bind(&VTable::OnRowAfterInsert, this, ph::_1, ph::_2, ph::_3);
		
		auto fnBR = std::bind(&VTable::OnRowBeforeRemove, this, ph::_1, ph::_2);
		auto fnAR = std::bind(&VTable::OnRowAfterRemove, this, ph::_1, ph::_2);

		auto fnAC = std::bind(&VTable::OnChange, this, ph::_1, ph::_2);


		mConnAppend = mModel->ConnAfterInsert(fnAI);
		mConnRemove = mModel->ConnectBeforeRemove(fnBR);
		mConnAR = mModel->ConnectAfterRemove(fnAR);
		mConnChange = mModel->ConnectChangeSlot(fnAC);
		

		mDataViewModel->Reset(mModel->GetChildQty());
		OnChangeVecState(mModel->GetState());
	}


};
//-----------------------------------------------------------------------------
void VTable::OnRowAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	if (itemBefore)
	{
		size_t pos;
		if (vec.GetItemPosition(itemBefore, pos))
		{
			for (const auto& curr : newItems)
				mDataViewModel->RowInserted(pos++);
		}
	}
	else
	{
		for (const auto& curr : newItems)
			mDataViewModel->RowAppended();
	}
	OnChangeVecState(vec.GetState());
}//OnAppend
//-----------------------------------------------------------------------------
void VTable::OnRowBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	if (vec.size() != remVec.size() )
	{
		size_t pos;
		wxArrayInt	itemsArray;
		for (const auto& remItem : remVec)
		{
			if (vec.GetItemPosition(remItem, pos))
				itemsArray.push_back(pos);
		}
		mDataViewModel->RowsDeleted(itemsArray);
	}
	else
		mDataViewModel->Reset(0);
}//OnRemove
//-----------------------------------------------------------------------------
void VTable::OnRowAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	OnChangeVecState(vec.GetState());
}
//-----------------------------------------------------------------------------
void VTable::OnChange(const IModel& newVec, const std::vector<unsigned int>& itemVec)
{
	auto changedQty = itemVec.size();
	bool changeAll = newVec.GetChildQty() == itemVec.size();

	if (!changeAll && changedQty < 100)
	{
		for (const unsigned int& item : itemVec)
			mDataViewModel->RowChanged(item);
	}
	else
	{
		mDataViewModel->Reset(changedQty);
		this->Refresh();
	}
	OnChangeVecState(newVec.GetState());
}//OnChange
//-----------------------------------------------------------------------------
void VTable::OnChangeVecState(ModelState state)
{
	if (msExist == state)
		this->SetBackgroundColour(wxColour(240, 255, 245));
	else
		this->SetBackgroundColour(wxColour(255, 250, 250));
}
//-----------------------------------------------------------------------------
bool VTable::GetAttrByRow(unsigned int row, unsigned int WXUNUSED(col),
	wxDataViewItemAttr &attr) const 
{
	if (mModel)
	{
		const ModelState state = mModel->GetChild(row)->GetState();
		switch (state)
		{
			//msNull
		default:  break;
		case msCreated:
			attr.SetBold(true);
			attr.SetColour(*wxBLUE);
			break;
		case msExist:
			attr.SetBold(false);
			attr.SetColour(*wxBLACK);
			break;
		case msUpdated:
			attr.SetBold(true);
			attr.SetColour(wxColour(128, 64, 0));
			break;
		case msDeleted:
			attr.SetBold(true);
			attr.SetColour(*wxRED);
			break;
		}//switch
		return true;
	}

	return false;
}
//-----------------------------------------------------------------------------
void VTable::GetValueByRow(wxVariant& val, unsigned int row, unsigned int col) 
{
	if (!mModel)
		return;
	mModel->GetValueByRow(val,row,col);
}
