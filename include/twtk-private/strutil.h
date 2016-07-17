#ifndef __TWTK_STRUTIL_H
#define __TWTK_STRUTIL_H

#include <memory.h>

#define STRBUF_FREE(buf)			\
    if (buf!=NULL) {				\
	free(buf);				\
	buf = NULL;				\
    }

#define STRBUF_SET(buf,value)			\
    if (1) {					\
	if (buf != NULL)			\
	    free(buf);				\
	if ((value == NULL) || (value[0]==0))	\
	    buf = NULL;				\
	else					\
	    buf = strdup(value);		\
    }

#define STRBUF_VALUE(buf)			\
    ((buf == NULL) ? "" : buf)

#define STRBUF_CMPSTR(buf, str)			\
    (strcmp(STRBUF_VALUE(buf), (str==NULL) ? "" : str)==0)

#endif
