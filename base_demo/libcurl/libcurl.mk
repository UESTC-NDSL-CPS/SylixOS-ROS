#*********************************************************************************************************
#
#                                    中国软件开源组织
#
#                                   嵌入式实时操作系统
#
#                                SylixOS(TM)  LW : long wing
#
#                               Copyright All Rights Reserved
#
#--------------文件信息--------------------------------------------------------------------------------
#
# 文   件   名: libcurl.mk
#
# 创   建   人: RealEvo-IDE
#
# 文件创建日期: 2016 年 12 月 09 日
#
# 描        述: 本文件由 RealEvo-IDE 生成，用于配置 Makefile 功能，请勿手动修改
#*********************************************************************************************************

#*********************************************************************************************************
# Clear setting
#*********************************************************************************************************
include $(CLEAR_VARS_MK)

#*********************************************************************************************************
# Target
#*********************************************************************************************************
LOCAL_TARGET_NAME := libcurl.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
CURL_VERSION = 7.82.0

LOCAL_SRCS := \
curl-$(CURL_VERSION)/lib/altsvc.c \
curl-$(CURL_VERSION)/lib/amigaos.c \
curl-$(CURL_VERSION)/lib/asyn-ares.c \
curl-$(CURL_VERSION)/lib/asyn-thread.c \
curl-$(CURL_VERSION)/lib/base64.c \
curl-$(CURL_VERSION)/lib/bufref.c \
curl-$(CURL_VERSION)/lib/c-hyper.c \
curl-$(CURL_VERSION)/lib/conncache.c \
curl-$(CURL_VERSION)/lib/connect.c \
curl-$(CURL_VERSION)/lib/content_encoding.c \
curl-$(CURL_VERSION)/lib/cookie.c \
curl-$(CURL_VERSION)/lib/curl_addrinfo.c \
curl-$(CURL_VERSION)/lib/curl_ctype.c \
curl-$(CURL_VERSION)/lib/curl_des.c \
curl-$(CURL_VERSION)/lib/curl_endian.c \
curl-$(CURL_VERSION)/lib/curl_fnmatch.c \
curl-$(CURL_VERSION)/lib/curl_gethostname.c \
curl-$(CURL_VERSION)/lib/curl_get_line.c \
curl-$(CURL_VERSION)/lib/curl_gssapi.c \
curl-$(CURL_VERSION)/lib/curl_memrchr.c \
curl-$(CURL_VERSION)/lib/curl_multibyte.c \
curl-$(CURL_VERSION)/lib/curl_ntlm_core.c \
curl-$(CURL_VERSION)/lib/curl_ntlm_wb.c \
curl-$(CURL_VERSION)/lib/curl_path.c \
curl-$(CURL_VERSION)/lib/curl_range.c \
curl-$(CURL_VERSION)/lib/curl_rtmp.c \
curl-$(CURL_VERSION)/lib/curl_sasl.c \
curl-$(CURL_VERSION)/lib/curl_sspi.c \
curl-$(CURL_VERSION)/lib/curl_threads.c \
curl-$(CURL_VERSION)/lib/dict.c \
curl-$(CURL_VERSION)/lib/doh.c \
curl-$(CURL_VERSION)/lib/dotdot.c \
curl-$(CURL_VERSION)/lib/dynbuf.c \
curl-$(CURL_VERSION)/lib/easy.c \
curl-$(CURL_VERSION)/lib/easygetopt.c \
curl-$(CURL_VERSION)/lib/easyoptions.c \
curl-$(CURL_VERSION)/lib/escape.c \
curl-$(CURL_VERSION)/lib/file.c \
curl-$(CURL_VERSION)/lib/fileinfo.c \
curl-$(CURL_VERSION)/lib/formdata.c \
curl-$(CURL_VERSION)/lib/ftp.c \
curl-$(CURL_VERSION)/lib/ftplistparser.c \
curl-$(CURL_VERSION)/lib/getenv.c \
curl-$(CURL_VERSION)/lib/getinfo.c \
curl-$(CURL_VERSION)/lib/gopher.c \
curl-$(CURL_VERSION)/lib/hash.c \
curl-$(CURL_VERSION)/lib/hmac.c \
curl-$(CURL_VERSION)/lib/hostasyn.c \
curl-$(CURL_VERSION)/lib/hostip.c \
curl-$(CURL_VERSION)/lib/hostip4.c \
curl-$(CURL_VERSION)/lib/hostip6.c \
curl-$(CURL_VERSION)/lib/hostsyn.c \
curl-$(CURL_VERSION)/lib/hsts.c \
curl-$(CURL_VERSION)/lib/http.c \
curl-$(CURL_VERSION)/lib/http2.c \
curl-$(CURL_VERSION)/lib/http_aws_sigv4.c \
curl-$(CURL_VERSION)/lib/http_chunks.c \
curl-$(CURL_VERSION)/lib/http_digest.c \
curl-$(CURL_VERSION)/lib/http_negotiate.c \
curl-$(CURL_VERSION)/lib/http_ntlm.c \
curl-$(CURL_VERSION)/lib/http_proxy.c \
curl-$(CURL_VERSION)/lib/idn_win32.c \
curl-$(CURL_VERSION)/lib/if2ip.c \
curl-$(CURL_VERSION)/lib/imap.c \
curl-$(CURL_VERSION)/lib/inet_ntop.c \
curl-$(CURL_VERSION)/lib/inet_pton.c \
curl-$(CURL_VERSION)/lib/krb5.c \
curl-$(CURL_VERSION)/lib/ldap.c \
curl-$(CURL_VERSION)/lib/llist.c \
curl-$(CURL_VERSION)/lib/md4.c \
curl-$(CURL_VERSION)/lib/md5.c \
curl-$(CURL_VERSION)/lib/memdebug.c \
curl-$(CURL_VERSION)/lib/mime.c \
curl-$(CURL_VERSION)/lib/mprintf.c \
curl-$(CURL_VERSION)/lib/mqtt.c \
curl-$(CURL_VERSION)/lib/multi.c \
curl-$(CURL_VERSION)/lib/netrc.c \
curl-$(CURL_VERSION)/lib/nonblock.c \
curl-$(CURL_VERSION)/lib/openldap.c \
curl-$(CURL_VERSION)/lib/parsedate.c \
curl-$(CURL_VERSION)/lib/pingpong.c \
curl-$(CURL_VERSION)/lib/pop3.c \
curl-$(CURL_VERSION)/lib/progress.c \
curl-$(CURL_VERSION)/lib/psl.c \
curl-$(CURL_VERSION)/lib/rand.c \
curl-$(CURL_VERSION)/lib/rename.c \
curl-$(CURL_VERSION)/lib/rtsp.c \
curl-$(CURL_VERSION)/lib/select.c \
curl-$(CURL_VERSION)/lib/sendf.c \
curl-$(CURL_VERSION)/lib/setopt.c \
curl-$(CURL_VERSION)/lib/sha256.c \
curl-$(CURL_VERSION)/lib/share.c \
curl-$(CURL_VERSION)/lib/slist.c \
curl-$(CURL_VERSION)/lib/smb.c \
curl-$(CURL_VERSION)/lib/smtp.c \
curl-$(CURL_VERSION)/lib/socketpair.c \
curl-$(CURL_VERSION)/lib/socks.c \
curl-$(CURL_VERSION)/lib/socks_gssapi.c \
curl-$(CURL_VERSION)/lib/socks_sspi.c \
curl-$(CURL_VERSION)/lib/speedcheck.c \
curl-$(CURL_VERSION)/lib/splay.c \
curl-$(CURL_VERSION)/lib/strcase.c \
curl-$(CURL_VERSION)/lib/strdup.c \
curl-$(CURL_VERSION)/lib/strerror.c \
curl-$(CURL_VERSION)/lib/strtok.c \
curl-$(CURL_VERSION)/lib/strtoofft.c \
curl-$(CURL_VERSION)/lib/system_win32.c \
curl-$(CURL_VERSION)/lib/telnet.c \
curl-$(CURL_VERSION)/lib/tftp.c \
curl-$(CURL_VERSION)/lib/timeval.c \
curl-$(CURL_VERSION)/lib/transfer.c \
curl-$(CURL_VERSION)/lib/url.c \
curl-$(CURL_VERSION)/lib/urlapi.c \
curl-$(CURL_VERSION)/lib/version.c \
curl-$(CURL_VERSION)/lib/version_win32.c \
curl-$(CURL_VERSION)/lib/warnless.c \
curl-$(CURL_VERSION)/lib/wildcard.c \
curl-$(CURL_VERSION)/lib/h2h3.c \
curl-$(CURL_VERSION)/lib/vauth/cleartext.c \
curl-$(CURL_VERSION)/lib/vauth/cram.c \
curl-$(CURL_VERSION)/lib/vauth/digest.c \
curl-$(CURL_VERSION)/lib/vauth/digest_sspi.c \
curl-$(CURL_VERSION)/lib/vauth/gsasl.c \
curl-$(CURL_VERSION)/lib/vauth/krb5_gssapi.c \
curl-$(CURL_VERSION)/lib/vauth/krb5_sspi.c \
curl-$(CURL_VERSION)/lib/vauth/ntlm.c \
curl-$(CURL_VERSION)/lib/vauth/ntlm_sspi.c \
curl-$(CURL_VERSION)/lib/vauth/oauth2.c \
curl-$(CURL_VERSION)/lib/vauth/spnego_gssapi.c \
curl-$(CURL_VERSION)/lib/vauth/spnego_sspi.c \
curl-$(CURL_VERSION)/lib/vauth/vauth.c \
curl-$(CURL_VERSION)/lib/vquic/ngtcp2.c \
curl-$(CURL_VERSION)/lib/vquic/quiche.c \
curl-$(CURL_VERSION)/lib/vquic/vquic.c \
curl-$(CURL_VERSION)/lib/vssh/libssh.c \
curl-$(CURL_VERSION)/lib/vssh/libssh2.c \
curl-$(CURL_VERSION)/lib/vssh/wolfssh.c \
curl-$(CURL_VERSION)/lib/vtls/bearssl.c \
curl-$(CURL_VERSION)/lib/vtls/gskit.c \
curl-$(CURL_VERSION)/lib/vtls/gtls.c \
curl-$(CURL_VERSION)/lib/vtls/keylog.c \
curl-$(CURL_VERSION)/lib/vtls/mbedtls.c \
curl-$(CURL_VERSION)/lib/vtls/mbedtls_threadlock.c \
curl-$(CURL_VERSION)/lib/vtls/nss.c \
curl-$(CURL_VERSION)/lib/vtls/openssl.c \
curl-$(CURL_VERSION)/lib/vtls/rustls.c \
curl-$(CURL_VERSION)/lib/vtls/schannel.c \
curl-$(CURL_VERSION)/lib/vtls/schannel_verify.c \
curl-$(CURL_VERSION)/lib/vtls/sectransp.c \
curl-$(CURL_VERSION)/lib/vtls/vtls.c \
curl-$(CURL_VERSION)/lib/vtls/wolfssl.c \
curl-$(CURL_VERSION)/lib/vtls/hostcheck.c \
curl-$(CURL_VERSION)/lib/vtls/x509asn1.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your hearder files search path")
#*********************************************************************************************************
LOCAL_INC_PATH := -I"curl-$(CURL_VERSION)/include/"
LOCAL_INC_PATH += -I"curl-$(CURL_VERSION)/lib/vtls"
LOCAL_INC_PATH += -I"curl-$(CURL_VERSION)/lib/vauth"
LOCAL_INC_PATH += -I"curl-$(CURL_VERSION)/lib/vquic"
LOCAL_INC_PATH += -I"curl-$(CURL_VERSION)/lib"
LOCAL_INC_PATH += -I"$(SYLIXOS_BASE_PATH)/openssl/openssl/include"

