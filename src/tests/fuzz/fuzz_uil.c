/**
 * Motif
 *
 * Copyright (c) 2026 The Motif contributors.
 * Licensed under the LGPL 2.1 license.
 *
 * The UIL compiler, lexer to UID writer, through the callable
 * interface of libUil (Uil()).  The input is written to a file because
 * the compiler reads its source by name.  No display is needed.
 */
#include "UilAPI.h"

#include "fuzz_common.h"

static char uid_path[256];

/*
 * Known libUil bug: sym_initialize_storage() allocates the error value
 * entry only while sym_az_error_value_entry is NULL, and
 * Uil_sym_cleanup_storage() frees it with every other node at the end
 * of Uil() without clearing the pointer, so a second Uil() call in the
 * same process writes to freed memory (UilSymStor.c).  Clear it here
 * so that the fuzzer can call Uil() more than once.
 */
extern void *sym_az_error_value_entry;

static Uil_continue_type quiet_message(char *data, int msg_number,
				       int severity, char *msg_buffer,
				       char *src_buffer, char *ptr_buffer,
				       char *loc_buffer, int message_count[])
{
	(void)data; (void)msg_number; (void)severity; (void)msg_buffer;
	(void)src_buffer; (void)ptr_buffer; (void)loc_buffer;
	(void)message_count;
	return Uil_k_continue;
}

static Uil_continue_type quiet_status(char *data, int percent, int lines,
				      char *file, int message_count[])
{
	(void)data; (void)percent; (void)lines; (void)file;
	(void)message_count;
	return Uil_k_continue;
}

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
	(void)argc;
	(void)argv;
	snprintf(uid_path, sizeof uid_path, "%s/motif-fuzz-%ld.uid",
		 access("/dev/shm", W_OK) == 0 ? "/dev/shm" : "/tmp",
		 (long)getpid());
	return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	Uil_command_type cmd;
	Uil_compile_desc_type desc;
	char *include_dirs[] = { "." };

	/* Lines are read into fixed buffers; very long inputs only slow
	 * things down. */
	if (size > 64 * 1024)
		return 0;
	memset(&cmd, 0, sizeof cmd);
	memset(&desc, 0, sizeof desc);
	cmd.source_file = (char *)fuzz_write_file(".uil", data, size);
	cmd.resource_file = uid_path;
	cmd.include_dir_count = 1;
	cmd.include_dir = include_dirs;
	cmd.resource_file_flag = 1;
	cmd.report_info_msg_flag = 0;
	cmd.report_warn_msg_flag = 0;
	cmd.issue_summary = 0;
	sym_az_error_value_entry = NULL;
	Uil(&cmd, &desc, (Uil_continue_type (*)())quiet_message, NULL,
	    (Uil_continue_type (*)())quiet_status, NULL);
	unlink(uid_path);
	return 0;
}
