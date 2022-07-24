#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstdio>
#include <list>

#include <tidy.h>
#include <tidybuffio.h>
#include <locale.h>
#include <curl/curl.h>
#include <pugixml.hpp>

using std::cbegin;
using std::cend;
using std::cout;
using std::list;
using std::ifstream;
using std::string;
using std::istreambuf_iterator;

using namespace pugi;

class HttpClient
{
    CURL *curl;
    /**
     * ptr: new data
     * size: always 1
     * nmemb: number of data bytes
     */
    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        cout << size << '\n';
        cout << nmemb << '\n';
        list<char> &buf = *static_cast<list<char>*>(userdata);

        buf.insert(buf.end(), ptr, ptr + nmemb);

        return nmemb;
    }
public:
    static void init()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }
    static void cleanup()
    {
        curl_global_cleanup();
    }
    HttpClient()
    {
        init();
        curl = curl_easy_init();

        if (!curl)
        {
            throw std::runtime_error{"failed to initialize easy curl"};
        }
    }
    HttpClient& redirect(bool allowRedirections)
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

    string get(string url)
    {
        // set url for request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

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
            throw std::runtime_error{"curl_easy_perform() failed: "s +  curl_easy_strerror(res)};
        }
        return string(cbegin(buf), cend(buf));
    }
    ~HttpClient()
    {
        curl_easy_cleanup(curl);
        cleanup();
    }
};

struct MyListSink {
    list<char> buf;
    TidyOutputSink tidySink { &buf, &MyListSink::appender };
    static void appender(void *sinkData, byte nextByte)
    {
        list<char> &buf = *static_cast<list<char>*>(sinkData);
        buf.push_back(nextByte);
    }
    string buffer_to_string()
    {
        return string(cbegin(buf), cend(buf));
    }
};

string convert_to_xhtml(string html)
{
    TidyDoc tdoc = tidyCreate();

    tidyOptSetBool(tdoc, TidyMark, no); // disable <meta> for indicating tidied doc
    // tidyOptSaveSink(tdoc, &sink.tidySink);

    // disable info, warnings and errors output
    tidyOptSetInt(tdoc, TidyShowErrors, 0); // 0 for no errors, 6 all errors shown
    tidyOptSetBool(tdoc, TidyShowWarnings, no);
    tidyOptSetBool(tdoc, TidyShowInfo, no);
    tidyOptSetBool(tdoc, TidyWarnPropAttrs, no); // no warnings on proprietary arguments
    tidyOptSetBool(tdoc, TidyQuiet, yes); // disable other messages 

    // make tidy output XHTML
    tidyOptSetBool(tdoc, TidyXmlDecl, yes); // <?xml ... ?>
    tidyOptSetBool(tdoc, TidyXmlSpace, yes); // preserve space in <pre>
    tidyOptSetBool(tdoc, TidyXhtmlOut, yes);
    
    tidyOptSetBool(tdoc, TidyIndentAttributes, no); // default: no
    tidyOptSetBool(tdoc, TidyIndentCdata, no); // default: no

    int status = tidyParseString(tdoc, html.c_str());

    if ( status >= 0 )
        status = tidyCleanAndRepair( tdoc );

    // if ( status >= 0 ) {
    //     status = tidyRunDiagnostics( tdoc );
    // }
    // tidySaveStdout(tdoc);
    
    MyListSink sink;
    tidySaveSink(tdoc, &sink.tidySink);

    tidyRelease(tdoc);
    return sink.buffer_to_string();
}

string read_file(string file_path)
{
    ifstream ifs{file_path};
    return {istreambuf_iterator<char>{ifs}, {}};
}

string do_main()
{
    string url = read_file("../src/pods.txt");
    HttpClient client{};
    return client.redirect(true).get("http://example.com");
}

int main()
{
    string html = read_file("../src/test.html");
    string xml = convert_to_xhtml(html);
    cout << xml;

    return 0;
}