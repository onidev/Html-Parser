#include <onidev/markup/html.h>
#include "html_lexer.h"
#include <onidev/markup/html_parser.h>
#include "html_markup.h"
#include <onidev/make_unique.h>
#include <algorithm>
#include <vector>
#include <cstdio>

// http://www.startyourdev.com/html/tag-html-index
// todo: faire partie stricte, et partie fermante...
static const std::vector<std::string> autoClose = {
    "area", "base", "basefont", "br", "col", "hr", "img",
    "input", "link", "meta", "param", "span"
};

namespace od
{

HtmlNode::HtmlNode():
    _parent(nullptr),
    _markup(true),
    _closed(false)
{
}

HtmlNode::HtmlNode(const std::string & name):
    _name(name),
    _parent(nullptr),
    _markup(true),
    _closed(false)
{
}

HtmlNode::HtmlNode(const HtmlNode& other):
    _name( other._name ),
    _parent( nullptr ),
    _markup( other._markup ),
    _closed( other._closed )
{
    other._copyRecursive(_nodes);
}

HtmlNode::HtmlNode(HtmlNode&& other):
    HtmlNode()
{
    swap(*this, other);
}

HtmlNode::~HtmlNode()
{
}

HtmlNode& HtmlNode::operator=(HtmlNode other)
{
    swap(*this, other);
    return *this;
}

void HtmlNode::swap(HtmlNode& first, HtmlNode& second)
{
    using std::swap;
    swap(first._name, second._name);
    swap(first._params, second._params);
    swap(first._nodes, second._nodes);
    swap(first._parent, second._parent);
    swap(first._markup, second._markup);
    swap(first._closed, second._closed);
    
    for(auto&n : first._nodes)
        n->_parent = &first;
    for(auto&n : second._nodes)
        n->_parent = &second;
}

void HtmlNode::_copyRecursive(std::vector< std::unique_ptr<HtmlNode> >& dest) const
{
    dest.resize( _nodes.size() );
    for(unsigned i=0; i<_nodes.size(); ++i)
    {
        dest[i].reset( new HtmlNode );
        dest[i]->_name   = _nodes[i]->_name;
        dest[i]->_params = _nodes[i]->_params;
        dest[i]->_markup = _nodes[i]->_markup;
        dest[i]->_closed = _nodes[i]->_closed;
        _nodes[i]->_copyRecursive(dest[i]->_nodes);
    }
}

void HtmlNode::_updateParentRecursive()
{
    for(unsigned i=0; i<_nodes.size(); ++i)
    {
        _nodes[i]->_parent = this;
        _nodes[i]->_updateParentRecursive();
    }
}

std::unique_ptr<HtmlNode> HtmlNode::parseString(const std::string & str)
{
    HtmlParser parser;
    return parser.parseString(str);
}

HtmlNode& HtmlNode::add(const std::string & str)
{
    _nodes.push_back( std::make_unique<HtmlNode>(str) );
    _nodes.back()->_parent = this;
    return *this;
}

HtmlNode& HtmlNode::addText(const std::string & text)
{
    _nodes.push_back( std::make_unique<HtmlNode>(text) );
    _nodes.back()->_markup = false;
    _nodes.back()->_parent = this;
    return *this;
}

HtmlNode& HtmlNode::set(const std::string & str)
{
    _name = str;
    return *this;
}

HtmlNode& HtmlNode::push()
{
    return *_nodes.back();
}

HtmlNode* HtmlNode::parent() const
{
    return _parent;
}

HtmlNode* HtmlNode::prev() const
{
    HtmlNode* node = _parent;
    if(node)
    {
        unsigned i=0;
        for(; i<_parent->_nodes.size(); ++i)
        {
            if(_parent->_nodes[i].get() == this)
            {
                if(i > 0)
                {
                    return _parent->_nodes[i-1].get();
                }
                break;
            }
        }
    }
    return nullptr;
}

HtmlNode* HtmlNode::next() const
{
    HtmlNode* node = _parent;
    if(node)
    {
        unsigned i=0;
        for(; i<_parent->_nodes.size(); ++i)
        {
            if(_parent->_nodes[i].get() == this)
            {
                if(i < _parent->_nodes.size()-1)
                {
                    return _parent->_nodes[i+1].get();
                }
                break;
            }
        }
    }
    return nullptr;
}

const std::string & HtmlNode::name() const
{
    return _name;
}
bool HtmlNode::markup() const
{
    return _markup;
}
bool HtmlNode::closed() const
{
    return _closed;
}

const std::vector< std::unique_ptr<HtmlNode> > & HtmlNode::childs() const
{
    return _nodes;
}
std::vector< std::unique_ptr<HtmlNode> > & HtmlNode::childs()
{
    return _nodes;
}
const std::vector<HtmlNode::Element> & HtmlNode::attributes() const
{
    return _params;
}

void HtmlNode::_asStringRec(std::string & ret, const HtmlNode & node, bool show_attribs, int space, bool newline) const
{
    int spaces_add = 1;
    
    if(newline)
        ret += "\n";
    
    for(int i=0; i<space; ++i)
        ret += " ";
    
    if(!node.name().empty())
    {
        if(node.markup())
        {
            ret += "<" + node.name();
            
            if(show_attribs)
            {
                for(const Element & e: node.attributes())
                    ret += " " + e.name + "=\"" + e.value + "\"";
            }
            
            if(node.closed())
            {
                ret += "/>";
            }
            else
            {
                ret += ">";
            }
        }
        else
        {
            if(!node.name().empty())
                ret += node.name();
        }
    }
    else
        spaces_add = 0;
    
    for(const auto& n: node.childs())
        _asStringRec(ret, *n, show_attribs, space + spaces_add, node.markup());
    
    if(node.markup() && !node.closed() && !node.name().empty())
    {
        ret += "\n";
        for(int i=0; i<space; ++i)
            ret += " ";
        ret += "</" + node.name() + ">";
    }
}

std::string HtmlNode::asString(bool show_attribs) const
{
    std::string ret;
    _asStringRec(ret, *this, show_attribs, 0, false);
    return ret;
}

}
