#ifndef __MODEL_CLSPATH_H
#define __MODEL_CLSPATH_H

#include "ModelBrowserData.h"

namespace wh {
//-----------------------------------------------------------------------------

class ModelClsPath
{
	std::deque<std::shared_ptr<const ICls64>>	mData;

	static const std::shared_ptr<const ICls64> mRoot;
	void Refresh(const int64_t& cid);
public:
	ModelClsPath();
	std::shared_ptr<const ICls64> GetCurrent();
	wxString AsString()const;
	void SetId(const wxString& str);
	void SetId(const int64_t& val);

};
//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H	