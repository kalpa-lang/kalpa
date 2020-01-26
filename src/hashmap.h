#ifndef HASHMAP_H
#define HASHMAP_H

#include <unordered_map>

namespace klp {
template<typename Key, typename Val>
using HashMap = std::unordered_map<Key, Val>;
}

#endif
