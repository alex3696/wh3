#include "_pch.h"
#include "whTableMgr.h"


//---------------------------------------------------------------------------
/// ������� �������� ���� ���������� � ��������� ��������
wxString SortDirToStr(const whSortDir dir)
{
	switch(dir)
	{	
		default: break;
		case whSORT_ASC: return "�� �����������";
		case whSORT_DESC: return "�� ��������";
	}
	return "��� ����������";
}
/// ������� ���������� �������� ���������� � �������� ���� 
whSortDir StrToSortDir(const wxString& str)
{
	if(str=="�� �����������")
		return whSORT_ASC;
	else if(str=="�� ��������")
		return whSORT_DESC;
	return whSORT_NO;
}




//---------------------------------------------------------------------------
/// ������� �������� ���� �������� ��������� � ��������� ��������
wxString CompOpToStr(const whCompOp dir)
{
	switch(dir)
	{	
		default: break;
		case whCO_EQ: return "=";
		case whCO_NOTEQ: return "<>";
		case whCO_LESS: return "<";
		case whCO_MORE: return ">";
		case whCO_LESSEQ: return "<=";
		case whCO_MOREEQ: return ">=";
		case whCO_LIKE: return "LIKE";
		case whCO_NOTLIKE: return "NOT LIKE";
	}
	return "����������";
}
/// ������� ���������� �������� �������� ��������� � �������� ���� 
whCompOp StrToCompOp(const wxString& str)
{
	if(str=="=")		return whCO_EQ;
	else if(str=="<>")	return whCO_NOTEQ;
	else if(str=="<")	return whCO_LESS;
	else if(str==">")	return whCO_MORE;
	else if(str=="<=")	return whCO_LESSEQ;
	else if(str==">=")	return whCO_MOREEQ;
	else if(str=="LIKE")	return whCO_LIKE;
	else if(str=="NOT LIKE")return whCO_NOTLIKE;
	
	
	return whCO_UNKNOWN;
}










//---------------------------------------------------------------------------
/// ������� �������� ���������� �������� � ��������� ��������
wxString LogicOpToStr(const whLogicOp dir)
{
	switch(dir)
	{	
		default: break;
		case whLO_OR: return "OR";
		case whLO_AND: return "AND";
	}
	return "����������";
}
/// ������� ���������� �������� � �������� ����������� ���� 
whLogicOp StrToLogicOp(const wxString& str)
{
	if(str=="OR")		return whLO_OR;
	else if(str=="AND")	return whLO_AND;
	return whLO_UNKNOWN;
}
