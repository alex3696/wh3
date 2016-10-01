#ifndef __MFAVORITES_H
#define __MFAVORITES_H

#include "TModelArray.h"
#include "db_rec.h"
//-------------------------------------------------------------------------
namespace wh{

class MFavProp
{
public:
	unsigned long	mId;
	wxString		mLabel;
	FieldType		mType;
	wxString		mVarArray;
	wxString		mVarStrict;
};

class MFavPropVal
{
public:
	MFavProp		mProp;
	wxString		mVal;
};

class MFavObj
{
public:
	unsigned long	mId;
	wxString		mLabel;
	wxString		mQty;
	unsigned long	mLastMoveLogId;
	unsigned long	mLastActLogId;

	std::shared_ptr<MFavObj>	mPathParent;
	
	std::vector<MFavPropVal>	mProp;

};



class MFavType
{
public:
	unsigned long	mId;
	wxString		mLabel;
	unsigned long	mType;
	wxString		mComment;
	wxString		mMeasure;

	std::shared_ptr<MFavType>	mParent;
	std::shared_ptr<MFavType>	mDefaultParent;

	std::vector<MFavPropVal>	mProp;

	std::vector<MFavObj>		mObj;

};


class MFavNode
{
public:
	unsigned long	mId;
	wxString		mLabel;
	
	// ����������� �� �����
	// ����������� �� ����
	// ��� �����������
	
	// ������ �� ������
	
	// ������ �� �����

	// ������ �� ��������� ???

	// ������ �� ��������� ???

	// ���������� �������� ����

	// ���������� ����� �������� �������� (����, �����, ������������)
	// ���������� ���������������� �������� ��������
	//��������_1
	//	��������_1
	//	��������_2

	// ���������� ����������� ��������


};






}//namespace wh{
//-------------------------------------------------------------------------
#endif // __****_H