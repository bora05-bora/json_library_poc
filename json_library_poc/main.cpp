#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#define NOMINMAX
#include <windows.h>

#include "include/nlohmann/json.hpp"

using json = nlohmann::json;

const std::string DATA_FILE = "members.json";

// ──────────────────────────────────────────────
// 파일 I/O
// ──────────────────────────────────────────────
json load_members()
{
    std::ifstream ifs(DATA_FILE);
    if (!ifs.is_open())
        return { {"members", json::array()} };

    json root;
    try {
        ifs >> root;
    }
    catch (const json::parse_error& e) {
        std::cerr << "[Parse Error] " << e.what() << "\n";
        return { {"members", json::array()} };
    }
    return root;
}

void save_members(const json& root)
{
    std::ofstream ofs(DATA_FILE);
    ofs << root.dump(4);
}

// ──────────────────────────────────────────────
// 유틸
// ──────────────────────────────────────────────
std::string today_string()
{
    auto today = std::chrono::year_month_day{
        std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())
    };
    return std::format("{:04}-{:02}-{:02}",
        (int)today.year(), (unsigned)today.month(), (unsigned)today.day());
}

int next_id(const json& root)
{
    int max_id = 0;
    for (const auto& m : root["members"])
        max_id = std::max(max_id, m["id"].get<int>());
    return max_id + 1;
}

void print_member(const json& m)
{
    std::cout << "  ID       : " << m["id"].get<int>() << "\n";
    std::cout << "  이름     : " << m["name"].get<std::string>() << "\n";
    std::cout << "  이메일   : " << m["email"].get<std::string>() << "\n";
    std::cout << "  전화번호 : " << m["phone"].get<std::string>() << "\n";
    std::cout << "  가입일   : " << m["created_at"].get<std::string>() << "\n";
}

// ──────────────────────────────────────────────
// Create
// ──────────────────────────────────────────────
void create_member(json& root)
{
    std::cout << "\n=== 회원 추가 ===\n";

    std::string name, email, phone;
    std::cout << "이름     : "; std::getline(std::cin, name);
    std::cout << "이메일   : "; std::getline(std::cin, email);
    std::cout << "전화번호 : "; std::getline(std::cin, phone);

    json member = {
        {"id",         next_id(root)},
        {"name",       name},
        {"email",      email},
        {"phone",      phone},
        {"created_at", today_string()}
    };

    root["members"].push_back(member);
    save_members(root);

    std::cout << "[완료] 회원이 추가되었습니다. (ID: " << member["id"].get<int>() << ")\n";
}

// ──────────────────────────────────────────────
// Read
// ──────────────────────────────────────────────
void read_all_members(const json& root)
{
    std::cout << "\n=== 회원 전체 조회 ===\n";

    const auto& members = root["members"];
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

void read_member(const json& root)
{
    std::cout << "\n=== 회원 단건 조회 ===\n";
    std::cout << "조회할 ID : ";
    int id; std::cin >> id; std::cin.ignore();

    for (const auto& m : root["members"]) {
        if (m["id"].get<int>() == id) {
            std::cout << "----------------------------------------\n";
            print_member(m);
            std::cout << "----------------------------------------\n";
            return;
        }
    }
    std::cout << "[오류] ID " << id << " 회원을 찾을 수 없습니다.\n";
}

// ──────────────────────────────────────────────
// Update
// ──────────────────────────────────────────────
void update_member(json& root)
{
    std::cout << "\n=== 회원 수정 ===\n";
    std::cout << "수정할 ID : ";
    int id; std::cin >> id; std::cin.ignore();

    for (auto& m : root["members"]) {
        if (m["id"].get<int>() == id) {
            std::cout << "[현재 정보]\n";
            print_member(m);

            std::string name, email, phone;
            std::cout << "\n새 이름     (Enter=유지) : "; std::getline(std::cin, name);
            std::cout << "새 이메일   (Enter=유지) : "; std::getline(std::cin, email);
            std::cout << "새 전화번호 (Enter=유지) : "; std::getline(std::cin, phone);

            if (!name.empty())  m["name"]  = name;
            if (!email.empty()) m["email"] = email;
            if (!phone.empty()) m["phone"] = phone;

            save_members(root);
            std::cout << "[완료] 회원 정보가 수정되었습니다.\n";
            return;
        }
    }
    std::cout << "[오류] ID " << id << " 회원을 찾을 수 없습니다.\n";
}

// ──────────────────────────────────────────────
// Delete
// ──────────────────────────────────────────────
void delete_member(json& root)
{
    std::cout << "\n=== 회원 삭제 ===\n";
    std::cout << "삭제할 ID : ";
    int id; std::cin >> id; std::cin.ignore();

    auto& members = root["members"];
    for (auto it = members.begin(); it != members.end(); ++it) {
        if ((*it)["id"].get<int>() == id) {
            std::cout << "[삭제 대상]\n";
            print_member(*it);
            std::cout << "삭제하시겠습니까? (y/n) : ";
            char confirm; std::cin >> confirm; std::cin.ignore();
            if (confirm == 'y' || confirm == 'Y') {
                members.erase(it);
                save_members(root);
                std::cout << "[완료] 회원이 삭제되었습니다.\n";
            }
            else {
                std::cout << "[취소] 삭제가 취소되었습니다.\n";
            }
            return;
        }
    }
    std::cout << "[오류] ID " << id << " 회원을 찾을 수 없습니다.\n";
}

// ──────────────────────────────────────────────
// main
// ──────────────────────────────────────────────
int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    json root = load_members();

    while (true) {
        std::cout << "\n==============================\n";
        std::cout << "      회원 관리 시스템\n";
        std::cout << "==============================\n";
        std::cout << "1. 전체 조회\n";
        std::cout << "2. 단건 조회\n";
        std::cout << "3. 회원 추가\n";
        std::cout << "4. 회원 수정\n";
        std::cout << "5. 회원 삭제\n";
        std::cout << "0. 종료\n";
        std::cout << "------------------------------\n";
        std::cout << "선택 : ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }
        std::cin.ignore();

        switch (choice) {
        case 1: read_all_members(root); break;
        case 2: read_member(root);      break;
        case 3: create_member(root);    break;
        case 4: update_member(root);    break;
        case 5: delete_member(root);    break;
        case 0:
            std::cout << "종료합니다.\n";
            return 0;
        default:
            std::cout << "[오류] 올바른 메뉴를 선택하세요.\n";
        }
    }
}
