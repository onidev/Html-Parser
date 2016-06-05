#ifndef ONIDEV_MARKUP_HTML_LEXER_H_INCLUDED
#define ONIDEV_MARKUP_HTML_LEXER_H_INCLUDED

#include <string>

namespace od
{

class HtmlLexer
{
public:
    enum Token
    {
        MarkupOpen, // <
        MarkupClose,// >
        Slash,      // /
        Identifier, // identifier
        Equal,      // =
        Value,      // "value"
        Text,       // <markup>text</markup>
        DoctypeHtml,
        EndOfFile
    };
    
    struct State
    {
        unsigned cursor;
        Token    token;
        char     lastchar;
    };

private:
    std::string _str;
    unsigned&   _cursor;
    char        _last_char;
    std::string _identifier;
    std::string _value;
    Token       _last;
    
    char  _getchar();
    Token _gettok(bool markup);
    
public:
    HtmlLexer(const std::string & str, unsigned& cursor);
    
    const std::string & identifier() const;
    const std::string & value() const;
    const std::string & text() const;
    
    Token lastToken() const;
    Token getToken(bool markup);
    
    State saveState() const;
    void restoreState(const State & state);
};

}

#endif
