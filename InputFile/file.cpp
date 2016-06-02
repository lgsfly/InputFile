//#include "stdafx.h"
//#include "libbase.h"
#include "file.h"
#include "stringUtils.h"
//#include "base/liblog.h"
//#include <stdio.h>
//#include <sys/stat.h>
//#include <sys/utime.h>
#ifdef WIN32
#include <io.h>
#include <sys/locking.h>
#include <direct.h>
//#include <WinSock2.h>
//#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#define _access access
#include <dirent.h>
#include <utime.h>
#include <errno.h>
extern int errno;
#endif


//#include <BossLog.h>
#ifdef WIN32

#undef MoveFile
#undef CopyFile
#undef GetCurrentDirectory

#define chsize _chsize
#define mkdir _mkdir
#define unlink _unlink
#define fstat _fstat
#endif

#define _BOSS_MAX_FNAME 255
#ifndef WIN32
#define _getcwd getcwd
#endif

//__WILLOW_BEGIN_NAMESPACE

#if !defined(WIN32)

const char   File::separatorChar     = '/';
const char   File::separator[]={"/"};

const char   File::pathSeparatorChar = ':';
const char   File::pathSeparator[]={":"};

const char   File::lineSeparator[]={"\n"};

#else

const char   File::separatorChar     = '\\';
const char   File::separator[]={"\\"};

const char   File::pathSeparatorChar = ';';
const char   File::pathSeparator[]={";"};

const char   File::lineSeparator[]={"\r\n"};

#endif




static inline TBOOL IsDirSep(char ch)
{
	return ch == File::separatorChar ;
}

void AfxGetRoot(LPCSTR lpszPath, string& strRoot);

#ifdef WIN32
int winstat(const char *path,struct stat *status);
int winstat(const char *path, struct stat *status)
{
    char rpath[512];
    struct _finddata_t cf;
    long hFile;

    strcpy(rpath,path);
    if(rpath[strlen(rpath)-1]=='\\')
    {
        rpath[strlen(rpath)-1]='\0';
    }

    if((hFile=_findfirst(rpath, &cf))==-1L)
       return(-1);

    memset(status,0,sizeof(struct _stat));
    status->st_size=cf.size;
    status->st_gid=0;
    status->st_atime=cf.time_access;
    status->st_ctime=cf.time_create;
    status->st_mtime=cf.time_write;
    cf.attrib|=_S_IREAD;
    if(!(cf.attrib&_A_RDONLY))
        status->st_mode|=_S_IWRITE;
    if(!(cf.attrib&_A_SUBDIR))
        status->st_mode|=_S_IFDIR;
    else
        status->st_mode|=_S_IFREG;
    status->st_nlink=1;

    if(_findnext(hFile,&cf)==0)
    {
        return(-1);
    }
    
    _findclose(hFile);
    return(0);
}
#endif



////////////////////////////////////////////////////////////////////////////
// File implementation

File::File()
{
    m_pStream = NULL;
    m_bCloseOnDelete = FALSE;
}

File::File(FILE *hFile)
{
    m_pStream = hFile;
    m_bCloseOnDelete = FALSE;
}


File::File(LPCSTR lpszFileName, LPCSTR lpszOpenFlags)
{
    BASE_ASSERT(lpszFileName!=NULL);

    Open(lpszFileName, lpszOpenFlags);
}

File::~File()
{
    if (m_pStream != NULL && m_bCloseOnDelete)
        Close();
}


File* File::Duplicate() const
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    File* pFile = new File(NULL);
    FILE *hFile;
    if((hFile=freopen(m_strFileName.c_str(),(const char*)m_strOpenFlags.c_str(),m_pStream))==NULL)
    {
        delete pFile;
        return(NULL);
    }
    pFile->m_pStream = hFile;
    BASE_ASSERT(pFile->m_pStream != NULL);
    pFile->m_bCloseOnDelete = m_bCloseOnDelete;
    return pFile;
}


