#include "XmlNode.hpp"

XmlNode::XmlNode(xmlNode *xml_node) : xml_node(xml_node) {}

string XmlNode::name()
{
    return to_string(xml_node->name);
}

auto XmlNode::type()
{
    return xml_node->type;
}