openssh
==================
-----

version
---------------
9.2p1

	Source:
	https://cdn.openbsd.org/pub/OpenBSD/OpenSSH/portable/openssh-9.2p1.tar.gz
	
## Official Documentation

The official documentation for OpenSSH are the man pages for each tool:

* [sshd(8)](https://man.openbsd.org/sshd.8)
* [ssh-keygen(1)](https://man.openbsd.org/ssh-keygen.1)
* [sftp-server(8)](https://man.openbsd.org/sftp-server.8)

Intro
---------------
OpenSSH is a complete implementation of the SSH protocol (version 2) for secure remote login, command execution and file transfer. It includes a client ``ssh`` and server ``sshd``, file transfer utilities ``scp`` and ``sftp`` as well as tools for key generation (``ssh-keygen``), run-time key storage (``ssh-agent``) and a number of supporting programs.

This is a port of OpenBSD's [OpenSSH](https://openssh.com) to most Unix-like operating systems, including Linux, OS X and Cygwin. Portable OpenSSH polyfills OpenBSD APIs that are not available elsewhere, adds sshd sandboxing for more operating systems and includes support for OS-native authentication and auditing (e.g. using PAM).
Cross Compiling for Sylixos

Compile for SylixOS
-------------
	Build-OS: windows7
	Host-OS: SylixOS
	SylixOS kernel version: 2.5.1
	RealEvo-IDE:5.0.5

Copyright
---------
	This file is part of the OpenSSH software.
	
	The licences which components of this software fall under are as
	follows.  First, we will summarize and say that all components
	are under a BSD licence, or a licence more free than that.
	 
	OpenSSH contains no GPL code.

	To COPYING file <src/openssh-9.2p1/LICENCE>

Dependencies
---------
* zlib


Use for SylixOS
---------
	1. ����ɹ���ʹ��IDEһ������Ŀ�����
	2. ��һ������ʹ��ssh-keygen��������Ĭ����Կ��
	3. ִ��/usr/bin/ssh-keygen��������ʾ�س���ֱ������������/root/.ssh/�����ɹ�Կ��˽Կ��
	3. ִ��/usr/bin/ssh-keygen -A������/etc/sshĿ¼������Ĭ����Կ��
	4. ִ��cp /root/.ssh/id_rsa.pub /root/.ssh/authorized_keys
	5. ����/usr/sbin/sshd���񵽿��������ű�/etc/startup.sh�С�
	   ������Ҫע���������ֶ����л��ǿ�������������ʹ��sshd�ľ���·��������/usr/sbin/sshd.
	6. �������ϲ����������ps�������sshd�����Ѿ�����������

sshd_config file
---------
/etc/ssh/sshd_config

	Port 22 					--- 	����sshd�����Ķ˿ں�
	PermitRootLogin yes 		--- 	����root�û���¼
	MaxAuthTries 3 				--- 	����������֤���󽫲������������
    MaxSessions 5 				--- 	ͬһʱ�����Ự����
	PubkeyAuthentication yes 	---		�������ܵ�¼
	IgnoreRhosts no				---		������֤��ʱ���Ƿ�ʹ�á�rhosts���͡�shosts���ļ�
	StrictModes no				---		����ssh�ڽ��յ�¼����֮ǰ�Ƿ����û���Ŀ¼��rhosts�ļ���Ȩ�޺�����Ȩ��
	RSAAuthentication yes		--- 	�����Ƿ�����ֻ��RSA��ȫ��֤
	MaxStartups 10:30:100		---     δ��֤�����������(���ͬʱ��֤��������10������30%�ļ�����֤ʧ�ܣ��������100����100%��¼ʧ�ܣ�


������SylixOS�������ã�����ѡ�����Ȥ�������в��Ĺٷ��ĵ�


---