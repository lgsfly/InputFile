/**
 *  file.h
 *
 *  Copyright (C) 2002-2010  SunRise Co., Ltd
 *
 *  zosatapo  dertyang@hotmail.com
 */
 
#ifndef __WILLOW_FILE_H__
#define __WILLOW_FILE_H__

#include "libbase.h"
//#include "date.h"
#include <vector>
#include <string>

using namespace std;


#ifdef WIN32
#undef MoveFile
#undef CopyFile
#undef GetCurrentDirectory
#endif


#define _BOSS_MAX_PATH 256


//__WILLOW_BEGIN_NAMESPACE


struct FileStatus
{
//    Date m_ctime;						// creation date/time of file
//    Date m_mtime;						// last modification date/time of file
//    Date m_atime;						// last access date/time of file
		LONG m_size;						// logical size of file in bytes
    LONG m_attribute;					// logical OR of File::Attribute enum values
    BYTE _m_padding;					// pad the structure to a WORD
    char m_szFullName[_BOSS_MAX_PATH];	// absolute path name
};

TBOOL IsDirectory(FileStatus& rStatus);


class File
{
 
public:

  /**
   * The system-dependent default name-separator character. 
   * On UNIX systems the value of this  field is <code>'/'</code>;
   */
  const static char   separatorChar ;
  
  /**
   * The system-dependent default name-separator character, represented as a
   * string for convenience.  This string contains a single character, namely
   * <code>{@link #separatorChar}</code>.
   */ 
  const static char separator[2];

  /**
   * The system-dependent path-separator character.  This field is
   * initialized to contain the first character of the value of the system
   * property <code>path.separator</code>.  This character is used to
   * separate filenames in a sequence of files given as a <em>path list</em>.
   * On UNIX systems, this character is <code>':'</code>
   */
  const static  char pathSeparatorChar; 

  /**
   * The system-dependent path-separator character, represented as a string
   * for convenience.  This string contains a single character, namely
   * <code>{@link #pathSeparatorChar}</code>.
   */
  const static char pathSeparator[2];  
  
  
  /**
   * The system-dependent line-separator character, represented as a string
   * for convenience.
   */
  const static char lineSeparator[3];  

enum OpenFlags 
    {
        modeRead =          0x0000,
        modeWrite =         0x0001,
        modeReadWrite =     0x0002,
        shareCompat =       0x0000,
        shareExclusive =    0x0010,
        shareDenyWrite =    0x0020,
        shareDenyRead =     0x0030,
        shareDenyNone =     0x0040,
        modeNoInherit =     0x0080,
        modeCreate =        0x1000,
        modeNoTruncate =    0x2000,
        typeText =          0x4000, // typeText and typeBinary are used in
        typeBinary =   (int)0x8000 // derived classes only
    };


    enum Attribute 
    {
        normal =    0x00,
        readOnly =  0x01,
        hidden =    0x02,
        system =    0x04,
        volume =    0x08,
        directory = 0x10,
        archive =   0x20
    };


    enum SeekPosition 
        { 
        begin = 0x0, 
        current = 0x1, 
        end = 0x2 
        };
        

	
	/**
	 * 文件类构造函数
	 */
    File();

	/**
	 * 文件类构造函数，传入FILE *指针
	 * @param hFile 文件指针
	 */
    File(FILE* hFile);

	/**
	 * 文件类构造函数
	 * @param lpszFileName 文件名
	 * @param lpszOpenFlags 打开方式
	 */
    File(LPCSTR lpszFileName,LPCSTR lpszOpenFlags);    		
	
	/**
	 * 文件类析构函数
	 */
    virtual ~File();

	
public:
	/** 
	 * 获取当前文件的读取的位置
	 * @return 返回文件读取位置
	 */
    virtual long GetPosition() const;

	/**
	 * 获取文件当前状态
	 * @param rstatus 文件状态（输出参数）
	 * @param TBOOL   返回是否成功
	 */
    TBOOL GetStatus(FileStatus& rStatus) const;

	/**
	 * 获取文件的名字
	 * @return 返回文件名字
	 */
    virtual string GetFileName() const;

	/**
	 * 返回文件标题
	 * @return
	 */
    virtual string GetFileTitle() const;

	/**
	 * 返回文件完整路径
	 */
    virtual string GetFilePath() const;

	/** 
	 * 打开文件
	 */
    virtual TBOOL Open(LPCSTR lpszFileName,LPCSTR lpszOpenFlags);

	
	/**
	 * 文件指针跳到文件尾
	 */
    unsigned long SeekToEnd();

	/**
	 * 文件指针跳到文件头
	 */
    void SeekToBegin();

    // backward compatible ReadHuge and WriteHuge

	/**
	 * 读取大批量数据到缓冲中(向后兼容)
	 * @param lpBuffer 缓冲区指针
	 * @param dwCount  读取缓冲区大小
	 * @return 返回读取的字节数
	 */
    unsigned long ReadHuge(void* lpBuffer, unsigned long dwCount);

	/**
	 * 写大批量数据到文件中(向后兼容)
	 * @param lpBuffer 缓冲区指针
	 * @param dwCount  读取缓冲区大小
	 * @return 无
	 */
    void WriteHuge(const void* lpBuffer, unsigned long dwCount);
	
