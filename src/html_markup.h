#ifndef ONIDEV_MARKUP_HTML_MARKUP_H_INCLUDED
#define ONIDEV_MARKUP_HTML_MARKUP_H_INCLUDED

#include <onidev/markup/html.h>
#include "html_lexer.h"
#include <string>

namespace od
{

struct HtmlMarkup
{
    enum Kind
    {
        Text,     // x
        Open,     // <x>
        Close,    // </x>
        OpenClose,// <x/>
        Error
    };
    
    Kind kind;
    std::string name;
    std::vector<HtmlNode::Element> params;
    
    HtmlMarkup():kind(Error){}
    explicit HtmlMarkup(Kind kind):kind(kind){}
    explicit HtmlMarkup(Kind kind, const std::string & name):kind(kind), name(name){}
};

class HtmlMarkupLexer
{
    const std::string & str;
    HtmlLexer  lex;
    unsigned   cursor;
    HtmlMarkup markup;
    bool       initialized;
    
    HtmlMarkup & get();
    
public:
    HtmlMarkupLexer(const std::string & str);
    
    const HtmlMarkup & getToken();
    bool eof() const;
};

}

#endif // ONIDEV_MARKUP_HTML_MARKUP_H_INCLUDED
