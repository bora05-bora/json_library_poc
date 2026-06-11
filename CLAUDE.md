# CLAUDE.md — json_library_poc

## 프로젝트 개요

nlohmann/json 라이브러리를 사용하는 C++ 프로젝트.
**회원 정보(Member)를 JSON 파일로 영속화하는 CRUD 기능 구현**이 목적이다.

기존 POC 레포지터리를 기반으로 개발한다:
- GitHub: https://github.com/bora05-bora/json_library_poc

---

## 개발 환경

| 항목 | 내용 |
|------|------|
| IDE | Visual Studio 2022 (v145 toolset) |
| 언어 표준 | C++20 (`stdcpp20`) |
| JSON 라이브러리 | nlohmann/json v3.11.3 (헤더 온리) |
| 플랫폼 | Windows 10/11, x64 |
| 빌드 방식 | MSBuild (.vcxproj) |

---

## 프로젝트 구조

```
json_library_poc/
├── CLAUDE.md
├── json_library_poc.slnx
└── json_library_poc/
    ├── include/
    │   └── nlohmann/
    │       └── json.hpp          # nlohmann/json v3.11.3 단일 헤더
    ├── main.cpp                  # 진입점
    ├── data.json                 # 런타임 데이터 파일 (빌드 후 출력 폴더로 복사됨)
    ├── json_library_poc.vcxproj
    └── json_library_poc.vcxproj.filters
```

---

## 빌드 설정 (vcxproj 핵심 항목)

- **PrecompiledHeader**: `NotUsing` — pch.h 없는 단일 파일 구조
- **AdditionalOptions**: `/utf-8` — 소스 파일 인코딩을 UTF-8로 강제
- **PostBuildEvent**: `data.json`을 출력 디렉터리(`$(OutDir)`)로 자동 복사

---

## 한글 출력 주의사항

Windows 콘솔은 기본 코드페이지가 CP949(EUC-KR)이다.
`main()` 진입 직후 반드시 아래 두 줄을 호출해야 한글이 깨지지 않는다.

```cpp
SetConsoleOutputCP(CP_UTF8);
SetConsoleCP(CP_UTF8);
```

---

## POC 코드 구조 (기존)

### 파일 구성

단일 파일(`main.cpp`) 구조. 기능별로 함수를 분리하고 `main()`에서 순서대로 호출한다.

```
main.cpp
├── #include 블록          (iostream, fstream, string, unordered_map, windows.h, nlohmann/json.hpp)
├── using json = nlohmann::json
├── demo_to_string()       함수 정의
├── demo_to_map()          함수 정의
├── demo_build_json()      함수 정의
├── demo_safe_access()     함수 정의
└── main()
    ├── SetConsoleOutputCP(CP_UTF8) / SetConsoleCP(CP_UTF8)
    ├── ifstream으로 data.json 열기
    ├── ifs >> root  (스트림 파싱)
    ├── demo_to_string(root)
    ├── demo_to_map(root)
    ├── demo_build_json()
    └── demo_safe_access(root)
```

### 각 함수의 코드 패턴

**`demo_to_string`** — JSON에서 타입별 값 꺼내기
```cpp
std::string app_name = j.at("application").get<std::string>();
int    port    = j["server"]["port"].get<int>();
bool   debug   = j["debug"].get<bool>();
double timeout = j["server"]["timeout"].get<double>();

// 배열 순회
for (const auto& tag : j["tags"])
    std::cout << tag.get<std::string>() << "\n";
```

**`demo_to_map`** — JSON object를 `unordered_map`으로 변환
```cpp
// items()로 key-value 순회 → map 삽입
std::unordered_map<std::string, std::string> flat_map;
for (const auto& [key, val] : j.items()) {
    if (val.is_string())         flat_map[key] = val.get<std::string>();
    else if (val.is_number_integer()) flat_map[key] = std::to_string(val.get<int>());
    else if (val.is_boolean())   flat_map[key] = val.get<bool>() ? "true" : "false";
    else                         flat_map[key] = val.dump();
}

// 중첩 object 순회
for (const auto& [k, v] : j["server"].items())
    server_map[k] = v.dump();

// 배열 → unordered_map<int, string>
for (const auto& user : j["users"])
    user_map[user["id"].get<int>()] = user["name"].get<std::string>();
```

