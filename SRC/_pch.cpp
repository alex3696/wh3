#include "_pch.h"

#ifdef __WX__
wxMenuItem* AppendBitmapMenu(wxMenu* menu,int id,const wxString& label,const wxBitmap &bmp)
{
	auto item = new wxMenuItem(menu,id,label);
	item->SetBitmap(bmp);	
	menu->Append(item);
	return item;
}
#endif // #ifdef __WXMSW__ || __WX__