TBOOL File::Open(LPCSTR lpszFileName, LPCSTR lpszOpenFlags/*,TCFileException* pException*/)
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(lpszFileName!=NULL);


    m_bCloseOnDelete = FALSE;
    m_pStream = NULL; //(UINT)NULL;

    m_strFileName=lpszFileName;
    
    m_strOpenFlags=lpszOpenFlags;

    // attempt file creation
    FILE *hFile = fopen(lpszFileName, m_strOpenFlags.c_str());
    if (hFile == NULL)
    {
        return FALSE;
    }
    m_pStream = hFile;
    m_bCloseOnDelete = TRUE;

    return TRUE;
}


LONG File::Read(void* lpBuf, LONG nCount)
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    if (nCount == 0)
        return 0;

    BASE_ASSERT(lpBuf != NULL);

    long dwRead;
    if((dwRead=fread(lpBuf, sizeof(char),(int)nCount,m_pStream))==0)
    {
        if(ferror(m_pStream)!=0)
            return(-1);
    }

    return dwRead;
}


TBOOL File::Write(const void* lpBuf, LONG nCount)
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    if (nCount == 0)
        return(TRUE);     // avoid Win32 "null-write" option

    BASE_ASSERT(lpBuf != NULL);

    long nWritten;
    nWritten=(long)fwrite(lpBuf,sizeof(char),(size_t)nCount,m_pStream);

    if (nWritten != nCount)
        return(FALSE);
    else
        return(TRUE);
}


LONG File::Seek(LONG lOff, LONG nFrom)
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);
    BASE_ASSERT(nFrom == begin || nFrom == end || nFrom == current);

    long dwNew = fseek(m_pStream,lOff,(int)nFrom);
    if (dwNew  == (unsigned long)-1)
        return(-1);

    return dwNew;
}


long File::GetPosition() const
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    long dwPos = ftell(m_pStream);
    if (dwPos  == (unsigned long)-1)
        return(-1);

    return dwPos;
}


TBOOL File::Flush()
{
    BASE_ASSERT_VALID(this);

    if (m_pStream == NULL)
        return(FALSE);

    if (fflush(m_pStream)!=0)
        return(FALSE);
    return(TRUE);
}


unsigned long File::ReadHuge(void* lpBuffer, unsigned long dwCount)
{
    return (unsigned long)Read(lpBuffer, (UINT)dwCount);
}


void File::WriteHuge(const void* lpBuffer, unsigned long dwCount)
{ 
    Write(lpBuffer, (UINT)dwCount); 
}


unsigned long File::SeekToEnd()
{ 
    return Seek(0, File::end); 
}


void File::SeekToBegin()
{ 
    Seek(0, File::begin); 
}



TBOOL File::Close()
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    TBOOL bError = FALSE;
    if (m_pStream != NULL)
        bError = (fclose(m_pStream)!=0);

    m_pStream = NULL;
    m_bCloseOnDelete = FALSE;
    m_strFileName.clear();

	

    if (bError)
        return(FALSE);
    return(TRUE);
}


void File::Abort()
{
    BASE_ASSERT_VALID(this);
    if (m_pStream != NULL)
    {
        // close but ignore errors
        fclose(m_pStream);
        m_pStream = NULL;
    }
    //m_strFileName.Empty();
	m_strFileName.clear() ;
}


TBOOL File::LockRange(unsigned long dwPos, unsigned long dwCount)
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    LONG lCurrentPos=GetPosition();
    if(Seek(dwPos,File::begin)==FALSE)
    {
        return(FALSE);
    }

#ifdef WIN32
    if(_locking(fileno(m_pStream),_LK_LOCK,dwCount))
#else
    if(lockf(fileno(m_pStream),F_LOCK,dwCount))
#endif
        return(FALSE);
    Seek(lCurrentPos,File::begin);
    
    return(TRUE);
}


TBOOL File::UnlockRange(unsigned long dwPos, unsigned long dwCount)
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    LONG lCurrentPos=GetPosition();
    if(Seek(dwPos,File::begin)==FALSE)
    {
        return(FALSE);
    }

#ifdef WIN32
    if(_locking(fileno(m_pStream),_LK_UNLCK,dwCount))
#else
    if(lockf(fileno(m_pStream),F_ULOCK,dwCount))
#endif
    {
        return(FALSE);
    }
    Seek(lCurrentPos,File::begin);
    
    return(TRUE);
}


