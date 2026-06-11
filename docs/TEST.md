# 테스트 개요

> 참고 자료: `[CRA_AI] Day2_2_Test, Verify, Report, PR.pdf` 5-8페이지

---

## 1. Unit Test의 두 가지 목적

Unit Test는 두 가지 목적을 동시에 수행한다.

### Regression (회귀 검증)
기존에 정상 동작하던 기능이 코드 변경 이후에도 동일하게 동작하는지 확인한다.  
리팩토링이나 기능 추가 이후 **의도치 않은 버그(회귀 버그)를 방지**하는 것이 목적이다.

> 비유: 부품에 **보호 코팅**을 바르는 것. 코팅이 손상되는 순간 즉시 비상벨이 울린다.

- 코드가 조금이라도 변경될 때마다 자주 실행해야 한다.
- 테스트 케이스는 "이미 동작함이 확인된 행동"을 명세화(spec)한다.
- 리팩토링 내성(refactoring resistance)이 높아야 한다 → 구현이 바뀌어도 테스트는 통과해야 한다.

### Correctness (정확성 검증)
현재 작성된 코드가 요구사항에 맞게 정확하게 동작하는지 검증한다.  
다양한 입력과 조건에 대해 기대한 결과를 올바르게 반환하는지 확인한다.

> 한계: 개발자 본인이 예상하고 작성한 경우가 많아, 편향(bias)이 개입된다.  
> 테스트 범위가 좁고, 코드 작성자의 사각지대가 그대로 반영될 수 있다.

---

## 2. Safety Testing (안전성 테스트)

Unit Test만으로는 Correctness 효과가 제한적이다.  
개발자 본인의 관점이 아닌 **제3자(혹은 AI)의 시각**에서 시스템을 의도적으로 망가뜨리려는 시도가 필요하다.

### Safety Test의 특징
- 이상한 값 입력: 빈 문자열, 음수 ID, 극단적으로 긴 문자열, 특수문자
- 예외 유발: 손상된 파일, 잘못된 JSON, 존재하지 않는 ID 조작
- 적대적 사용자 가정: 일반 사용자가 아닌 "망가뜨리려는 사용자"의 관점
- AI 활용: 사람의 편향을 보완하여 더 넓고 공격적인 테스트 생성 가능

### 주의사항 (PDF 9페이지)
> Safety/공격적 테스트는 **리팩토링 내성이 낮다**.  
> 구현 세부 사항에 강하게 결합되는 경우가 많아, 코드 변경 시 테스트 자체도 수정이 필요할 수 있다.

- **임시 테스트로 분류**한다.
- 이상적으로는 Git에 포함하지 않는 임시 경로에서 실행한다.
- 본 프로젝트에서는 학습 목적상 `tests/test_safety.cpp`에 포함하되, 운영 환경에서는 별도 관리를 권장한다.

---

## 3. 테스트 파일 구성

| 파일 | 목적 | 분류 |
|------|------|------|
| `tests/test_member_service.cpp` | 각 service 함수의 Correctness 검증 | Correctness |
| `tests/test_member_repository.cpp` | Repository 파일 I/O Correctness 검증 | Correctness |
| `tests/test_regression.cpp` | CRUD 전체 흐름 Regression 보호 | **Regression** |
| `tests/test_safety.cpp` | 비정상 입력·경계값·적대적 시나리오 | **Safety (임시)** |

---

## 4. Regression 테스트 케이스

CRUD 전체 흐름이 코드 변경 이후에도 동일하게 동작하는지 보호한다.

| # | 테스트명 | 검증 내용 |
|---|----------|-----------|
| R-01 | `FullCrudFlow` | 추가→조회→수정→삭제 순서로 전체 흐름 검증 |
| R-02 | `MultipleAddAndFind` | 여러 회원 추가 후 각 ID로 정확히 조회됨 |
| R-03 | `NextIdAfterDelete` | 삭제 후 next_id는 기존 최대 ID+1을 유지 |
| R-04 | `SaveLoadPreservesAllFields` | save_all → load_all 시 모든 필드가 손실 없이 복원 |
| R-05 | `DeletedIdNotFound` | 삭제된 ID는 find_by_id에서 nullopt 반환 |
| R-06 | `UpdatePreservesOtherMembers` | 한 명 수정 시 나머지 회원 데이터 영향 없음 |

---

## 5. Safety 테스트 케이스

비정상 입력, 경계값, 적대적 시나리오를 시도하여 시스템 안정성을 확인한다.

| # | 테스트명 | 시나리오 |
|---|----------|----------|
| S-01 | `EmptyNameField` | 이름이 빈 문자열인 회원 추가 |
| S-02 | `EmptyAllFields` | 모든 문자열 필드가 빈 값인 회원 추가·저장·복원 |
| S-03 | `JsonSpecialCharsInName` | 이름에 큰따옴표·백슬래시·개행 포함 |
| S-04 | `UnicodeKoreanName` | 이름이 한글인 회원 추가·저장·복원 |
| S-05 | `VeryLongStringFields` | 1000자 이상의 이름·이메일 처리 |
| S-06 | `ExtremeIdValues` | ID=0, ID=INT_MAX 처리 |
| S-07 | `DuplicateIdAdd` | 동일 ID로 두 번 추가 후 find_by_id 동작 확인 |
| S-08 | `DeleteSameIdTwice` | 동일 ID 두 번 삭제 시 두 번째는 false |
| S-09 | `DeleteFromEmpty` | 빈 목록에서 삭제 시도 |
| S-10 | `CorruptedJsonFile` | 손상된 JSON 파일 load 시 빈 목록 반환 |
| S-11 | `UpdateNonExistentMember` | 존재하지 않는 ID로 수정 시 목록 불변 |
| S-12 | `NullCharInName` | 이름에 null 문자(`\0`) 포함 처리 |
