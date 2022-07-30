#pragma once
// std
#include <list>
#include <string>
#include <memory>
#include <vector>
// libs
#include <curl/curl.h>
#include <tidy.h>

using std::string;
using std::list;
using std::vector;
using std::unique_ptr;

namespace limw 
{
    struct HttpHeader
    {
        string name;
        string value;
        inline string to_string()
        {
            return name + ": " + value;
        }
    };

    using HeaderList = vector<HttpHeader>;

    struct CurlSlistDeleter
    {
        void operator() (curl_slist *list)
        {
            curl_slist_free_all(list);
        }
    };

    struct CurlHttpHeaderList
    {
        CurlHttpHeaderList(vector<HttpHeader> headers);
        ~CurlHttpHeaderList() {}
        CurlHttpHeaderList &operator=(const CurlHttpHeaderList&) = delete;
        CurlHttpHeaderList &operator=(CurlHttpHeaderList&& moved)
        {
            curl_linked_list = move(moved.curl_linked_list);
            return *this;
        }
        curl_slist *convert()
        {
            return curl_linked_list.get();
        }
    private:
        unique_ptr<curl_slist, decltype(&curl_slist_free_all)> curl_linked_list{nullptr, curl_slist_free_all};
        void append_header(const HttpHeader&);
    };

    class HttpClient
    {
        CURL *curl;
        /**
         * ptr: new data
         * size: always 1
         * nmemb: number of data bytes
         */
        static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
        unique_ptr<CurlHttpHeaderList> curl_headers_list{ nullptr };
        bool shouldClearHeadersAfterRun = true;
    public:
        static void init();
        static void cleanup();
        HttpClient();
        ~HttpClient();
        HttpClient &user_agent(string);
        HttpClient &clear_headers_after_run(bool clear); // default: true
        HttpClient &redirect(bool allowRedirections); // default: false
        HttpClient &headers(HeaderList);
        string get(string url);
    };
}