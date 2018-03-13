#ifndef __MODEL_OBJPATH_H
#define __MODEL_OBJPATH_H

#include "ModelBrowserData.h"

namespace wh {
//-----------------------------------------------------------------------------

class ModelObjPath
{
	std::deque<std::shared_ptr<const IObj64>>	mData;

	static const std::shared_ptr<const IObj64> mRoot;
	void Refresh(const int64_t& cid);
public:
	ModelObjPath();
	std::shared_ptr<const IObj64> GetCurrent();
	wxString AsString()const;
	void SetId(const wxString& str);
	void SetId(const int64_t& val);

};
//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H	