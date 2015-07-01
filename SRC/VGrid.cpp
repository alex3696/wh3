#include "_pch.h"
#include "VGrid.h"

using namespace wh;
using namespace wh::view;

//-------------------------------------------------------------------------
void VGrid::SetModel(std::shared_ptr<rdbs::Table> model)
{
	if (!model || mModel == model)
		return;
		
	mConnAppend.disconnect();
	mConnRemove.disconnect();
	mConnChange.disconnect();

	this->mDataViewModel->Reset(0);
	this->ClearColumns();

	mModel = model;

	const auto& fields = mModel->GetFieldArray();
	int colIndex = 1;

	for (const auto& field : fields)
	{
		wxString name = field.mName;
		auto width = GetColumnWidthBy(field.mType);
		if (ftText == field.mType)
			EnableAutosizeColumn(colIndex - 1);

		if (1 == colIndex)
			AppendIconTextColumn(name, colIndex++, wxDATAVIEW_CELL_INERT, width,
			wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
		else
			AppendTextColumn(name, colIndex++, wxDATAVIEW_CELL_INERT, width,
			wxALIGN_NOT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

	}

	using namespace rdbs;
	namespace sph = std::placeholders;

	rdbs::TriggerFunc triggerOnInsert 
		= std::bind(&VGrid::OnInsert, this, sph::_1, sph::_2, sph::_3, sph::_4);
	rdbs::TriggerFunc triggerOnDelete 
		= std::bind(&VGrid::OnDelete, this, sph::_1, sph::_2, sph::_3, sph::_4);
	rdbs::TriggerFunc triggerOnUpdate 
		= std::bind(&VGrid::OnUpdate, this, sph::_1, sph::_2, sph::_3, sph::_4);



	mConnAppend = mModel->ConnectTrigger(TgOp::toAfterInsert, triggerOnInsert);
	mConnRemove = mModel->ConnectTrigger(TgOp::toAfterDelete, triggerOnDelete);
	mConnChange = mModel->ConnectTrigger(TgOp::toAfterUpdate, triggerOnUpdate);



	//mDataViewModel->Reset(mModel->GetChildQty());
	//OnChangeVecState(mModel->GetState());
}

//-------------------------------------------------------------------------
void VGrid::OnInsert(const rdbs::Table* table,
	rdbs::RecordSp& newItem, rdbs::RecordSp& oldItem, const rdbs::TgOp tg_op)
{
	mDataViewModel->RowAppended();
	OnChangeVecState(newItem->GetState());
}//OnAppend
//-------------------------------------------------------------------------
void VGrid::OnDelete(const rdbs::Table* table,
	rdbs::RecordSp& newItem, rdbs::RecordSp& oldItem, const rdbs::TgOp tg_op)
{
	const auto& recArray = mModel->GetRecordArray();
	auto& ptrIdx = recArray.get<1>();

	auto oldIt = ptrIdx.find(oldItem.get());

	if (ptrIdx.end() != oldIt)
	{
		auto oldRndIt = recArray.project<0>(oldIt);
		auto pos = std::distance(recArray.begin(), oldRndIt);
		mDataViewModel->RowDeleted(pos);

		OnChangeVecState(oldItem->GetState());
	}
	
}//OnRemove
//-------------------------------------------------------------------------
void VGrid::OnUpdate(const rdbs::Table* table,
	rdbs::RecordSp& newItem, rdbs::RecordSp& oldItem, const rdbs::TgOp tg_op)
{
	const auto& recArray = mModel->GetRecordArray();
	auto& ptrIdx = recArray.get<1>();

	auto it = ptrIdx.find(oldItem.get());

	if (ptrIdx.end() != it)
	{
		auto rndIt = recArray.project<0>(it);
		auto pos = std::distance(recArray.cbegin(), rndIt);
		mDataViewModel->RowDeleted(pos);

		OnChangeVecState(newItem->GetState());
	}

}//OnChange

//-------------------------------------------------------------------------
void VGrid::GetSelected(std::vector<unsigned int>& selected)
{
	wxDataViewItemArray viewSelArr;
	this->GetSelections(viewSelArr);
	for (const wxDataViewItem& viewItem : viewSelArr)
	{
		if (viewItem.IsOk())
		{
			unsigned int pos = GetRow(viewItem);
			selected.emplace_back(pos);
		}
	}
}

//-------------------------------------------------------------------------
void VGrid::OnChangeVecState(ModelState state)
{
	if (msExist == state)
		this->SetBackgroundColour(wxColour(240, 255, 245));
	else
		this->SetBackgroundColour(wxColour(255, 250, 250));
}

//-------------------------------------------------------------------------
bool VGrid::GetAttrByRow(unsigned int row, unsigned int col,
	wxDataViewItemAttr &attr) const 
{
	if (mModel)
	{
		const ModelState state = mModel->GetRecordArray().at(row)->GetState();
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
//-------------------------------------------------------------------------
void VGrid::GetValueByRow(wxVariant& val, unsigned int row, unsigned int col) 
{
	auto onError = [&col, &val]()
	{
		if (1 == col)
			val << wxDataViewIconText(L"**error**");
		else
			val << wxDataViewIconText(wxEmptyString);
	};
	
	if (!mModel)
		onError();
	
	rdbs::RecordSp rec = mModel->GetRecordArray().at(row);

	try
	{
		val = rec->GetData(col);
	}
	catch (...)
	{
		onError();
	}



}