TBOOL File::SetLength(unsigned long dwNewLen)
{
	//BASE_LOG_DEBUG("This method SetLength is not realized.\n") ;
	return FALSE ;


	/*
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    Seek((LONG)dwNewLen, (UINT)begin);

#ifdef _SCO_UNIX
    if(ftruncate(fileno(m_pStream),(long)dwNewLen))
#else
    if(_chsize(fileno(m_pStream),(long)dwNewLen))
#endif
        return(FALSE);
    else
        return(TRUE);
		*/
}


unsigned long File::GetLength() const
{
    BASE_ASSERT_VALID(this);

    unsigned long dwLen, dwCur;

    // Seek is a non const operation
    File* pFile = (File*)this;
#ifndef WIN32 
    dwCur = pFile->Seek(0L, current);
    dwLen = pFile->SeekToEnd();
	BASE_VERIFY(dwCur == (unsigned long)pFile->Seek(dwCur, begin));
#else
	dwCur = pFile->GetPosition() ;
	BASE_ASSERT(pFile->SeekToEnd() == 0 ) ;
	dwLen = pFile->GetPosition() ;
	pFile->Seek(dwCur,begin) ;
#endif
   
    return dwLen;
}

// File does not support direct buffering (CMemFile does)
UINT File::GetBufferPtr(UINT nCommand, UINT /*nCount*/,
    void** /*ppBufStart*/, void** /*ppBufMax*/)
{
    BASE_ASSERT(nCommand == bufferCheck);
    return 0;   // no support
}


TBOOL File::Rename(LPCSTR lpszOldName, LPCSTR lpszNewName)
{
    if (rename(lpszOldName, lpszNewName))
        return(FALSE);
    else
        return(TRUE);
    //TCFileException::ThrowOsError((LONG)-1);
}

TBOOL File::Remove(LPCSTR lpszFileName)
{
    if (unlink(lpszFileName))
    {
        return(FALSE);
    }
    else
        return(TRUE);
    //    TCFileException::ThrowOsError((LONG)-1);
}

TBOOL AfxComparePath(LPCSTR lpszPath1, LPCSTR lpszPath2)
{
    // use case insensitive compare as a starter
    if (strcasecmp(lpszPath1, lpszPath2) != 0)
        return FALSE;

    int nLen = strlen(lpszPath1);
    if (nLen != (int)strlen(lpszPath2))
        return FALSE;
    
    BASE_ASSERT(nLen < _BOSS_MAX_PATH);

    return TRUE; // otherwise file name is truly the same
}


UINT stGetFileName(LPCSTR lpszPathName, LPSTR lpszTitle, UINT nMax)
{
    BASE_ASSERT(lpszPathName!=NULL);

    // always capture the complete file name including extension (if present)
    LPSTR lpszTemp = (LPSTR)lpszPathName;
    for (LPCSTR lpsz = lpszPathName; *lpsz != '\0'; lpsz ++ ) //= _tcsinc(lpsz))
    {
        // remember last directory/drive separator
        if (*lpsz == '\\' || *lpsz == '/' || *lpsz == ':')
            lpszTemp = (LPSTR) (lpsz+1) ; //_tcsinc(lpsz);
    }

    // lpszTitle can be NULL which just returns the number of bytes
    if (lpszTitle == NULL)
        return strlen(lpszTemp)+1;

    // otherwise copy it into the buffer provided
    strncpy(lpszTitle, lpszTemp, nMax);
    return 0;
}

UINT stGetFileTitle(LPCSTR lpszPathName, LPSTR lpszTitle, UINT nMax)
{
    BASE_ASSERT(lpszPathName!=NULL);

    return(stGetFileName(lpszPathName,lpszTitle,nMax));
}


string File::GetFileName() const
{
    BASE_ASSERT_VALID(this);

    FileStatus status;
    GetStatus(status);
    
	strcpy(status.m_szFullName,m_strFileName.c_str()) ;
	char strResult[_BOSS_MAX_FNAME+1] ;
    stGetFileName(status.m_szFullName, strResult,
        _BOSS_MAX_FNAME);
   
    return strResult;
}

//#ifdef WIN32
//#undef GetFileTitle
//#endif


