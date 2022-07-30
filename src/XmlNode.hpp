#pragma once

#include <libxml/tree.h>

#include "MyUtils.hpp"

class XmlNode
{
    xmlNode *xml_node;
    public:
    XmlNode(xmlNode *xml_node);
    string name();
    auto type();
};
