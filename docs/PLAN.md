# Refactoring Plan

## 1. 현재 구조의 문제점

현재 `main.cpp` 단일 파일에 **UI(콘솔 입출력)**, **비즈니스 로직**, **파일 I/O**가 뒤섞여 있다.
이로 인해 아래 문제가 발생한다.

| 문제 | 원인 | 영향 |
|------|------|------|
| 함수 단위 테스트 불가 | `create_member()`가 `std::cin`에서 직접 입력받음 | 테스트 시 실제 콘솔 입력 필요 |
| 파일 I/O 모킹 불가 | `load_members()`/`save_members()`가 파일명을 하드코딩 | 테스트마다 실제 파일 생성/삭제 필요 |
| 관심사 미분리 | `update_member()`가 입력·탐색·수정·저장을 한 함수에서 수행 | 변경 시 영향 범위 파악 어려움 |
| 데이터 표현 고정 | 회원 데이터가 `json` 타입으로만 전달됨 | 타입 안전성 없음, IDE 자동완성 불가 |

---

## 2. 리팩터링 목표

1. **레이어 분리**: 데이터 모델 / 저장소 / 서비스 / UI를 각각 독립적인 파일로 분리
2. **의존성 주입**: 파일 경로를 외부에서 주입받아 테스트 시 임시 파일로 교체 가능하게
3. **순수 함수 분리**: 콘솔 I/O와 비즈니스 로직을 분리해 로직 함수만 단독 테스트 가능하게
4. **타입 안전성**: `json` 대신 `Member` 구조체로 데이터를 전달

---

## 3. 목표 파일 구조

```
json_library_poc/
├── json_library_poc/
│   ├── include/
│   │   └── nlohmann/json.hpp
│   ├── src/
│   │   ├── member.h            # Member 구조체 + 직렬화 정의
│   │   ├── member_repository.h # MemberRepository 인터페이스
│   │   ├── member_repository.cpp
│   │   ├── member_service.h    # 비즈니스 로직 (순수 함수)
│   │   ├── member_service.cpp
│   │   └── member_view.h       # 콘솔 UI 함수
│   └── main.cpp                # 진입점 (의존성 조립 + 메뉴 루프)
└── tests/
    ├── test_member_service.cpp  # 서비스 레이어 단위 테스트
    └── test_member_repository.cpp
```

---

## 4. 레이어별 책임

### 4-a. `member.h` — 데이터 모델

```cpp
struct Member {
    int         id;
    std::string name;
    std::string email;
    std::string phone;
    std::string created_at;
};

// nlohmann/json 자동 직렬화
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Member, id, name, email, phone, created_at)
```

- `json` 타입 대신 `Member` 구조체로 데이터를 전달해 타입 안전성 확보
- 직렬화/역직렬화는 `NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE` 매크로로 자동화

### 4-b. `member_repository.h/.cpp` — 파일 I/O

```cpp
class MemberRepository {
public:
    explicit MemberRepository(std::string path);  // 파일 경로 주입

    std::vector<Member> load_all() const;
    void save_all(const std::vector<Member>& members) const;
};
```

- 파일 경로를 생성자에서 주입받아 테스트 시 임시 파일 경로로 교체 가능
- `json` 파싱/직렬화만 담당, 비즈니스 로직 없음

### 4-c. `member_service.h/.cpp` — 비즈니스 로직

```cpp
// 모두 순수 함수 — 콘솔 I/O, 파일 I/O 없음
int  next_id(const std::vector<Member>& members);
Member make_member(int id, std::string name, std::string email,
                   std::string phone, std::string created_at);

std::optional<Member>              find_by_id(const std::vector<Member>& members, int id);
std::vector<Member>                add_member(std::vector<Member> members, const Member& m);
std::vector<Member>                update_member(std::vector<Member> members, const Member& updated);
std::pair<std::vector<Member>, bool> delete_member(std::vector<Member> members, int id);
```

- **콘솔 I/O 없음** → `std::cin`/`std::cout` 의존 없이 단독 테스트 가능
- **파일 I/O 없음** → `vector<Member>`를 입력받고 결과를 반환하는 순수 함수
- 반환값으로 성공 여부를 표현해 예외 대신 `optional`/`pair` 사용

