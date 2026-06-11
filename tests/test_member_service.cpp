#include <gtest/gtest.h>
#include "../json_library_poc/src/member_service.h"

// ──────────────────────────────────────────────
// next_id
// ──────────────────────────────────────────────
TEST(NextIdTest, EmptyList)
{
    std::vector<Member> members;
    EXPECT_EQ(next_id(members), 1);
}

TEST(NextIdTest, WithMembers)
{
    std::vector<Member> members = {
        {1, "A", "", "", ""},
        {3, "B", "", "", ""},
        {5, "C", "", "", ""}
    };
    EXPECT_EQ(next_id(members), 6);
}

// ──────────────────────────────────────────────
// add_member
// ──────────────────────────────────────────────
TEST(AddMemberTest, IncreasesCount)
{
    std::vector<Member> members;
    Member m = make_member(1, "Alice", "a@test.com", "010-0000-0001", "2026-01-01");
    auto result = add_member(members, m);
    EXPECT_EQ(result.size(), 1u);
}

TEST(AddMemberTest, AssignsCorrectId)
{
    std::vector<Member> members;
    int id = next_id(members);
    Member m = make_member(id, "Alice", "a@test.com", "010-0000-0001", "2026-01-01");
    auto result = add_member(members, m);
    EXPECT_EQ(result[0].id, 1);
}

// ──────────────────────────────────────────────
// find_by_id
// ──────────────────────────────────────────────
TEST(FindByIdTest, ExistingId)
{
    std::vector<Member> members = {
        {1, "Alice", "a@test.com", "010-0000-0001", "2026-01-01"},
        {2, "Bob",   "b@test.com", "010-0000-0002", "2026-01-02"}
    };
    auto result = find_by_id(members, 1);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->name, "Alice");
}

TEST(FindByIdTest, MissingId)
{
    std::vector<Member> members = {
        {1, "Alice", "a@test.com", "010-0000-0001", "2026-01-01"}
    };
    auto result = find_by_id(members, 99);
    EXPECT_FALSE(result.has_value());
}

// ──────────────────────────────────────────────
// update_member
// ──────────────────────────────────────────────
TEST(UpdateMemberTest, ChangesFields)
{
    std::vector<Member> members = {
        {1, "Alice", "a@test.com", "010-0000-0001", "2026-01-01"}
    };
    Member updated = {1, "Alice2", "a2@test.com", "010-0000-9999", "2026-01-01"};
    auto result = update_member(members, updated);
    EXPECT_EQ(result[0].name,  "Alice2");
    EXPECT_EQ(result[0].email, "a2@test.com");
    EXPECT_EQ(result[0].phone, "010-0000-9999");
}

TEST(UpdateMemberTest, NotFoundLeavesListUnchanged)
{
    std::vector<Member> members = {
        {1, "Alice", "a@test.com", "010-0000-0001", "2026-01-01"}
    };
    Member updated = {99, "Ghost", "", "", ""};
    auto result = update_member(members, updated);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].name, "Alice");
}

// ──────────────────────────────────────────────
// delete_member
// ──────────────────────────────────────────────
TEST(DeleteMemberTest, RemovesEntry)
{
    std::vector<Member> members = {
        {1, "Alice", "a@test.com", "010-0000-0001", "2026-01-01"},
        {2, "Bob",   "b@test.com", "010-0000-0002", "2026-01-02"}
    };
    auto [result, ok] = delete_member(members, 1);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].id, 2);
}

TEST(DeleteMemberTest, NotFoundReturnsFalse)
{
    std::vector<Member> members = {
        {1, "Alice", "a@test.com", "010-0000-0001", "2026-01-01"}
    };
    auto [result, ok] = delete_member(members, 99);
    EXPECT_FALSE(ok);
    EXPECT_EQ(result.size(), 1u);
}
