#include "_pch.h"
#include "ModelHistory.h"

using namespace wh;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelPageHistory::ModelPageHistory(const std::shared_ptr<rec::PageHistory>& data)
	:IModelWindow(), mGuiModel(*data)
{
}
//---------------------------------------------------------------------------

void ModelPageHistory::Update()
{
	mDataModel.Load();
}
//---------------------------------------------------------------------------
void ModelPageHistory::PageForward()
{
	auto offset = mDataModel.GetRowsOffset();
	const auto rpp = mDataModel.GetRowsLimit();
	offset += rpp;
	mDataModel.SetRowsOffset(offset);
}
//---------------------------------------------------------------------------
void ModelPageHistory::PageBackward()
{
	auto offset = mDataModel.GetRowsOffset();
	const auto rpp = mDataModel.GetRowsLimit();
	if (offset > rpp)
		offset -= rpp;
	else
		offset = 0;
	mDataModel.SetRowsOffset(offset);
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageHistory::UpdateTitle() //override;
{
	sigUpdateTitle(mTitle, mIco);
	Update();
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageHistory::Show()//override;
{
	//Update();
	sigShow();
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageHistory::Load(const boost::property_tree::ptree& page_val)//override;
{
	size_t offset = page_val.get<size_t>("CtrlPageHistory.Offset", 0);
	size_t limit = page_val.get<size_t>("CtrlPageHistory.Limit", 20);
	mDataModel.SetRowsOffset(offset);
	mDataModel.SetRowsLimit(limit);
	//Update();
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageHistory::Save(boost::property_tree::ptree& page_val)//override;
{
	using ptree = boost::property_tree::ptree;
	ptree content;
	content.put("Offset", mDataModel.GetRowsOffset());
	content.put("Limit", mDataModel.GetRowsLimit());
	page_val.push_back(std::make_pair("CtrlPageHistory", content));
	//page_val.put("CtrlPageLogList.id", 33);

}
//---------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class MHTDImpl :public ModelHistoryTableData
{
	const LogTable&		mLog;
public:
	MHTDImpl(const LogTable& log)
		:mLog(log)
	{
	}

	virtual size_t size()const override
	{
		return mLog.size();
	}
	virtual const wxString& GetLogId(const size_t row)const override
	{
		return mLog[row]->mId;
	}
	virtual const wxString& GetUser(const size_t row)const override
	{
		return mLog[row]->mUser;
	}
	virtual const wxString& GetDate(const size_t row)const override
	{
		return mLog[row]->mTimestamp;
	}

	virtual const IObj& GetObj(const size_t row)const override
	{
		return *mLog[row]->mObj;
	}

	virtual const IAct& GetAct(const size_t row)const override
	{
		return mLog[row]->mDetail->GetActRec();
	}
	virtual const PropValTable& GetActProperties(const size_t row)const override
	{
		return mLog[row]->mDetail->GetActProperties();
	}

	virtual const IObjPath& GetDstPath(const size_t row)const
	{ 
		return mLog[row]->mDetail->GetDstPath();
	};


};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ModelHistory::ModelHistory()
	:mRowsLimit(50), mRowsOffset(0)
{

}
//-----------------------------------------------------------------------------
void ModelHistory::Load()
{
	auto p0 = GetTickCount();
	mCls.clear();
	mObj.clear();
	mProp.clear();
	mAct.clear();
	mLog.clear();
	mActProp.clear();
		
	wxLogMessage(wxString::Format("%d \t ModelHistory : \t clear list", GetTickCount() - p0));

	wxString query = wxString::Format(
		"SELECT log_id, log_dt, log_user"
		" ,mcls_id, mcls_title, mobj_id, mobj_title, qty"
		" ,act_id, act_title, act_color, prop"
		" ,src_cid, src_oid, src_ipath, src_path"
		" ,dst_cid, dst_oid, dst_ipath, dst_path"
		" ,prop_lid, mcls_kind, mcls_measure "
		//" ,log_date, log_time "
		" FROM log "
		//" WHERE log_dt  > '2017.01.02' "
		" ORDER BY log_dt DESC "
		);
	
	if (mRowsLimit > 0)
		query += wxString::Format(" LIMIT %d", mRowsLimit);

	if (mRowsOffset > 0)
		query += wxString::Format(" OFFSET %d", mRowsOffset);


	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);

	p0 = GetTickCount();
	if (table && table->GetRowCount())
	{
		unsigned int rowQty = table->GetRowCount();
		if (rowQty)
		{
			//mRepList.reserve(rowQty);
			for (unsigned int i = 0; i < rowQty; ++i)
			{
				auto log_rec = std::make_shared<LogRec>();
				table->GetAsString(0, i, log_rec->mId);
				table->GetAsString(1, i, log_rec->mTimestamp);
				table->GetAsString(2, i, log_rec->mUser);
				log_rec = *mLog.emplace_back(log_rec).first;

				auto cls_rec = std::make_shared<ClsRec>();
				table->GetAsString(3, i, cls_rec->mId);
				table->GetAsString(4, i, cls_rec->mTitle);
				table->GetAsString(21, i, cls_rec->mKind);
				table->GetAsString(22, i, cls_rec->mMeasure);
				cls_rec = *mCls.emplace_back(cls_rec).first;
				
				auto obj_rec = std::make_shared<ObjRec>();
				table->GetAsString(5, i, obj_rec->mId);
				table->GetAsString(6, i, obj_rec->mTitle);
				table->GetAsString(7, i, obj_rec->mQty);

				obj_rec->mCls = cls_rec;
				obj_rec = *mObj.emplace_back(obj_rec).first;

				log_rec->mObj = obj_rec;

				wxString aid;
				table->GetAsString(8, i, aid);

				if (aid.empty())
				{
					auto log_mov_rec = std::make_shared<LogMovRec>();
					
					auto md = std::make_shared<LogMovRec>();
					table->GetAsString(19, i, log_mov_rec->mDstPath.mPath);
					table->GetAsString(20, i, log_mov_rec->mPropLId);
					//table->GetAsString(11, i, *log_mov_rec->mProperties);
					
					log_rec->mDetail = log_mov_rec;
				}
				else
				{
					auto act_rec = std::make_shared<ActRec>();
					act_rec->mId = aid;
					table->GetAsString(9, i, act_rec->mTitle);
					table->GetAsString(10, i, act_rec->mColour);
					act_rec = *mAct.emplace_back(act_rec).first;

					auto log_act_rec = std::make_shared<LogActRec>();
					log_act_rec->mActRec = act_rec;
					// load properties
					boost::property_tree::wptree prop_arr;
					wxString json_prop;
					table->GetAsString(11, i, json_prop);
					if (!json_prop.IsEmpty())
					{
						std::wstringstream ss; ss << json_prop;
						boost::property_tree::read_json(ss, prop_arr);
					}

					auto propval_table = std::make_shared<PropValTable >();
					for (const auto& pv : prop_arr)
					{
						auto prop_rec = std::make_shared<PropRec>();
						prop_rec->mId = wxString(pv.first.c_str());
						prop_rec = *mProp.emplace_back(prop_rec).first;

						auto propval_rec = std::make_shared<PropValRec>();
						propval_rec->mProp = prop_rec;
						propval_rec->mVal = pv.second.get_value<std::wstring>();

						propval_table->emplace(propval_rec);
						
					}
					obj_rec->mProperties = propval_table;
						

					
					log_rec->mDetail = log_act_rec;
				}



				table->GetAsString(15, i, obj_rec->mPath.mPath);

			}
		}
		

	}

	LoadPropertyDetails(mProp);
	LoadActProp(mActProp);

	whDataMgr::GetDB().Commit();
	wxLogMessage(wxString::Format("%d \t ModelHistory : \t download results", GetTickCount() - p0));
	
	PrepareProperties();
	
	auto sigData = std::make_shared<MHTDImpl>(mLog);
	sigAfterLoad(sigData);
}
//---------------------------------------------------------------------------
void ModelHistory::LoadPropertyDetails(PropTable& prop_table)
{
	wxString where_prop_id;
	for (const auto& prop : prop_table)
		where_prop_id += wxString::Format("OR id=%s ", prop->mId);

	where_prop_id.Replace("OR", "WHERE", false);

	wxString query = wxString::Format(
		"SELECT id, title, kind, var "//, var_strict
		" FROM prop "
		" %s "
		, where_prop_id);
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (!table || !table->GetRowCount())
		return;
	
	unsigned int rowQty = table->GetRowCount();
	if (!rowQty)
		return;

	for (unsigned int i = 0; i < rowQty; ++i)
	{
		wxString id;
		table->GetAsString(0, i, id);
		auto it = prop_table.get<1>().find(id);
		if (it != prop_table.get<1>().end())
		{
			table->GetAsString(1, i, (*it)->mTitle);
			table->GetAsString(2, i, (*it)->mKind);
		}//if (it != prop_table.get<1>().end())
	}//for (unsigned int i = 0; i < rowQty; ++i)

}
//---------------------------------------------------------------------------
void ModelHistory::LoadActProp(ActPropTable& act_prop_table)
{
	wxString where_act_id;
	for (const auto& act : mAct)
		where_act_id += wxString::Format("OR act_id=%s ", act->mId);

	where_act_id.Replace("OR", "WHERE", false);

	wxString query = wxString::Format(
		"SELECT id, act_id, prop_id "
		" FROM ref_act_prop "
		" %s "
		, where_act_id);
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (!table || !table->GetRowCount())
		return;

	unsigned int rowQty = table->GetRowCount();
	if (!rowQty)
		return;


	const auto& aid_idx = mAct.get<1>();
	const auto& pid_idx = mProp.get<1>();

	for (unsigned int i = 0; i < rowQty; ++i)
	{
		
		wxString aid,pid;
		table->GetAsString(1, i, aid);
		table->GetAsString(2, i, pid);
		auto aid_it = aid_idx.find(aid);
		auto pid_it = pid_idx.find(pid);
		if (aid_idx.end() != aid_it && pid_idx.end() != pid_it)
		{
			auto act_prop = std::make_shared<ActPropRec>();
			act_prop->mAct = *aid_it;
			act_prop->mProp = *pid_it;
			mActProp.emplace(act_prop);
		}
		
	}//for (unsigned int i = 0; i < rowQty; ++i)
}
//---------------------------------------------------------------------------
void ModelHistory::PrepareProperties()
{
	auto p0 = GetTickCount();

	for (const auto& log_rec : mLog)
	{
		const wxString& aid = log_rec->mDetail->GetActRec().mId;
		if (!aid.IsEmpty())
		{
			log_rec->mDetail->mActProperties = std::make_shared<PropValTable>();

			auto range = mActProp.get<0>().equal_range(aid);
			while (range.first != range.second)
			{
				const wxString pid = (*range.first)->mProp->mId;
				const auto& pid_idx_allprop = log_rec->mObj->mProperties->get<0>();
				auto it = pid_idx_allprop.find(pid);
				if (pid_idx_allprop.end() != it)
				{
					log_rec->mDetail->mActProperties->emplace(*it);
				}
				++range.first;
			}

			struct title_sorter {
				bool operator() (const std::shared_ptr<IPropVal>& v1
					, const std::shared_ptr<IPropVal>& v2)const
				{
					return v1->GetProp().GetTitle() < v2->GetProp().GetTitle();
				}
			} sorter;

			auto& title_idx_actprop = log_rec->mDetail->mActProperties->get<1>();
			title_idx_actprop.sort(sorter);

		}

	}


	wxLogMessage(wxString::Format("%d \t ModelHistory : \t prepare properties", GetTickCount() - p0));

}