string File::GetFileTitle() const
{
    BASE_ASSERT_VALID(this);

    FileStatus status;
    GetStatus(status);
    char strResult[_BOSS_MAX_FNAME+1];
    stGetFileTitle(status.m_szFullName, strResult,
        _BOSS_MAX_FNAME);    
    return strResult;
}


string File::GetFilePath() const
{
    BASE_ASSERT_VALID(this);

    FileStatus status;
    GetStatus(status);
    return status.m_szFullName;
}

/////////////////////////////////////////////////////////////////////////////
// File Status implementation

TBOOL File::GetStatus(FileStatus& rStatus) const
{
    BASE_ASSERT_VALID(this);

    //memset(&rStatus, 0, sizeof(FileStatus));

    // copy file name from cached m_strFileName
    strncpy(rStatus.m_szFullName, m_strFileName.c_str(),
        _countof(rStatus.m_szFullName));
	
	
#ifdef WIN32
    struct _stat status;
#else
    struct stat status;
#endif
    if(m_pStream != NULL)
    {
        
#ifdef WIN32
        if(fstat(fileno(m_pStream),&status))
#else
        if(fstat(fileno(m_pStream),&status))
#endif
            return(FALSE);

//        rStatus.m_ctime=status.st_ctime;
//        rStatus.m_atime=status.st_atime;
//        rStatus.m_mtime=status.st_mtime;
        rStatus.m_size=status.st_size;
        rStatus.m_attribute=(unsigned long)status.st_mode;

//        if (rStatus.m_ctime.getTime() == 0)
//            rStatus.m_ctime = rStatus.m_mtime;

//        if (rStatus.m_atime.getTime() == 0)
//            rStatus.m_atime = rStatus.m_mtime;

    }
    return TRUE;
}


TBOOL File::GetStatus(LPCSTR lpszFileName, FileStatus& rStatus)
{
    // attempt to fully qualify path first
    /*
    if (!BossAfxFullPath(rStatus.m_szFullName, lpszFileName))
    {
        rStatus.m_szFullName[0] = '\0';
        return FALSE;
    }
    */
    
    strcpy(rStatus.m_szFullName,lpszFileName);
    
    struct stat status;
    if(stat(lpszFileName,&status))
    {
#ifdef _DEBUG
        //BASE_LOG_ERROR("File::GetStatus [%s] failed(errno=%d,%s)\n",
         //          lpszFileName,errno,strerror(errno));
		printf("File::GetStatus [%s] failed(errno=%d,%s)\n",
			          lpszFileName,errno,strerror(errno));
#endif
        return(FALSE);
    }

//    rStatus.m_ctime=status.st_ctime;
//    rStatus.m_atime=status.st_atime;
//    rStatus.m_mtime=status.st_mtime;
    rStatus.m_size=status.st_size;
    rStatus.m_attribute=(unsigned long)status.st_mode;

//    if (rStatus.m_ctime.getTime() == 0)
//        rStatus.m_ctime = rStatus.m_mtime;

//    if (rStatus.m_atime.getTime() == 0)
//        rStatus.m_atime = rStatus.m_mtime;

    return TRUE;
}


void File::SetStatus(LPCSTR /*lpszFileName*/, const FileStatus& /*status*/)
{
	// 没有实现
    return;
}


// 文件操作工具函数
TBOOL File::FileExist(const string& strFileName)
{
#ifndef WIN32
    struct stat status;
#endif
    
#ifdef WIN32
    //if(!winstat((LPCSTR)strFileName,&status))
    if(_access((LPCSTR)strFileName.c_str(),00)!=-1)
#else
    if(!stat((LPCSTR)strFileName.c_str(),&status))
#endif
        return(TRUE);
    else
        return(FALSE);
}

// 删除文件

TBOOL File::RemoveFile(const string& strFileName)
{
    if(unlink((LPCSTR)strFileName.c_str())==0)
        return(TRUE);
    else
        return(FALSE);

}

// 创建空文件

TBOOL File::CreateBlankFile(const string& strFileName)
{
    FILE *f;
    f=fopen((LPCSTR)strFileName.c_str(),"w");
    if(f==NULL)
        return(FALSE);
    else
    {
        fclose(f);
        return(TRUE);
    }
}


