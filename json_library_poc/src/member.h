#pragma once
#include <string>
#include "../include/nlohmann/json.hpp"

struct Member {
    int         id;
    std::string name;
    std::string email;
    std::string phone;
    std::string created_at;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Member, id, name, email, phone, created_at)
