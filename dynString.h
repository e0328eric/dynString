/*
 * Copyright (C) 2021-2022 Sungbae Jeong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Dynamically sized string library for pure C
 * version : 1.0.1
 *
 * v1.0.1 : add mkStringFmt, appendFmtStr and appendFmtStrBack
 *
 * Usage
 * This library uses the stb header only library style. So, defining
 * DYN_STRING_IMPL macro on the one of the c source code to generate the
 * whole implementations of this library.
 */

#ifndef DYN_STRING_H_
#define DYN_STRING_H_

#include <ctype.h>

#if defined(__APPLE__) || defined(__linux__)
#include <sys/types.h>
#elif defined(_WIN32)
typedef long long ssize_t;
#endif

#ifndef DYNSTRDEF
#define DYNSTRDEF
#endif// DYNSTRDEF

typedef struct String String;

DYNSTRDEF String* mkString(const char* str);
DYNSTRDEF String* mkNString(const char* str, size_t strLen);
DYNSTRDEF String* mkStringFmt(const char* format, ...);

DYNSTRDEF void freeString(String* pString);

DYNSTRDEF void appendChar(String* pString, char chr);
DYNSTRDEF void appendStr(String* pString, const char* str);
DYNSTRDEF void appendNStr(String* pString, const char* str, size_t strLen);
DYNSTRDEF void appendFmtStr(String* pString, const char* format, ...);
DYNSTRDEF void appendCharBack(String* pString, char chr);
DYNSTRDEF void appendStrBack(String* pString, const char* str);
DYNSTRDEF void appendNStrBack(String* pString, const char* str, size_t strLen);
DYNSTRDEF void appendFmtStrBack(String* pString, const char* format, ...);

DYNSTRDEF void concatString(String* dst, const String* src);
DYNSTRDEF void concatStringBack(String* dst, const String* src);
DYNSTRDEF void concatFreeString(String* dst, String* src);
DYNSTRDEF void concatFreeStringBack(String* dst, String* src);

DYNSTRDEF int cmpString(const String* pString1, const String* pString2);
DYNSTRDEF int cmpStringStr(const String* pString, const char* str);
DYNSTRDEF int cmpStrString(const char* str, const String* pString);

DYNSTRDEF ssize_t findChar(const String* pString, char chr);
DYNSTRDEF ssize_t findCharNth(const String* pString, char chr, size_t nth);
DYNSTRDEF ssize_t findCharReverse(const String* pString, char chr);
DYNSTRDEF ssize_t findCharReverseNth(const String* pString, char chr,
									 size_t nth);

DYNSTRDEF void clearStringAfter(String* pString, ssize_t pos);
DYNSTRDEF void clearStringBefore(String* pString, ssize_t pos);
DYNSTRDEF void clearEntireString(String* pString);

DYNSTRDEF const char* getStr(const String* const pString);
DYNSTRDEF size_t getLen(const String* const pString);
DYNSTRDEF size_t getCapacity(const String* const pString);

#define DYNS_FMT "%s"
#define DYNS_ARG(str) (getStr(str))

#ifdef DYN_STRING_IMPLEMENTATION

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct String {
	size_t capacity;
	size_t len;
	char* inner;
};

struct String* mkString(const char* str)
{
	struct String* string = malloc(sizeof(struct String));

	if (!str)
	{
		string->capacity = 0;
		string->len = 0;
		string->inner = NULL;
	}
	else
	{
		size_t strLen = strlen(str);

		string->capacity = (strLen + 1) << 1;
		string->len = strLen;
		string->inner = malloc(string->capacity);
		memcpy(string->inner, str, strLen);
		string->inner[strLen] = '\0';
	}

	return string;
}

struct String* mkNString(const char* str, size_t strLen)
{
	struct String* string = malloc(sizeof(struct String));

	if (!str || strLen == 0)
	{
		string->capacity = 0;
		string->len = 0;
		string->inner = NULL;
	}
	else
	{
		string->capacity = (strLen + 1) << 1;
		string->len = strLen;
		string->inner = malloc(string->capacity);
		memcpy(string->inner, str, strLen);
		string->inner[strLen] = '\0';
	}

	return string;
}

struct String* mkStringFmt(const char* format, ...)
{
	struct String* string = malloc(sizeof(struct String));
	ssize_t len;
	char* formattedString = NULL;
	va_list args;
	va_start(args, format);
	len = vasprintf(&formattedString, format, args);
	va_end(args);

