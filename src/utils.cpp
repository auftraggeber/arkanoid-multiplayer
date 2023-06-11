#ifndef UTILS_CPP
#define UTILS_CPP

#include <iostream>
#include <map>
#include <memory>
#include <vector>

template<typename T1, typename T2>//
void insert_element(std::map<T1, T2> &map, T2 &element)
{
  if (map.contains(element->id())) { map.erase(map.find(element->id())); }

  map.insert(std::pair<T1, T2>{ element->id(), std::move(element) });
}

template<typename T1, typename T2>//
std::vector<T2 *> map_values(std::map<T1, std::unique_ptr<T2>> const &map)
{
  std::vector<T2 *> vector;

  std::for_each(map.begin(), map.end(), [&vector](auto const &pair) { vector.push_back(pair.second.get()); });

  return vector;
}
#endif