#include "html_lexer.h"
#include <cstdio>
#include <cstdlib>

namespace od
{

char HtmlLexer::_getchar()
{
    if(_cursor < _str.length())
    {
        return _str[_cursor++];
    }
    return '\0';
}

HtmlLexer::Token HtmlLexer::_gettok(bool markup)
{
    // Skip spaces
    while( isspace(_last_char) || _last_char == '\n' || _last_char == '\t' )
    {
        _last_char = _getchar();
    }
    
    // Skip comments
    if(_last_char == '<') // open markup?
    {
        _last_char = _getchar();
        
        // skip <?xml version.. ?> - moche, a revoir
        if(_last_char == '?')
        {
            while( (_last_char = _getchar()) != '?' )
            {
                if((_last_char = _getchar()) == '>')
                    break;
            }
            _last_char = _getchar();
            return _gettok(markup);
        }
        
        if(_last_char == '!')
        {
            _last_char = _getchar();
            if(_last_char == '-')
            {
                //@todo check errors
                _last_char = _getchar(); // -
                
                while(true)
                {
                    while( (_last_char = _getchar()) != '-' )
                    {
                        if(_last_char == '\0')
                            return EndOfFile;
                    }
                    _last_char = _getchar();
                    
                    if(_last_char == '\0')
                        return EndOfFile;
                    
                    if(_last_char == '-')
                    {
                        _last_char = _getchar();
                        if(_last_char == '>')
                            break;
                    }
                }
                
                _last_char = _getchar();
                return _gettok(markup);
            }
            else
            {
                while( (_last_char=_getchar()) != '>');
                _last_char=_getchar();
                return DoctypeHtml;
            }
        }
        // Bien differencier une balise ouvrante d'un texte
        else if(_last_char == '<' || _last_char == ' ' || _last_char == '\n' || _last_char == '\t')
        {
            // <html><</html>
            // texte
            _identifier = '<';
            
            while(true)
            {
                if(_last_char == '\0')
                    exit(33);
                
                if(_last_char == '<')
                {
                    unsigned temp = _cursor;
                    char next = _getchar();
                    _cursor = temp;
                    
                    if(!(next == '<' || next == ' ' || next == '\n' || next == '\t'))
                    {
                        break;
                    }
                }
                
                _identifier += _last_char;
                _last_char = _getchar();
            }
            return Text;
        }
        
        return MarkupOpen;
    }
    
    if(markup)
    {
        Token ret = EndOfFile;
        if(_last_char == '>')
            ret = MarkupClose;
        else if(_last_char == '=')
            ret = Equal;
        else if(_last_char == '/')
            ret = Slash;
        
        if(ret != EndOfFile)
        {
            _last_char = _getchar();
            return ret;
        }
        
        if(_last_char == '"')
        {
            _value.clear();
            
            while((_last_char = _getchar()) != '"')
            {
                _value += _last_char;
            }
            _last_char = _getchar();
            return Value;
        }
        
        if(_last_char == '\'')
        {
            _value.clear();
            
            while((_last_char = _getchar()) != '\'')
            {
                _value += _last_char;
            }
            _last_char = _getchar();
            return Value;
        }
    }
    
    
    if(markup && _last_char == '#')
    {
        _identifier = _last_char;
        while( isalnum(_last_char = _getchar()) || _last_char == '_' )
            _identifier += _last_char;
        return Identifier;
    }
    else if(markup && (isalnum(_last_char) || _last_char == '_'))
    {
        _identifier = _last_char;
        
        while( isalnum(_last_char = _getchar()) || _last_char == '_' || _last_char == ':' || _last_char == '-' )
            _identifier += _last_char;
        
        if(_last_char == '%') {
            _identifier += _last_char;
            _last_char = _getchar();
        }
        
        return Identifier;
    }
    else if(!markup && _last_char != '\0')
    {
        _identifier = _last_char;
        while( (_last_char = _getchar()) != '<' && _last_char != '\0')
            _identifier += _last_char;
        return Text;
    }
    
    return EndOfFile;
}

HtmlLexer::HtmlLexer(const std::string & str, unsigned& cursor):
    _str(str),
    _cursor(cursor),
    _last_char(' ')
{
}

const std::string & HtmlLexer::identifier() const
{
    return _identifier;
}

const std::string & HtmlLexer::value() const
{
    return _value;
}

const std::string & HtmlLexer::text() const
{
    return _identifier;
}

HtmlLexer::Token HtmlLexer::lastToken() const
{
    return _last;
}

HtmlLexer::Token HtmlLexer::getToken(bool markup)
{
    _last = _gettok(markup);
    return _last;
}

HtmlLexer::State HtmlLexer::saveState() const
{
    State state = { _cursor, lastToken(), _last_char };
    return state;
}

void HtmlLexer::restoreState(const State & state)
{
    _cursor = state.cursor;
    _last  = state.token;
    _last_char = state.lastchar;
}

}