#*********************************************************************************************************
# Pre-defined macro (eg. -DYOUR_MARCO=1)
#*********************************************************************************************************
LOCAL_DSYMBOL := -DHAVE_CONFIG_H
LOCAL_DSYMBOL += -DBUILDING_LIBCURL
LOCAL_DSYMBOL += -DCURL_HIDDEN_SYMBOLS
LOCAL_DSYMBOL += -DOPENSSL_SUPPRESS_DEPRECATED 
LOCAL_DSYMBOL += -Dlibcurl_EXPORTS

#*********************************************************************************************************
# Compiler flags
#*********************************************************************************************************
LOCAL_CFLAGS   := 
LOCAL_CXXFLAGS := 

#*********************************************************************************************************
# Depend library (eg. LOCAL_DEPEND_LIB := -la LOCAL_DEPEND_LIB_PATH := -L"Your library search path")
#*********************************************************************************************************
LOCAL_DEPEND_LIB      := -lcrypto -lssl 
LOCAL_DEPEND_LIB_PATH := -L"$(SYLIXOS_BASE_PATH)/libcextern/$(OUTDIR)"
LOCAL_DEPEND_LIB_PATH += -L"$(SYLIXOS_BASE_PATH)/openssl/$(OUTDIR)"

#*********************************************************************************************************
# C++ config
#*********************************************************************************************************
LOCAL_USE_CXX        := no
LOCAL_USE_CXX_EXCEPT := no

#*********************************************************************************************************
# Code coverage config
#*********************************************************************************************************
LOCAL_USE_GCOV := no

#*********************************************************************************************************
# User link command
#*********************************************************************************************************
LOCAL_PRE_LINK_CMD   := 
LOCAL_POST_LINK_CMD  := 
LOCAL_PRE_STRIP_CMD  := 
LOCAL_POST_STRIP_CMD := 

include $(LIBRARY_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************
