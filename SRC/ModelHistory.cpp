#include "_pch.h"
#include "ModelHistory.h"

using namespace wh;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelPageHistory::ModelPageHistory(const std::shared_ptr<rec::PageHistory>& cfg)
	:IModelWindow()
{
	SetGuiModel(*cfg);
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
void wh::ModelPageHistory::ConvertToExcel()
{
	mDataModel.ConvertToExcel();
}
//---------------------------------------------------------------------------
void ModelPageHistory::SetGuiModel(rec::PageHistory&& cfg)
{
	if (mGuiModel != cfg)
	{
		std::swap(mGuiModel, cfg);
		mDataModel.SetRowsOffset(mGuiModel.mRowsOffset);
		mDataModel.SetRowsLimit(mGuiModel.mRowsLimit);
		sigCfgUpdated(mGuiModel);
	}
}
//---------------------------------------------------------------------------
void ModelPageHistory::SetGuiModel(const rec::PageHistory& cfg)
{
	if (mGuiModel != cfg)
	{
		mGuiModel = cfg;
		mDataModel.SetRowsOffset(mGuiModel.mRowsOffset);
		mDataModel.SetRowsLimit(mGuiModel.mRowsLimit);
		sigCfgUpdated(mGuiModel);
	}
	//mGuiModel.mColAutosize = true;
	//mGuiModel.mPathInProperties = false;
	//mGuiModel.mRowsLimit = 30;
	//mGuiModel.mRowsOffset = 0;
	//mDataModel.SetRowsOffset(mGuiModel.mRowsOffset);
	//mDataModel.SetRowsLimit(mGuiModel.mRowsLimit);
	//mGuiModel.mShowFilterList = true;
	//mGuiModel.mShowPropertyList = true;
	//mGuiModel.mStringPerRow = 5;
}
//---------------------------------------------------------------------------
void ModelPageHistory::SetGuiCfgDefault(const rec::PageHistory& cfg)
{
	auto value = std::make_shared<rec::PageHistory>(cfg);
	value->mVisibleColumnClsObj = true;
	auto cont = whDataMgr::GetInstance()->mContainer;
	cont->RegInstance("DefaultLogListInfo", value);
}
//---------------------------------------------------------------------------
const rec::PageHistory& ModelPageHistory::GetGuiModel()const
{
	return mGuiModel;
}

//---------------------------------------------------------------------------
//virtual 
void ModelPageHistory::UpdateTitle() //override;
{
	sigUpdateTitle("����� �������", ResMgr::GetInstance()->m_ico_history24);
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
void ModelPageHistory::Load(const boost::property_tree::wptree& page_val)//override;
{
	rec::PageHistory cfg;
	cfg.mRowsOffset = page_val.get<size_t>(L"CtrlPageHistory.Offset", 0);
	cfg.mRowsLimit = page_val.get<size_t>(L"CtrlPageHistory.Limit", 20);
	cfg.mStringPerRow = page_val.get<size_t>(L"CtrlPageHistory.StringPerRow", 4);
	cfg.mShowFilterList = page_val.get<bool>(L"CtrlPageHistory.ShowFilterList", false);
	cfg.mShowPropertyList = page_val.get<bool>(L"CtrlPageHistory.ShowPropertyList", false);
	cfg.mColAutosize = page_val.get<bool>(L"CtrlPageHistory.ColAutosize", false);
	cfg.mPathInProperties = page_val.get<bool>(L"CtrlPageHistory.PathInProperties", false);
	cfg.mVisibleColumnClsObj = page_val.get<bool>(L"CtrlPageHistory.VisibleColumnClsObj", true);
	SetGuiModel(std::move(cfg));

	/////Update();
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageHistory::Save(boost::property_tree::wptree& page_val)//override;
{
	using ptree = boost::property_tree::wptree;
	ptree content;
	content.put(L"Offset", mGuiModel.mRowsOffset);
	content.put(L"Limit", mGuiModel.mRowsLimit);
	content.put(L"StringPerRow", mGuiModel.mStringPerRow);
	content.put(L"ShowFilterList", mGuiModel.mShowFilterList);
	content.put(L"ShowPropertyList", mGuiModel.mShowPropertyList);
	content.put(L"ColAutosize", mGuiModel.mColAutosize);
	content.put(L"PathInProperties", mGuiModel.mPathInProperties);
	content.put(L"VisibleColumnClsObj", mGuiModel.mVisibleColumnClsObj);
	page_val.push_back(std::make_pair(L"CtrlPageHistory", content));
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
	virtual const PropValTable& GetProperties(const size_t row)const override
	{
		return mLog[row]->mDetail->GetProperties();
	}

	virtual const IObjPath&	GetPath(const size_t row)const override
	{
		return mLog[row]->mDetail->GetPath();
	}

	virtual const IObjPath& GetDstPath(const size_t row)const override
	{ 
		return mLog[row]->mDetail->GetDstPath();
	}
	virtual const wxString& GetQty(const size_t row)const override
	{
		return mLog[row]->mDetail->GetQty();
	};


};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ModelHistory::ModelHistory()
	:mRowsLimit(50), mRowsOffset(0)
{
	//std::vector<ModelFilterList::NotyfyItem> ins_vector;
	//auto new_item = std::make_shared<const ModelFilter>("User", "User", FilterKind::EqOneValue, ftText);
	//std::shared_ptr<const ModelFilter> old_item;
	//ins_vector.emplace_back(ModelFilterList::NotyfyItem(old_item, new_item));
	//mModelFilterList->Update(ins_vector);

	mSort = " log_dt DESC ";

	mModelFilterList->Insert("�����", "log_dt", FilterOp::foBetween, ftDateTime);
	mModelFilterList->Insert("������������", "log_user", FilterOp::foLike, ftText);
	mModelFilterList->Insert("���", "mcls_title", FilterOp::foLike, ftText);
	mModelFilterList->Insert("������", "mobj_title", FilterOp::foLike, ftText);
	mModelFilterList->Insert("��������", "act_title", FilterOp::foLike, ftText);
	mModelFilterList->Insert("������", "src_path", FilterOp::foLike, ftText);
	mModelFilterList->Insert("����", "dst_path", FilterOp::foLike, ftText);

	connApply = mModelFilterList->sigApply
		.connect(std::bind(&ModelHistory::SetWhere, this,std::placeholders::_1));
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
		
	wxLogMessage(wxString::Format("ModelHistory:\t%d\t clear list", GetTickCount() - p0));

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
		//" WHERE  mobj_title = '259' AND act_id IS NOT NULL "
		);
	
	if (mWhere.size())
		query += " WHERE "+mWhere;

	if (mSort.size())
		query += " ORDER BY " + mSort;

	if (mRowsLimit > 0)
		query += wxString::Format(" LIMIT %d", mRowsLimit);

	if (mRowsOffset > 0)
		query += wxString::Format(" OFFSET %d", mRowsOffset);


	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);

	p0 = GetTickCount();
	if (table)
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
				auto ins_it_cls = mCls.emplace_back(cls_rec);
				cls_rec = std::dynamic_pointer_cast<ClsRec>(*ins_it_cls.first);
				
				auto obj_rec = std::make_shared<ObjRec>();
				table->GetAsString(5, i, obj_rec->mId);
				table->GetAsString(6, i, obj_rec->mTitle);
				

				obj_rec->mCls = cls_rec;
				auto inst_it = mObj.emplace_back(obj_rec);
				obj_rec = std::dynamic_pointer_cast<ObjRec>(*inst_it.first);

				log_rec->mObj = obj_rec;

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
					auto iprop_rec = *mProp.emplace_back(prop_rec).first;

					auto propval_rec = std::make_shared<PropValRec>();
					propval_rec->mProp = iprop_rec;
					propval_rec->mVal = pv.second.get_value<std::wstring>();

					propval_table->emplace(propval_rec);
				}//for
				// load properties

				wxString aid;
				table->GetAsString(8, i, aid);

				if (aid.empty() || aid=="0")
				{
					auto log_mov_rec = std::make_shared<LogMovRec>();
					
					table->GetAsString(7 , i, log_mov_rec->mQty);
					table->GetAsString(19, i, log_mov_rec->mDstPath.mPath);
					table->GetAsString(20, i, log_mov_rec->mPropLId);
					//table->GetAsString(11, i, *log_mov_rec->mProperties);
					log_mov_rec->mProperties = propval_table;
					log_rec->mDetail = log_mov_rec;
				}
				else
				{
					auto act_rec = std::make_shared<ActRec>();
					act_rec->mId = aid;
					
					table->GetAsString(9, i, act_rec->mTitle);
					table->GetAsString(10, i, act_rec->mColour);
					//act_rec = *mAct.emplace_back(act_rec).first;
					auto ins_it_act = mAct.emplace_back(act_rec);
					act_rec = std::dynamic_pointer_cast<ActRec>(*ins_it_act.first);

					auto log_act_rec = std::make_shared<LogActRec>();
					log_act_rec->mActRec = act_rec;
					
					log_act_rec->mProperties = propval_table;
					
					log_rec->mDetail = log_act_rec;
				}// else if (aid.empty())


				table->GetAsString(15, i, log_rec->mDetail->mPath.mPath);

			}//for (unsigned int i = 0; i < rowQty; ++i)
		}//if (rowQty)
		

	}

	LoadPropertyDetails();
	LoadActProp();

	whDataMgr::GetDB().Commit();
	wxLogMessage(wxString::Format("ModelHistory:\t%d\t download results", GetTickCount() - p0));
	
	PrepareProperties();
	
	auto sigData = std::make_shared<MHTDImpl>(mLog);
	sigAfterLoad(sigData);
}
//---------------------------------------------------------------------------
void ModelHistory::LoadPropertyDetails()
{
	wxString where_prop_id;
	for (const auto& prop : mProp)
		where_prop_id += wxString::Format("OR id=%s ", prop->GetId());

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
		auto it = mProp.get<1>().find(id);
		if (it != mProp.get<1>().end())
		{
			// multiitndex Modifier
			mProp.get<1>().modify(it,
				[table,i](std::shared_ptr<IProp>& e)
				{
					auto prop_rec = std::dynamic_pointer_cast<PropRec>(e);
					table->GetAsString(1, i, prop_rec->mTitle);
					table->GetAsString(2, i, prop_rec->mKind);
				});
			//auto prop_rec = std::dynamic_pointer_cast<PropRec>(*it);
			//table->GetAsString(1, i, prop_rec->mTitle);
			//table->GetAsString(2, i, prop_rec->mKind);
		}//if (it != prop_table.get<1>().end())
	}//for (unsigned int i = 0; i < rowQty; ++i)

}
//---------------------------------------------------------------------------
void ModelHistory::LoadActProp()
{
	wxString where_act_id;
	for (const auto& act : mAct)
		where_act_id += wxString::Format("OR act_id=%s ", act->GetId());

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

			auto act_rec = std::dynamic_pointer_cast<ActRec>(*aid_it);
			act_rec->mPropList.emplace_back(*pid_it);
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
		if (!aid.IsEmpty() && "0"!= aid)
		{
			log_rec->mDetail->mActProperties = std::make_shared<PropValTable>();

			auto range = mActProp.get<0>().equal_range(aid);
			while (range.first != range.second)
			{
				const wxString& pid = (*range.first)->mProp->GetId();
				const auto& pid_idx_allprop = log_rec->mDetail->mProperties->get<0>();
				auto it = pid_idx_allprop.find(pid);
				if (pid_idx_allprop.end() != it)
				{
					log_rec->mDetail->mActProperties->emplace(*it);
				}
				++range.first;
			}
			
			//struct title_sorter {
			//	bool operator() (const std::shared_ptr<IPropVal>& v1
			//		, const std::shared_ptr<IPropVal>& v2)const
			//	{
			//		return v1->GetProp().GetTitle() < v2->GetProp().GetTitle();
			//	}
			//} sorter;
			//auto& title_idx_actprop = log_rec->mDetail->mActProperties->get<1>();
			//title_idx_actprop.sort(sorter);

		}//if (!aid.IsEmpty())

	}//for (const auto& log_rec : mLog)


	wxLogMessage(wxString::Format("ModelHistory:\t%d\t prepare properties", GetTickCount() - p0));

}
//---------------------------------------------------------------------------
void ModelHistory::SetWhere(const wxString& where)
{
	mWhere = where;
	SetRowsOffset(0);
	Load();

}
//---------------------------------------------------------------------------
void ModelHistory::SelectHistoryItem(const wxString& str_log_id)
{
	const auto& logIdIdx = mLog.get<1>();
	auto it = logIdIdx.find(str_log_id);
	if (logIdIdx.end() != it)
	{
		const auto& detail = (*it)->mDetail;
		if (detail->HasProperties())
		{
			const IAct* act_rec = &detail->GetActRec();
			mModelObjPropList->sigUpdatePropList(detail->GetProperties(), act_rec);
		}
		else
		{
			// TODO ��� ����� ����� ������� ��� �� ������������
			auto src_prop = std::make_shared<PropRec>("-1","��������","0");
			auto dst_prop = std::make_shared<PropRec>("-2", "�������", "0");
			auto qty_prop = std::make_shared<PropRec>("-3", "����������", "100");

			auto ap = std::make_shared<PropValTable>();
			auto pvr_src = std::make_shared<PropValRec>(src_prop, detail->GetPath().AsString());
			auto pvr_dst = std::make_shared<PropValRec>(dst_prop, detail->GetDstPath().AsString());
			auto pvr_qty = std::make_shared<PropValRec>(qty_prop, detail->GetQty());
			
			ap->emplace(pvr_src);
			ap->emplace(pvr_dst);
			ap->emplace(pvr_qty);

			mModelObjPropList->sigUpdatePropList(*ap, nullptr);
		}
	}
}
//---------------------------------------------------------------------------
void wh::ModelHistory::ConvertToExcel()
{
	wxString fpath = wxStandardPaths::Get().GetTempDir();


	wxString fname = "rep.csv";
	wxString ofname;
	ofname = wxString::Format("%s\\%s", fpath, fname);

	int i = 0;
	std::ofstream cout;
	cout.open(ofname.wc_str(), std::ofstream::binary | std::ofstream::out | std::ofstream::trunc);

	while (!cout.is_open())
	{
		fname = wxString::Format("rep_%d.csv", i);
		ofname = wxString::Format("%s\\%s", fpath, fname);
		cout.open(ofname.wc_str(), std::ofstream::binary | std::ofstream::out | std::ofstream::trunc);
		i++;
	}
	// generate column titles
	cout << "\"Date\";" << "\"Time\";" << "\"User\";";
	cout << "\"Type\";" << "\"Object\";";
	cout << "\"Action\";" << "\"Qty\";";
	cout << "\"Path\";" << "\"DstPath\";";
	for (const auto& prop : mProp)
			cout << "\"" << prop->GetTitle() << "\";";
	cout << std::endl;

	wxString format_d = wxLocale::GetInfo(wxLOCALE_SHORT_DATE_FMT, wxLOCALE_CAT_DATE);
	wxString format_t = wxLocale::GetInfo(wxLOCALE_TIME_FMT);
	wxDateTime dt;
	for (const auto& log : mLog)
	{
		dt.ParseISOCombined(log->mTimestamp, ' ');
		
		cout << dt.Format(format_d) << ";" << dt.Format(format_t) << ";" << log->mUser << ";";
		cout << log->mObj->GetCls().GetTitle() << ";'" << log->mObj->GetTitle() << "';";
		cout << log->mDetail->GetActRec().GetTitle() << ";" << log->mDetail->GetQty() << ";";
		
		cout << log->mDetail->GetPath().AsString() << ";";

		auto act_rec = std::dynamic_pointer_cast<LogActRec>(log->mDetail);
		if (act_rec)
		{
			cout << ";";//DstPath
			auto act_prop = log->mDetail->GetActProperties();
			const auto& pid_idx = act_prop.get<0>();
			for (const auto& prop : mProp)
			{
				const auto& it = pid_idx.find(prop->GetId());
				if (pid_idx.end() != it)
				{
					wxString val = (*it)->GetValue();
					val.Replace("\r", " ", true);
					val.Replace("\n", " ", true);
					val.Replace("\t", " ", true);
					cout << "\"" << val << "\"";
				}
					
				cout << ";";
			}
		}
		else
		{
			cout << log->mDetail->GetDstPath().AsString() << ";";
			for (const auto& prop : mProp)
				cout << ";";
		}
		
		cout << std::endl;

	}
	cout.close();

	auto cont = whDataMgr::GetInstance()->mContainer;
	auto fn = cont->GetObject<wxString>(wxString("CSVFilePath"));
	(*fn) = ofname;
	cont->GetObject<void>("CtrlCSVFileOpen");
}