	if (len < 0)
	{
		string->capacity = 0;
		string->len = 0;
		string->inner = NULL;
	}
	else
	{

		string->capacity = (len + 1) << 1;
		string->len = len;
		string->inner = malloc(string->capacity);
		memcpy(string->inner, formattedString, len);
		string->inner[len] = '\0';
		free(formattedString);
	}

	return string;
}

void freeString(String* pString)
{
	if (!pString)
		return;

	free(pString->inner);
	free(pString);
}

void appendChar(struct String* pString, char chr)
{
	if (pString->len + 1 >= pString->capacity)
	{
		pString->capacity = (pString->capacity + 1) << 1;
		pString->inner = realloc(pString->inner, pString->capacity);
	}
	pString->inner[pString->len++] = chr;
	pString->inner[pString->len] = '\0';
}

void appendStr(struct String* pString, const char* str)
{
	if (!str)
	{
		return;
	}

	size_t strLen = strlen(str);
	if (pString->len + strLen + 1 >= pString->capacity)
	{
		pString->capacity = (pString->capacity + strLen + 1) << 1;
		pString->inner = realloc(pString->inner, pString->capacity);
	}
	memcpy(pString->inner + pString->len, str, strLen);
	pString->inner[pString->len + strLen] = '\0';
	pString->len += strLen;
}

void appendNStr(struct String* pString, const char* str, size_t strLen)
{
	if (!str || strLen == 0)
	{
		return;
	}

	if (pString->len + strLen + 1 >= pString->capacity)
	{
		pString->capacity = (pString->capacity + strLen + 1) << 1;
		pString->inner = realloc(pString->inner, pString->capacity);
	}
	memcpy(pString->inner + pString->len, str, strLen);
	pString->inner[pString->len + strLen] = '\0';
	pString->len += strLen;
}

void appendFmtStr(struct String* pString, const char* format, ...)
{
	char* formattedString = NULL;
	ssize_t strLen;
	va_list args;
	va_start(args, format);
	strLen = vasprintf(&formattedString, format, args);
	va_end(args);

	if (strLen < 0)
	{
		return;
	}

	if (pString->len + (size_t)strLen + 1 >= pString->capacity)
	{
		pString->capacity = (pString->capacity + (size_t)strLen + 1) << 1;
		pString->inner = realloc(pString->inner, pString->capacity);
	}
	memcpy(pString->inner + pString->len, formattedString, (size_t)strLen);
	pString->inner[pString->len + (size_t)strLen] = '\0';
	pString->len += (size_t)strLen;

	// Since vasprintf allocates memories, we must free it
	free(formattedString);
}

void appendCharBack(struct String* pString, char chr)
{
	if (pString->len + 1 >= pString->capacity)
	{
		pString->capacity = (pString->capacity + 1) << 1;
		pString->inner = realloc(pString->inner, pString->capacity);
	}

	memmove(pString->inner + 1, pString->inner, ++pString->len);
	pString->inner[0] = chr;
}

void appendStrBack(struct String* pString, const char* str)
{
	size_t strLen = strlen(str);
	if (pString->len + strLen + 1 >= pString->capacity)
	{
		pString->capacity = (pString->capacity + strLen + 1) << 1;
		pString->inner = realloc(pString->inner, pString->capacity);
	}
	memmove(pString->inner + strLen, pString->inner, pString->len);
	memcpy(pString->inner, str, strLen);
	pString->inner[pString->len + strLen] = '\0';
	pString->len += strLen;
}

void appendNStrBack(struct String* pString, const char* str, size_t strLen)
{
	if (pString->len + strLen + 1 >= pString->capacity)
	{
		pString->capacity = (pString->capacity + strLen + 1) << 1;
		pString->inner = realloc(pString->inner, pString->capacity);
	}
	memmove(pString->inner + strLen, pString->inner, pString->len);
	memcpy(pString->inner, str, strLen);
	pString->inner[pString->len + strLen] = '\0';
	pString->len += strLen;
}

void appendFmtStrBack(struct String* pString, const char* format, ...)
{
	char* formattedString = NULL;
	ssize_t strLen;
	va_list args;
	va_start(args, format);
	strLen = vasprintf(&formattedString, format, args);
	va_end(args);

	if (strLen < 0)
	{
		return;
	}

	if (pString->len + (size_t)strLen + 1 >= pString->capacity)
	{
		pString->capacity = (pString->capacity + (size_t)strLen + 1) << 1;
		pString->inner = realloc(pString->inner, pString->capacity);
	}
	memmove(pString->inner + (size_t)strLen, pString->inner, pString->len);
	memcpy(pString->inner, formattedString, (size_t)strLen);
	pString->inner[pString->len + (size_t)strLen] = '\0';
	pString->len += (size_t)strLen;

	// Since vasprintf allocates memories, we must free it
	free(formattedString);
}

