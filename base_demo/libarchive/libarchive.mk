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
# 文   件   名: libarchive.mk
#
# 创   建   人: RealEvo-IDE
#
# 文件创建日期: 2020 年 12 月 24 日
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
LOCAL_TARGET_NAME := libarchive.so

#*********************************************************************************************************
# Source list
#*********************************************************************************************************
LOCAL_SRCS :=  \
src/libarchive/libarchive/archive_acl.c \
src/libarchive/libarchive/archive_blake2sp_ref.c \
src/libarchive/libarchive/archive_blake2s_ref.c \
src/libarchive/libarchive/archive_check_magic.c \
src/libarchive/libarchive/archive_cmdline.c \
src/libarchive/libarchive/archive_cryptor.c \
src/libarchive/libarchive/archive_digest.c \
src/libarchive/libarchive/archive_disk_acl_darwin.c \
src/libarchive/libarchive/archive_disk_acl_freebsd.c \
src/libarchive/libarchive/archive_disk_acl_linux.c \
src/libarchive/libarchive/archive_disk_acl_sunos.c \
src/libarchive/libarchive/archive_entry.c \
src/libarchive/libarchive/archive_entry_copy_bhfi.c \
src/libarchive/libarchive/archive_entry_copy_stat.c \
src/libarchive/libarchive/archive_entry_link_resolver.c \
src/libarchive/libarchive/archive_entry_sparse.c \
src/libarchive/libarchive/archive_entry_stat.c \
src/libarchive/libarchive/archive_entry_strmode.c \
src/libarchive/libarchive/archive_entry_xattr.c \
src/libarchive/libarchive/archive_getdate.c \
src/libarchive/libarchive/archive_hmac.c \
src/libarchive/libarchive/archive_match.c \
src/libarchive/libarchive/archive_options.c \
src/libarchive/libarchive/archive_pack_dev.c \
src/libarchive/libarchive/archive_pathmatch.c \
src/libarchive/libarchive/archive_ppmd7.c \
src/libarchive/libarchive/archive_ppmd8.c \
src/libarchive/libarchive/archive_random.c \
src/libarchive/libarchive/archive_rb.c \
src/libarchive/libarchive/archive_read.c \
src/libarchive/libarchive/archive_read_add_passphrase.c \
src/libarchive/libarchive/archive_read_append_filter.c \
src/libarchive/libarchive/archive_read_data_into_fd.c \
src/libarchive/libarchive/archive_read_disk_entry_from_file.c \
src/libarchive/libarchive/archive_read_disk_posix.c \
src/libarchive/libarchive/archive_read_disk_set_standard_lookup.c \
src/libarchive/libarchive/archive_read_extract.c \
src/libarchive/libarchive/archive_read_extract2.c \
src/libarchive/libarchive/archive_read_open_fd.c \
src/libarchive/libarchive/archive_read_open_file.c \
src/libarchive/libarchive/archive_read_open_filename.c \
src/libarchive/libarchive/archive_read_open_memory.c \
src/libarchive/libarchive/archive_read_set_format.c \
src/libarchive/libarchive/archive_read_set_options.c \
src/libarchive/libarchive/archive_read_support_filter_all.c \
src/libarchive/libarchive/archive_read_support_filter_by_code.c \
src/libarchive/libarchive/archive_read_support_filter_bzip2.c \
src/libarchive/libarchive/archive_read_support_filter_compress.c \
src/libarchive/libarchive/archive_read_support_filter_grzip.c \
src/libarchive/libarchive/archive_read_support_filter_gzip.c \
src/libarchive/libarchive/archive_read_support_filter_lrzip.c \
src/libarchive/libarchive/archive_read_support_filter_lz4.c \
src/libarchive/libarchive/archive_read_support_filter_lzop.c \
src/libarchive/libarchive/archive_read_support_filter_none.c \
src/libarchive/libarchive/archive_read_support_filter_program.c \
src/libarchive/libarchive/archive_read_support_filter_rpm.c \
src/libarchive/libarchive/archive_read_support_filter_uu.c \
src/libarchive/libarchive/archive_read_support_filter_xz.c \
src/libarchive/libarchive/archive_read_support_filter_zstd.c \
src/libarchive/libarchive/archive_read_support_format_7zip.c \
src/libarchive/libarchive/archive_read_support_format_all.c \
src/libarchive/libarchive/archive_read_support_format_ar.c \
src/libarchive/libarchive/archive_read_support_format_by_code.c \
src/libarchive/libarchive/archive_read_support_format_cab.c \
src/libarchive/libarchive/archive_read_support_format_cpio.c \
src/libarchive/libarchive/archive_read_support_format_empty.c \
src/libarchive/libarchive/archive_read_support_format_iso9660.c \
src/libarchive/libarchive/archive_read_support_format_lha.c \
src/libarchive/libarchive/archive_read_support_format_mtree.c \
src/libarchive/libarchive/archive_read_support_format_rar.c \
src/libarchive/libarchive/archive_read_support_format_rar5.c \
src/libarchive/libarchive/archive_read_support_format_raw.c \
src/libarchive/libarchive/archive_read_support_format_tar.c \
src/libarchive/libarchive/archive_read_support_format_warc.c \
src/libarchive/libarchive/archive_read_support_format_xar.c \
src/libarchive/libarchive/archive_read_support_format_zip.c \
src/libarchive/libarchive/archive_string.c \
src/libarchive/libarchive/archive_string_sprintf.c \
src/libarchive/libarchive/archive_util.c \
src/libarchive/libarchive/archive_version_details.c \
src/libarchive/libarchive/archive_virtual.c \
src/libarchive/libarchive/archive_write.c \
src/libarchive/libarchive/archive_write_add_filter.c \
src/libarchive/libarchive/archive_write_add_filter_b64encode.c \
src/libarchive/libarchive/archive_write_add_filter_by_name.c \
src/libarchive/libarchive/archive_write_add_filter_bzip2.c \
src/libarchive/libarchive/archive_write_add_filter_compress.c \
src/libarchive/libarchive/archive_write_add_filter_grzip.c \
src/libarchive/libarchive/archive_write_add_filter_gzip.c \
src/libarchive/libarchive/archive_write_add_filter_lrzip.c \
src/libarchive/libarchive/archive_write_add_filter_lz4.c \
src/libarchive/libarchive/archive_write_add_filter_lzop.c \
src/libarchive/libarchive/archive_write_add_filter_none.c \
src/libarchive/libarchive/archive_write_add_filter_program.c \
src/libarchive/libarchive/archive_write_add_filter_uuencode.c \
src/libarchive/libarchive/archive_write_add_filter_xz.c \
src/libarchive/libarchive/archive_write_add_filter_zstd.c \
src/libarchive/libarchive/archive_write_disk_posix.c \
src/libarchive/libarchive/archive_write_disk_set_standard_lookup.c \
src/libarchive/libarchive/archive_write_open_fd.c \
src/libarchive/libarchive/archive_write_open_file.c \
src/libarchive/libarchive/archive_write_open_filename.c \
src/libarchive/libarchive/archive_write_open_memory.c \
src/libarchive/libarchive/archive_write_set_format.c \
src/libarchive/libarchive/archive_write_set_format_7zip.c \
src/libarchive/libarchive/archive_write_set_format_ar.c \
src/libarchive/libarchive/archive_write_set_format_by_name.c \
src/libarchive/libarchive/archive_write_set_format_cpio.c \
src/libarchive/libarchive/archive_write_set_format_cpio_binary.c \
src/libarchive/libarchive/archive_write_set_format_cpio_newc.c \
src/libarchive/libarchive/archive_write_set_format_cpio_odc.c \
src/libarchive/libarchive/archive_write_set_format_filter_by_ext.c \
src/libarchive/libarchive/archive_write_set_format_gnutar.c \
src/libarchive/libarchive/archive_write_set_format_iso9660.c \
src/libarchive/libarchive/archive_write_set_format_mtree.c \
src/libarchive/libarchive/archive_write_set_format_pax.c \
src/libarchive/libarchive/archive_write_set_format_raw.c \
src/libarchive/libarchive/archive_write_set_format_shar.c \
src/libarchive/libarchive/archive_write_set_format_ustar.c \
src/libarchive/libarchive/archive_write_set_format_v7tar.c \
src/libarchive/libarchive/archive_write_set_format_warc.c \
src/libarchive/libarchive/archive_write_set_format_xar.c \
src/libarchive/libarchive/archive_write_set_format_zip.c \
src/libarchive/libarchive/archive_write_set_options.c \
src/libarchive/libarchive/archive_write_set_passphrase.c \
src/libarchive/libarchive/filter_fork_posix.c \
src/libarchive/libarchive/xxhash.c

