#pragma once
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include "member.h"

// 순수 함수 — 콘솔 I/O, 파일 I/O 없음

int  next_id(const std::vector<Member>& members);

Member make_member(int id, const std::string& name, const std::string& email,
                   const std::string& phone, const std::string& created_at);

std::optional<Member> find_by_id(const std::vector<Member>& members, int id);

std::vector<Member> add_member(std::vector<Member> members, const Member& m);

std::vector<Member> update_member(std::vector<Member> members, const Member& updated);

std::pair<std::vector<Member>, bool> delete_member(std::vector<Member> members, int id);
