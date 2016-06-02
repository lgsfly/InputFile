#include "stdio_file.h"

#include <errno.h>
#ifdef WIN32
#include <io.h>
//#include <afx.h>
#endif
#include <fcntl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//__WILLOW_BEGIN_NAMESPACE
////////////////////////////////////////////////////////////////////////////
// StdioFile implementation

//##ModelId=3C8F7CC80028
StdioFile::StdioFile()
{
    m_pStream = NULL;
}

//##ModelId=3C8F7CC80032
StdioFile::StdioFile(FILE* pOpenStream) : File(NULL)
{
    m_pStream = pOpenStream;
    BASE_ASSERT(!m_bCloseOnDelete);
}

//StdioFile::StdioFile(LPCSTR lpszFileName, UINT nOpenFlags)
//##ModelId=3C8F7CC8003C
StdioFile::StdioFile(LPCSTR lpszFileName, LPCSTR lpszOpenFlags)
{
    BASE_ASSERT(lpszFileName != NULL);

    Open(lpszFileName, lpszOpenFlags);
}

//##ModelId=3C8F7CC8008C
StdioFile::~StdioFile()
{
    BASE_ASSERT_VALID(this);

    if (m_pStream != NULL && m_bCloseOnDelete)
        Close();
}

//int StdioFile::Open(LPCSTR lpszFileName, UINT nOpenFlags,
//  FileException* pException)
//##ModelId=3C8F7CC800DC
int StdioFile::Open(LPCSTR lpszFileName, LPCSTR lpszOpenFlags)
{
    BASE_ASSERT(lpszFileName != NULL);

    m_pStream = NULL;
    if (!File::Open(lpszFileName, lpszOpenFlags))
        return FALSE;

    BASE_ASSERT(m_bCloseOnDelete);

    /*
    char szMode[4]; // C-runtime open string
    int nMode = 0;
    
    // determine read/write mode depending on File mode
    if (nOpenFlags & modeCreate)
    {
        if (nOpenFlags & modeNoTruncate)
            szMode[nMode++] = 'a';
        else
            szMode[nMode++] = 'w';
    }
    else if (nOpenFlags & modeWrite)
        szMode[nMode++] = 'a';
    else
        szMode[nMode++] = 'r';

    // add '+' if necessary (when read/write modes mismatched)
    if (szMode[0] == 'r' && (nOpenFlags & modeReadWrite) ||
        szMode[0] != 'r' && !(nOpenFlags & modeWrite))
    {
        // current szMode mismatched, need to add '+' to fix
        szMode[nMode++] = '+';
    }

    // will be inverted if not necessary
    int nFlags = _O_RDONLY|_O_TEXT;
    if (nOpenFlags & (modeWrite|modeReadWrite))
        nFlags ^= _O_RDONLY;

    if (nOpenFlags & typeBinary)
        szMode[nMode++] = 'b', nFlags ^= _O_TEXT;
    else
        szMode[nMode++] = 't';
    szMode[nMode++] = '\0';

    */
    /*
    // open a C-runtime low-level file handle
    int nHandle = _open_osfhandle(m_hFile, nFlags);

    // open a C-runtime stream from that handle
    if (nHandle != -1)
        m_pStream = _fdopen(nHandle, szMode);
    */
    
    if (m_pStream == NULL)
    {
        File::Abort(); // close m_hFile
        return FALSE;
    }

    return TRUE;
}

//##ModelId=3C8F7CC800F0
long StdioFile::Read(void* lpBuf, unsigned int nCount)
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    if (nCount == 0)
        return 0;   // avoid Win32 "null-read"

    BASE_ASSERT(lpBuf!=NULL);

		unsigned int nRead = 0;

    if ((nRead = fread(lpBuf, sizeof(BYTE), nCount, m_pStream)) == 0 && !feof(m_pStream))
        return(-1);

    if (ferror(m_pStream))
    {
        clearerr(m_pStream);
        return(-1);
    }
    return nRead;
}

//##ModelId=3C8F7CC80104
int StdioFile::Write(const void* lpBuf, unsigned int nCount)
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);
    BASE_ASSERT(lpBuf!=NULL);

    if (fwrite(lpBuf, sizeof(BYTE), nCount, m_pStream) != nCount)
        return(FALSE);
    return(TRUE);
}

//##ModelId=3C8F7CC80050
int StdioFile::WriteString(LPCSTR lpsz)
{
    BASE_ASSERT(lpsz != NULL);
    BASE_ASSERT(m_pStream != NULL);

    if (fputs(lpsz, m_pStream) == EOF)
        return(FALSE);
    return(TRUE);
}

