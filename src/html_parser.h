#ifndef ONIDEV_MARKUP_HTML_PARSER_H_INCLUDED
#define ONIDEV_MARKUP_HTML_PARSER_H_INCLUDED

#include <onidev/markup/html.h>
#include <vector>

namespace od
{
class HtmlMarkup;

class HtmlParser
{
    int m_warnings;
    int m_errors;
    
    bool m_showWarnings;
    bool m_showErrors;
    
public:
    HtmlParser();
    
    std::unique_ptr<HtmlNode> parseString(const std::string & str);
    void parse(std::vector<HtmlMarkup>::iterator beg, std::vector<HtmlMarkup>::iterator end, HtmlNode& node);
    
    void showWarnings(bool enable);
    void showErrors(bool enable);
    
    int warnings() const;
    int errors() const;
};

}

#endif // ONIDEV_MARKUP_HTML_PARSER_H_INCLUDED
