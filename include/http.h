#ifndef HTTP_H
#define HTTP_H


#define HTTP_METHOD_MAXSIZE 8
#define HTTP_PATH_MAXSIZE   256


struct http_request {
    char method[HTTP_METHOD_MAXSIZE];
    char path[HTTP_PATH_MAXSIZE];
};


int http_parse(const char* request_text, struct http_request* request);


#endif