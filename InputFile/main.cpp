#include <iostream>
#include "InputFile.h"
#include "stdio_file.h"
#include <vector>
#include "libbase.h"
#include "stringUtils.h"

using namespace std;

struct S_MonFee
{
	long long		m_iUserId ;
	long long		m_iAcctID ;
	double			m_dAmt ;         //账单金额
	double			m_dDiscAmt ;     //优惠金额
	int					m_iRatingTime ;  //计费时长
	int					m_iCallTimes ;   //通话次数
	int					m_iKoufei;       //1为欠费。其他为已销账费用
	int					m_iDaifu;        //2为代付，0为自付

	std::string  ToString() const ;
};

string S_MonFee::ToString() const
{
	char szBuf[1024] ;
	sprintf(szBuf,BASE_TEXT("%lld~%s~%s~%d~%d~%lld~%.2f~%.2f~%s~%s~%d~%d"),m_iUserId," "," ",
			m_iRatingTime, m_iCallTimes, m_iAcctID, m_dAmt, m_dDiscAmt," "," ",m_iKoufei,m_iDaifu) ;
	return szBuf;
}

typedef vector<S_MonFee>	MONFEE_VEC ;

MONFEE_VEC g_vecMonFee;

int GetOneData(CInputFile *inputFile)
{
	S_MonFee monfee ;
	memset(&monfee, 0, sizeof(monfee));
	string callno,strBrand,billSrc,other;
	if ( (*inputFile) ++ == 0 )
	{
		return -1 ;
	}
	else
	{
		(*inputFile) >> monfee.m_iUserId
				>> callno
				>> strBrand
				>> monfee.m_iRatingTime
				>> monfee.m_iCallTimes
				>> monfee.m_iAcctID
				>> monfee.m_dAmt
				>> monfee.m_dDiscAmt
				>> billSrc
				>> other
				>> monfee.m_iKoufei
				>> monfee.m_iDaifu;
	}
	g_vecMonFee.push_back(monfee) ;
	return 0 ;
}

//ostream& operator<<(ostream& os,const S_MonFee& monfee)
//{
//	os << monfee.ToString() ;
//	return os ;
//}

int main()
{
	CInputFile *inputFile = new CInputFile();
	if ( -1 == inputFile->Open("TextFee", '~') )
	{
		printf("打开文件[%s]出错!","TextFee") ;
		return -1;
	}
	while ( GetOneData(inputFile) == 0 );

	string strFile = "TextFile";
	StdioFile textFile ;
	if ( !textFile.Open(strFile.c_str(),"w") )
	{
		printf("打开文件[%s]出错!","TextFile") ;
	}

	for ( int i = 0; i < g_vecMonFee.size(); ++i)
	{
		string str = g_vecMonFee[i].ToString() ;
		textFile.WriteF("%s\n",str.c_str());
	}

	cout << "Hello World!" << endl;
	delete inputFile;
	return 0;
}

