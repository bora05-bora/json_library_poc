#include "member_service.h"
#include <algorithm>

int next_id(const std::vector<Member>& members)
{
    int max_id = 0;
    for (const auto& m : members)
        max_id = std::max(max_id, m.id);
    return max_id + 1;
}

Member make_member(int id, const std::string& name, const std::string& email,
                   const std::string& phone, const std::string& created_at)
{
    return { id, name, email, phone, created_at };
}

std::optional<Member> find_by_id(const std::vector<Member>& members, int id)
{
    for (const auto& m : members)
        if (m.id == id) return m;
    return std::nullopt;
}

std::vector<Member> add_member(std::vector<Member> members, const Member& m)
{
    members.push_back(m);
    return members;
}

std::vector<Member> update_member(std::vector<Member> members, const Member& updated)
{
    for (auto& m : members) {
        if (m.id == updated.id) {
            m = updated;
            return members;
        }
    }
    return members;
}

std::pair<std::vector<Member>, bool> delete_member(std::vector<Member> members, int id)
{
    auto it = std::find_if(members.begin(), members.end(),
                           [id](const Member& m) { return m.id == id; });
    if (it == members.end())
        return { members, false };
    members.erase(it);
    return { members, true };
}
