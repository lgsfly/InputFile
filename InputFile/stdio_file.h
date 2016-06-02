/**
 *  file.h
 *
 *  Copyright (C) 2002-2010  SunRise Co., Ltd
 *
 *  zosatapo  dertyang@hotmail.com
 */

#ifndef __WILLOW_STDIOFILE_H__
#define __WILLOW_STDIOFILE_H__

#include "file.h"
#include "libbase.h"
#include <string>
using std::string;

//__WILLOW_BEGIN_NAMESPACE
/////////////////////////////////////////////////////////////////////////////
// STDIO file implementation

class StdioFile : public File
{
	//BOSS_DECLARE_DYNAMIC(StdioFile)

public:
	// Constructors

	StdioFile();

	StdioFile(FILE* pOpenStream);

	StdioFile(LPCSTR lpszFileName, LPCSTR lpszOpenFlags);

	virtual int WriteString(LPCSTR lpsz);

	int WriteF(const char *fmt,...) ;


	virtual LPSTR ReadString(LPSTR lpsz, unsigned int nMax);

	virtual int ReadString(string& rString);

	// Implementation
public:

	virtual ~StdioFile();

	virtual long GetPosition() const;

	virtual int Open(LPCSTR lpszFileName, LPCSTR lpszOpenFlags);
	//virtual int Open(LPCSTR lpszFileName, UINT nOpenFlags,
	//  TCFileException* pError = NULL);

	virtual long Read(void* lpBuf, unsigned int nCount);

	virtual int Write(const void* lpBuf, unsigned int nCount);

	virtual long Seek(long lOff, unsigned int nFrom);

	virtual void Abort();

	virtual int Flush();

	virtual int Close();

	// Unsupported APIs

	virtual File* Duplicate() const;

	virtual int LockRange(unsigned long dwPos, unsigned long dwCount);

	virtual int UnlockRange(unsigned long dwPos, unsigned long dwCount);
};

//__WILLOW_END_NAMESPACE


#endif
