#include <iostream>
#include <chrono>
#include <format>
#define NOMINMAX
#include <windows.h>

#include "src/member.h"
#include "src/member_repository.h"
#include "src/member_service.h"
#include "src/member_view.h"

static std::string today_string()
{
    auto today = std::chrono::year_month_day{
        std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())
    };
    return std::format("{:04}-{:02}-{:02}",
        (int)today.year(), (unsigned)today.month(), (unsigned)today.day());
}

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    MemberRepository repo("members.json");
    auto members = repo.load_all();

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
        case 1:
            std::cout << "\n=== 회원 전체 조회 ===\n";
            print_all_members(members);
            break;

        case 2: {
            std::cout << "\n=== 회원 단건 조회 ===\n조회할 ID : ";
            auto found = find_by_id(members, input_id());
            if (found) {
                std::cout << "----------------------------------------\n";
                print_member(*found);
                std::cout << "----------------------------------------\n";
            }
            else {
                std::cout << "[오류] 해당 ID의 회원을 찾을 수 없습니다.\n";
            }
            break;
        }

        case 3: {
            std::cout << "\n=== 회원 추가 ===\n";
            std::cout << "이름     : "; auto name  = input_line();
            std::cout << "이메일   : "; auto email = input_line();
            std::cout << "전화번호 : "; auto phone = input_line();
            auto m = make_member(next_id(members), name, email, phone, today_string());
            members = add_member(members, m);
            repo.save_all(members);
            std::cout << "[완료] 회원이 추가되었습니다. (ID: " << m.id << ")\n";
            break;
        }

        case 4: {
            std::cout << "\n=== 회원 수정 ===\n수정할 ID : ";
            auto found = find_by_id(members, input_id());
            if (!found) {
                std::cout << "[오류] 해당 ID의 회원을 찾을 수 없습니다.\n";
                break;
            }
            std::cout << "[현재 정보]\n";
            print_member(*found);
            std::cout << "\n새 이름     (Enter=유지) : "; auto name  = input_line();
            std::cout << "새 이메일   (Enter=유지) : "; auto email = input_line();
            std::cout << "새 전화번호 (Enter=유지) : "; auto phone = input_line();
            Member updated = *found;
            if (!name.empty())  updated.name  = name;
            if (!email.empty()) updated.email = email;
            if (!phone.empty()) updated.phone = phone;
            members = update_member(members, updated);
            repo.save_all(members);
            std::cout << "[완료] 회원 정보가 수정되었습니다.\n";
            break;
        }

        case 5: {
            std::cout << "\n=== 회원 삭제 ===\n삭제할 ID : ";
            int id = input_id();
            auto found = find_by_id(members, id);
            if (!found) {
                std::cout << "[오류] 해당 ID의 회원을 찾을 수 없습니다.\n";
                break;
            }
            std::cout << "[삭제 대상]\n";
            print_member(*found);
            std::cout << "삭제하시겠습니까? (y/n) : ";
            if (confirm_delete()) {
                auto [updated, ok] = delete_member(members, id);
                members = updated;
                repo.save_all(members);
                std::cout << "[완료] 회원이 삭제되었습니다.\n";
            }
            else {
                std::cout << "[취소] 삭제가 취소되었습니다.\n";
            }
            break;
        }

        case 0:
            std::cout << "종료합니다.\n";
            return 0;

        default:
            std::cout << "[오류] 올바른 메뉴를 선택하세요.\n";
        }
    }
}
