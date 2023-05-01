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

#define APPINFO_BUFSIZ 4096
struct appinfo {
	char abspath0[APPINFO_BUFSIZ];
	char abspath1[APPINFO_BUFSIZ];
	const char * dirp;
	const char * fname;
	const char * args;
};

static struct appinfo * appinfo_get(void)
{
	DWORD ret;
	char * strp, * argv;
	struct appinfo * appi;

	appi = NULL;
	appi = (struct appinfo *) calloc(0x1, sizeof(*appi));
	if (appi == NULL) {
		fputs("Error, system out of memory!\n", stderr);
		fflush(stderr);
		return NULL;
	}

	ret = GetModuleFileName(NULL, appi->abspath0, APPINFO_BUFSIZ - 1);
	if (ret == 0) {
		fprintf(stderr, "Error, failed to get abspath: %u\n",
			(unsigned int) GetLastError());
		fflush(stderr);
		free(appi);
		return NULL;
	}

	strncpy(appi->abspath1, appi->abspath0, APPINFO_BUFSIZ);
	strp = strrchr(appi->abspath1, '.');
	if (strp != NULL)
		*strp = '\0';
	strp = strrchr(appi->abspath1, '\\');
	if (strp == NULL) {
		fprintf(stderr, "Error, failed to find last backslash: %s\n",
			appi->abspath0);
		fflush(stderr);
		free(appi);
		return NULL;
	}

	*strp = '\0';
	appi->dirp = appi->abspath1;
	appi->fname = strp + 0x1;
	appi->args = NULL;

	argv = GetCommandLine();
	if (argv == NULL)
		return appi;

	if (*argv == '"') {
		argv = strchr(argv + 1, '"');
		if (argv != NULL)
			argv++;
	} else
		argv = strchr(argv, ' ');
	if (argv != NULL) {
		for (;;) {
			char cha = *argv;
			if (cha == '\0')
				break;
			if (cha != ' ' && cha != '\t')
				break;
			argv++;
		}
		if (*argv != '\0')
			appi->args = argv;
	}
	return appi;
}

static char * find_interp(const char * inter, const char * pathenv)
{
	char * paths;
	char * tmpbuf;

	paths = NULL;
	tmpbuf = (char *) malloc(APPINFO_BUFSIZ);
	if (tmpbuf == NULL) {
		fputs("Error, system out of memory!\n", stderr);
		fflush(stderr);
		return NULL;
	}

	if (pathenv && pathenv[0] != '\0') {
		paths = strdup(pathenv);
	} else {
		paths = getenv("PATH");
		if (paths != NULL)
			paths = strdup(paths);
	}
	if (paths == NULL) {
		free(tmpbuf);
		fputs("Error, failed to get PATH environment variable.\n", stderr);
		fflush(stderr);
		return NULL;
	}

	for (;;) {
		FILE * rfp;
		char * sepstr, * strp;

		sepstr = strrchr(paths, ';');
		strp = sepstr + 0x1;
		if (sepstr == NULL)
			strp = paths;
		sprintf(tmpbuf, "%s\\%s.exe", strp, inter);
		rfp = fopen(tmpbuf, "rb");
		if (rfp != NULL) {
			fclose(rfp);
			free(paths);
			return tmpbuf;
		}

		if (sepstr == NULL)
			break;
		*sepstr = '\0';
	}

	free(paths);
	free(tmpbuf);
	return NULL;
}