### 4-d. `member_view.h` — 콘솔 UI

```cpp
// 출력
void print_member(const Member& m);
void print_all_members(const std::vector<Member>& members);

// 입력 (콘솔에서 값을 받아 반환)
int         input_id();
std::string input_name();
std::string input_email();
std::string input_phone();
bool        confirm_delete();
```

- UI 관련 `std::cin`/`std::cout` 코드를 전부 이쪽으로 격리
- 서비스 함수를 호출하지 않음 — 단방향 의존 (View → Service 호출은 `main.cpp`에서만)

### 4-e. `main.cpp` — 조립 및 메뉴 루프

```cpp
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    MemberRepository repo("members.json");  // 경로 주입
    auto members = repo.load_all();

    while (true) {
        // 메뉴 출력 → 선택 → view로 입력받기 → service 호출 → repo 저장
    }
}
```

- 의존성을 직접 생성해 주입하는 유일한 지점
- 메뉴 루프만 담당, 비즈니스 로직 없음

---

## 5. 의존성 방향

```
main.cpp
  ├── MemberRepository  (파일 경로 주입)
  ├── MemberService     (순수 함수, 의존성 없음)
  └── MemberView        (콘솔 I/O만)

MemberRepository  →  Member (직렬화)
MemberService     →  Member (데이터 처리)
MemberView        →  Member (출력)
```

사이클 없음. 모든 레이어가 `Member`에만 공통으로 의존.

---

## 6. 단위 테스트 시나리오

테스트 프레임워크: **GoogleTest** (vcpkg로 설치)

### `test_member_service.cpp`

| 테스트 케이스 | 검증 내용 |
|--------------|-----------|
| `next_id_empty_list` | 빈 목록에서 next_id → 1 반환 |
| `next_id_with_members` | id가 1, 3, 5인 목록 → 6 반환 |
| `add_member_increases_count` | add 후 size 1 증가 |
| `add_member_assigns_correct_id` | add된 멤버의 id가 next_id와 일치 |
| `find_by_id_existing` | 존재하는 id → `optional`에 값 있음 |
| `find_by_id_missing` | 없는 id → `std::nullopt` |
| `update_member_changes_fields` | 수정 후 해당 필드만 변경됨 |
| `update_member_not_found` | 없는 id 수정 → 목록 변화 없음 |
| `delete_member_removes_entry` | 삭제 후 size 1 감소, `true` 반환 |
| `delete_member_not_found` | 없는 id 삭제 → 목록 변화 없음, `false` 반환 |

### `test_member_repository.cpp`

| 테스트 케이스 | 검증 내용 |
|--------------|-----------|
| `load_nonexistent_file` | 파일 없으면 빈 벡터 반환 |
| `save_and_load_roundtrip` | 저장 후 로드 시 동일 데이터 복원 |
| `load_invalid_json` | 파싱 오류 → 빈 벡터 반환 (예외 전파 없음) |

---

## 7. 마이그레이션 순서

1. `src/member.h` 작성 — `Member` 구조체 및 직렬화 매크로
2. `src/member_repository.h/.cpp` 작성 — 기존 `load_members()` / `save_members()` 이전
3. `src/member_service.h/.cpp` 작성 — 기존 CRUD 함수에서 I/O 분리
4. `src/member_view.h` 작성 — 기존 CRUD 함수에서 콘솔 I/O 분리
5. `main.cpp` 재작성 — 의존성 조립 + 메뉴 루프
6. `tests/` 프로젝트 추가 — GoogleTest 설치 및 테스트 작성
7. 기존 `data.json` POC 데모 코드 정리

---

## 8. vcxproj 변경 사항

- `src/` 폴더 내 `.h`/`.cpp` 파일을 프로젝트에 추가
- `tests/` 별도 프로젝트로 솔루션에 추가
- GoogleTest를 vcpkg로 설치: `vcpkg install gtest`
- 테스트 프로젝트에서 `src/` 헤더 경로 참조
