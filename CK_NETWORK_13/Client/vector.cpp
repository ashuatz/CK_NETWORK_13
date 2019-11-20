#include "vector.h"

vector2::vector2(const vector2Int &other) : x{ static_cast<float>(other.x) }, y{ static_cast<float>(other.y) } {}
vector2::vector2(const vector3 &other) : x{ other.x }, y{ other.y } {}
vector2::vector2(const vector3Int &other) : x{ static_cast<float>(other.x) }, y{ static_cast<float>(other.y) } {}

vector2Int::vector2Int(const vector2 &other) : x{ static_cast<int>(other.x) }, y{ static_cast<int>(other.y) } {}
vector2Int::vector2Int(const vector3 &other) : x{ static_cast<int>(other.x) }, y{ static_cast<int>(other.y) } {}
vector2Int::vector2Int(const vector3Int &other) : x{ other.x }, y{ other.y } {}

vector3::vector3(const vector2 &other) : x{ other.x }, y{ other.y }, z{ 0.0f } {}
vector3::vector3(const vector2Int &other) : x{ static_cast<float>(other.x) }, y{ static_cast<float>(other.y) }, z{ 0.0f } {}
vector3::vector3(const vector3Int &other) : x{ static_cast<float>(other.x) }, y{ static_cast<float>(other.y) }, z{ static_cast<float>(other.z) } {}

vector3Int::vector3Int(const vector2 &other) : x{ static_cast<int>(other.x) }, y{ static_cast<int>(other.y) }, z{ 0 } {}
vector3Int::vector3Int(const vector2Int &other) : x{ other.x }, y{ other.y }, z{ 0 } {}
vector3Int::vector3Int(const vector3 &other) : x{ static_cast<int>(other.x) }, y{ static_cast<int>(other.y) }, z{ static_cast<int>(other.z) } {}