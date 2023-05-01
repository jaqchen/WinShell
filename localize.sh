#!/bin/sh

# Created by yejq.jiaqiang@gmail.com
# Simpile localization script for win32
# 2023/05/01

find_path() {
	local fpath="$1"
	if [ -d "C:\\${fpath}" ] ; then
		WPATH="${WPATH};C:\\${fpath}"
	elif [ -d "C:\\Program Files\\${fpath}" ] ; then
		WPATH="${WPATH};C:\\Program Files\\${fpath}"
	elif [ -d "C:\\Program Files (x86)\\${fpath}" ] ; then
		WPATH="${WPATH};C:\\Program Files (x86)\\${fpath}"
	elif [ -d "D:\\${fpath}" ] ; then
		WPATH="${WPATH};D:\\${fpath}"
	elif [ -d "D:\\Program Files\\${fpath}" ] ; then
		WPATH="${WPATH};D:\\Program Files\\${fpath}"
	elif [ -d "D:\\Program Files (x86)\\${fpath}" ] ; then
		WPATH="${WPATH};D:\\Program Files (x86)\\${fpath}"
	elif [ -d "E:\\${fpath}" ] ; then
		WPATH="${WPATH};E:\\${fpath}"
	elif [ -d "E:\\Program Files\\${fpath}" ] ; then
		WPATH="${WPATH};E:\\Program Files\\${fpath}"
	elif [ -d "E:\\Program Files (x86)\\${fpath}" ] ; then
		WPATH="${WPATH};E:\\Program Files (x86)\\${fpath}"
	else
		echo "Warning, path not found: ${fpath}"
	fi
}

gen_pathenv() {
	local outfile="$1"
	local WPATH='C:\Windows\System32;C:\Windows;C:\Windows\System32\Wbem'
	find_path 'CMake\bin'
	find_path 'mingw32\bin'
	find_path 'Rust1.68\bin'
	find_path 'Vim90\bin'
	find_path 'Git\cmd'
	find_path 'Git\usr\bin'
	if [ -n "${outfile}" ] ; then
		echo "Updating '${outfile}'..."
		echo "$2${WPATH}" > "${outfile}"
	else
		echo "${WPATH}"
	fi
}

link_file() {
	if [ ! -e "$1" ] ; then
		busybox ln -v 'runner.exe' "$1"
		return $?
	fi
	return 0
}

link_executables() {
	cd "${WEDIR}/bin" || return 1
	link_file 'ls.exe'
	link_file 'bitdump.exe'
	return 0
}

[ ! -f "${WEDIR}/bin/pathenv.txt" ] && \
	gen_pathenv "./bin/pathenv.txt"
[ ! -f "${WEDIR}/customize.cmd" ] && \
	gen_pathenv "./customize.cmd" 'set PATH=%WEDIR%/bin;'
link_executables
exit $?
