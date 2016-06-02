
//////////////////////////////////////////////////////////////////////
// InputFile.cpp: implementation for the CInputFile class.
//
//////////////////////////////////////////////////////////////////////
#include "stdio.h"
#include "InputFile.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CInputFile::CInputFile()
{
	m_nBufferLen = 0;
	m_ProcessRecord = 0 ;
	m_fp = NULL;
	m_nColNum = 0;
	m_nCurCol = 0;
	m_bFixLen = false;
}

CInputFile::~CInputFile()
{
	Close() ;
}


CInputFile& CInputFile::operator>>(string& rStrData)
{
	char str[1024];
	memset( str , 0 , sizeof(str));
	GetData( m_nCurCol ,str  ) ;

	rStrData = str ;
	m_nCurCol ++;
	return *this;
}
CInputFile&  CInputFile::operator>>(int& rIntData)
{
	char str[1024];
	memset( str , 0 , sizeof(str));
	GetData( m_nCurCol ,str  );
	rIntData = atoi( str );
	m_nCurCol ++;
	return *this;
}

CInputFile& CInputFile::operator>>(long long& rIntData)
{
	char str[1024];
	memset( str , 0 , sizeof(str));
	GetData( m_nCurCol ,str  );
	rIntData = atoll( str );
	m_nCurCol ++;
	return *this;
}
CInputFile&  CInputFile::operator>>(long& rLongData)
{
	char str[1024];
	memset( str , 0 , sizeof(str));
	GetData( m_nCurCol ,str  );
	rLongData = atol( str);
	m_nCurCol ++;
	return *this;
}
CInputFile&  CInputFile::operator>>(double& rDoubleData)
{
			char str[1024];
	memset( str , 0 , sizeof(str));
	 GetData( m_nCurCol ,str  );
	rDoubleData = atof(str);
	m_nCurCol ++;
	return *this;
}
CInputFile&  CInputFile::operator>>(char* CharData)
{
	char str[1024];
	memset( str , 0 , sizeof(str));
	GetData( m_nCurCol ,str  );
	strcpy( CharData , str );
	m_nCurCol ++;
	return *this;
}

/*
int CInputFile::Open(const string & str , int* nCols , int nColNum)
{
	return Open(str.c_str() , nCols , nColNum);
}
*/
//打开固定长度文件
int  CInputFile::Open(const char *filename,int *nCols , int nColNum )
{
	
	if ( (m_fp = fopen(filename,"r")) == NULL )
	{
		//theLog<< " open file failed!\n" << ende;
		//theLog<< "open file failed!" << __FILE__ << __LINE__ << errno << ende;
		printf("open file failed,errno:[%d]!",errno) ;
		return -1;
	}
	m_bFixLen = true;//为固定长度
	m_nColNum = nColNum;
	for(int i = 0 ; i < nColNum ; i++)
	{
		m_nCols[ i ] = nCols[ i ];
	}
	m_ProcessRecord = 0;
	m_nCurCol = 0;
	return 0;
}
//打开非固定长度文件
int  CInputFile::Open(char *filename,char chSplit, bool bIgnore, bool bSavespace )
{
	const char* filename1 = filename;
	return Open(filename1 ,  chSplit, bIgnore);
}

int  CInputFile::Open(const char *filename,char chSplit, bool bIgnore, bool bSavespace )
{
	if ( (m_fp = fopen(filename,"r")) == NULL )
	{
		//theLog<< " open file failed!\n" << ende;
		//theLog<< "open file failed!" << __FILE__ << __LINE__ << errno << ende;
		printf("open file failed,errno:[%d]!",errno) ;
		return -1;
	}
	m_bFixLen = false;//为以分隔符分隔文件
	m_chSplit = chSplit; 
	m_ProcessRecord = 0;
	m_nCurCol = 0;
	m_bIgnore = bIgnore;
	m_bSavespace = bSavespace;
	return 0;
}
int CInputFile::Close()
{
	if ( m_fp ) 
		fclose(m_fp);
	m_fp = NULL ;
	return 0 ;
	
}
int CInputFile::operator ++(int)
{
	return FetchData();
}

string CInputFile::operator [](int nColNo)
{
	char str[1024];
	memset(str,0,sizeof(str));
	GetData(nColNo , str);
	return str;
	
}

void CInputFile::GetIntColumn(int nColNo , int &nValue)
{
	char str[1024];
	int n;
	memset(str,0,sizeof(str));
	GetData(nColNo , str);
	nValue = atoi( str );
}

void CInputFile::GetDoubleColumn(int nColNo , double &dValue)
{
	char str[1024];
	memset(str,0,sizeof(str));
	GetData(nColNo , str);
	dValue = atof( str );
}


int CInputFile::FetchData()
{
	int m_nCurColumn  = 0;
	memset(m_buffer , 0 , sizeof(m_buffer));
	int n = util_getline( m_fp , 1024 ,m_buffer);
	if(n >= 0)
	{
		m_nBufferLen = n;
		m_ProcessRecord ++;	
		m_nCurCol = 0;
		return n;
	}
	else
		return 0;
}

// 取文件的一行 hm 
int CInputFile::FetchData2(string & str)
{
	int m_nCurColumn  = 0;
	memset(m_buffer , 0 , sizeof(m_buffer));
	int n = util_getline( m_fp , 1024 ,m_buffer);
	if(n >= 0)
	{
		m_nBufferLen = n;
		m_ProcessRecord ++;	
		m_nCurCol = 0;
		str.assign(m_buffer);
		return n;
	}
	else
		return 0;
}