**`demo_build_json`** — JSON 직접 구성 및 직렬화
```cpp
// 중괄호 초기화로 JSON 빌드
json j = {
    {"name",   "Charlie"},
    {"age",    29},
    {"scores", {95, 82, 77}},
    {"address", {{"city", "Seoul"}}}
};
std::cout << j.dump(2);   // 들여쓰기 2칸 출력

// unordered_map → JSON
std::unordered_map<std::string, std::string> props = { ... };
json from_map(props);
```

**`demo_safe_access`** — 안전한 접근 패턴
```cpp
// 키 없을 때 기본값
std::string region = j.value("region", "unknown");

// null 체크
j["metadata"]["nullable_field"].is_null();

// 키 존재 여부
j.contains("debug");

// 없는 키 접근 시 예외
try {
    j.at("server").at("nonexistent").get<std::string>();
} catch (const json::out_of_range& e) { ... }
```

### 데이터 흐름

```
data.json (파일)
    │
    ▼  ifstream >> json
json root (메모리)
    │
    ├── .get<T>()           → C++ 기본 타입 (string, int, bool, double)
    ├── .items()            → key-value 반복자 → unordered_map
    └── .dump(n)            → std::string (직렬화)
```

---

## 구현 목표: 회원 CRUD

### 회원 데이터 구조 (예정)

```json
{
  "members": [
    {
      "id": 1,
      "name": "홍길동",
      "email": "hong@example.com",
      "phone": "010-1234-5678",
      "created_at": "2026-06-11"
    }
  ]
}
```

### 구현할 기능

| 기능 | 설명 |
|------|------|
| **Create** | 새 회원 추가, id 자동 채번, JSON 파일에 저장 |
| **Read** | 전체 목록 조회 / id로 단건 조회 |
| **Update** | id로 회원 검색 후 필드 수정, JSON 파일에 반영 |
| **Delete** | id로 회원 검색 후 삭제, JSON 파일에 반영 |

### 설계 방침 (확정)

- **파일 구조**: POC와 동일하게 `main.cpp` 단일 파일. 기능별 함수 분리
- **데이터 파일**: `members.json` 하나로 관리 (런타임 읽기/쓰기)
- **id 채번**: 현재 최대 id + 1로 자동 채번 (`next_id()` 함수)
- **파일 저장**: `dump(4)` (들여쓰기 4칸) 고정
- **한글 입출력**: `SetConsoleOutputCP(CP_UTF8)` + `/utf-8` 컴파일 옵션

### 구현된 함수 목록

```
main.cpp
├── load_members()        members.json 읽기, 없으면 빈 배열 반환
├── save_members()        members.json 쓰기 (dump(4))
├── today_string()        현재 날짜를 "YYYY-MM-DD" 문자열로 반환 (std::chrono)
├── next_id()             members 배열 최대 id + 1 반환
├── print_member()        회원 1명 정보 콘솔 출력
├── create_member()       회원 추가 (C: 입력 → push_back → save)
├── read_all_members()    전체 조회 (R: 배열 순회 출력)
├── read_member()         단건 조회 (R: id로 선형 탐색)
├── update_member()       회원 수정 (U: id 탐색 → 필드 교체 → save)
├── delete_member()       회원 삭제 (D: id 탐색 → erase → save)
└── main()                메뉴 루프 (while + switch)
```

---

## nlohmann/json 주요 API 참고

```cpp
// 파싱
std::ifstream ifs("file.json");
json j; ifs >> j;

// 필드 접근
j["key"].get<std::string>()   // 타입 변환
j.at("key")                   // 없으면 out_of_range 예외
j.value("key", "default")     // 없으면 기본값 반환
j.contains("key")             // 키 존재 여부

// 배열 조작
j["arr"].push_back(item);
j["arr"].erase(iterator);

// 직렬화
j.dump(4)                     // 들여쓰기 4칸 pretty print

// 커스텀 직렬화 (구조체 연동)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Member, id, name, email, phone, created_at)
```
