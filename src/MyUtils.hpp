#pragma once
#include <string>
#include <list>

#include <tidy.h>
#include <libxml/xmlstring.h>
#include <libxml/xmlmemory.h>

using std::list;
using std::string;

string read_file(string file_path);

void write_file(string file_path, string text);

string to_string(const xmlChar* str);

string convert_to_xhtml(string html);

struct MyListSink {
    list<char> buf;
    TidyOutputSink tidySink { &buf, &MyListSink::appender };
    static void appender(void *sinkData, byte nextByte);
    string buffer_to_string();
};