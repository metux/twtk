#ifndef __TWTK_PRIVATE_WIDGET_HELPER
#define __TWTK_PRIVATE_WIDGET_HELPER

#define TWTK_WIDGET_OP_BEGIN						\
    assert(widget);							\
    assert(widget->cls == &(_WIDGET_CLASS_INF));			\
    _WIDGET_PRIV_TYPE *priv = (_WIDGET_PRIV_TYPE *)(widget->priv);

#define TWTK_WIDGET_OP_END(ret)						\
    { return (ret); }

#define TWTK_WIDGET_OP_RETURN(ret)					\
    { return (ret); }

#define TWTK_WIDGET_SET_BEGIN						\
    assert(widget);							\
    assert(widget->cls == &(_WIDGET_CLASS_INF));			\
    _WIDGET_PRIV_TYPE *priv = (_WIDGET_PRIV_TYPE *)(widget->priv);

#define TWTK_WIDGET_SET_END	\
    return -ENOENT;

#define TWTK_WIDGET_SET_STR_ATTR(ATTR, FIELD)	\
    if (strcmp(name, ATTR) == 0)		\
    {						\
	STRBUF_SET(priv->FIELD, value);		\
	twtk_widget_dirty(widget);		\
	return 0;				\
    }

#define TWTK_WIDGET_SET_FLOAT_ATTR(ATTR, FIELD)	\
    if (strcmp(name, ATTR) == 0)		\
    {						\
	priv->FIELD = value;			\
	twtk_widget_dirty(widget);		\
	return 0;				\
    }

#define TWTK_WIDGET_SET_INT_ATTR(ATTR, FIELD)	\
    if (strcmp(name, ATTR) == 0)		\
    {						\
	priv->FIELD = value;			\
	twtk_widget_dirty(widget);		\
	return 0;				\
    }

#define TWTK_WIDGET_SET_COLOR_ATTR(ATTR, FIELD)	\
    if (strcmp(name, ATTR) == 0)		\
    {						\
	priv->FIELD = value;			\
	twtk_widget_dirty(widget);		\
	return 0;				\
    }

#define TWTK_WIDGET_SET_FONTSPEC_FLOAT_ATTRS		\
    if (strcmp(name, "font-size") == 0)			\
    {							\
	twtk_fontspec_set_size(&priv->fontspec, value);	\
	twtk_widget_dirty(widget);			\
	return 0;					\
    }

#define TWTK_WIDGET_SET_FONTSPEC_INT_ATTRS		\
    if (strcmp(name, "font-size") == 0)			\
    {							\
	twtk_fontspec_set_size(&priv->fontspec, value);	\
	twtk_widget_dirty(widget);			\
	return 0;					\
    }

#define TWTK_WIDGET_SET_FONTSPEC_COLOR_ATTRS		\
    if (strcmp(name, "font-color") == 0)		\
    {							\
	twtk_fontspec_set_color(&priv->fontspec, value);\
	twtk_widget_dirty(widget);			\
	return 0;					\
    }

#define TWTK_WIDGET_SET_FONTSPEC_STR_ATTRS		\
    if (strcmp(name, "font-name") == 0)			\
    {							\
	twtk_fontspec_set_name(&priv->fontspec, value);	\
	twtk_widget_dirty(widget);			\
	return 0;					\
    }

#endif /* __TWTK_PRIVATE_WIDGET_HELPER */
