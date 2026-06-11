#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stdexcept>

#include "include/nlohmann/json.hpp"

using json = nlohmann::json;

// ──────────────────────────────────────────────
// 1. JSON → string 변환 예제
// ──────────────────────────────────────────────
void demo_to_string(const json& j)
{
    std::cout << "=== [1] JSON → string ===\n";

    // 전체 JSON을 보기 좋게 출력
    std::string pretty = j.dump(4);
    std::cout << "[pretty dump]\n" << pretty << "\n\n";

    // 특정 필드를 string으로 꺼내기
    std::string app_name  = j.at("application").get<std::string>();
    std::string host      = j["server"]["host"].get<std::string>();
    int         port      = j["server"]["port"].get<int>();
    bool        debug     = j["debug"].get<bool>();
    double      timeout   = j["server"]["timeout"].get<double>();

    std::cout << "application : " << app_name  << "\n";
    std::cout << "host        : " << host       << "\n";
    std::cout << "port        : " << port       << "\n";
    std::cout << "debug       : " << std::boolalpha << debug << "\n";
    std::cout << "timeout     : " << timeout    << "\n\n";

    // 배열을 문자열로 순회
    std::cout << "[tags]\n";
    for (const auto& tag : j["tags"])
        std::cout << "  - " << tag.get<std::string>() << "\n";
    std::cout << "\n";
}

// ──────────────────────────────────────────────
// 2. JSON → unordered_map 변환 예제
// ──────────────────────────────────────────────
void demo_to_map(const json& j)
{
    std::cout << "=== [2] JSON → unordered_map ===\n";

    // --- 2-a. 최상위 평탄화: string → string ---
    // 값이 primitive인 필드만 골라서 map으로 만들기
    std::unordered_map<std::string, std::string> flat_map;
    for (const auto& [key, val] : j.items())
    {
        if (val.is_string())
            flat_map[key] = val.get<std::string>();
        else if (val.is_number_integer())
            flat_map[key] = std::to_string(val.get<int>());
        else if (val.is_boolean())
            flat_map[key] = val.get<bool>() ? "true" : "false";
        else
            flat_map[key] = val.dump(); // object/array는 JSON 문자열로
    }

    std::cout << "[flat_map (top-level)]\n";
    for (const auto& [k, v] : flat_map)
        std::cout << "  " << k << " = " << v << "\n";
    std::cout << "\n";

    // --- 2-b. 서버 설정 → unordered_map<string, string> ---
    std::unordered_map<std::string, std::string> server_map;
    for (const auto& [k, v] : j["server"].items())
        server_map[k] = v.dump();

    std::cout << "[server map]\n";
    for (const auto& [k, v] : server_map)
        std::cout << "  " << k << " = " << v << "\n";
    std::cout << "\n";

    // --- 2-c. users 배열 → unordered_map<id, name> ---
    std::unordered_map<int, std::string> user_map;
    for (const auto& user : j["users"])
        user_map[user["id"].get<int>()] = user["name"].get<std::string>();

    std::cout << "[user_map (id → name)]\n";
    for (const auto& [id, name] : user_map)
        std::cout << "  " << id << " → " << name << "\n";
    std::cout << "\n";
}

// ──────────────────────────────────────────────
// 3. JSON 직접 빌드 & 직렬화 예제
// ──────────────────────────────────────────────
void demo_build_json()
{
    std::cout << "=== [3] JSON 직접 빌드 ===\n";

    // 리터럴로 구성
    json j = {
        {"name",    "Charlie"},
        {"age",     29},
        {"active",  true},
        {"scores",  {95, 82, 77}},
        {"address", {
            {"city",    "Seoul"},
            {"zipcode", "04524"}
        }}
    };

    std::cout << j.dump(2) << "\n\n";

    // unordered_map에서 JSON으로 역변환
    std::unordered_map<std::string, std::string> props = {
        {"framework", "nlohmann/json"},
        {"version",   "3.11.3"},
        {"license",   "MIT"}
    };
    json from_map(props);
    std::cout << "[map → JSON]\n" << from_map.dump(2) << "\n\n";
}

// ──────────────────────────────────────────────
// 4. 안전한 접근 & null 처리 예제
// ──────────────────────────────────────────────
void demo_safe_access(const json& j)
{
    std::cout << "=== [4] 안전한 접근 & null 처리 ===\n";

    // value() : 키 없으면 기본값 반환
    std::string region = j.value("region", "unknown");
    std::cout << "region (missing key) : " << region << "\n";

    // null 체크
    auto& nullable = j["metadata"]["nullable_field"];
    std::cout << "nullable_field is null : " << std::boolalpha << nullable.is_null() << "\n";

    // contains() : 키 존재 여부
    std::cout << "has 'debug'   : " << j.contains("debug")   << "\n";
    std::cout << "has 'missing' : " << j.contains("missing") << "\n";

    // 예외 안전 get_to
    try {
        std::string bad = j.at("server").at("nonexistent").get<std::string>();
    }
    catch (const json::out_of_range& e) {
        std::cout << "[expected exception] " << e.what() << "\n";
    }
    std::cout << "\n";
}

// ──────────────────────────────────────────────
// main
// ──────────────────────────────────────────────
int main()
{
    // data.json 읽기 (실행파일 옆에 있어야 함)
    std::ifstream ifs("data.json");
    if (!ifs.is_open())
    {
        std::cerr << "[Error] data.json 파일을 열 수 없습니다.\n"
                  << "실행 파일과 같은 디렉터리에 data.json이 있는지 확인하세요.\n";
        return 1;
    }

    json root;
    try {
        ifs >> root; // 스트림에서 파싱
    }
    catch (const json::parse_error& e) {
        std::cerr << "[Parse Error] " << e.what() << "\n";
        return 1;
    }

    demo_to_string(root);
    demo_to_map(root);
    demo_build_json();
    demo_safe_access(root);

    std::cout << "=== 완료 ===\n";
    return 0;
}