// 标准化目录名
void File::NormalizeDirectory(string& strDirectory,char chSeparator)
{
    if(chSeparator==0x00)
    {
		int result = -1 ;
        if( (result = strDirectory.find('/')) >=0 )
            chSeparator='/';
        else if(strDirectory.find('\\')>=0)
            chSeparator='\\';
        else 
#ifdef WIN32
            chSeparator='\\';
#else
            chSeparator='/';
#endif            
    }
        
    // if(strDirectory.Right(1)!=chSeparator)
	if ( strDirectory.at(strDirectory.length()-1) != chSeparator )
    {
        strDirectory+=chSeparator;
    }
    return;
}

// 目录创建工具函数

TBOOL File::AssureDirectory(const string& strDirectory,int mode)
{
    char chSeparator;
	int iPos = strDirectory.find('/') ;
    if(iPos>=0)
        chSeparator='/';
    else if( (iPos=strDirectory.find('\\')) >=0)
        chSeparator='\\';
    else 
#ifdef WIN32
        chSeparator='\\';
#else
        chSeparator='/';
#endif            

    //if(strDirectory.at(0) == chSeparator)
    //    return(TRUE);

#ifdef WIN32
	if(StringUtils::right(strDirectory,2)==":\\")
        return(TRUE);
#else
    if(strDirectory=="/")
        return(TRUE);
#endif

    FileStatus rStatus;
    if(File::GetStatus(strDirectory.c_str(),rStatus)!=FALSE)
    {
        return(TRUE);
        //if(rStatus.m_attribute&File::directory)
        //    return(TRUE);
        //else
        //    return(FALSE);
    }
    // 要求逐层创建目录
    string strHead;
    string strTail=strDirectory;
    string strTempDir;

	long i = 0 ;
	while(i!= -1 && i < strTail.length()-1)
    {
        i=strTail.find(chSeparator,i+1);
        if(i!=-1)
        {
            //strHead+=strTail.Left(i+1);
            //strTail=strTail.Mid(i+1);
			strHead = StringUtils::left(strTail,i+1) ;
			//strTail = StringUtils::mid(strTail,i+1) ;
        }
        else
        {
            strHead=strTail;
            //strTail.clear();
        }

        if(strHead=="/")
            continue;

		if((StringUtils::right(strHead,2)==":\\")||(strHead=="\\"))
            continue;


            
        // 查目录名strHead是否确实存在
        TBOOL bOK=FALSE;
        if(File::GetStatus(strHead.c_str(),rStatus)!=FALSE)
        {
            if(rStatus.m_attribute&File::directory)
                bOK=TRUE;
        }
        if(bOK==FALSE)
        {
#ifdef WIN32
            if(!_mkdir(strHead.c_str()))
                bOK=TRUE;
            else if(::GetLastError()==ERROR_ALREADY_EXISTS)
                bOK=TRUE;
#ifdef _DEBUG
            else
                //BASE_LOG_ERROR("_mkdir [%s] failed(errcode=%d)\n",
                 //          (LPCSTR)strHead.c_str(),::GetLastError());
				printf("_mkdir [%s] failed(errcode=%d)\n",
				(LPCSTR)strHead.c_str(),::GetLastError());
#endif
#else
            if(!mkdir(strHead.c_str(),mode))
                bOK=TRUE;
            else if(errno==EEXIST)
                bOK=TRUE;
#ifdef _DEBUG
            else
                //BASE_LOG_ERROR("_mkdir [%s] failed(errno=%d,%s)\n",
                //           (LPCSTR)strHead.c_str(),errno,strerror(errno));
				printf("_mkdir [%s] failed(errno=%d,%s)\n",
				           (LPCSTR)strHead.c_str(),errno,strerror(errno));

#endif
#endif
        }
        if(bOK==FALSE)
        {
            return(FALSE);
        }
    }
    return(TRUE);
}

TBOOL File::AssureFile(const string& strFileName)
{
    if(File::FileExist(strFileName)==TRUE)
        return(TRUE);

    string strDirPath,strFileID;
    File::ParseFilePath(strFileName,strDirPath,strFileID);
    if(File::AssureDirectory(strDirPath)==FALSE)
        return(FALSE);
    
    return(File::CreateBlankFile(strFileName));
}