#*********************************************************************************************************
# Header file search path (eg. LOCAL_INC_PATH := -I"Your header files search path")
#*********************************************************************************************************
LOCAL_INC_PATH :=  \
-I"./src/libarchive/libarchive" \
-I"./src/libarchive/libarchive_fe"

#*********************************************************************************************************
# Pre-defined macro (eg. -DYOUR_MARCO=1)
#*********************************************************************************************************
LOCAL_DSYMBOL :=  \
-DHAVE_CONFIG_H \
-Darchive_EXPORTS

#*********************************************************************************************************
# Compiler flags
#*********************************************************************************************************
LOCAL_CFLAGS :=
LOCAL_CXXFLAGS :=

#*********************************************************************************************************
# Depend library (eg. LOCAL_DEPEND_LIB := -la LOCAL_DEPEND_LIB_PATH := -L"Your library search path")
#*********************************************************************************************************
LOCAL_DEPEND_LIB :=  \
-lcextern

LOCAL_DEPEND_LIB_PATH :=  \
-L"$(SYLIXOS_BASE_PATH)/libcextern/$(OUTDIR)"

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
# OpenMP config
#*********************************************************************************************************
LOCAL_USE_OMP := no

#*********************************************************************************************************
# Use short command for link and ar
#*********************************************************************************************************
LOCAL_USE_SHORT_CMD := yes

#*********************************************************************************************************
# User link command
#*********************************************************************************************************
LOCAL_PRE_LINK_CMD := 
LOCAL_POST_LINK_CMD := 
LOCAL_PRE_STRIP_CMD := 
LOCAL_POST_STRIP_CMD := 

#*********************************************************************************************************
# Depend target
#*********************************************************************************************************
LOCAL_DEPEND_TARGET :=

include $(LIBRARY_MK)

#*********************************************************************************************************
# End
#*********************************************************************************************************
