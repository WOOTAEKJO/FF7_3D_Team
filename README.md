# C++ Data-Driven Runtime Tool System

DirectX11 기반 3D 팀 프로젝트에서 구현한
**C++ 데이터 기반 런타임 객체 관리 및 내부 편집 툴 시스템**입니다.

원본 프로젝트는 3D 액션 게임 형태로 제작되었지만, 이 Repository에서는 게임 연출 결과보다
**툴에서 데이터를 제작하고 저장한 뒤, 런타임에서 이를 로드하여 객체를 생성·실행·재사용하는 구조**에 초점을 맞췄습니다.

> **Internal Tool → JSON Data → Runtime Load → Object Creation → Execution → Object Reuse**

주요 구현 및 연동 요소는 다음과 같습니다.

* ImGui 기반 내부 편집 툴 구현
* JSON 기반 데이터 저장 / 로드
* 런타임 객체 생성 및 생명주기 관리
* 팀 공용 Prototype 구조와 런타임 객체 생성 과정 연동
* 팀 공용 Object Pool 구조와 반복 객체 재사용 과정 연동
* Manager를 통한 데이터 로드 및 객체 생성 진입점 구성
* Group 기반 복수 객체 순차 실행
* 실시간 상태 및 GPU Buffer 데이터 갱신

---

## 1. 개발 정보

| 항목              | 내용                                                                           |
| --------------- | ---------------------------------------------------------------------------- |
| 프로젝트 형태         | 팀 프로젝트                                                                       |
| 원본 프로젝트         | DirectX11 기반 3D 액션 게임                                                        |
| 담당 영역           | 내부 편집 툴, 데이터 저장/로드, Effect Manager, 런타임 객체, Group 실행 구조, 공용 객체 생성/재사용 시스템 연동 |
| Language        | C++                                                                          |
| UI / Tool       | ImGui, ImGuizmo                                                              |
| Data            | JSON                                                                         |
| Graphics API    | DirectX11                                                                    |
| Shader          | HLSL                                                                         |
| IDE             | Visual Studio                                                                |
| Version Control | GitHub, Sourcetree                                                           |

### 담당 역할

* ImGui 기반 내부 편집 툴 구현
* 객체 데이터 JSON 저장 / 로드 구조 구현
* Effect 데이터 로드 및 런타임 객체 생성 Manager 구현
* 팀 공용 Prototype 시스템과 Effect 객체 생성 과정 연동
* 팀 공용 Object Pool 시스템과 Effect 객체 재사용 과정 연동
* Group 기반 복수 객체 실행 구조 구현
* 객체별 Transform, 상태, 수명, 실행 여부 관리
* 런타임 데이터 실시간 갱신
* Instancing / Dynamic Vertex Buffer 기반 데이터 처리

> **Prototype과 Object Pool의 기반 시스템 자체는 팀 공용 구조를 사용했으며,
> Effect 시스템에서 해당 구조를 활용할 수 있도록 데이터 로드 및 객체 생성·재사용 과정을 연동했습니다.**

---

## 2. 시스템 구조

```text
┌─────────────────────────────┐
│        Internal Tool        │
│                             │
│  Object Edit UI             │
│  Resource Search / Preview  │
│  Transform Gizmo            │
│  Runtime Preview            │
│  Group Editor               │
└──────────────┬──────────────┘
               │ Save / Load
               ▼
┌─────────────────────────────┐
│          Data Layer         │
│                             │
│  JSON Object Data           │
│  JSON Group Data            │
│  Resource Path Data         │
└──────────────┬──────────────┘
               │ Load
               ▼
┌─────────────────────────────┐
│      Effect Runtime Layer   │
│                             │
│  Effect Manager             │ ← Implemented
│  Effect Group               │ ← Implemented
│  Runtime Effect Objects     │ ← Implemented
│                             │
│  Prototype System           │ ← Team Framework / Integrated
│  Object Pool                │ ← Team Framework / Integrated
└─────────────────────────────┘
```

