#include "member_repository.h"
#include <fstream>
#include <iostream>
#include "../include/nlohmann/json.hpp"

using json = nlohmann::json;

MemberRepository::MemberRepository(std::string path) : path_(std::move(path)) {}

std::vector<Member> MemberRepository::load_all() const
{
    std::ifstream ifs(path_);
    if (!ifs.is_open())
        return {};

    json root;
    try {
        ifs >> root;
    }
    catch (const json::parse_error& e) {
        std::cerr << "[Parse Error] " << e.what() << "\n";
        return {};
    }

    std::vector<Member> members;
    for (const auto& item : root["members"])
        members.push_back(item.get<Member>());
    return members;
}

void MemberRepository::save_all(const std::vector<Member>& members) const
{
    json root;
    root["members"] = members;
    std::ofstream ofs(path_);
    ofs << root.dump(4);
}
