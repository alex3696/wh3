#include "_pch.h"
#include "ReportModel.h"
#include "ReportListModel.h"


using namespace wh;
//-----------------------------------------------------------------------------
ReportModel::ReportModel(const std::shared_ptr<wxString>& rep_id)
	:IModelWindow(), mRepId(*rep_id)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto modelRepList = container->GetObject<ReportListModel>("ModelPageReportList");
	if(!modelRepList->Size())
		modelRepList->UpdateList();

	connListItemChange = modelRepList->sigChReport.connect
		([this](const std::shared_ptr<const rec::ReportItem>& ri, const wxString& old_rep_id)
	{
		if (old_rep_id != mRepId)
			return;
		sigUpdateTitle(ri->mTitle, mIco);
		sigSetNote(ri->mNote);
		DoUpdateView();
		
	});

	connListItemRemove = modelRepList->sigRmReport.connect
		([this](const std::shared_ptr<const rec::ReportItem>& ri)
	{
		if (ri->mId != mRepId)
			return;
		sigUpdateTitle("Îò÷¸ò óäàë¸í", mIco);
		sigSetNote(wxEmptyString);
		sigExecuted(rec::ReportTable());
		sigSetFilterTable(rec::ReportFilterTable());
		
	});


	connListItemUpdate = modelRepList->sigListUpdated.connect
		([this](const rec::ReportList& rl)
	{
		Update();
	});

}
//-----------------------------------------------------------------------------
void ReportModel::BuildFilterTable(rec::ReportFilterTable& rft)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto modelRepList = container->GetObject<ReportListModel>("ModelPageReportList");
	auto ri = modelRepList->GetRep(mRepId);
	if (ri)
	{
		const wxChar term = '?';
		size_t pos = 0;
		size_t start = pos;
		while (pos < ri->mScript.size())
		{

			if (term == ri->mScript[pos])
			{
				rec::ReportFilter fi;
				start = pos++;
				while (pos < ri->mScript.size() && ri->mScript[pos] != term)
					++pos;
				fi.mName = ri->mScript.SubString(start + 1, pos - 1);
				start = pos++;
				while (pos < ri->mScript.size() && ri->mScript[pos] != term)
					++pos;
				fi.mType = ri->mScript.SubString(start + 1, pos - 1);
				start = pos++;
				while (pos < ri->mScript.size() && ri->mScript[pos] != term)
					++pos;
				fi.mDefault = ri->mScript.SubString(start + 1, pos - 1);
				rft.emplace_back(fi);
			}
			pos++;

		}

	}
}
//-----------------------------------------------------------------------------
void ReportModel::DoUpdateView()
{
	rec::ReportTable& rt = mReportTable;
	rt.mColNames.clear();
	rt.mRowList.clear();
	sigExecuted(rt);

	rec::ReportFilterTable rft;
	BuildFilterTable(rft);
	sigSetFilterTable(rft);

	
}
//-----------------------------------------------------------------------------
void ReportModel::Update()
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto modelRepList = container->GetObject<ReportListModel>("ModelPageReportList");
	auto ri = modelRepList->LoadAll(mRepId);
	if (!ri)
		return;
	//DoUpdateView();


}
//-----------------------------------------------------------------------------
void ReportModel::Execute(const std::vector<wxString>& filter_vec)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto modelRepList = container->GetObject<ReportListModel>("ModelPageReportList");
	auto ri = modelRepList->GetRep(mRepId);
	if (!ri)
		return;

	wxString query = ri->mScript;
	
	rec::ReportFilterTable rft;
	BuildFilterTable(rft);
	if (rft.size() == filter_vec.size())
	{
		for (size_t i = 0; i < rft.size(); ++i)
		{
			wxString str_to_replace = wxString::Format("?%s?%s?%s?"
				, rft[i].mName, rft[i].mType, rft[i].mDefault);
			query.Replace(str_to_replace, filter_vec[i]);
		}
	}

	//wxString query = wxString::Format(" SELECT * FROM prop ");

	rec::ReportTable& rt = mReportTable;

	rt.mColNames.clear();
	rt.mRowList.clear();

	
	/*
	COLUMN_UNKNOWN = 0,
	COLUMN_NULL,
	COLUMN_INTEGER,
	COLUMN_STRING,
	COLUMN_DOUBLE,
	COLUMN_BOOL,
	COLUMN_BLOB,
	COLUMN_DATE,

	*/
	wxString format = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);
	//wxString format1 = wxLocale::GetOSInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);

	if (table && table->GetRowCount())
	{
		const size_t col_qty = table->GetColumnCount();
		const size_t row_qty = table->GetRowCount();

		rt.mColNames.reserve(col_qty);

		//int ctype = 0;
		for (size_t i = 0; i < col_qty; i++)
		{
			//ctype = table->GetColumnType(i);
			rt.mColNames.emplace_back(table->GetColumnName(i));
		}

		rt.mRowList.reserve(col_qty);

		for (size_t r = 0; r < row_qty; r++)
		{
			rec::ReportTable::Row row(col_qty);
			for (size_t i = 0; i < col_qty; i++)
			{
				table->GetAsString(i, r, row[i]);
				if (1700 == table->GetColumnType(i))
				{ 
					row[i].Replace('.', format);
				}
				
			}
			rt.mRowList.emplace_back(std::move(row));
		}

	}

	whDataMgr::GetDB().Commit();
	sigExecuted(rt);
}
//-----------------------------------------------------------------------------
void ReportModel::Export()
{
	wxString sexcel;
	HKEY hKey;
	DWORD dwType = REG_SZ;
	wchar_t buf[255] = { 0 };
	DWORD dwBufSize = sizeof(buf);

	long lError = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\excel.exe",
		0,
		KEY_READ,
		&hKey);
	if (lError == ERROR_SUCCESS)
	{
		lError = RegQueryValueEx(hKey, L"Path", NULL, &dwType, (LPBYTE)buf, &dwBufSize);
		if (lError == ERROR_SUCCESS)
			sexcel = wxString(buf) + L"\\excel.exe";
		//wxString sexcel = "\"c:\\Program Files (x86)\\Microsoft Office\\Office14\\excel.exe\"";
	}


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
		ofname = wxString::Format("%s\\%s", fpath,fname);
		cout.open(ofname.wc_str(), std::ofstream::binary | std::ofstream::out | std::ofstream::trunc);
		i++;
	}

	//wxFileOutputStream fos("rep.csv");
	//wxTextOutputStream cout(fos);

	const rec::ReportTable& rt = mReportTable;

	const auto colQty = rt.mColNames.size();

	for (size_t i = 0; i < colQty; i++)
	{
		cout << "\"" << rt.mColNames[i] << "\";";
	}
	cout << std::endl;

	for (size_t row = 0; row < rt.mRowList.size(); row++)
	{
		for (size_t col = 0; col < colQty; col++)
		{
			cout << "\"" << rt.mRowList[row][col] << "\";";
		
		}
		cout << std::endl;
	}
	cout.close();

	HINSTANCE h;
	if (sexcel.IsEmpty())
		h = ShellExecuteW(NULL, L"open", ofname, NULL, fpath, SW_SHOWNORMAL);
	else
		h = ShellExecuteW(NULL, L"open", sexcel, fname, fpath, SW_SHOWNORMAL);
	

	
	if (ERROR_FILE_NOT_FOUND == (long)h)
		return;

	if (ERROR_PATH_NOT_FOUND == (long)h)
		return;

}
//-----------------------------------------------------------------------------

