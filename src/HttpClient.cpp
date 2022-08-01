#include "HttpClient.hpp"
// std
#include <iostream>
// libs
using std::cout;
using std::cbegin;
using std::cend;
using std::end;
using std::to_string;

using namespace limw;

CurlHttpHeaderList::CurlHttpHeaderList(HeaderList headers)
{
    for (const auto &header: headers)
    {
        append_header(header);
    }
}

void CurlHttpHeaderList::append_header(const HttpHeader &header)
{
    auto header_line = header.name + ": " + header.value;
    if (curl_linked_list == nullptr)
    {
        curl_linked_list = {curl_slist_append(nullptr, header_line.c_str()), curl_slist_free_all};
    }
    else
    {
        curl_slist_append(curl_linked_list.get(), header_line.c_str());
    }
}

// default: no user-agent header is sent
HttpClient &HttpClient::user_agent(string user_agent)
{
    curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
    return *this;
}
    
// default: true
HttpClient &HttpClient::clear_headers_after_run(bool clear)
{
    shouldClearHeadersAfterRun = clear;
    return *this;
}

size_t HttpClient::write_callback(char *ptr, size_t, size_t nmemb, void *userdata)
{
    list<char> &buf = *static_cast<list<char>*>(userdata);

    buf.insert(end(buf), ptr, ptr + nmemb);

    return nmemb;
}

void HttpClient::init()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}
void HttpClient::cleanup()
{
    curl_global_cleanup();
}
HttpClient::HttpClient()
{
    init();
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");
    if (!curl)
    {
        throw std::runtime_error{"failed to initialize easy curl"};
    }
}
// follow HTTP 3xx redirects, default: false
HttpClient& HttpClient::redirect(bool allowRedirections)
{
    if (allowRedirections)
    {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    }
    else {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L);
    }
    return *this;
}

string HttpClient::get(string url)
{
    // set url for request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    if (curl_headers_list != nullptr)
    {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers_list->convert());
    }

    // set accept encoding same as firefox.
    // zstd is also supported but will not be used because it is fingerprintable
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip, deflate, br");

    // write response to string
    list<char> buf;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &HttpClient::write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

    /* Perform the request, res will get the return code */
    CURLcode res = curl_easy_perform(curl);
    
    /* Check for errors */
    if(res != CURLE_OK)
    {
        using namespace std::string_literals;
        throw std::runtime_error{"curl_easy_perform() failed ("s + std::to_string(res) + "): " +  curl_easy_strerror(res)};  
    }

    if (shouldClearHeadersAfterRun)
    {
        curl_headers_list = nullptr;
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr); // no headers
    }

    return string(cbegin(buf), cend(buf));
}
HttpClient::~HttpClient()
{
    curl_easy_cleanup(curl);
    cleanup();
}

// if the list contains a header set using options this one will be used
HttpClient &HttpClient::headers(HeaderList headerList)
{
    curl_headers_list = std::make_unique<CurlHttpHeaderList>(headerList);
    return *this;
}
