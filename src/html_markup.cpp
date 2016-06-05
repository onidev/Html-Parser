#include "html_markup.h"
#include "html_lexer.h"
#include <algorithm>

namespace od
{

HtmlMarkupLexer::HtmlMarkupLexer(const std::string & str):
    str(str),
    lex(str, cursor),
    cursor(0),
    initialized(false)
{
}

HtmlMarkup & HtmlMarkupLexer::get()
{
    markup = HtmlMarkup();
    
    // '<'
    if(lex.lastToken() != HtmlLexer::MarkupOpen)
    {
        if(lex.lastToken() == HtmlLexer::Text)
        {
            markup = HtmlMarkup(HtmlMarkup::Text, lex.text());
            return markup;
        }
        else
        {
            return markup = HtmlMarkup();
        }
    }
    
    lex.getToken(true);
    
    // '/' | identifier
    if(lex.lastToken() == HtmlLexer::Slash)
    {
        markup.kind = HtmlMarkup::Close;
        lex.getToken(true);
    }
    else if(lex.lastToken() == HtmlLexer::Identifier)
    {
        markup.kind = HtmlMarkup::Open;
    }
    else
    {
        return markup = HtmlMarkup();
    }
    
    // identifier
    if(lex.lastToken() == HtmlLexer::Identifier)
    {
        markup.name = lex.identifier();
        std::transform(markup.name.begin(), markup.name.end(), markup.name.begin(), ::tolower);
    }
    else
    {
        return markup = HtmlMarkup();
    }
    
    // attributes
    lex.getToken(true);
    if(markup.kind == HtmlMarkup::Open)
    {
        while(lex.lastToken() == HtmlLexer::Identifier)
        {
            std::string att = lex.identifier();
            
            if(lex.getToken(true) != HtmlLexer::Equal)
            {
                markup.params.push_back( HtmlNode::Element(lex.identifier(), "") );
                continue;
            }
            if(lex.getToken(true) != HtmlLexer::Value)
            {
                
                if(lex.lastToken() == HtmlLexer::Identifier)
                {
                    markup.params.push_back( HtmlNode::Element(att, lex.identifier()) );
                }
                else
                    return markup = HtmlMarkup();
            }
            else
            {
                markup.params.push_back( HtmlNode::Element(lex.identifier(), lex.value()) );
            }
            
            lex.getToken(true);
        }
    }
    
    // '/' | '>'
    if(lex.lastToken() == HtmlLexer::Slash && markup.kind == HtmlMarkup::Open)
    {
        markup.kind = HtmlMarkup::OpenClose;
        if(lex.getToken(true) != HtmlLexer::MarkupClose)
        {
            return markup = HtmlMarkup();
        }
    }
    else if(lex.lastToken() != HtmlLexer::MarkupClose)
    {
        return markup = HtmlMarkup();
    }
    
    return markup;
}

namespace hidden {
    void skipJs(const std::string & str, unsigned & cursor);
}

const HtmlMarkup & HtmlMarkupLexer::getToken()
{
    if(!initialized)
    {
        if(lex.getToken(false) == HtmlLexer::DoctypeHtml)
            lex.getToken(false);
        get();
        initialized = true;
        return markup;
    }
    else
    {
        if(markup.kind == HtmlMarkup::Open && markup.name == "script")
        {
            unsigned start = cursor;
            cursor--;
            //printf("SKIP JS\n");
            hidden::skipJs(str, cursor);
            
            if(cursor >= start)
            {
                std::string js = str.substr(start-1, cursor-start+1);
                markup = HtmlMarkup(HtmlMarkup::Text, js);
            }
            else
            {
                 markup = HtmlMarkup(HtmlMarkup::Text);
            }
            
            HtmlLexer::State state = {cursor, HtmlLexer::MarkupOpen, ' '};
            lex.restoreState(state);
            return markup;
        }
        else
        {
            lex.getToken(false);
            get();
            return markup;
        }
    }
}

bool HtmlMarkupLexer::eof() const
{
    if(!initialized) return false;
    return markup.kind == HtmlMarkup::Error;
}

}
