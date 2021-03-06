/*
 * Copyright (C) 2000,2001	Onlyer	(onlyer@263.net)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "common/setup_before.h"
#include "setup.h"

#include <stdio.h>
#include <ctype.h>
#ifdef HAVE_STRING_H
# include <string.h>
#else
# ifdef HAVE_STRINGS_H
#  include <strings.h>
# endif
# ifdef HAVE_MEMORY_H
#  include <memory.h>
# endif
#endif
#ifdef STDC_HEADERS
# include <stdlib.h>
#else
# ifdef HAVE_MALLOC_H
#  include <malloc.h>
# endif
#endif
#include "compat/memcpy.h"
#include "compat/memmove.h"
#include "compat/strdup.h"
#include "common/util.h"
#include "xstring.h"
#include "common/setup_after.h"


extern unsigned char xtoi(unsigned char ch)
{
	unsigned char retval;

	if (isalpha(ch)) retval=tolower(ch);
	else retval=ch;
	if (retval < 'A') retval -= ('0'-0);
	else retval -= ('a' - 0xa);
	return retval;
}

extern char * str_strip_affix(char * str, char const * affix)
{
	unsigned int i, j, n;
	int		match;

	if (!str) return NULL;
	if (!affix) return str;
	for (i=0; str[i]; i++) {
		match=0;
		for (n=0; affix[n]; n++) {
			if (str[i]==affix[n]) {
				match=1;
				break;
			}
		}
		if (!match) break;
	}
	for (j=strlen(str)-1; j>=i; j--) {
		match=0;
		for (n=0; affix[n]; n++) {
			if (str[j]==affix[n]) {
				match=1;
				break;
			}
		}
		if (!match) break;
	}
	if (i>j) {
		str[0]='\0';
	} else {
		memmove(str,str+i,j-i+1);
		str[j-i+1]='\0';
	}
	return str;
}


extern unsigned int hexstrtoraw(unsigned char const * src, char * data, unsigned int datalen)
{
	unsigned char	ch;
	unsigned int	i, j;

	for (i=0, j=0; j<datalen; i++) {
		ch=src[i];
		if (!ch) break;
		if (ch=='\\') {
			i++;
			ch=src[i];
			if (!ch) {
				break;
			} else if (ch=='\\') {
				data[j++]=ch;
			} else if (ch=='x') {
				if (isxdigit(src[i+1])) { 
					if (isxdigit(src[i+2])) {
						data[j++]=xtoi(src[i+1]) * 0x10 + xtoi(src[i+2]);
						i+=2;
					} else {
						data[j++]=xtoi(src[i+1]);
						i++;
					}
				} else {
					data[j++]=ch;
				}
			} else if (ch=='n') {
				data[j++]='\n';
			} else if (ch=='r') {
				data[j++]='\r';
			} else if (ch=='a') {
				data[j++]='\a';
			} else if (ch=='t') {
				data[j++]='\t';
			} else if (ch=='b') {
				data[j++]='\b';
			} else if (ch=='f') {
				data[j++]='\f';
			} else if (ch=='v') {
				data[j++]='\v';
			} else {
				data[j++]=ch;
			}
		} else {
			data[j++]=ch;
			continue;
		}
	}
	return j;
}

#define SPLIT_STRING_INIT_COUNT		32
#define	SPLIT_STRING_INCREASEMENT	32
extern char * * strtoarray(char const * str, char const * delim, int * count)
{
	int	i ,n, index_size;
	int	in_delim, match;
	char	* temp, * result;
	int	* pindex;
	char	* pd;
	char	const * ps;
	char	* realloc_tmp;

	if (!str || !delim || !count) return NULL;

	temp=malloc(strlen(str)+1);
	if (!temp) return NULL;

	n = SPLIT_STRING_INIT_COUNT;
	pindex=malloc(sizeof(char *) * n);
	if (!pindex) {
		free(temp);
		return NULL;
	}

	*count=0;
	in_delim=1;
	ps=str;
	pd=temp;
	pindex[0]=0;
	while (*ps!='\0') {
		match=0;
		for (i=0; delim[i]!='\0'; i++) {
			if ( *ps == delim[i]) {
				match=1;
				if (!in_delim) {
					*pd = '\0';
					pd++;
					(*count)++;
					in_delim=1;
				}
				break;
			}
		}
		if (!match) {
			if (in_delim) {
				if (*count>=n) {
					n += SPLIT_STRING_INCREASEMENT;
					if (!(realloc_tmp=realloc(pindex,n * sizeof(char *)))) {
						free(pindex);
						free(temp);
						return NULL;
					}
					pindex=(int *)realloc_tmp;
				}
				pindex[*count]= pd-temp;
				in_delim = 0;
			}
			*pd = * ps;
			pd++;
		}
		ps++;
	}
	if (!in_delim) {
		*pd='\0';
		pd++;
		(*count)++;
	}
	index_size=*count * sizeof(char *);
	if (!index_size) {
		free(temp);
		free(pindex);
		return NULL;
	}
	result=malloc(pd-temp+index_size);
	if (!result) {
		free(temp);
		free(pindex);
		return NULL;
	}
	memcpy(result+index_size,temp,pd-temp);
	for (i=0; i< *count; i++) {
		pindex[i]+=(int)result+index_size;
	}
	memcpy(result,pindex,index_size);
	free(temp);
	free(pindex);
	return (char **) result;
}

extern char * * strtoargv(char const * str, unsigned int * count)
{
	unsigned int	n, index_size;
	char		* temp;
	int		i, j;
	int		* pindex;
	char		* result;
	char		* realloc_tmp;

	if (!str || !count) return NULL;
	temp=malloc(strlen(str)+1);
	if (!temp) return NULL;
	n = SPLIT_STRING_INIT_COUNT;
	pindex=malloc(n * sizeof (char *));
	if (!pindex) return NULL;

	i=j=0;
	*count=0;
	while (str[i]) {
		while (str[i]==' ' || str[i]=='\t') i++;
		if (!str[i]) break;
		if (*count >=n ) {
			n += SPLIT_STRING_INCREASEMENT;
			if (!(realloc_tmp=realloc(pindex,n * sizeof(char *)))) {
				free(pindex);
				free(temp);
				return NULL;
			}
			pindex=(int *)realloc_tmp;
		}
		pindex[*count]=j;
		(*count)++;
		if (str[i]=='"') {
			i++;
			while (str[i]) {
				if (str[i]=='\\') {
					i++;
					if (!str[i]) break;
				} else if (str[i]=='"') {
					i++;
					break;
				}
				temp[j++]=str[i++];
			}
		} else {
			while (str[i] && str[i] != ' ' && str[i] != '\t') {
				temp[j++]=str[i++];
			}
		}
		temp[j++]='\0';
	}
	index_size= *count * sizeof(char *);
	if (!index_size) {
		free(temp);
		free(pindex);
		return NULL;
	}
	result=malloc(j+index_size);
	if (!result) {
		free(temp);
		free(pindex);
		return NULL;
	}
	memcpy(result+index_size,temp,j);
	for (i=0; i< *count; i++) {
		pindex[i] +=(int)result+index_size;
	}
	memcpy(result,pindex,index_size);
	free(temp);
	free(pindex);
	return (char * *)result;
}

#define COMBINE_STRING_INIT_LEN		1024
#define COMBINE_STRING_INCREASEMENT	1024
extern char * arraytostr(char * * array, char const * delim, int count)
{
	int	i,n;
	char	* result;
	char	* realloc_tmp;
	int	need_delim;

	if (!delim || !array) return NULL;

	n=COMBINE_STRING_INIT_LEN;
	result=malloc(n);
	if (!result) return NULL;
	result[0]='\0';

	need_delim=0;
	for (i=0; i<count; i++) {
		if (!array[i]) continue;	
		if (strlen(result)+strlen(array[i])+strlen(delim)>=n) {
			n+=COMBINE_STRING_INCREASEMENT;
			if (!(realloc_tmp=realloc(result,n))) {
				free(result);
				return NULL;
			}
			result=realloc_tmp;
		}
		if (need_delim) {
			strcat(result,delim);
		}
		strcat(result,array[i]);
		need_delim=1;
	}
	if (!(realloc_tmp=realloc(result,strlen(result)+1))) {
		return result;
	}
	result=realloc_tmp;
	return result;
}

