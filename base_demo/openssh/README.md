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
	1. 编译成功后，使用IDE一键部署到目标机。
	2. 第一次运行使用ssh-keygen工具生成默认秘钥。
	3. 执行/usr/bin/ssh-keygen，根据提示回车，直到结束，将在/root/.ssh/下生成公钥与私钥。
	3. 执行/usr/bin/ssh-keygen -A，将在/etc/ssh目录下生成默认秘钥。
	4. 执行cp /root/.ssh/id_rsa.pub /root/.ssh/authorized_keys
	5. 设置/usr/sbin/sshd服务到开机启动脚本/etc/startup.sh中。
	   这里需要注意无论是手动运行还是开机启动，必须使用sshd的绝对路径启动，/usr/sbin/sshd.
	6. 操作以上步骤后重启，ps命令将看到sshd服务已经正常启动。

sshd_config file
---------
/etc/ssh/sshd_config

	Port 22 					--- 	设置sshd监听的端口号
	PermitRootLogin yes 		--- 	允许root用户登录
	MaxAuthTries 3 				--- 	超过三次验证错误将不允许继续尝试
    MaxSessions 5 				--- 	同一时刻最大会话数量
	PubkeyAuthentication yes 	---		开启免密登录
	IgnoreRhosts no				---		设置验证的时候是否使用“rhosts”和“shosts”文件
	StrictModes no				---		设置ssh在接收登录请求之前是否检查用户家目录和rhosts文件的权限和所有权。
	RSAAuthentication yes		--- 	设置是否允许只有RSA安全验证
	MaxStartups 10:30:100		---     未验证的最大连接数(如果同时验证数量超过10，将有30%的几率验证失败，如果超过100，则100%登录失败）


以上是SylixOS常用配置，其他选项感兴趣可以自行查阅官方文档


---