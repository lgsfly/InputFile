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
	 * �ļ��๹�캯��
	 */
    File();

	/**
	 * �ļ��๹�캯��������FILE *ָ��
	 * @param hFile �ļ�ָ��
	 */
    File(FILE* hFile);

	/**
	 * �ļ��๹�캯��
	 * @param lpszFileName �ļ���
	 * @param lpszOpenFlags �򿪷�ʽ
	 */
    File(LPCSTR lpszFileName,LPCSTR lpszOpenFlags);    		
	
	/**
	 * �ļ�����������
	 */
    virtual ~File();

	
public:
	/** 
	 * ��ȡ��ǰ�ļ��Ķ�ȡ��λ��
	 * @return �����ļ���ȡλ��
	 */
    virtual long GetPosition() const;

	/**
	 * ��ȡ�ļ���ǰ״̬
	 * @param rstatus �ļ�״̬�����������
	 * @param TBOOL   �����Ƿ�ɹ�
	 */
    TBOOL GetStatus(FileStatus& rStatus) const;

	/**
	 * ��ȡ�ļ�������
	 * @return �����ļ�����
	 */
    virtual string GetFileName() const;

	/**
	 * �����ļ�����
	 * @return
	 */
    virtual string GetFileTitle() const;

	/**
	 * �����ļ�����·��
	 */
    virtual string GetFilePath() const;

	/** 
	 * ���ļ�
	 */
    virtual TBOOL Open(LPCSTR lpszFileName,LPCSTR lpszOpenFlags);

	
	/**
	 * �ļ�ָ�������ļ�β
	 */
    unsigned long SeekToEnd();

	/**
	 * �ļ�ָ�������ļ�ͷ
	 */
    void SeekToBegin();

    // backward compatible ReadHuge and WriteHuge

	/**
	 * ��ȡ���������ݵ�������(������)
	 * @param lpBuffer ������ָ��
	 * @param dwCount  ��ȡ��������С
	 * @return ���ض�ȡ���ֽ���
	 */
    unsigned long ReadHuge(void* lpBuffer, unsigned long dwCount);

	/**
	 * д���������ݵ��ļ���(������)
	 * @param lpBuffer ������ָ��
	 * @param dwCount  ��ȡ��������С
	 * @return ��
	 */
    void WriteHuge(const void* lpBuffer, unsigned long dwCount);
	
	/**
	 * �ٴδ��ļ����������ļ�ָ��
	 */

    virtual File* Duplicate() const;

	/**
	 * �ļ�ָ��������Ӧ��λ��
	 * @param lOff   �ļ�ָ���ƶ�ƫ����
	 * @param nFrom  �ļ�ָ���ƶ���ʼλ��
	 * @return 
	 */
    virtual LONG Seek(LONG lOff, LONG nFrom);

	/**
	 * �����ļ����ȣ���Ч��
	 */
    virtual TBOOL SetLength(unsigned long dwNewLen);

	/** 
	 * ��ȡ�ļ�����
	 */
    virtual unsigned long GetLength() const;

	/**
	 * ��ȡ���ݵ�������
	 * @param lpBuffer ������ָ��
	 * @param dwCount  ��ȡ��������С
	 * @return ���ض�ȡ���ֽ���
	 */
    virtual LONG Read(void* lpBuf, LONG nCount);

	/**
	 * д���ݵ��ļ���
	 * @param lpBuffer ������ָ��
	 * @param dwCount  ��ȡ��������С
	 * @return ��
	 */
    virtual TBOOL Write(const void* lpBuf, LONG nCount);

	
	/**
	 * ��ע�ļ��е�һ������
	 */
    virtual TBOOL LockRange(unsigned long dwPos, unsigned long dwCount);

	/**
	 * �����ļ��е�һ������
	 */
    virtual TBOOL UnlockRange(unsigned long dwPos, unsigned long dwCount);

	/**
	 * �ļ����������˳�ʱ��
	 */
    virtual void Abort();

	/**
	 * ���ļ�����ˢ�µ������ļ���
	 */
    virtual TBOOL Flush();

	/**
	 * �ر��ļ�ָ��
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

	// ָʾ��������ʱ���Ƿ�ر��ļ�ָ��
    TBOOL m_bCloseOnDelete;

	// �ļ���
    string m_strFileName;
    
	// �򿪱�־���ַ�����
    string m_strOpenFlags;
	

public:  // �ļ��������ߺ���
    
	
	/**
	 * ����ļ��Ƿ����
	 */
	static TBOOL FileExist(const string& strFileName);
	/**
	 * �ƶ��ļ�
	 */
	static TBOOL MoveFile(const string& strSrcFileName,const string& strDestFileName);
	/** 
	 * �����ļ���Ŀ¼
	 */
	static TBOOL BackupFile(const string& strSrcFileName,const string& strDestDir);
	/**
	 * �����ļ�
	 */
	static TBOOL CopyFile(const string& strSrcFileName,const string& strDestFileName);
	/**
	 * ɾ���ļ�
	 */
	static TBOOL RemoveFile(const string& strFileName);
	/**
	 * �������ļ�
	 */
	static TBOOL CreateBlankFile(const string& strFileName);
	/**
	 * ��׼��Ŀ¼��
	 */
	static void NormalizeDirectory(string& strDirectory,char chSeperator=0x00);
	/** 
	 * ����Ŀ¼
	 */
	static TBOOL AssureDirectory(const string& strDirectory,int mode=0744);
	/** 
	 * ��֤�ļ����ڣ���������ھʹ����ļ�
	 */
	static TBOOL AssureFile(const string& strFileName);

	/**
	 * �ļ�������
	 */
	static TBOOL ParseFilePath(const string& strPathName, string& strDirPath, string& strFileID);
	/**
	 * �ļ����ϲ�
	 */
	static void PackFilePath(string& strPathName, const string& strDirPath, const string& strFileID,char chSeparator=0x00);
	
	/** 
	 * ��ȡ��ǰĿ¼
	 */
	static string GetCurrentDirectory();

	/**
	 * �������ļ�(��̬����)
	 */
    static TBOOL Rename(LPCSTR lpszOldName,LPCSTR lpszNewName);

	/** 
	 * ɾ���ļ�
	 * @param lpszFileName Ҫɾ�����ļ���
	 * @return �ɹ�����true�����򷵻�false
	 */
    static TBOOL Remove(LPCSTR lpszFileName);

	/**
	 * �����ļ�����ȡ�ļ���״̬����̬������
	 * @param lpszFileName Ҫ��ȡ״̬���ļ���
	 * @param rStatus      �ļ���״̬�����������
	 */
    static TBOOL GetStatus(LPCSTR lpszFileName,
                FileStatus& rStatus);

	/**
	 * �����ļ���״̬����̬������
	 * @param lpszFileName Ҫ��ȡ״̬���ļ���
	 * @param rStatus      �ļ���״̬
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
