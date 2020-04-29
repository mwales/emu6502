#ifndef SIMPLE_MAP_H
#define SIMPLE_MAP_H

#include <vector>
#include <utility>
#include <iostream>
#include <cassert>

/**
 * Slower to access than an ordered map, but easier to use because you should
 * only need operator==() and operator!=() for your class
 */
template<typename K, typename V>
class SimpleMap
{
public:
    SimpleMap()
    {
        
    }
    
    ~SimpleMap()
    {
        
    }
    
    bool doesExist(K const & key)
    {
        for(auto elem: theList)
        {
            if (key == elem.first)
            {
                return true;
            }
        }
        
        return false;
    }
    
    void add(K const & key, V const & value)
    {
        if (doesExist(key))
        {
            remove(key);
        }
        
        theList.push_back(std::make_pair(key, value));
        
    }
    
    void remove(K const & key)
    {
        for(int i = 0; i < theList.size(); i++)
        {
            if (theList[i].first == key)
            {
                theList.erase(theList.begin() + i);
                return;
            }
        }
        
    }
    
    void printMap()
    {
        for(auto elem: theList)
        {
            std::cout << "[" << elem.first << " -> " << elem.second << "] ";
        }
        
        std::cout << std::endl;		
    }
    
    std::vector<K> getKeys()
    {
        std::vector<K> retVal;
        for(auto curKey: theList)
        {
            retVal.push_back(curKey.first);
        }
        return retVal;
    }
    
    bool getValue(K const & keyVal, V* valReturn)
    {
        for(auto listItems: theList)
        {
            if (listItems.first == keyVal)
            {
                *valReturn = listItems.second;
                return true;
            }
        }
        
        return false;
    }
    
    V getValue(K const & keyVal)
    {
        for(auto listItems: theList)
        {
            if (listItems.first == keyVal)
            {
                return listItems.second;
            }
        }
        
        assert(false);
        return theList.front().second;
    }
    
protected:
    
    std::vector<std::pair<K,V> > theList;
    
};

#endif