### 실행 흐름

1. 내부 편집 툴에서 객체 속성을 생성하거나 수정합니다.
2. 편집한 데이터를 JSON 파일로 저장합니다.
3. 런타임 초기화 과정에서 Effect Manager가 JSON 데이터를 로드합니다.
4. 로드한 데이터를 기반으로 팀 공용 Prototype 시스템에 Effect 객체를 등록합니다.
5. 반복 생성이 필요한 객체는 설정된 개수를 기준으로 팀 공용 Object Pool 시스템과 연결합니다.
6. 런타임에서는 Effect Manager를 통해 필요한 객체를 요청합니다.
7. 생성된 객체는 상태, 수명, Transform, 실행 조건에 따라 갱신됩니다.
8. 복수 객체를 조합한 경우 Group이 각 객체의 시작 시간을 기준으로 실행 순서를 제어합니다.
9. 실행이 종료된 객체는 프로젝트의 공용 객체 관리 구조를 통해 재사용됩니다.

---

## 3. 핵심 구현

### 3.1 ImGui 기반 내부 편집 툴

#### 문제

객체의 위치, 회전, 크기, 색상, 수명, 이동 방식, 리소스 등의 값을 코드에 직접 작성하면 수정할 때마다 코드 변경과 재실행이 필요했습니다.

또한 객체 종류와 설정값이 늘어날수록 반복적인 데이터 수정과 관리 비용이 증가했습니다.

#### 구현

ImGui 기반 내부 편집 툴을 만들어 런타임 객체의 데이터를 UI에서 직접 생성하고 수정할 수 있도록 했습니다.

주요 기능은 다음과 같습니다.

* 객체 생성 / 삭제
* 객체 데이터 저장 / 로드
* 텍스처 및 모델 리소스 선택
* 리소스 목록 자동 로드
* 리소스 검색 및 미리보기
* Transform 편집
* ImGuizmo 기반 위치 / 회전 / 크기 조작
* 변경 데이터 실시간 Preview
* 여러 객체를 묶는 Group 편집
* Group 내부 객체별 시작 시간 설정
* 저장된 Group 재로드 및 수정

#### 결과

코드를 직접 수정하지 않고 객체 데이터를 편집하고 결과를 확인할 수 있게 되었으며,
툴에서 제작한 데이터를 런타임에서 다시 사용할 수 있는 제작 흐름을 구성했습니다.

---

### 3.2 JSON 기반 데이터 저장 / 로드

#### 문제

객체 설정값이 코드에 직접 포함되어 있으면 값을 변경할 때마다 소스를 수정해야 하며, 여러 객체의 상태를 일관된 형태로 저장하고 복원하기 어렵습니다.

특히 여러 객체를 하나의 Group으로 구성할 경우 객체의 타입, 리소스, Transform, 실행 시간 등의 정보를 함께 관리할 필요가 있었습니다.

#### 구현

객체별 데이터를 JSON으로 직렬화하고 다시 역직렬화할 수 있도록 `Write / Load` 구조를 구현했습니다.

저장 데이터는 객체 타입에 따라 다르지만 주요 데이터는 다음과 같습니다.

* 객체 타입
* 리소스 이름 및 경로
* Transform
* 수명 및 실행 조건
* 색상
* 속도 및 상태 변화 값
* 렌더링 처리 타입
* Group 내부 시작 시간
* Object Pool에 준비할 객체 개수

단일 객체 데이터와 Group 데이터를 분리했으며, 런타임에서는 개별 객체 데이터를 먼저 로드한 뒤 Group 데이터를 구성하도록 처리했습니다.

#### 결과

객체 설정을 실행 코드에서 데이터로 분리하여
**툴에서 제작한 데이터를 저장하고 런타임에서 동일한 데이터로 객체를 구성하는 흐름**을 만들었습니다.

