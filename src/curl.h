#ifndef CURL_H_
#define CURL_H_

#include "util/int.h"
#include "util/string-view.h"
#include "util/string.h"
#include "util/vector.h"

struct Header {
    StringView name;
    StringView value;

    Header(StringView name, StringView value) noexcept;
};

struct FormData {
    StringView name;
    StringView value;

    FormData(StringView name, StringView value) noexcept;
};

struct Curl;

Curl*
curlMake(bool verbose = false) noexcept;
void
curlDestroy(Curl* self) noexcept;

void
curlSetCookie(Curl* self, String cookie) noexcept;

StringView
curlGet(
    Curl* self,
    String* response,
    StringView url,
    Vector<Header> headers
) noexcept;

StringView
curlForm(
    Curl* self,
    String* response,
    StringView url,
    Vector<Header> headers,
    Vector<FormData> data
) noexcept;

#endif
