#ifndef __TMODELARRAY_H
#define __TMODELARRAY_H

#include "TModel.h"

namespace wh{

template< typename T_Item >
class TModelArray
	: public IModel
{
public:
	using ItemType = T_Item;

	TModelArray(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild)
	:IModel(option)
	{
	}

	virtual std::shared_ptr<IModel> CreateChild()override
	{
		auto child = std::make_shared < T_Item > ();
		child->SetData(T_Item::T_Data());
		return child;
		/*
		auto child = new T_Item;
		T_Item::T_Data data;
		child->SetData(data);
		return std::shared_ptr<IModel>(child);
		*/
	}

	virtual const std::vector<Field>& GetFieldVector()const override
	{
		auto tmpChild = std::make_shared<T_Item>();
		return tmpChild->GetFieldVector();
	}
protected:
	virtual void LoadChilds()override
	{
		// 1 ��������� ����������� ��������, ���� ��������, ��� ������������� ������ ��� ������� ����� ������ ����
		auto enableParentNotify = (mOption & ModelOption::EnableParentNotify);
		mOption = mOption & ~ModelOption::EnableParentNotify;
		Clear(); // �������, ���� � ������� ���� �������, �� ��� ���������
		// ��������� �������
		wxString query;
		const bool queryExist = GetSelectChildsQuery(query);
		if (queryExist)
		{
			auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
			if (table)
			{
				VecChange sigVec;
				unsigned int rowQty = table->GetRowCount();
				if (rowQty)
				{
					if (!mVec)
						mVec.reset(new BaseStore);
					
					unsigned int dbNo = 0;
					unsigned int elemNo = 0;
					while (dbNo < rowQty)
					{
						std::shared_ptr<IModel> child = CreateChild();
						if (child && LoadChildDataFromDb(child, table, dbNo))
						{
							child->MarkSaved();
							if (AppendChildWithoutSignal(child))
								sigVec.emplace_back(elemNo++);
						}
						dbNo++;
					}
					
				}//if (rowQty)

				// ��������� �������� �������
				if (rowQty && (mOption & ModelOption::CascadeLoad) )
					for (auto item : *mVec)
						item->LoadChilds();
				
				DoSigAppendChild(sigVec);
			}//if(table)
		}
		// 3 �������� ��������� ��������� �� ���������
		if (enableParentNotify)
			mOption = mOption | ModelOption::EnableParentNotify;
		DoNotifyParent();


	}
	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>& table, const size_t pos)override
	{
		auto childModel = std::dynamic_pointer_cast<T_Item>(child);
		if (childModel)
			return childModel->LoadThisDataFromDb(table, pos);
		return false;
	}
};



}//namespace wh{
#endif // __****_H