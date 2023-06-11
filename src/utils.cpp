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

template<typename T1, typename T2>//
void draw(T1 &canvas, T2 &drawable)
{
  if (!drawable.exists()) { return; }

  for (int y = drawable.top(); y <= drawable.bottom(); y += 2) {
    canvas.DrawBlockLine(drawable.left(), y, drawable.right(), y, drawable.color());
  }
}

template<typename T1, typename T2>//
[[nodiscard]] T1 dynamic_multiple_cast(T2 t2_1, T2 t2_2)// todo: ...
{
  auto t1_1 = dynamic_cast<T1>(t2_1);
  auto t1_2 = dynamic_cast<T1>(t2_2);

  if (t1_1 == nullptr) { return t1_2; }
  return t1_1;
}

#endif