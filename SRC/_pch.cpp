#include "_pch.h"

wxMenuItem* AppendBitmapMenu(wxMenu* menu,int id,const wxString& label,const wxBitmap &bmp)
{
	auto item = new wxMenuItem(menu,id,label);
	item->SetBitmap(bmp);	
	menu->Append(item);
	return item;
}

