#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <magic-instruction.h>

/* Path to the device file created by the simics_shm kernel module. */
#define SIMICS_SHM_FPATH "/dev/simics_shm"

/* Size (in bytes) of the simics_shm memory region. */
#define SIMICS_SHM_SIZE 0x1000

/* An arbitrary number sent to Simics when it's supposed to read simics_shm. */
#define MAGIC_NUMBER 28420

size_t libsan_strnlen(const char *s, size_t maxlen)
{
	size_t i = 0;
	for (; i < maxlen && s[i] != '\0'; i++);
	return i;
}

/* Original definition: https://github.com/llvm/llvm-project/blob/4dc34b0d660a52744164a37466ce245764126296/compiler-rt/lib/sanitizer_common/sanitizer_printf.cpp#L302 */
void __sanitizer_on_print(const char *str)
{
	FILE *file;
	size_t str_len, n_bytes_written;

	file = fopen(SIMICS_SHM_FPATH, "w");
	if (file == NULL) {
		perror("libsan-overlay: Failed to open "SIMICS_SHM_FPATH);
		return;
	}

	/* Write the contents of str, truncated to SIMICS_SHM_SIZE and null-terminated. */
	str_len = libsan_strnlen(str, SIMICS_SHM_SIZE - 1);
	n_bytes_written = fwrite(str, sizeof(*str), str_len, file);
	if (n_bytes_written != str_len) {
		fprintf(stderr, "libsan-overlay: Could not write the full str into "SIMICS_SHM_FPATH" ! (%lu/%lu written): %s\n",
		        (unsigned long)n_bytes_written, (unsigned long)str_len,
		        strerror(errno));
	}
	if (fputc('\0', file) != '\0') {
		perror("libsan-overlay: Failed to append null-terminator to "SIMICS_SHM_FPATH);
	}

	if (fclose(file) != 0) {
		perror("libsan-overlay: Failed to close "SIMICS_SHM_FPATH);
	}

	/* Notify Simics that a write has taken place */
	if (n_bytes_written > 0) {
		MAGIC(MAGIC_NUMBER);
	}
}
