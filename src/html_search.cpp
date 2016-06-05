#include <onidev/markup/html.h>

static const std::string empty_string;

namespace od
{

const std::string & HtmlNode::findAttribute(const std::string & att) const
{
    for(const Element & e: _params)
    {
        if(e.name == att)
        {
            return e.value;
        }
    }
    return empty_string;
}

HtmlNode* HtmlNode::find(const std::string & value)
{
    
    for(auto& node: _nodes)
    {
        if(node->name() == value)
        {
            return node.get();
        }
    }
    return nullptr;
}

HtmlNode* HtmlNode::findRec(const std::string & markupName)
{
    // breadth-first
    std::queue<HtmlNode*> q;
    q.push(this);
    
    while(!q.empty())
    {
        HtmlNode * node = q.front();
        q.pop();
        
        if(node->markup() && node->name() == markupName)
        {
            return node;
        }
        
        for(auto& n: node->_nodes)
        {
            q.push(n.get());
        }
    }
    return nullptr;
}

HtmlNode* HtmlNode::findRec(const std::string & markupName, const std::string & att, const std::string & value)
{
    // breadth-first
    std::queue<HtmlNode*> q;
    q.push(this);
    
    while(!q.empty())
    {
        HtmlNode * node = q.front();
        q.pop();
        
        if(node->markup() && node->name() == markupName && node->findAttribute(att) == value)
        {
            return node;
        }
        
        for(auto& n: node->_nodes)
        {
            q.push(n.get());
        }
    }
    return nullptr;
}

HtmlNode* HtmlNode::findTextRec(const std::string& substring)
{
    // breadth-first
    std::queue<HtmlNode*> q;
    q.push(this);
    
    while(!q.empty())
    {
        HtmlNode * node = q.front();
        q.pop();
        
        if(!node->markup() && node->name().find(substring) != std::string::npos)
        {
            return node;
        }
        
        for(auto& n: node->_nodes)
        {
            q.push(n.get());
        }
    }
    return nullptr;
}

}
