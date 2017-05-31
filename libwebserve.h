#ifndef __LIBWEBSERVE_H__
#define __LIBWEBSERVE_H__



#ifdef __cplusplus
extern "C"{
#endif

void *create_web_serve(void *arg);



int
lws_serve_http_file(struct lws *wsi, const char *file, const char *content_type,
		    const char *other_headers, int other_headers_len);



#ifdef __cplusplus
}
#endif



#endif



