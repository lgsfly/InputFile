
//////////////////////////////////////////////////////////////////////
// InputFile.h: interface for the CInputFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef INPUTFILE_H
#define INPUTFILE_H

#include <string.h>

#include <string>
#include <algorithm>

using namespace std ;


#define ende ('\001') // error
#define endw ('\002') // warning
#define endi ('\003') // information
#define endt ('\004') // log to table
#define endr ('\005') // log for report


int util_getlineex( FILE *fd , int nLineLen , char *line ,int nstype);
int util_getline( FILE *fd , int nLineLen , char *line);
void util_trim(char *buf );

class CInputFile
{
public:
	CInputFile();
	virtual ~CInputFile();
	CInputFile& operator>>(string& rStrData);
	CInputFile& operator>>(int& rIntData);
	CInputFile& operator>>(long long& rIntData);
	CInputFile& operator>>(long& rLongData);
	CInputFile& operator>>(double& rDoubleData);
	CInputFile& operator>>(char* CharData);
	string operator [](int nColNo);
	void GetIntColumn(int nColNo , int &nValue);
	void GetDoubleColumn(int nColNo , double &dValue); // 修改
	int operator ++(int);
	int FetchData();
	int FetchData2(string & str);
	bool GetData(int  m_nColumn ,char *str  );	


	int	m_ProcessRecord;
	int  Open(const char *filename,int *nCols , int nColNum ); //根据设定列长度
	//根据分隔符(不定长文件)
	// bIgnore=true 忽略连续的分隔符，即多个连续的分隔符看作是一个分隔符
	// bSavespace=true 保留连续的空格，即多个空格包含在一个字段中
	int Open(char * filename , char chSplit, bool bIgnore = false, bool bSavespace = false); 
	
	//int Open(const string & str , int *nCols , int nColNum);
	int  Open(const char *filename,char chSplit, bool bIgnore = false, bool bSavespace = false );
	//***begin*** yangping  2011/06/13 add

	int  getColumnSize();

	//***end*** yangping  2011/06/13 add
	
	int Close();
	FILE *m_fp;
	int m_nCols[100]; // 最多处理100列
	int m_nColNum;
	char m_buffer[1024];
	int m_nBufferLen;
	int m_nCurCol;	
	bool m_bFixLen;
	char m_chSplit;
	bool m_bIgnore; // 是否忽略连续的分隔符，即多个连续的分隔符看作是一个分隔符
	bool m_bSavespace; // 是否保留连续的空格
};
#endif
