// std
#include <iostream>
#include <fstream>
#include <list>
// local
#include "XmlDoc.hpp"
#include "XmlNode.hpp"
#include "HttpClient.hpp"
#include "Iterator.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::list;
using std::string;

using limw::HttpClient;
using limw::HeaderList;

string do_main()
{
    string url = read_file("../src/pods.txt");
    HttpClient client{};
    return client.redirect(true).get("http://example.com");
}

void print_element_names(XmlNode a_node)
{
    XmlNode cur_node = a_node;
    // while (true)
    // {
    //     if (cur_node.type() == XML_ELEMENT_NODE)
    //     {
    //         cout << "node type: Element, name: " << cur_node.name() << "\n";
    //     }
    //     print_element_names(cur_node.child());
    //     cur_node = cur_node.next();
    // }
    cout << "node type: Element, name: " << cur_node.name() << "\n";
}

// already provided headers: user-agent (firefox), accept-ending (firefox), host
HeaderList headers = {
    // {"Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8"}, 
    {"Accept", "application/xhtml+xml,application/xml;q=0.9"}, 
    // {"Accept-Encoding", "gzip, deflate, br"}, // provided as list
    {"Accept-Language", "en-US"}, 
    {"Dnt", "1"}, 
    {"Upgrade-Insecure-Requests", "1"}, 
    {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; rv:103.0) Gecko/20100101 Firefox/103.0"}
};

int main()
{
    string url = read_file("../src/pods.txt");
    cout << "URL: \"" << url << "\"\n";
    auto client = HttpClient{};
    client.clear_headers_after_run(false).headers(headers).redirect(true);
    string html;
    try
    {
        html = client.get(url);
        write_file("tmp.xml", html);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
        exit(1);
    }
//     string html = read_file("../src/test.html");
//     string xml = convert_to_xhtml(html);
//     cout << "==== XML BEGINS ====\n";
//     cout << xml;
//     cout << "\n==== XML ENDS ====\n";
//     auto doc = XmlDocument::load_text(xml);

// #ifndef NDEBUG
//     cout << "MAIN STARTS IGNORE ABOVE\n";
// #endif
//     print_element_names(doc.get_root());
//     // cout << xml;
    
    return 0;
}