// 文件名分离
TBOOL File::ParseFilePath(const string& strPathName, string& strDirPath, string& strFileID)
{
    if(strPathName.empty()==TRUE)
        return(FALSE);

    long nPos=strPathName.find_last_of('/');
    if(nPos==-1)
        nPos=strPathName.find_last_of('\\');
    if(nPos==-1)
    {
        strDirPath="";
        strFileID=strPathName;
    }
    else
    {
        //strDirPath=strPathName.Left(nPos+1);
        //strFileID=strPathName.Mid(nPos+1);
		strDirPath = StringUtils::left(strPathName,nPos+1) ;
		strFileID = StringUtils::mid(strPathName,nPos+1) ;
    }
    return(TRUE);    
}

// 文件名合并
void File::PackFilePath(string& strPathName, const string& strDirPath, const string& strFileID,char chSeparator/*=0x00*/)
{
    if(chSeparator==0x00)
    {
#ifdef WIN32
        chSeparator='\\';
#else
        chSeparator='/';
#endif
    }
	string strDot=StringUtils::right(strDirPath,1);
    if(strDot.at(0)==chSeparator)
    {
        strPathName=strDirPath+strFileID;
    }
    else
    {
        strPathName=strDirPath+chSeparator+strFileID;
    }

    return;
}


/*
class CFileListItem
{
public:
	
    string m_strFileID;

    time_t   m_tmModified;
public:
	//##ModelId=40EE7BEA02C4
    CFileListItem(){m_tmModified=0L;}
	//##ModelId=40EE7BEA02C5
    CFileListItem(const string& strFileID,time_t tmModified)
    {
        m_strFileID=strFileID;
        m_tmModified=tmModified;
    }
	//##ModelId=40EE7BEA02D0
    ~CFileListItem(){}
};

int stSortFileListByFileNameAsc(const void *rec1, const void *rec2)
{
    void **rec11=(void **)rec1;
    void **rec21=(void **)rec2;
    CFileListItem *p1=(CFileListItem *)*rec11;
    CFileListItem *p2=(CFileListItem *)*rec21;
    return(strcmp(p1->m_strFileID.c_str(),p2->m_strFileID.c_str()));
}

int stSortFileListByFileNameDesc(const void *rec1, const void *rec2)
{
    void **rec11=(void **)rec1;
    void **rec21=(void **)rec2;
    CFileListItem *p1=(CFileListItem *)*rec11;
    CFileListItem *p2=(CFileListItem *)*rec21;
    return(strcmp(p2->m_strFileID.c_str(),p1->m_strFileID.c_str()));
}

int stSortFileListByFileNameAscNoCase(const void *rec1, const void *rec2)
{
    void **rec11=(void **)rec1;
    void **rec21=(void **)rec2;
    CFileListItem *p1=(CFileListItem *)*rec11;
    CFileListItem *p2=(CFileListItem *)*rec21;
    return(strcasecmp(p1->m_strFileID.c_str(),p2->m_strFileID.c_str()));
}

int stSortFileListByFileNameDescNoCase(const void *rec1, const void *rec2)
{
    void **rec11=(void **)rec1;
    void **rec21=(void **)rec2;
    CFileListItem *p1=(CFileListItem *)*rec11;
    CFileListItem *p2=(CFileListItem *)*rec21;
    return(strcasecmp(p2->m_strFileID.c_str(),p1->m_strFileID.c_str()));
}

int stSortFileListByFileTimeAsc(const void *rec1, const void *rec2)
{
    void **rec11=(void **)rec1;
    void **rec21=(void **)rec2;
    CFileListItem *p1=(CFileListItem *)*rec11;
    CFileListItem *p2=(CFileListItem *)*rec21;
    return(p1->m_tmModified-p2->m_tmModified);
}

int stSortFileListByFileTimeDesc(const void *rec1, const void *rec2)
{
    void **rec11=(void **)rec1;
    void **rec21=(void **)rec2;
    CFileListItem *p1=(CFileListItem *)*rec11;
    CFileListItem *p2=(CFileListItem *)*rec21;
    return(p2->m_tmModified-p1->m_tmModified);
}
*/


//移动文件