void concatString(struct String* dst, const struct String* src)
{
	if (!dst || !src)
		return;

	if (dst->len + src->len + 1 >= dst->capacity)
	{
		dst->capacity = (dst->capacity + src->len + 1) << 1;
		dst->inner = realloc(dst->inner, dst->capacity);
	}
	memcpy(dst->inner + dst->len, src->inner, src->len);
	dst->inner[dst->len + src->len] = '\0';
	dst->len += src->len;
}

void concatFreeString(struct String* dst, struct String* src)
{
	concatString(dst, src);
	freeString(src);
}

void concatStringBack(struct String* dst, const struct String* src)
{
	if (!dst || !src)
		return;

	if (dst->len + src->len + 1 >= dst->capacity)
	{
		dst->capacity = (dst->capacity + src->len + 1) << 1;
		dst->inner = realloc(dst->inner, dst->capacity);
	}
	memmove(dst->inner + src->len, dst->inner, dst->len);
	memcpy(dst->inner, src->inner, src->len);
	dst->inner[dst->len + src->len] = '\0';
	dst->len += src->len;
}

void concatFreeStringBack(struct String* dst, struct String* src)
{
	concatStringBack(dst, src);
	freeString(src);
}

int cmpString(const struct String* pString1, const struct String* pString2)
{
	if (!pString1)
		return -(int)pString2->len;
	if (!pString2)
		return (int)pString1->len;
	return strcmp(pString1->inner, pString2->inner);
}

int cmpStringStr(const struct String* pString, const char* str)
{
	if (!pString)
		return -(int)strlen(str);
	if (!str)
		return (int)pString->len;
	return strcmp(pString->inner, str);
}

int cmpStrString(const char* str, const struct String* pString)
{
	if (!str)
		return -(int)pString->len;
	if (!pString)
		return (int)strlen(str);
	return strcmp(str, pString->inner);
}

ssize_t findChar(const struct String* pString, char chr)
{
	return findCharNth(pString, chr, 1);
}

ssize_t findCharNth(const struct String* pString, char chr, size_t nth)
{
	if (!pString)
		return -1;

	char* ptr = pString->inner - 1;
	while (nth--)
	{
		ptr = strchr(++ptr, chr);
		if (!ptr)
			return -1;
	}

	return ptr - pString->inner;
}

ssize_t findCharReverse(const struct String* pString, char chr)
{
	return findCharReverseNth(pString, chr, 1);
}

ssize_t findCharReverseNth(const String* pString, char chr, size_t nth)
{
	if (!pString)
		return -1;

	char* ptr = pString->inner - 1;
	while (nth--)
	{
		ptr = strrchr(++ptr, chr);
		if (!ptr)
			return -1;
	}

	return ptr - pString->inner;
}

void clearStringAfter(struct String* pString, ssize_t pos)
{
	if (!pString)
		return;

	size_t u_pos = pos < 0 ? pString->len + 1 + (size_t)pos : (size_t)pos;
	memset(pString->inner + u_pos, 0, sizeof(char) * (pString->len - u_pos));
	pString->len = u_pos;
}

void clearStringBefore(struct String* pString, ssize_t pos)
{
	if (!pString)
		return;

	size_t u_pos = pos < 0 ? pString->len + 1 + (size_t)pos : (size_t)pos;
	memmove(pString->inner, pString->inner + u_pos,
			sizeof(char) * (pString->len - u_pos));
	memset(pString->inner + pString->len - u_pos, 0, sizeof(char) * u_pos);
	pString->len -= u_pos;
}

void clearEntireString(struct String* pString)
{
	clearStringAfter(pString, 0);
}

const char* getStr(const String* const pString)
{
	if (!pString)
		return NULL;
	return pString->inner;
}
size_t getLen(const struct String* const pString)
{
	if (!pString)
		return 0;
	return pString->len;
}

size_t getCapacity(const struct String* const pString)
{
	if (!pString)
		return 0;
	return pString->capacity;
}

#endif// DYN_STRING_IMPLEMENTATION

#endif// DYN_STRING_H_
