#pragma once
#include <string>
#include <vector>
#include "member.h"

class MemberRepository {
public:
    explicit MemberRepository(std::string path);

    std::vector<Member> load_all() const;
    void                save_all(const std::vector<Member>& members) const;

private:
    std::string path_;
};
