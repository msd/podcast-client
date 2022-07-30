#include "XmlDoc.hpp"

// std
#include <stdexcept> 
#include <memory>
// libs
#include <libxml/tree.h>
// local
#include "XmlNode.hpp"
#include "MyUtils.hpp"

using std::runtime_error;
using std::unique_ptr;


size_t XmlDocument::count = 0;

XmlDocument::XmlDocument(xmlDoc *new_doc)
{
    if (new_doc == nullptr)
    {
        throw runtime_error("given xmlDoc is null");
    }
    xml_doc = {new_doc, xmlFreeDoc};
    ++count;
}

XmlDocument::~XmlDocument()
{
    if (xml_doc != nullptr) // not moved
    {
        --count;
    }
    if (count == 0)
    {
        xmlCleanupParser();    // Free globals
    }
}

XmlDocument::XmlDocument (XmlDocument&& moved)
{
    this->xml_doc = move(moved.xml_doc);
}

XmlDocument& XmlDocument::operator= (XmlDocument&& moved)
{
    xml_doc = move(moved.xml_doc);
    return *this;
}

XmlNode XmlDocument::get_root()
{
    return XmlNode{xmlDocGetRootElement(xml_doc.get())};
}

XmlDocument XmlDocument::load_text(string xml)
{
    auto xml_c_str = unique_ptr<xmlChar, decltype(xmlFree)>(xmlCharStrdup(xml.c_str()), xmlFree);
    xmlDoc *xml_doc = xmlParseDoc(xml_c_str.get());
    if (xml_doc == nullptr)
    {
        throw runtime_error("error: could not parse string");
    }
    return XmlDocument{xml_doc};
}

XmlDocument XmlDocument::load_path(string file_path)
{
    xmlDoc *xml_doc = xmlReadFile(file_path.c_str(), NULL, 0);
    if (xml_doc == nullptr)
    {
        throw runtime_error("error: could not parse file " +  file_path);
    }
    return XmlDocument{xml_doc};
}