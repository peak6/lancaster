#include "error.h"
#include "spin.h"
#include "status.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static volatile int msg_lock;
static char prog_name[128], last_msg[256], saved_msg[256];
static int last_code, saved_code, saved_errno;

const char* error_get_program_name(void)
{
	return prog_name;
}

void error_set_program_name(const char* name)
{
	strncpy(prog_name, name, sizeof(prog_name) - 2);
	prog_name[sizeof(prog_name) - 1] = '\0';
}

int error_last_code(void)
{
	return last_code;
}

const char* error_last_msg(void)
{
	return last_msg;
}

void error_reset(void)
{
	last_code = 0;
	last_msg[0] = '\0';
}

void error_report_fatal(void)
{
	if (fputs(last_msg, stderr) == EOF || fputc('\n', stderr) == EOF)
		abort();

	exit(-last_code);
}

int error_msg(const char* msg, int code, ...)
{
	char buf[256];
	va_list ap;

	if (!msg || code >= 0) {
		error_invalid_arg("error_msg");
		error_report_fatal();
	}

	va_start(ap, code);
	vsprintf(buf, msg, ap);
	va_end(ap);

	SPIN_WRITE_LOCK(&msg_lock, no_ver);

	last_code = code;
	last_msg[0] = '\0';

	if (prog_name[0] != '\0') {
		strcpy(last_msg, prog_name);
		strcat(last_msg, ": ");
	}

	strncat(last_msg, buf, sizeof(last_msg) - strlen(last_msg) - 1);

	SPIN_UNLOCK(&msg_lock, no_ver);
	return code;
}

static int format(const char* func, const char* desc, int code)
{
	return error_msg("%s: %s", code, func, desc);
}

int error_eof(const char* func)
{
	if (!func) {
		error_invalid_arg("error_eof");
		error_report_fatal();
	}

	return format(func, "end of file", EOF + CACHESTER_ERROR_BASE);
}

int error_errno(const char* func)
{
	if (!func) {
		error_invalid_arg("error_errno");
		error_report_fatal();
	}

	return format(func, strerror(errno), ERRNO_ERROR_BASE - errno);
}

int error_invalid_arg(const char* func)
{
	errno = EINVAL;
	return error_errno(func);
}

int error_unimplemented(const char* func)
{
	errno = ENOSYS;
	return error_errno(func);
}

void error_save_last(void)
{
	SPIN_WRITE_LOCK(&msg_lock, no_ver);

	if (last_code != 0) {
		saved_errno = errno;
		saved_code = last_code;
		strcpy(saved_msg, last_msg);

		error_reset();
	}

	SPIN_UNLOCK(&msg_lock, no_ver);
}

void error_restore_last(void)
{
	SPIN_WRITE_LOCK(&msg_lock, no_ver);

	if (saved_code != 0) {
		errno = saved_errno;
		last_code = saved_code;
		strcpy(last_msg, saved_msg);

		saved_code = 0;
		saved_msg[0] = '\0';
	}

	SPIN_UNLOCK(&msg_lock, no_ver);
}
