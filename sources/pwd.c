/*
 * Copyright 2023 Ye Jiaqiang <yejq.jiaqiang@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <windows.h>
#define PWD_BUFSIZ 4096

int main(int argc, char *argv[])
{
	char * tbuf;
	DWORD ret, idx;

	tbuf = (char *) calloc(0x1, PWD_BUFSIZ);
	if (tbuf == NULL) {
		fputs("Error, system out of memory!\n", stderr);
		fflush(stderr);
		return 1;
	}

	ret = GetCurrentDirectory(PWD_BUFSIZ - 1, tbuf);
	for (idx = 0; idx < ret; ++idx) {
		if (tbuf[idx] == '\\')
			tbuf[idx] = '/';
	}
	fprintf(stdout, "%s\n", tbuf);
	fflush(stdout);
	free(tbuf);
	return 0;
}