static int invoke_interp(const char * interp, const char * inter,
	const char * script, const char * argv)
{
	BOOL ret;
	DWORD rval;
	char * args;
	size_t arglen;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));

	arglen = argv ? strlen(argv) : 0;
	arglen += strlen(inter);
	arglen += strlen(script);
	args = (char *) malloc(arglen + 0x20);
	if (args == NULL) {
		fputs("Error, system out of memory!\n", stderr);
		fflush(stderr);
		return 1;
	}

	if (argv != NULL) {
		sprintf(args, "%s.exe \"%s\" %s", inter, script, argv);
	} else {
		sprintf(args, "%s.exe \"%s\"", inter, script);
	}

	ret = CreateProcess(interp, args,
		NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!ret) {
		fprintf(stderr, "Error, failed to run '%s': %u\n",
			interp, (unsigned int) GetLastError());
		fflush(stderr);
		free(args);
		return 2;
	}

	rval = WaitForSingleObject(pi.hProcess, INFINITE);
	if (rval != WAIT_OBJECT_0) {
		fprintf(stderr, "Error, failed to wait-object %#x: %u\n",
			(unsigned int) rval, (unsigned int) GetLastError());
		fflush(stderr);
	}

	rval = 0;
	ret = GetExitCodeProcess(pi.hProcess, &rval);
	if (!ret) {
		rval = 3;
		fprintf(stderr, "Error, failed to get process exit code: %u\n",
			(unsigned int) GetLastError());
		fflush(stderr);
	}

	free(args);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
	return (int) rval;
}

int main(int argc, char *argv[])
{
	int idx, rval, found;
	struct appinfo * appi;
	char * newapp, * paths;
	static const char * interps[] = {
		"lua",    "perl",    "ruby",    "python", NULL };
	static const char * suffixes[]  = {
		".lua",   ".pl",     ".rb",     ".py",    NULL };

	rval = 3;
	found = 0;
	newapp = paths = NULL;
	appi = appinfo_get();
	if (appi == NULL)
		return 1;

	paths = (char *) malloc(APPINFO_BUFSIZ);
	newapp = (char *) malloc(APPINFO_BUFSIZ);
	if (paths == NULL || newapp == NULL) {
		fputs("Error, system out of memory!\n", stderr);
		fflush(stderr);
		free(appi);
		if (newapp != NULL)
			free(newapp);
		if (paths != NULL)
			free(paths);
		return 2;
	}

	do {
		FILE * pfile;
		size_t rl1 = 0;
		sprintf(paths, "%s\\pathenv.txt", appi->dirp);
		pfile = fopen(paths, "rb");
		if (pfile != NULL) {
			rl1 = fread(paths, 0x1, APPINFO_BUFSIZ - 1, pfile);
			while (rl1 > 0) {
				char cha = paths[rl1 - 0x1];
				if (cha != '\r' && cha != '\n' && cha != '\t' && cha != ' ')
					break;
				rl1--;
			}
			fclose(pfile);
		}
		paths[rl1] = '\0';
		if (rl1 > 0) {
			BOOL retval = SetEnvironmentVariable("PATH", paths);
			if (!retval) {
				fprintf(stderr, "Error, failed to update PATH as '%s': %u\n",
					paths, (unsigned int) GetLastError());
				fflush(stderr);
			}
		}
	} while (0);

	for (idx = 0; interps[idx] != NULL; ++idx) {
		FILE * rfp;
		char * rinter;

		memset(newapp, 0, APPINFO_BUFSIZ);
		sprintf(newapp, "%s\\scripts\\%s%s",
			appi->dirp, appi->fname, suffixes[idx]);
		rfp = fopen(newapp, "rb");
		if (rfp == NULL)
			continue;
		fclose(rfp);

		rinter = find_interp(interps[idx], paths);
		if (rinter != NULL) {
			found = -1;
			rval = invoke_interp(rinter,
				interps[idx], newapp, appi->args);
			free(rinter);
			break;
		}
	}

	if (found == 0) {
		fprintf(stdout, "abspath:           %s\n", appi->abspath0);
		fprintf(stdout, "parent directory:  %s\n", appi->dirp);
		fprintf(stdout, "file name:         %s\n", appi->fname);
		fprintf(stdout, "command line:      %s\n", GetCommandLine());
		fprintf(stdout, "argv:              %s\n", appi->args ? appi->args : "");
	}

	free(appi);
	free(paths);
	free(newapp);
	return rval;
}
