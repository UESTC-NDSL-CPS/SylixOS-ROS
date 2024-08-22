libarchive
==================
libarchive 3.4.3

libarchive��Ŀ������һ������ֲ�ģ���Ч��C����Զ�ȡ��д����ָ�ʽ�����浵��

Features
-------------
* **libarchive**: a library for reading and writing streaming archives
* **tar**: the 'bsdtar' program is a full-featured 'tar' implementation built on libarchive

Cross Compiling for Sylixos
-------------
	Build-OS: windows7
	Host-OS: SylixOS
	kernel version: 1.12.9
	RealEvo-IDE:3.11.0 


Copyright
---------
GNU and BSD 

Example
---------
* ͷ�ļ�·��-I$(WORKSPACE_libarchive)/src/libarchive/libarchive
* ���ļ�·��-L$(WORKSPACE_libarchive)/$(Output) -larchive

tar: manipulate archive files
---------
    First option must be a mode specifier:
      -c Create  -r Add/Replace  -t List  -u Update  -x Extract
    Common Options:
      -b  Use # 512-byte records per I/O block
      -f  <filename>  Location of archive (default /dev/tape)
      -v  Verbose
      -w  Interactive
    Create: bsdtar -c [options] [<file> | <dir> | @<archive> | -C <dir> ]
      <file>, <dir>  add these items to archive
      -z, -j, -J, --lzma  Compress archive with gzip/bzip2/xz/lzma
      --format  {ustar|pax|cpio|shar}  Select archive format
      --exclude  <pattern>  Skip files that match pattern
      -C <dir>  Change to <dir> before processing remaining files
      @<archive>  Add entries from <archive> to output
    List: bsdtar -t [options] [<patterns>]
      <patterns>  If specified, list only entries that match
    Extract: bsdtar -x [options] [<patterns>]
      <patterns>  If specified, extract only entries that match
      -k  Keep (don't overwrite) existing files
      -m  Don't restore modification times
      -O  Write entries to stdout, don't restore to disk
      -p  Restore permissions (including ACLs, owner, file flags) 

---