bool CInputFile::GetData(int  nCurCol ,char *str  )
{
	//定长文件,计算在文件行中的开始位置
	if(m_bFixLen)
	{
		int nBeginPos = 0;
		for(int i = 0 ; i < nCurCol ; i++)
			nBeginPos += m_nCols[ i ];
		int nEndPos = nBeginPos + m_nCols[nCurCol];
		if(nEndPos > m_nBufferLen)
		{
			//theLog<< "get column failed!" << __FILE__ << __LINE__ << errno << ende;
			printf("get column failed!") ;
			return false;
		}
		strncpy(str , m_buffer + nBeginPos , nEndPos - nBeginPos);
		str[ nEndPos - nBeginPos ] = '\0';
		if(m_bSavespace == false) util_trim( str );
	}
	else if(!m_bIgnore)
	{
		 
		char *szBeginPos = m_buffer;
		for(int i = 0 ; i < nCurCol   ; i ++)
		{
			char *szTemp = szBeginPos ;
			szBeginPos = strchr( szTemp , m_chSplit );
			if (i != (nCurCol-1)) szBeginPos++;
			if( szBeginPos >= m_nBufferLen + m_buffer )
			{
				//theLog<< "get column failed!" << __FILE__ << __LINE__ << errno << ende;
				printf("get column failed!") ;
				return false;
			}
			
		}
		
		if (szBeginPos == NULL) return false;
		if (nCurCol != 0) szBeginPos++;
		char *szEndPos = strchr( szBeginPos  , m_chSplit );
		if(szEndPos == NULL )
			szEndPos = m_buffer + m_nBufferLen;
		strncpy(str , szBeginPos, szEndPos - szBeginPos);
		str[szEndPos - szBeginPos] = '\0';
		if(m_bSavespace == false) util_trim( str );
	}
	else
	{
		 
		char *szBeginPos = m_buffer;
		for(int i = 0 ; i < nCurCol   ; i ++)
		{
			char *szTemp = szBeginPos ;
			szBeginPos = strchr( szTemp , m_chSplit );
			while (*(szBeginPos) == m_chSplit) szBeginPos++;
			if( szBeginPos >= m_nBufferLen + m_buffer )
			{
				//theLog<< "get column failed!" << __FILE__ << __LINE__ << errno << ende;
				printf("get column failed!") ;
				return false;
			}
			
		}
		
		if (szBeginPos == NULL) return false;
		char *szEndPos = strchr( szBeginPos  , m_chSplit );
		if(szEndPos == NULL )
			szEndPos = m_buffer + m_nBufferLen;
		strncpy(str , szBeginPos, szEndPos - szBeginPos);
		str[szEndPos - szBeginPos] = '\0';
		if(m_bSavespace == false) util_trim( str );
	}
	return true ;
	// bIgnore=true 忽略连续的分隔符，即多个连续的分隔符看作是一个分隔符
} 


int CInputFile::getColumnSize()
{
	string strtmp(m_buffer);	
	int nCount = count(strtmp.begin(), strtmp.end(), m_chSplit);
	return nCount;
}



/////////////////////////////////////////////////////////////////////////////////////////////
	/*enforced getline function , you can specify the type of file*/
static const char *crlfs[] =
{	
	"\x0d\x0a",			/*	DOS/Windows style*/
	"\x0a\x0d",			/*	UNIX style*/
	"\x0d\x0d\x0a",     /*UNIX style2*/
	"\x0a"				/*Macintosh style*/
};
int util_getlineex( FILE *fd , int nLineLen , char *line ,int nstype)
{
	int c;
	int i = 0;	
	int nCrlfPtr = 0;
	char *crlf;
	memset( line , 0 , sizeof( line) );
	c = fgetc( fd );
	if( c == EOF )
		return -1;
	if( nstype < 0 || nstype > 3 )
	{
		//theLog<< "util_getlineex nstype error!\n"<<__FILE__<<__LINE__<<ende ;
		printf("util_getlineex nstype error!\n") ;
		return -1;
	}
	crlf = (char *)crlfs[ nstype ];
	while(  c != EOF && i < nLineLen )
	{
		if ((char) c == crlf[nCrlfPtr])
		{
			nCrlfPtr ++;
			//到达了换行符的中止位置
			if (crlf[nCrlfPtr] == 0)
			{
				nCrlfPtr = 0;
				break;
			}
		}
		else
			nCrlfPtr = 0;
		line[ i++ ] = c;
		c = fgetc(fd );
	}
	line[ i ] = '\0';
	return i;
}

int util_getline( FILE *fd , int nLineLen , char *line)
{
	int c;
	int i = 0;
	memset( line , 0 , sizeof( line) );
	c = fgetc( fd );
	if( c == EOF )
		return -1;
	while( c == '\r' || c == '\n' )
	{
		c = fgetc( fd );

		if( c == EOF )
			return -1;
	}
	while( c != '\r' && c != '\n' && c != EOF && i < nLineLen )
	{
		
		line[ i++ ] = char(c);

		c = fgetc(fd );
	}
	line[ i ] = '\0';
	return i;
}

/*delete control characters*/
void util_trim(char *buf )
{
	char tmp[1024]; 
	char *pbegin = buf;
	int nlen = strlen(buf);
	
	char *pend = buf + nlen - 1;

	memset( tmp , 0 , sizeof( tmp ));
	while( pbegin < buf + nlen && strchr( "\r\n\t " , *pbegin))
		pbegin ++;
	while( pend >= buf && strchr( "\r\n\t " , *pend))
		pend --;
	if( pend >= pbegin )
		strncpy( tmp , pbegin , pend - pbegin + 1);
	memset( buf , 0 , sizeof( buf ));
	strcpy( buf , tmp );
}


