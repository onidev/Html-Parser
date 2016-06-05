#include <iostream>
#include <cstdio>

namespace 
{

class LexJs
{
    char last_char;
    const std::string & str;
    unsigned & cursor;
    
    char getchar();
    void moveback();
    
public:
    LexJs(const std::string & str, unsigned & cursor):
        last_char(' '),
        str(str),
        cursor(cursor)
    {
    }
    
    bool eof() const { return cursor >= str.length(); }
    
    bool gettok();
};

bool LexJs::gettok()
{
    // Skip spaces
    while( isspace(last_char) || last_char == '\n' || last_char == '\t' )
    {
        last_char = getchar();
    }
    
    // Skip comments
    if(last_char == '/')
    {
        last_char = getchar();
        // One line comment
        if(last_char == '/')
        {
            while( last_char != '\n' && last_char != '\0' )
            {
                if(last_char == '<')
                {
                    last_char = getchar();
                    if(last_char == '/')
                    {
                        moveback();
                        moveback();
                        return true;
                    }
                }
                else
                    last_char = getchar();
            }
            
            if(last_char != '\0')
                return gettok();
            else
            {
                printf("[eof0]");
                return true; // eof
            }
        }
        else
        // Multi lines comment
        if(last_char == '*')
        {
            while(true)
            {
                while( (last_char = getchar()) != '*' )
                {
                    if(last_char == '\0')
                        return true;
                }
                last_char = getchar();
                
                if(last_char == '\0')
                    return true;
                
                if(last_char == '/')
                    break;
            }
            
            last_char = getchar();
            return gettok();
        }
        else
        // regex /motif/flags
        {
            while(true)
            {
                if(last_char == '\\') // skip possible \/
                {
                    last_char = getchar();
                    continue;
                }
                
                if(last_char == '\0')
                    return true;
                
                if( (last_char = getchar()) == '/')
                    break;
            }
            
            last_char = getchar();
            return gettok();
        }
    }
    
    if(last_char == '"')
    {
        //printf("ENTER");
        last_char = getchar();
        while(last_char != '"')
        {
            //printf("%c", last_char);
            if(last_char == '\\')
            {
                last_char = getchar();
                if(last_char == '"')
                    last_char = getchar();
            }
            else
            {
                last_char = getchar();
            }
        }
        last_char = getchar();
        return false;
    }
    
    if(last_char == '<')
    {
        // ajouter verif script?
        last_char = getchar();
        if(last_char == '/')
        {
            moveback();
            moveback();
            return true;
        }
        else
            return gettok();
    }
    
    char t[] = ":;.,()[]{}!?=+-*%^&|~>$";
    for(size_t i=0; i<sizeof(t); ++i)
    {
        if(last_char == t[i])
        {
            last_char = getchar();
            return false;
        }
            
    }
    
    if(isalpha(last_char) || last_char == '_')
    {
        while( isalnum(last_char = getchar()) || last_char == '_' );
        return false;
    }
    
    if(isdigit(last_char))
    {
        last_char = getchar();
        while(isdigit(last_char))
        {
            last_char = getchar();
        }
        return false;
    }
    
    
    if(last_char == '\'')
    {
        last_char = getchar();
        while(last_char != '\'')
        {
            if(last_char == '\\')
            {
                last_char = getchar();
                if(last_char == '\'')
                    last_char = getchar();
            }
            else
            {
                last_char = getchar();
            }
        }
        last_char = getchar();
        return false;
    }
    
    return true;
}

char LexJs::getchar()
{
    if(!eof())
    {
        return str[cursor++];
    }
    return '\0'; // eof
}

void LexJs::moveback()
{
    cursor--;
}

}

namespace od
{
namespace hidden
{

void skipJs(const std::string & str, unsigned & cursor)
{
    if(str[cursor] == '<' && str[cursor+1] == '/')
    {
        return;
    }
    
    LexJs skip(str, cursor);
    
    while(!skip.eof() && !skip.gettok())
    {
    }
}

}
}