void ReportModel::Load(const boost::property_tree::ptree& page_val)
{
	mRepId = page_val.get<std::string>("CtrlPageReport.RepId");

	auto container = whDataMgr::GetInstance()->mContainer;
	auto modelRepList = container->GetObject<ReportListModel>("ModelPageReportList");
	auto ri = modelRepList->LoadAll(mRepId);
	if (!ri)
		return;
}
//-----------------------------------------------------------------------------
void ReportModel::Save(boost::property_tree::ptree& page_val)
{
	using ptree = boost::property_tree::ptree;
	ptree content;
	content.put("RepId", mRepId.c_str());
	page_val.push_back(std::make_pair("CtrlPageReport", content));
	
}
//-----------------------------------------------------------------------------
void ReportModel::Show()
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto modelRepList = container->GetObject<ReportListModel>("ModelPageReportList");
	auto ri = modelRepList->LoadAll(mRepId);
	if (!ri)
		return;
	sigShow();
}
//-----------------------------------------------------------------------------
void ReportModel::UpdateTitle()
{
	/*
	wxString lbl = mTitle;
	
	auto container = whDataMgr::GetInstance()->mContainer;
	auto modelRepList = container->GetObject<ReportListModel>("ModelPageReportList");
	
	auto ri = modelRepList->GetRep(mRepId);
	if (ri)
	{
		lbl = ri->mTitle;
	}



	sigUpdateTitle(lbl, mIco);
	*/
}
