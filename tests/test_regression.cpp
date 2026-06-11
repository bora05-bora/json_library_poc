#include <gtest/gtest.h>
#include <filesystem>
#include "../json_library_poc/src/member_service.h"
#include "../json_library_poc/src/member_repository.h"

namespace fs = std::filesystem;

class RegressionTest : public ::testing::Test {
protected:
    std::string tmp_path;

    void SetUp() override {
        tmp_path = (fs::temp_directory_path() / "regression_members.json").string();
    }

    void TearDown() override {
        fs::remove(tmp_path);
    }
};

// R-01: 전체 CRUD 흐름이 순서대로 정상 동작해야 한다
TEST_F(RegressionTest, FullCrudFlow)
{
    std::vector<Member> members;

    // 추가
    Member m = make_member(next_id(members), "Alice", "alice@test.com", "010-1111-0001", "2026-01-01");
    members = add_member(members, m);
    ASSERT_EQ(members.size(), 1u);

    // 조회
    auto found = find_by_id(members, 1);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->name, "Alice");

    // 수정
    Member updated = {1, "Alice Updated", "new@test.com", "010-9999-9999", "2026-01-01"};
    members = update_member(members, updated);
    auto after_update = find_by_id(members, 1);
    ASSERT_TRUE(after_update.has_value());
    EXPECT_EQ(after_update->name, "Alice Updated");
    EXPECT_EQ(after_update->email, "new@test.com");

    // 삭제
    auto [result, ok] = delete_member(members, 1);
    EXPECT_TRUE(ok);
    EXPECT_TRUE(result.empty());
}

// R-02: 여러 회원 추가 후 각 ID로 정확히 조회되어야 한다
TEST_F(RegressionTest, MultipleAddAndFind)
{
    std::vector<Member> members;
    for (int i = 0; i < 5; ++i) {
        int id = next_id(members);
        members = add_member(members, make_member(id, "User" + std::to_string(i),
            "u" + std::to_string(i) + "@test.com", "010-0000-000" + std::to_string(i), "2026-01-01"));
    }
    ASSERT_EQ(members.size(), 5u);

    for (int id = 1; id <= 5; ++id) {
        auto found = find_by_id(members, id);
        ASSERT_TRUE(found.has_value()) << "id=" << id << " should be found";
        EXPECT_EQ(found->id, id);
    }
}

// R-03: 삭제 후 next_id는 기존 최대 ID+1을 유지해야 한다
TEST_F(RegressionTest, NextIdAfterDelete)
{
    std::vector<Member> members = {
        {1, "A", "", "", ""},
        {2, "B", "", "", ""},
        {3, "C", "", "", ""}
    };

    auto [after_delete, ok] = delete_member(members, 2);
    EXPECT_TRUE(ok);
    // 삭제 후에도 next_id는 max(1,3)+1 = 4
    EXPECT_EQ(next_id(after_delete), 4);
}

// R-04: save_all → load_all 시 모든 필드가 손실 없이 복원되어야 한다
TEST_F(RegressionTest, SaveLoadPreservesAllFields)
{
    std::vector<Member> original = {
        {1, "Alice",   "alice@test.com", "010-1111-1111", "2026-01-01"},
        {2, "Bob",     "bob@test.com",   "010-2222-2222", "2026-01-02"},
        {3, "Charlie", "cha@test.com",   "010-3333-3333", "2026-01-03"}
    };

    MemberRepository repo(tmp_path);
    repo.save_all(original);
    auto loaded = repo.load_all();

    ASSERT_EQ(loaded.size(), original.size());
    for (size_t i = 0; i < original.size(); ++i) {
        EXPECT_EQ(loaded[i].id,         original[i].id);
        EXPECT_EQ(loaded[i].name,       original[i].name);
        EXPECT_EQ(loaded[i].email,      original[i].email);
        EXPECT_EQ(loaded[i].phone,      original[i].phone);
        EXPECT_EQ(loaded[i].created_at, original[i].created_at);
    }
}

// R-05: 삭제된 ID는 find_by_id에서 nullopt를 반환해야 한다
TEST_F(RegressionTest, DeletedIdNotFound)
{
    std::vector<Member> members = {
        {1, "Alice", "a@test.com", "010-0000-0001", "2026-01-01"},
        {2, "Bob",   "b@test.com", "010-0000-0002", "2026-01-02"}
    };

    auto [result, ok] = delete_member(members, 1);
    EXPECT_TRUE(ok);

    auto found = find_by_id(result, 1);
    EXPECT_FALSE(found.has_value());

    // 삭제되지 않은 회원은 여전히 조회 가능
    auto still_there = find_by_id(result, 2);
    EXPECT_TRUE(still_there.has_value());
}

// R-06: 한 회원 수정 시 나머지 회원 데이터에 영향이 없어야 한다
TEST_F(RegressionTest, UpdatePreservesOtherMembers)
{
    std::vector<Member> members = {
        {1, "Alice", "a@test.com", "010-1111-1111", "2026-01-01"},
        {2, "Bob",   "b@test.com", "010-2222-2222", "2026-01-02"},
        {3, "Carol", "c@test.com", "010-3333-3333", "2026-01-03"}
    };

    Member updated = {2, "Bob Updated", "bob_new@test.com", "010-9999-9999", "2026-01-02"};
    auto result = update_member(members, updated);

    // 수정 대상 확인
    auto found2 = find_by_id(result, 2);
    ASSERT_TRUE(found2.has_value());
    EXPECT_EQ(found2->name, "Bob Updated");

    // 나머지 회원 불변 확인
    auto found1 = find_by_id(result, 1);
    ASSERT_TRUE(found1.has_value());
    EXPECT_EQ(found1->name, "Alice");
    EXPECT_EQ(found1->email, "a@test.com");

    auto found3 = find_by_id(result, 3);
    ASSERT_TRUE(found3.has_value());
    EXPECT_EQ(found3->name, "Carol");
    EXPECT_EQ(found3->email, "c@test.com");
}
