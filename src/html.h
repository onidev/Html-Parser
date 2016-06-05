#ifndef ONIDEV_MARKUP_HTML_H_INCLUDED
#define ONIDEV_MARKUP_HTML_H_INCLUDED

#include <string>
#include <list>
#include <vector>
#include <queue>
#include <memory>

namespace od
{
class Lexer;

class HtmlNode
{
    friend class HtmlParser;
    
public:
    struct Element
    {
        std::string name;
        std::string value;
        Element(const std::string & name, const std::string & value):
            name(name), value(value) {}
    };
    
private:
    std::string          _name;
    std::vector<Element> _params;
    std::vector< std::unique_ptr<HtmlNode> > _nodes;
    HtmlNode*            _parent; // weak
    bool                 _markup;
    bool                 _closed;
    
    void swap(HtmlNode& first, HtmlNode& second);
    
    void _asStringRec(std::string & ret, const HtmlNode & node, bool show_attribs, int space, bool newline) const;
    void _copyRecursive(std::vector< std::unique_ptr<HtmlNode> >& dest) const;
    void _updateParentRecursive();
    
public:
    static std::unique_ptr<HtmlNode> parseString(const std::string & str);
    
    HtmlNode();
    explicit HtmlNode(const std::string & name); // cppcheck: add explicit
    HtmlNode(const HtmlNode& other);
    HtmlNode(HtmlNode&& other);
    ~HtmlNode();
    
    HtmlNode& operator=(HtmlNode other);
    
    HtmlNode& add(const std::string & str);
    HtmlNode& addText(const std::string & text);
    HtmlNode& set(const std::string & str);
    HtmlNode& push();
    
    const std::string & findAttribute(const std::string & att) const;
    // warning: you are not the owner of the returned ptr
    HtmlNode* find(const std::string & value);
    HtmlNode* findRec(const std::string & markupName);
    HtmlNode* findRec(const std::string & markupName, const std::string & att, const std::string & value);
    HtmlNode* findTextRec(const std::string& substring);
    HtmlNode* parent() const;
    HtmlNode* prev() const;
    HtmlNode* next() const;
    
    const std::string & name() const;
    bool markup() const;
    bool closed() const;
    
    const std::vector< std::unique_ptr<HtmlNode> > & childs() const;
    std::vector< std::unique_ptr<HtmlNode> > & childs();
    const std::vector<Element> & attributes() const;
    
    std::string asString(bool show_attribs = true) const;
};


class HtmlExplorer
{
    friend class iterator;
    
    HtmlNode* root;
    
public:
    explicit HtmlExplorer(HtmlNode& node):root(&node){} // cppcheck: add explicit
    
    class iterator
    {
        friend class HtmlExplorer;
        
        std::queue<HtmlNode*> q;
        HtmlExplorer * explorer;
        HtmlNode* node;
        
        iterator(HtmlExplorer * explorer, HtmlNode& node):
            explorer(explorer), node(&node) {}
        
    public:
        iterator():explorer(nullptr),node(nullptr){}
        
        void operator++();
        HtmlNode& operator*();
        
        bool operator!=(const iterator & other) { return node != other.node; }
    };
    
    iterator begin();
    iterator end();
};


}

#endif // ONIDEV_MARKUP_HTML_H_INCLUDED