---

### 3.3 Effect Manager와 공용 객체 생성 시스템 연동

#### 문제

Effect 객체 종류와 데이터 파일이 늘어나면서 각 사용 위치에서 데이터 로드, 객체 타입 판별, 객체 생성 과정을 직접 처리하면 생성 로직이 여러 곳으로 분산되는 문제가 있었습니다.

또한 반복적으로 생성되는 객체를 프로젝트의 공용 Object Pool 구조와 연결할 필요가 있었습니다.

#### 구현

Effect Manager가 Effect 데이터 디렉터리를 탐색하고 JSON 파일을 로드하도록 구성했습니다.

로드된 데이터의 객체 타입을 판별한 뒤 프로젝트에서 사용하던 **공용 Prototype 시스템에 Effect 객체를 등록**하도록 연결했습니다.

반복 생성되는 객체는 JSON에 저장된 Pool 개수를 읽어 **팀 공용 Object Pool 시스템에 필요한 객체를 준비하도록 연동**했습니다.

```text
JSON Effect Data
       ↓
 Effect Manager
       ↓
Object Type Check
       ↓
Team Prototype System
       ↓
Team Object Pool System
       ↓
Runtime Effect Object
```

런타임 코드에서는 구체적인 데이터 로드나 Prototype 등록 과정을 직접 처리하지 않고 Effect Manager를 통해 객체를 사용할 수 있도록 구성했습니다.

#### 역할 구분

* **Effect Manager 및 Effect 데이터 로드 흐름:** 직접 구현
* **Effect 객체와 Prototype 시스템 연결:** 담당
* **Effect 객체와 Object Pool 시스템 연결:** 담당
* **Prototype 기반 시스템 자체:** 팀 공용 시스템 사용
* **Object Pool 기반 시스템 자체:** 팀 공용 시스템 사용

#### 결과

Effect 객체의 데이터 로드와 생성 과정을 Manager를 중심으로 구성했고,
기존 팀 공용 객체 생성·재사용 시스템을 Effect 런타임에 연결하여 사용할 수 있도록 했습니다.

---

### 3.4 Group 기반 순차 실행 시스템

#### 문제

여러 객체로 하나의 복합 동작을 구성할 때 각 객체를 코드에서 개별 생성하고 실행 시간을 직접 관리하면 실행 순서와 상대 위치 관리가 복잡해집니다.

또한 한 번 구성한 복합 동작을 하나의 데이터 단위로 저장하고 다시 사용할 필요가 있었습니다.

#### 구현

여러 객체를 하나의 Group으로 묶고 각 객체의 `Start Time`을 데이터로 저장했습니다.

Group은 매 프레임 누적 시간과 각 객체의 시작 시간을 비교하여 실행 시점이 된 객체를 활성화합니다.

각 Group Object는 다음 데이터를 관리합니다.

* 객체 참조
* 객체 타입
* 데이터 경로 및 이름
* 시작 시간
* Local Transform
* 실행 종료 여부

Group Transform과 하위 객체의 Local Transform을 조합하여 Group 전체 위치가 변경되더라도 내부 객체의 상대적인 배치가 유지되도록 구성했습니다.

#### 결과

여러 객체의 **배치와 실행 순서를 하나의 데이터 단위로 저장하고 재사용**할 수 있게 되었습니다.

툴에서 Group을 생성하고 JSON으로 저장한 뒤 런타임에서 동일한 구성으로 실행할 수 있습니다.

---

### 3.5 실시간 상태 및 Buffer 데이터 갱신

런타임 Effect 객체는 매 프레임 위치, 크기, 회전, 색상, 수명 및 실행 상태 등의 데이터를 갱신합니다.

일부 Effect는 다수의 데이터를 한 번에 처리하거나 실시간으로 형상이 변화하기 때문에 DirectX11 Buffer를 직접 갱신하도록 구현했습니다.