	/**
	 * 再次打开文件，并返回文件指针
	 */

    virtual File* Duplicate() const;

	/**
	 * 文件指针跳到对应的位置
	 * @param lOff   文件指针移动偏移量
	 * @param nFrom  文件指针移动起始位置
	 * @return 
	 */
    virtual LONG Seek(LONG lOff, LONG nFrom);

	/**
	 * 设置文件长度（无效）
	 */
    virtual TBOOL SetLength(unsigned long dwNewLen);

	/** 
	 * 获取文件长度
	 */
    virtual unsigned long GetLength() const;

	/**
	 * 读取数据到缓冲中
	 * @param lpBuffer 缓冲区指针
	 * @param dwCount  读取缓冲区大小
	 * @return 返回读取的字节数
	 */
    virtual LONG Read(void* lpBuf, LONG nCount);

	/**
	 * 写数据到文件中
	 * @param lpBuffer 缓冲区指针
	 * @param dwCount  读取缓冲区大小
	 * @return 无
	 */
    virtual TBOOL Write(const void* lpBuf, LONG nCount);

	
	/**
	 * 锁注文件中的一块数据
	 */
    virtual TBOOL LockRange(unsigned long dwPos, unsigned long dwCount);

	/**
	 * 解锁文件中的一块数据
	 */
    virtual TBOOL UnlockRange(unsigned long dwPos, unsigned long dwCount);

	/**
	 * 文件清理工作（退出时）
	 */
    virtual void Abort();

	/**
	 * 把文件缓冲刷新到磁盘文件中
	 */
    virtual TBOOL Flush();

	/**
	 * 关闭文件指针
	 */
    virtual TBOOL Close();

public:

    enum BufferCommand 
      { 
        bufferRead, 
        bufferWrite, 
        bufferCommit, 
        bufferCheck 
      };

    virtual UINT GetBufferPtr(UINT nCommand, UINT nCount = 0,
        void** ppBufStart = NULL, void** ppBufMax = NULL);
    void ChangeSize(unsigned long nNewSize);

protected:

	FILE *m_pStream;

	// 指示在析构的时候是否关闭文件指针
    TBOOL m_bCloseOnDelete;

	// 文件名
    string m_strFileName;
    
	// 打开标志（字符串）
    string m_strOpenFlags;
	

public:  // 文件操作工具函数
    
	
	/**
	 * 检查文件是否存在
	 */
	static TBOOL FileExist(const string& strFileName);
	/**
	 * 移动文件
	 */
	static TBOOL MoveFile(const string& strSrcFileName,const string& strDestFileName);
	/** 
	 * 备份文件到目录
	 */
	static TBOOL BackupFile(const string& strSrcFileName,const string& strDestDir);
	/**
	 * 拷贝文件
	 */
	static TBOOL CopyFile(const string& strSrcFileName,const string& strDestFileName);
	/**
	 * 删除文件
	 */
	static TBOOL RemoveFile(const string& strFileName);
	/**
	 * 创建空文件
	 */
	static TBOOL CreateBlankFile(const string& strFileName);
	/**
	 * 标准化目录名
	 */
	static void NormalizeDirectory(string& strDirectory,char chSeperator=0x00);
	/** 
	 * 创建目录
	 */
	static TBOOL AssureDirectory(const string& strDirectory,int mode=0744);
	/** 
	 * 保证文件存在，如果不存在就创建文件
	 */
	static TBOOL AssureFile(const string& strFileName);

	/**
	 * 文件名分离
	 */
	static TBOOL ParseFilePath(const string& strPathName, string& strDirPath, string& strFileID);
	/**
	 * 文件名合并
	 */
	static void PackFilePath(string& strPathName, const string& strDirPath, const string& strFileID,char chSeparator=0x00);
	
	/** 
	 * 获取当前目录
	 */
	static string GetCurrentDirectory();

	/**
	 * 重命名文件(静态函数)
	 */
    static TBOOL Rename(LPCSTR lpszOldName,LPCSTR lpszNewName);

	/** 
	 * 删除文件
	 * @param lpszFileName 要删除的文件名
	 * @return 成功返回true，否则返回false
	 */
    static TBOOL Remove(LPCSTR lpszFileName);

	/**
	 * 根据文件名获取文件的状态（静态函数）
	 * @param lpszFileName 要获取状态的文件名
	 * @param rStatus      文件的状态（输出参数）
	 */
    static TBOOL GetStatus(LPCSTR lpszFileName,
                FileStatus& rStatus);

	/**
	 * 设置文件的状态（静态函数）
	 * @param lpszFileName 要获取状态的文件名
	 * @param rStatus      文件的状态
	 */
    static void SetStatus(LPCSTR lpszFileName,
                const FileStatus& status);


};

/*
#define FF_NONE         0
#define FF_TIME_FIFO    1
#define FF_TIME_LIFO    -1
#define FF_NAME_FIFO    2
#define FF_NAME_LIFO    -2
#define FF_NAME_FIFO_NOCASE 3
#define FF_NAME_LIFO_NOCASE -3
*/



//__WILLOW_END_NAMESPACE

#endif
