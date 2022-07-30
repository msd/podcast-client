#include "MyUtils.hpp"
// std
#include <vector>
#include <list>
#include <fstream>
// libs
#include <tidy.h>

using std::vector;
using std::list;
using std::cbegin;
using std::cend;

string to_string(const xmlChar* str)
{
    auto len = xmlStrlen(str);
    vector<char> vec;
    vec.resize(len);
    std::copy(str, str + len, vec.data());
    return string(cbegin(vec), cend(vec));
}

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
    
    // reduce output size
    tidyOptSetBool(tdoc, TidyIndentAttributes, no); // default: no
    tidyOptSetBool(tdoc, TidyIndentCdata, no); // default: no

    int status = tidyParseString(tdoc, html.c_str());

    if ( status >= 0 )
    {
        status = tidyCleanAndRepair( tdoc );
    }
    
    MyListSink sink;
    tidySaveSink(tdoc, &sink.tidySink);

    tidyRelease(tdoc);
    return sink.buffer_to_string();
}

void MyListSink::appender(void *sinkData, byte nextByte)
{
    list<char> &buf = *static_cast<list<char>*>(sinkData);
    buf.push_back(nextByte);
}
string MyListSink::buffer_to_string()
{
    return string(cbegin(buf), cend(buf));
}

string read_file(string file_path)
{
    std::ifstream ifs{file_path};
    return {std::istreambuf_iterator<char>{ifs}, {}};
}

void write_file(string file_path, string text)
{
    std::ofstream ofs{file_path};
    ofs << text;
}
