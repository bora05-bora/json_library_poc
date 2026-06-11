#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "../json_library_poc/src/member_repository.h"

namespace fs = std::filesystem;

// 테스트마다 임시 파일을 생성·삭제하는 픽스처
class MemberRepositoryTest : public ::testing::Test {
protected:
    std::string tmp_path;

    void SetUp() override {
        tmp_path = (fs::temp_directory_path() / "test_members.json").string();
    }

    void TearDown() override {
        fs::remove(tmp_path);
    }
};

// ──────────────────────────────────────────────
// load_all
// ──────────────────────────────────────────────
TEST_F(MemberRepositoryTest, LoadNonexistentFileReturnsEmpty)
{
    MemberRepository repo("nonexistent_file_xyz.json");
    auto members = repo.load_all();
    EXPECT_TRUE(members.empty());
}

TEST_F(MemberRepositoryTest, LoadInvalidJsonReturnsEmpty)
{
    std::ofstream ofs(tmp_path);
    ofs << "{ this is not valid json }";
    ofs.close();

    MemberRepository repo(tmp_path);
    auto members = repo.load_all();
    EXPECT_TRUE(members.empty());
}

// ──────────────────────────────────────────────
// save_all / load_all 왕복
// ──────────────────────────────────────────────
TEST_F(MemberRepositoryTest, SaveAndLoadRoundtrip)
{
    std::vector<Member> original = {
        {1, "Alice", "a@test.com", "010-0000-0001", "2026-01-01"},
        {2, "Bob",   "b@test.com", "010-0000-0002", "2026-01-02"}
    };

    MemberRepository repo(tmp_path);
    repo.save_all(original);
    auto loaded = repo.load_all();

    ASSERT_EQ(loaded.size(), 2u);
    EXPECT_EQ(loaded[0].id,    original[0].id);
    EXPECT_EQ(loaded[0].name,  original[0].name);
    EXPECT_EQ(loaded[0].email, original[0].email);
    EXPECT_EQ(loaded[0].phone, original[0].phone);
    EXPECT_EQ(loaded[1].id,    original[1].id);
    EXPECT_EQ(loaded[1].name,  original[1].name);
}