#### Instancing Buffer

다수의 Particle 상태를 각각 별도 객체로 관리하는 대신, 하나의 시스템에서 `per-instance` 데이터를 관리하도록 구성했습니다.

각 Instance는 다음과 같은 데이터를 가집니다.

* 위치
* 크기
* 회전
* 색상
* 수명
* 이동 방향
* Index 정보

Instance 데이터를 GPU Buffer에 갱신하고 일괄 처리하도록 구성했습니다.

#### Dynamic Vertex Buffer

움직이는 객체를 따라 실시간으로 변화하는 Trail 데이터를 처리하기 위해 정점 데이터를 매 프레임 갱신했습니다.

새로운 정점을 추가하고 오래된 정점을 제거하며, 필요한 구간에는 보간을 적용하여 연속적인 Trail 데이터를 생성하도록 구성했습니다.

---

## 4. Source Code

> 이 Repository의 `src` 디렉터리는 전체 팀 프로젝트 소스가 아니라
> **제가 직접 구현했거나 Effect 시스템에서 연동을 담당한 부분의 구조를 확인할 수 있도록 선별한 코드**입니다.

원본 프로젝트의 공통 `Engine`, `GameObject`, `Resource`, `Prototype`, `Object Pool` 시스템 등에 의존하는 코드가 포함되어 있기 때문에 **이 Repository 단독으로는 빌드되지 않습니다.**

Prototype과 Object Pool의 기반 구현은 팀 공용 시스템이므로 Repository에 포함하지 않았으며,
해당 시스템과 Effect Manager 및 Runtime Object를 연결하는 코드를 중심으로 포함했습니다.

```text
src/
├── Tool/       # ImGui 기반 데이터 편집 툴
├── Runtime/    # Effect Object / Manager / Group
├── Buffer/     # Instancing / Dynamic Buffer
└── Shader/     # Effect Rendering Shader
```

### Internal Tool

* [`Imgui_Window_EffectEdit`](src/Tool/Imgui_Window_EffectEdit.cpp)

  * Effect 편집 Window 및 전체 편집 흐름 관리

* [`Imgui_Tab_EffectTabBase`](src/Tool/Imgui_Tab_EffectTabBase.cpp)

  * Effect 타입별 공통 편집 인터페이스

* [`Imgui_Tab_ParticleEdit`](src/Tool/Imgui_Tab_ParticleEdit.cpp)

  * Particle 데이터 편집

* [`Imgui_Window_EffectGroup`](src/Tool/Imgui_Window_EffectGroup.cpp)

  * Group 구성 및 객체별 실행 시간 편집

### Runtime Object System

* [`Effect_Manager`](src/Runtime/Effect_Manager.cpp)

  * Effect 데이터 파일 탐색 및 로드
  * 객체 타입 판별
  * 공용 Prototype / Object Pool 시스템 연동
  * Runtime Effect Object 생성 흐름 관리

* [`Effect_Group`](src/Runtime/Effect_Group.cpp)

  * 복수 객체의 실행 시간 및 상태 관리
  * Group / Local Transform 처리

* [`Particle`](src/Runtime/Particle.cpp)

  * Particle 객체의 런타임 상태 및 데이터 처리

* [`Trail_Effect`](src/Runtime/Trail_Effect.cpp)

  * Trail 객체의 런타임 동작 처리

### Buffer / Runtime Data

* [`VIBuffer_Instancing`](src/Buffer/VIBuffer_Instancing.cpp)

  * Instance 상태 데이터 관리 및 GPU Buffer 갱신

* [`VIBuffer_Particle_Point`](src/Buffer/VIBuffer_Particle_Point.cpp)

  * Particle Instance 데이터 처리

* [`VIBuffer_Trail`](src/Buffer/VIBuffer_Trail.cpp)

  * 실시간 Trail Vertex 생성 및 갱신

### Shader / Rendering Data

