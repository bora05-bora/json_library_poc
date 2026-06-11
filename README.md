# json_library_poc

C++20 + nlohmann/json을 사용해 회원 정보를 JSON 파일로 관리하는 CRUD 콘솔 애플리케이션입니다.

## 개발 환경

| 항목 | 내용 |
|------|------|
| IDE | Visual Studio 2022 |
| 언어 표준 | C++20 |
| JSON 라이브러리 | [nlohmann/json](https://github.com/nlohmann/json) v3.11.3 (헤더 온리) |
| 플랫폼 | Windows 10/11 x64 |

## 프로젝트 구조

```
json_library_poc/
├── README.md
├── CLAUDE.md                         # 개발 가이드
├── json_library_poc.slnx             # Visual Studio 솔루션
└── json_library_poc/
    ├── include/
    │   └── nlohmann/
    │       └── json.hpp              # nlohmann/json v3.11.3
    ├── main.cpp                      # 진입점 및 CRUD 구현 (단일 파일)
    ├── members.json                  # 회원 데이터 파일
    ├── data.json                     # POC 샘플 데이터
    └── json_library_poc.vcxproj
```

## 빌드 및 실행

1. `json_library_poc.slnx`를 Visual Studio 2022로 열기
2. **빌드** (Ctrl+Shift+B)
3. **실행** (Ctrl+F5)

> 빌드 후 `members.json`이 출력 디렉터리로 자동 복사됩니다.

## 기능

실행하면 아래 메뉴가 표시됩니다.

```
==============================
      회원 관리 시스템
==============================
1. 전체 조회
2. 단건 조회
3. 회원 추가
4. 회원 수정
5. 회원 삭제
0. 종료
```

| 기능 | 설명 |
|------|------|
| 전체 조회 | 등록된 모든 회원 목록 출력 |
| 단건 조회 | ID로 특정 회원 정보 조회 |
| 회원 추가 | 이름·이메일·전화번호 입력 → ID 자동 채번, 가입일 자동 기록 |
| 회원 수정 | ID로 검색 후 항목별 수정 (Enter 입력 시 기존 값 유지) |
| 회원 삭제 | ID로 검색 후 확인(y/n) 절차를 거쳐 삭제 |

## 데이터 구조

회원 데이터는 `members.json`에 저장됩니다.

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

## 라이선스

nlohmann/json — [MIT License](https://github.com/nlohmann/json/blob/develop/LICENSE.MIT)
