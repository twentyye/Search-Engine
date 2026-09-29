#include "CompareSimhash.h"
#include "WebPage.h"

namespace wdcpp
{
/**
 *	Returns true if the web page is excluded, otherwise returns false.
 */
bool CompareSimhash::cut(const WebPage &page)
{
    uint64_t i = 0;
    _simhasher.make(page.getContent(), topN, i); //Calculate the 64-bit hash value of the page.

    uint64_t a = 0xff000000;
    uint64_t b = 0x00ff0000;
    uint64_t c = 0x0000ff00;
    uint64_t d = 0x000000ff;

    for (auto &list : keylist) 
    {
        if ((i & a) == list.key_A) // Hit key_A
        {
            for (auto &hash : list.A) 
            {
                if (Simhasher::isEqual(i, hash))
                {
                    return true;
                }
            }
            list.A.push_back(i); // If i is not found in list.A, add i to list.A
        }
        if ((i & b) == list.key_B) // hit key_B
        {
            for (auto &hash : list.B) 
            {
                if (Simhasher::isEqual(i, hash))
                {
                    return true;
                }
            }
            list.B.push_back(i); // If i is not found in list.B, add i to list.B
        }
        if ((i & c) == list.key_C) // hit key_C
        {
            for (auto &hash : list.C) 
            {
                if (Simhasher::isEqual(i, hash))
                {
                    return true;
                }
            }
            list.C.push_back(i); //When i is not found in list.C, add i to list.C
        }
        if ((i & d) == list.key_D) // hit key_D
        {
            for (auto &hash : list.D) 
            {
                if (Simhasher::isEqual(i, hash))
                {
                    return true;
                }
            }
            list.D.push_back(i); // When i is not found in list.D, add i to list.d
        }
    }

    //None of the list keys matched; add the hash value to keylist as a new list
    hashkey h(i);
    keylist.push_back(h);
    return false;
}
}; // namespace wdcpp