* `Shader_Effect_Particle`
* `Shader_Effect_Trail`
* `Shader_Effect_Mesh`

---

## 5. 설계하면서 고민한 부분

### 데이터와 런타임 로직의 분리

객체의 세부 설정을 코드에 직접 작성하기보다 JSON 데이터로 분리하고, 런타임 시스템은 해당 데이터를 읽어 객체를 구성하도록 했습니다.

이를 통해 객체 설정 변경과 실행 로직 변경을 분리할 수 있었습니다.

### 기존 공용 시스템과의 연동

프로젝트에는 이미 Prototype과 Object Pool 같은 공용 객체 관리 구조가 존재했습니다.

동일한 기능을 Effect 시스템 내부에 다시 구현하지 않고, Effect Manager가 데이터를 읽어 기존 공용 시스템에 필요한 정보를 전달하도록 연결했습니다.

이를 통해 팀 프로젝트의 기존 구조를 활용하면서 Effect 시스템의 데이터 흐름을 구성했습니다.

### 객체 생성 진입점의 집중

런타임의 여러 위치에서 Effect 데이터 로드와 객체 생성 과정을 직접 처리하지 않고 Effect Manager를 진입점으로 사용했습니다.

호출부는 구체적인 데이터 로드 및 공용 객체 관리 시스템과의 연동 과정을 알 필요 없이 필요한 Effect를 요청할 수 있도록 구성했습니다.

### 단일 객체와 복합 실행 구조의 분리

개별 Effect 객체 자체의 실행 로직과 여러 객체의 실행 순서를 관리하는 Group 로직을 분리했습니다.

개별 객체는 자신의 상태와 동작을 관리하고, Group은 시간과 배치 관계를 담당하도록 역할을 나눴습니다.

---

## 6. 개선하고 싶은 부분

현재 구조를 다시 설계한다면 다음 부분을 개선하고 싶습니다.

* Effect Manager가 담당하는 책임을 `Loader`, `Factory`, `Pool Registry` 등으로 분리
* JSON Schema 또는 별도 검증 계층을 통한 데이터 유효성 검사 추가
* 잘못된 리소스 경로 및 데이터에 대한 오류 처리 강화
* 객체 타입별 분기 로직을 Factory 구조로 정리
* Tool UI와 데이터 모델의 의존성 분리
* Group 실행 로직을 보다 일반적인 Timeline / Scheduler 구조로 확장
* 런타임 로그 및 디버깅 UI 강화
* 파일 변경 감지를 이용한 Hot Reload 구조 개선

---

## 7. 프로젝트를 통해 배운 점

초기에는 개별 Effect 객체가 정상적으로 동작하도록 구현하는 데 집중했지만, 객체의 종류와 데이터가 늘어나면서 **개별 기능뿐 아니라 데이터를 제작하고 관리하는 구조가 중요하다는 점**을 경험했습니다.

이 과정에서 내부 편집 툴, JSON 데이터 저장/로드, Effect Manager, Group 실행 구조를 구현하고, 프로젝트에서 사용하던 Prototype 및 Object Pool 시스템과 Effect 객체를 연결했습니다.

그 결과 다음과 같은 데이터 기반 실행 흐름을 구성했습니다.

> **Internal Tool → JSON Data → Runtime Load → Object Creation → Group Execution → Object Reuse**

또한 팀 프로젝트에서는 모든 기반 시스템을 직접 구현하는 것뿐 아니라, **기존 공용 시스템의 역할과 인터페이스를 이해하고 자신이 담당한 기능과 올바르게 연동하는 것도 중요한 설계 작업**이라는 점을 경험했습니다.

이를 통해 C++ 객체지향 설계, 데이터 직렬화, 내부 툴 제작, 런타임 객체 관리, 실시간 데이터 갱신뿐 아니라 **기존 시스템과 새로운 기능을 연결하는 과정**을 경험할 수 있었습니다.
