#include <stdio.h>
#include "http.h"


int http_parse(const char* request_text, struct http_request* request) {
    // Hämta http metod och sökväg.
    if (sscanf(request_text, "%7s %255s", request->method, request->path) != 2) {
        return -1;
    }

    return 0;
}