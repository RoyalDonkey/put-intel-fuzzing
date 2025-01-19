#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <magic-instruction.h>

/* Original definition: https://github.com/llvm/llvm-project/blob/4dc34b0d660a52744164a37466ce245764126296/compiler-rt/lib/sanitizer_common/sanitizer_printf.cpp#L302 */
void __sanitizer_on_print(const char *str)
{
	FILE* pFile;
	pFile = fopen("sanitizer-messages.txt", "a+");
	if(pFile == NULL)
	{
		printf("File is not opened.\n");
		return;
	}

	fprintf(pFile, "%s\n", str);

	fclose(pFile);
	
	// printf("!!! This message has been intercepted by a lazy programmer (%zu) !!!\n", strlen(str));
	/* TODO: write an interpreter of ASAN messages and emit MAGIC */
}
