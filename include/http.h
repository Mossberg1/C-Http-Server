#ifndef HTTP_H
#define HTTP_H


struct http_request {
    char method[8];
    char path[256];
};


int http_parse(const char* request_text, struct http_request* request);


#endif