int StdioFile::WriteF(const char *fmt,...)
{
	va_list ap;
	va_start(ap, fmt);
	int iRet = vfprintf(m_pStream, fmt, ap);
	va_end(ap);
	return iRet >= 0 ;
}

//##ModelId=3C8F7CC80064
LPSTR StdioFile::ReadString(LPSTR lpsz, unsigned int nMax)
{
    BASE_ASSERT(lpsz != NULL);
    BASE_ASSERT(m_pStream != NULL);

    LPSTR lpszResult = fgets(lpsz, nMax, m_pStream);
    if (lpszResult == NULL && !feof(m_pStream))
    {
        clearerr(m_pStream);
        return(NULL);
    }
    return lpszResult;
}

//##ModelId=3C8F7CC80079
int StdioFile::ReadString(string& rString)
{
    BASE_ASSERT_VALID(this);

    rString.clear(); // = &stEmptyNull;    // empty string without deallocating
    const int nMaxSize = 128;
	char szBuf[nMaxSize+2] ;
	memset(szBuf,0,nMaxSize+2) ;

    //LPSTR lpsz = rString.GetBuffer(nMaxSize);
    LPSTR lpszResult;
    int nLen = 0;
    for (;;)
    {
        lpszResult = fgets(szBuf, nMaxSize+1, m_pStream);
		
		rString += szBuf ;

        // handle error/eof case
        if (lpszResult == NULL && !feof(m_pStream))
        {
            clearerr(m_pStream);
            return(FALSE);
        }

        // if string is read completely or EOF
        if (lpszResult == NULL ||
            (nLen = strlen(szBuf)) < nMaxSize ||
            szBuf[nLen-1] == '\n')
            break;

        //nLen = rString.GetLength();
        //lpsz = rString.GetBuffer(nMaxSize + nLen) + nLen;
		memset(szBuf,0,nMaxSize+2) ;
    }

    // remove '\n' from end of string if present
    //lpsz = rString.GetBuffer(0);
    //nLen = rString.GetLength();
    //if (nLen != 0 && lpsz[nLen-1] == '\n')
    //    rString.GetBufferSetLength(nLen-1);

    return lpszResult != NULL;
}

//##ModelId=3C8F7CC80119
long StdioFile::Seek(long lOff, unsigned int nFrom)
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(nFrom == begin || nFrom == end || nFrom == current);
    BASE_ASSERT(m_pStream != NULL);

    if (fseek(m_pStream, lOff, nFrom) != 0)
        return(-1);

    long pos = ftell(m_pStream);
    return pos;
}

//##ModelId=3C8F7CC800C8
long StdioFile::GetPosition() const
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    long pos = ftell(m_pStream);
    if (pos == -1)
        return(-1);

    return pos;
}

//##ModelId=3C8F7CC80140
int StdioFile::Flush()
{
    BASE_ASSERT_VALID(this);

    if (m_pStream != NULL && fflush(m_pStream) != 0)
        return(FALSE);
    return(TRUE);
}

//##ModelId=3C8F7CC8014A
int StdioFile::Close()
{
    BASE_ASSERT_VALID(this);
    //BASE_ASSERT(m_pStream != NULL);

    int nErr = 0;

    if (m_pStream != NULL)
        nErr = fclose(m_pStream);

    m_bCloseOnDelete = FALSE;
    m_pStream = NULL;

    if (nErr != 0)
        return(FALSE);
    return(TRUE);
}

//##ModelId=3C8F7CC80136
void StdioFile::Abort()
{
    BASE_ASSERT_VALID(this);

    if (m_pStream != NULL && m_bCloseOnDelete)
        fclose(m_pStream);  // close but ignore errors
    m_pStream = NULL;
    m_bCloseOnDelete = FALSE;
}

//##ModelId=3C8F7CC80155
File* StdioFile::Duplicate() const
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    return NULL;
}

//##ModelId=3C8F7CC8015F
int StdioFile::LockRange(unsigned long /* dwPos */, unsigned long /* dwCount */)
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);

    return(FALSE);
}

//##ModelId=3C8F7CC80173
int StdioFile::UnlockRange(unsigned long /* dwPos */, unsigned long /* dwCount */)
{
    BASE_ASSERT_VALID(this);
    BASE_ASSERT(m_pStream != NULL);
    return(FALSE);
}

////////////////////////////////////////////////////////////////////////////
//__WILLOW_END_NAMESPACE