TBOOL File::MoveFile(const string& strSrcFileName,const string& strDestFileName)
{
    //extern int errno;

    if(File::FileExist(strDestFileName)==TRUE)
    {
        if(File::RemoveFile(strDestFileName)==FALSE)
        {			
						printf("RemoveFile %s failed\n",(LPCSTR)strDestFileName.c_str()) ;
        }
    }
#ifndef WIN32
    int nResult=rename(strSrcFileName.c_str(),strDestFileName.c_str());
    if(nResult==0)
        return(TRUE);
    else if(errno==18)
    {
        if(CopyFile(strSrcFileName,strDestFileName)==FALSE)
            return(FALSE);
        else
        {
            FileStatus fstatus;
            File::GetStatus(strSrcFileName.c_str(),fstatus);
            struct utimbuf ut;
            ut.actime=fstatus.m_atime.getTime();
            ut.modtime=fstatus.m_mtime.getTime();
            utime((LPCSTR)strDestFileName.c_str(),&ut);
            // 增加文件日期复制
            File::RemoveFile(strSrcFileName);
            return(TRUE);
        }
    }
    else
    {
#ifdef _DEBUG
        LOG_ERROR(("Move File %s to %s failed(errno=%d,%s)\n",
                   strSrcFileName,
                   strDestFileName,
                   errno,
                   strerror(errno)));
#endif
        return(FALSE);
    }
#else
    TBOOL bResult=::MoveFileA(strSrcFileName.c_str(),strDestFileName.c_str());
    if(bResult==FALSE)
    {
#ifdef _DEBUG


		//char lpMsgBuf[1024] ;

		LPVOID lpMsgBuf;

		WORD err = GetLastError()  ;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			0, // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
			);
		BASE_LOG_ERROR("Move File %s to %s failed(errno=%d,%s)\n",
			strSrcFileName.c_str(),
			strDestFileName.c_str(),
			err,
			(char*)lpMsgBuf);
		LocalFree( lpMsgBuf );		
#endif
        return(FALSE);
    }
    else 
        return(TRUE);
#endif
}

// 备份文件到目录
TBOOL File::BackupFile(const string& strSrcFileName,const string& strDestDir)
{
    string strDirPath,strFileID;
    File::ParseFilePath(strSrcFileName,strDirPath,strFileID);
    string strDestFileName;
    string strDestPath=strDestDir;
    File::NormalizeDirectory(strDestPath);
    File::PackFilePath(strDestFileName,strDestPath,strFileID);
    return(MoveFile(strSrcFileName,strDestFileName));
}

// 拷贝文件
TBOOL File::CopyFile(const string& strSrcFileName,const string& strDestFileName)
{
    //extern int errno;

    if(File::FileExist(strSrcFileName)==FALSE)
    {
        return(FALSE);
    }

    FILE *f=fopen(strSrcFileName.c_str(),"rb");
    if(f==NULL)
        return(FALSE);

    FILE *f2=fopen(strDestFileName.c_str(),"wb");
    if(f2==NULL)
    {
        fclose(f);
        return(FALSE);
    }

    char readbuff[4096];
    size_t nRead;
    while((nRead=fread(readbuff,sizeof(char),4096,f))>0)
    {
        if(fwrite(readbuff,sizeof(char),nRead,f2)<nRead)
        {
            fclose(f);
            fclose(f2);
            return(FALSE);
        }
    }

    fclose(f);
    fclose(f2);
    return(TRUE);
}

string File::GetCurrentDirectory()
{
   char buffer[_BOSS_MAX_PATH];

   if(_getcwd(buffer, _BOSS_MAX_PATH ) == NULL )
      buffer[0]='\0';
   return(string(buffer));
}

void File::ChangeSize(unsigned long nNewSize)
{
    if(m_pStream!=NULL)
    {
#ifdef WIN32
        chsize(fileno(m_pStream),nNewSize) ;
#else
        ftruncate(fileno(m_pStream),nNewSize) ;
#endif
    }
}

/////////////////////////////////////////////////////////////////////////////
TBOOL IsDirectory(FileStatus& rStatus)
{
    TBOOL bResult=FALSE;
    if((rStatus.m_attribute&S_IFDIR)==S_IFDIR)
        bResult=TRUE;
    return(bResult);
}


//__WILLOW_END_NAMESPACE
