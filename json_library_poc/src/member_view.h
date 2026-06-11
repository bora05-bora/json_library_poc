#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "member.h"

inline void print_member(const Member& m)
{
    std::cout << "  ID       : " << m.id         << "\n";
    std::cout << "  이름     : " << m.name        << "\n";
    std::cout << "  이메일   : " << m.email       << "\n";
    std::cout << "  전화번호 : " << m.phone       << "\n";
    std::cout << "  가입일   : " << m.created_at  << "\n";
}

inline void print_all_members(const std::vector<Member>& members)
{
    if (members.empty()) {
        std::cout << "등록된 회원이 없습니다.\n";
        return;
    }
    for (const auto& m : members) {
        std::cout << "----------------------------------------\n";
        print_member(m);
    }
    std::cout << "----------------------------------------\n";
    std::cout << "총 " << members.size() << "명\n";
}

inline int input_id()
{
    int id;
    std::cin >> id;
    std::cin.ignore();
    return id;
}

inline std::string input_line()
{
    std::string s;
    std::getline(std::cin, s);
    return s;
}

inline bool confirm_delete()
{
    char c;
    std::cin >> c;
    std::cin.ignore();
    return c == 'y' || c == 'Y';
}
