#include <onidev/markup/html_parser.h>
#include "html_markup.h"
#include <algorithm>

static const std::vector<std::string> autoClose = {
    "area", "base", "basefont", "br", "col", "hr", "img",
    "input", "link", "meta", "param", "span"
};

namespace od
{

HtmlParser::HtmlParser():
    m_warnings(0),
    m_errors(0)
{
    
}

void HtmlParser::parse(std::vector<HtmlMarkup>::iterator beg, std::vector<HtmlMarkup>::iterator end, HtmlNode& node)
{
    while(beg != end)
    {
        if(beg->kind == HtmlMarkup::Open)
        {
            const std::string & name = beg->name;
            int count = 1;
            
            auto it = beg+1;
            for(; count!=0 && it!=end; ++it)
            {
                if(it->kind == HtmlMarkup::Open && it->name == name)
                    count++;
                else if(it->kind == HtmlMarkup::Close && it->name == name)
                    count--;
            }
            
            if(count == 0)
            {
                HtmlNode& n = node.add(name).push();
                n._params = beg->params;
                parse(beg+1, it-1, n);
                beg = it;
                //printf("(%s:%s)", name.c_str(), it->name.c_str());
            }
            else if(std::find(autoClose.begin(), autoClose.end(), name) != autoClose.end())
            {
                //printf("%s ", name.c_str());
                HtmlNode& n = node.add(name).push();
                n._closed = true;
                n._params = beg->params;
                ++beg;
                // Warning
                m_warnings++;
                
            }
            else
            {
                ++beg;
                // Fatal error
                m_errors++;
                ///printf("[%s] ", name.c_str());
            }
        }
        else
        {
            if(beg->kind == HtmlMarkup::Text)
            {
                node.addText(beg->name);
            }
            else if(beg->kind == HtmlMarkup::OpenClose)
            {
                HtmlNode& n = node.add(beg->name).push();
                n._closed = true;
                n._params = beg->params;
            }
            else if(beg->kind == HtmlMarkup::Close)
            {
                // Fatal error
                m_errors++;
                ///printf("[%s] ", beg->name.c_str());
            }
            ++beg;
        }
    }
}

std::unique_ptr<HtmlNode> HtmlParser::parseString(const std::string & str)
{
    // 1 - Tokenize
    std::vector<HtmlMarkup> tokens;
    HtmlMarkupLexer tokenizer(str);
    while(!tokenizer.eof())
        tokens.push_back(tokenizer.getToken());
    
//    printf("Tokens number: %u\n", tokens.size());
//    
//    for(auto& token: tokens)
//        printf("%d ", (int)token.kind);
    
    // affichage de debug
//    for(HtmlMarkup & m: tokens)
//    {
//        if(m.kind != HtmlMarkup::Text)
//        {
//            if(m.kind == HtmlMarkup::Close) printf("/");
//            printf("%s", m.name.c_str());
//            if(m.kind == HtmlMarkup::OpenClose) printf("/");
//            printf(" ");
//        }
//    }
    
    m_warnings = 0;
    m_errors = 0;
    
    std::unique_ptr<HtmlNode> node(new HtmlNode);
    parse(tokens.begin(), tokens.end(), *node);
    
    return node;
}

void HtmlParser::showWarnings(bool enable)
{
    m_showWarnings = enable;
}

void HtmlParser::showErrors(bool enable)
{
    m_showErrors = enable;
}

int HtmlParser::warnings() const
{
    return m_warnings;
}

int HtmlParser::errors() const
{
    return m_errors;
}

}

