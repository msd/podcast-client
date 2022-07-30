#pragma once
// std
#include <string>
#include <memory>
// libs
#include <libxml/tree.h>
// locals
#include "XmlNode.hpp"

using std::string;
using std::unique_ptr;

class XmlDocument
{
    unique_ptr<xmlDoc, decltype(&xmlFreeDoc)> xml_doc{ nullptr, xmlFreeDoc };
    bool moved = false;
    XmlDocument(xmlDoc *new_doc);
    static size_t count;
    public:
    ~XmlDocument();

    XmlDocument (const XmlDocument&) = delete;
    XmlDocument (const XmlDocument&& moved) = delete;
    XmlDocument (XmlDocument&& moved);

    XmlDocument& operator= (const XmlDocument&) = delete;
    XmlDocument& operator= (XmlDocument&& moved);
    
    XmlNode get_root();

    static XmlDocument load_text(string xml);
    static XmlDocument load_path(string file_path);
};