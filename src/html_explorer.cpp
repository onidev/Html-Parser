#include <onidev/markup/html.h>

namespace od
{

HtmlExplorer::iterator HtmlExplorer::begin()
{
    iterator it(this, *root);
    it.q.push(root);
    return it;
}

HtmlExplorer::iterator HtmlExplorer::end()
{
    return iterator();
}

void HtmlExplorer::iterator::operator++()
{
    if(!q.empty())
    {
        node = q.front();
        q.pop();
        for(auto& n: node->childs())
        {
            q.push(n.get());
        }
    }
    else
    {
        node = nullptr;
    }
}

HtmlNode& HtmlExplorer::iterator::operator*()
{
    return *node;
}

}
