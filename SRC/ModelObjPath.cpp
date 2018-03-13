#include "_pch.h"
#include "ModelObjPath.h"

using namespace wh;

//-----------------------------------------------------------------------------
class PathClsRec64 :
	public ICls64
	//, public  std::enable_shared_from_this<ClsRec64>
{
public:
	int64_t		mId;
	wxString	mTitle;
	
	PathClsRec64() {}

	PathClsRec64(const int64_t id, const wxString& title)
		:mId(id), mTitle(title)
	{
	}

	virtual const int64_t&  GetId()const override { return mId; }
	virtual const wxString& GetTitle()const override { return mTitle; };

	bool SetId(const wxString& str) { return str.ToLongLong(&mId); }
	void SetId(const int64_t& id) { mId = id; }


	virtual int64_t GetParentId()const override { return 0; }
	
	virtual       ClsKind	GetKind()const override { return ClsKind::Abstract; };
	virtual const wxString& GetMeasure()const override { return wxEmptyString2; };
	virtual const wxString& GetObjectsQty()const override { return wxEmptyString2; };

	virtual const SpPropValConstTable&	GetProperties()const override { throw; };

	virtual void ClearObjTable() override { throw; };
	virtual const std::shared_ptr<const ObjTable> GetObjTable()const override { return nullptr; };
	virtual const std::shared_ptr<const ClsTable> GetClsChilds()const override { return nullptr; };

	virtual std::shared_ptr<const ICls64> GetParent()const override { return nullptr; };

	virtual const ConstPropValTable&	GetFavCPropValue()const override { throw; };
	virtual const ConstClsFavActTable&	GetFavAProp()const override { throw; };
	virtual const FavAPropValueTable&	GetFavAPropValue()const override { throw; };
	virtual const ConstPropTable&		GetFavOProp()const override { throw; };
};
//-----------------------------------------------------------------------------
class PathObjRec : public IObj64
{
public:
	int64_t			mId;
	wxString		mTitle;
	std::shared_ptr<PathClsRec64> mCls;
	
	PathObjRec() {};

	PathObjRec(	const int64_t oid,const wxString& otitle
			  , const int64_t cid, const wxString& ctitle)
		:mId(oid), mTitle(otitle), mCls(std::make_shared<PathClsRec64>(cid,ctitle))
	{
	}

	bool SetId(const wxString& str) { return str.ToLongLong(&mId); }
	void SetId(const int64_t& val) { mId = val; }

	virtual const int64_t&  GetId()const override { return mId; }
	virtual const wxString& GetTitle()const override { return mTitle; };

	virtual wxString					GetQty()const override { return wxEmptyString; };

	bool SetClsId(const wxString& str) { return str.ToLongLong(&mCls->mId); }
	bool SetClsId(const int64_t& cid) { return mCls->mId = cid; }

	virtual SpClsConst GetCls()const override { return nullptr; }
	virtual int64_t GetClsId()const override { return mCls->mId; }

	virtual std::shared_ptr<const IObjPath64> GetPath()const override { return nullptr; }

	virtual const SpPropValConstTable&	GetProperties()const override { throw; }

	virtual std::shared_ptr<const IObj64> GetParent()const override { return nullptr; }
	virtual int64_t GetParentId()const override	{ return 0;	}

	virtual int GetActPrevios(int64_t aid, wxDateTime& dt)const override { return 0; }
	virtual int GetActNext(int64_t aid, wxDateTime& dt)const override { return 0; }
	virtual int GetActLeft(int64_t aid, double& dt)const override { return 0; }

	virtual const FavAPropValueTable&	GetFavAPropValue()const override { throw; }
	virtual const ConstPropValTable&	GetFavOPropValue()const override { throw; }

};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const std::shared_ptr<const IObj64> ModelObjPath::mRoot
= std::make_shared<PathObjRec>(1, "ObjRoot", 1, "ClsRoot");
//-----------------------------------------------------------------------------
ModelObjPath::ModelObjPath()
{
}
//-----------------------------------------------------------------------------
std::shared_ptr<const IObj64> ModelObjPath::GetCurrent()
{
	return (mData.empty()) ? mRoot : mData.back();
}
//-----------------------------------------------------------------------------
void ModelObjPath::Refresh(const int64_t& cid)
{
	TEST_FUNC_TIME;
	wxLongLong ll_cid((cid <= 1) ? 1 : cid);
	auto id = ll_cid.ToString();

	mData.clear();

	wxString query = wxString::Format(
		"SELECT oid,otitle, cid,ctitle"
		" FROM get_path_obj_info(%s, 1)"
		, id);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		size_t row = 0;
		unsigned int rowQty = table->GetRowCount();
		for (size_t i = rowQty; i > 0; --i)
		{
			row = i - 1;
			auto obj = std::make_shared<PathObjRec>();
			obj->SetId(table->GetAsString(0, row));
			obj->mTitle = table->GetAsString(1, row);
			obj->mCls->SetId(table->GetAsString(2, row));
			obj->mCls->mTitle = table->GetAsString(3, row);
			mData.emplace_back(obj);
		}
	}//if (table)
	whDataMgr::GetDB().Commit();
}

//-----------------------------------------------------------------------------
wxString ModelObjPath::AsString() const
{
	wxString ret = "/";
	std::for_each(mData.crbegin(), mData.crend(), [&ret]
	(const std::shared_ptr<const IObj64>& curr)
	{
		ret = wxString::Format("/[%s]%s%s"
			, curr->GetCls()->GetTitle()
			, curr->GetCls()->GetIdAsString()
			, ret);
		
	});
	return ret;
}
//-----------------------------------------------------------------------------

void ModelObjPath::SetId(const wxString& str)
{
	if (!str.IsEmpty())
	{
		int64_t tmp;
		if (str.ToLongLong(&tmp))
		{
			SetId(tmp);
			return;
		}
	}
	SetId(1);
}
//-----------------------------------------------------------------------------
void ModelObjPath::SetId(const int64_t& val)
{
	Refresh(val);
}
