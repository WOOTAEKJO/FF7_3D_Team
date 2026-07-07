# DirectX11 3D Action Game Team Project

## 1. 프로젝트 소개

DirectX11 기반으로 제작한 3D 액션 게임 팀 프로젝트입니다.

FF7 Remake 스타일의 실시간 전투를 목표로 보스전, 캐릭터 액션, 전투 이펙트, 후처리 효과 등을 구현했습니다.

저는 이 프로젝트에서 **이펙트 시스템 전반과 이펙트 제작 툴 개발**을 담당했습니다.

주요 담당 범위는 Particle Effect, Trail Effect, Mesh Effect, Effect Tool, Effect Group, Effect Manager이며, 보스 파트에서는 기존 State / Bullet 구조를 활용하여 보스 패턴과 전투 이펙트를 연동했습니다.

이 프로젝트의 핵심은 단순히 개별 이펙트를 제작한 것이 아니라,

**이펙트 제작 툴 → 개별 이펙트 시스템 → 그룹 이펙트 → 런타임 로드 및 생성 관리**로 이어지는 이펙트 제작 파이프라인을 구성한 점입니다.

---

## 2. 개발 정보

| 항목 | 내용 |
| --- | --- |
| 프로젝트명 | DirectX11 기반 3D 팀 프로젝트 |
| 개발 형태 | 팀 프로젝트 |
| 개발 기간 | 2024/02 ~ 2024/04 |
| 개발 인원 | 5명 |
| 담당 역할 | 이펙트 시스템, 이펙트 툴, 일부 보스 패턴 및 이펙트 연동 |
| 사용 언어 | C++ |
| Graphics API | DirectX11 |
| Shader | HLSL |
| Tool/UI | ImGui, ImGuizmo |
| Data | JSON |
| IDE | Visual Studio |
| Version Control | GitHub, Sourcetree |

---

## 3. 담당 역할

이 프로젝트에서 저는 전투 연출에 사용되는 이펙트 시스템 전반을 담당했습니다.

### 주요 구현 범위

- ImGui / JSON 기반 Effect Tool 구현
- Instancing Buffer 기반 Particle Effect 구현
- Dynamic Vertex Buffer 기반 Trail Effect 구현
- 모델 리소스 기반 Mesh Effect 구현
- 여러 이펙트를 시간차로 재생하는 Effect Group 구현
- JSON 이펙트 데이터를 로드하고 Object Pool과 연동하는 Effect Manager 구현
- HLSL 기반 이펙트 셰이더 구현
- 보스 패턴 일부 구현 및 보스 전투 상황에 맞는 이펙트 연동

---

## 4. 시스템 구조

```
Effect Tool
 ├─ Particle Edit
 ├─ Mesh Edit
 ├─ Trail Edit
 └─ Group Effect Edit

Effect Data
 └─ JSON Save / Load

Runtime Effect System
 ├─ Particle Effect
 ├─ Mesh Effect
 ├─ Trail Effect
 ├─ Effect Group
 └─ Effect Manager

Rendering
 ├─ Instancing Buffer
 ├─ Dynamic Vertex Buffer
 ├─ HLSL Shader Pass
 └─ Blend Effect Render Group
```

이펙트 툴에서 제작한 데이터는 JSON으로 저장되고, 런타임에서는 Effect Manager가 해당 데이터를 로드하여 프로토타입과 Object Pool에 등록합니다.

이후 게임 로직에서는 Effect Manager를 통해 상황에 맞는 이펙트를 생성하고, Effect Group은 여러 개별 이펙트를 시간차로 재생하여 복합적인 전투 연출을 구성합니다.

---

## 5. 핵심 구현

## 5.1 ImGui / JSON 기반 Effect Tool

### 개요

파티클, 메쉬, 트레일 이펙트를 제작하기 위한 ImGui 기반 Effect Tool을 구현했습니다.

툴에서는 이펙트 타입별 편집 탭을 제공하여 텍스처, 모델, 셰이더 패스, Transform, 수명, 색상, 이동 방식, Dissolve, Bone Socket 등의 값을 조정할 수 있도록 구성했습니다.

또한 ImGuizmo를 연동하여 이펙트의 위치, 회전, 크기를 화면에서 직접 조작할 수 있게 했고, JSON 저장/로드를 통해 툴에서 제작한 이펙트 데이터를 런타임에서도 재사용할 수 있도록 만들었습니다.

### 문제

이펙트는 위치, 크기, 회전, 색상, 텍스처, 수명, 셰이더 패스, 이동 방식 등 조정해야 할 값이 많습니다.

이 값들을 코드에서 직접 수정하고 다시 빌드하는 방식은 반복 작업이 많고, 원하는 연출을 빠르게 확인하기 어렵습니다.

### 해결

ImGui 기반 Effect Tool을 구현하여 이펙트 값을 툴에서 직접 수정하고, 생성된 이펙트 객체에 실시간으로 반영되도록 구성했습니다.

또한 JSON 저장/로드 구조를 만들어 툴에서 제작한 데이터를 파일로 저장하고 런타임에서 재사용할 수 있도록 했습니다.

### 주요 기능

- Particle / Mesh / Trail Effect 타입별 편집 탭
- Diffuse / Mask / Noise / Dissolve Texture 선택
- Effect Model 선택
- 텍스처 미리보기
- 리소스 검색 기능
- ImGuizmo 기반 Transform 편집
- JSON 저장 / 로드
- Group Effect 편집
- 개별 이펙트 시작 시간 설정
- Object Pool 개수 설정
- 기존 Group Effect 내 이펙트 수정

### 결과

코드 수정 없이 이펙트를 제작, 수정, 저장, 로드할 수 있는 데이터 기반 제작 환경을 만들었습니다.

이를 통해 개별 이펙트 제작뿐 아니라 여러 이펙트를 조합한 Group Effect까지 툴에서 구성할 수 있었습니다.

### 자료 위치

- 시연 이미지: 
- 시연 영상: 
- 관련 코드:
    - `CImgui_Window_EffectEdit`
    - `CImgui_Tab_ParticleEdit`
    - `CImgui_Tab_TrailEdit`
    - `CImgui_Tab_TrailBufferEdit`
    - `CImgui_Window_EffectGroup`

---

## 5.2 Instancing Buffer 기반 Particle Effect

### 개요

다수의 파티클을 렌더링하기 위한 Instancing Buffer 기반 Particle Effect 시스템을 구현했습니다.

파티클 이펙트는 `CParticle`에서 생명주기와 렌더링 흐름을 관리하고, 실제 파티클 인스턴스 데이터는 `CVIBuffer_Instancing`에서 관리하도록 역할을 분리했습니다.

각 파티클 인스턴스는 위치, 크기, 회전, 색상, 수명, 이동 방향, 스프라이트 UV 등의 데이터를 가지고 있으며, 매 프레임 갱신된 인스턴스 데이터를 GPU 버퍼에 반영한 뒤 Instancing 방식으로 렌더링했습니다.

### 문제

전투 중에는 폭발, 먼지, 불꽃, 마법 효과처럼 짧은 시간 동안 다수의 파티클이 동시에 발생합니다.

이를 각각 개별 오브젝트로 관리하면 객체 수가 많아지고 렌더링 흐름이 복잡해질 수 있습니다.

### 해결

여러 파티클을 하나의 파티클 이펙트 객체 안에서 관리하고, 각 파티클의 상태를 인스턴스 데이터로 갱신하는 구조를 구현했습니다.

위치, 크기, 회전, 색상, 수명, 스프라이트, 마스크 인덱스 등을 per-instance 데이터로 관리하고, `DrawIndexedInstanced`를 통해 렌더링했습니다.

또한 Geometry Shader를 활용하여 Point 형태의 파티클을 Billboard Quad로 확장하고, Direction Billboard, Fire, Distortion 등 여러 셰이더 패스를 지원했습니다.

### 주요 기능

- Instancing Buffer 기반 파티클 렌더링
- per-instance 위치, 크기, 회전, 색상, 수명 관리
- Billboard / Direction Billboard 지원
- Fire / Distortion 파티클 셰이더 패스
- Sprite Animation
- Mask Array
- 다양한 이동 타입
- 다양한 종료 조건
- JSON 저장 / 로드
- Effect Tool 연동

### 결과

다수의 파티클을 하나의 렌더링 흐름에서 처리할 수 있었고, 파티클별 상태를 개별적으로 제어할 수 있었습니다.

이를 통해 보스전, 스킬, 폭발, 불꽃, 왜곡형 이펙트 등 다양한 전투 연출을 제작할 수 있었습니다.

### 자료 위치

- 시연 이미지: 
- 시연 영상: 
- 관련 코드:
    - `CParticle`
    - `CVIBuffer_Instancing`
    - `CVIBuffer_Particle_Point`
    - `Shader_Effect_Particle`

---

## 5.3 Dynamic Vertex Buffer 기반 Trail Effect

### 개요

캐릭터 공격, 무기 궤적, 스킬 연출 등에 사용할 수 있는 Trail Effect 시스템을 구현했습니다.

Trail Effect는 움직이는 오너 객체의 위치를 매 프레임 추적하여, 이전 위치와 현재 위치를 연결한 동적 메시 형태의 궤적을 생성하는 방식으로 구현했습니다.

`CTrail_Buffer`는 트레일 이펙트 객체의 생명주기와 오너 행렬 계산, 렌더링 흐름을 담당하고, `CVIBuffer_Trail`은 실제 Dynamic Vertex Buffer를 갱신하여 트레일 메시를 구성하도록 분리했습니다.

### 문제

무기 궤적이나 스킬 잔상은 단순 파티클과 다르게, 움직이는 오브젝트의 이전 위치와 현재 위치를 이어서 띠 형태의 메시를 만들어야 했습니다.

또한 빠르게 움직이는 오브젝트를 단순 직선으로 연결하면 트레일이 각져 보일 수 있습니다.

### 해결

Dynamic Vertex Buffer를 사용하여 매 프레임 오너 기준의 트레일 시작점과 끝점을 추가하고, 최대 정점 수를 초과하면 오래된 정점을 제거하도록 구성했습니다.

또한 Catmull-Rom 보간을 적용하여 빠른 움직임에서도 부드러운 궤적이 이어지도록 처리했습니다.

Bone Socket을 지원하여 무기나 캐릭터의 특정 본 위치를 기준으로 트레일이 생성되도록 구성했습니다.

### 주요 기능

- Dynamic Vertex Buffer 기반 Trail Mesh 갱신
- 오너 이동 추적
- Trail 시작점 / 끝점 설정
- 최대 정점 수 기반 길이 제한
- Catmull-Rom 보간
- Bone Socket 기반 위치 계산
- UV 좌표 실시간 갱신
- Diffuse / Mask / Noise / Distortion 셰이더 패스
- JSON 저장 / 로드
- Effect Tool 연동

### 결과

캐릭터 무기, 파츠, 이펙트 오브젝트에 자연스러운 궤적 이펙트를 붙일 수 있었습니다.

Bone Socket과 오너 타입별 행렬 계산을 지원하여 일반 오브젝트뿐 아니라 캐릭터 파츠나 무기 본에 붙는 트레일도 처리할 수 있었습니다.

### 자료 위치

- 시연 이미지: 
- 시연 영상: 
- 관련 코드:
    - `CTrail_Buffer`
    - `CVIBuffer_Trail`
    - `Shader_Effect_Trail`

---

## 6. 보조 구현

## 6.1 Model 기반 Mesh Effect

3D 모델 리소스를 활용한 Mesh Effect 시스템을 구현했습니다.

모델 태그를 기반으로 이펙트용 모델을 연결하고, 시간에 따라 위치, 크기, 회전, 색상, UV, Dissolve 값을 갱신하여 전투 연출에 사용할 수 있는 3D 이펙트 객체로 구성했습니다.

Mesh Effect 전용 셰이더에서는 Diffuse / Mask / Noise / Dissolve Texture를 조합하고, Default / Clamp / Distortion / Dissolve / RadialBlur 패스를 분리하여 다양한 표현을 지원했습니다.

또한 Dissolve가 시작될 때 Mesh Vertex 기반 Particle Effect를 함께 생성할 수 있도록 하여 모델 이펙트와 파티클 이펙트를 연동했습니다.

### 관련 코드

- `CTrail_Effect`
- `Shader_Effect_Mesh`
- `CEffect_Manager::Create_EffectMesh_VTX_Particle`

---

## 6.2 Effect Group / Effect Manager

여러 개별 이펙트를 하나의 복합 연출로 구성하기 위한 Effect Group 시스템과, 이펙트 데이터 로드 및 생성을 담당하는 Effect Manager를 구현했습니다.

Effect Group은 파티클, 메쉬, 트레일 이펙트를 그룹으로 묶고, 각 이펙트의 시작 시간을 기준으로 순차적으로 활성화합니다.

또한 그룹의 월드 행렬과 하위 이펙트의 로컬 행렬을 조합하여, 복합 이펙트의 상대 배치를 유지한 채 이동하거나 오너를 따라갈 수 있도록 구성했습니다.

Effect Manager는 JSON으로 저장된 이펙트 데이터를 로드하여 프로토타입과 Object Pool에 등록하고, 게임 로직에서 일반 이펙트, 히트 이펙트, 본 기반 이펙트, 메쉬 정점 기반 파티클 등을 쉽게 생성할 수 있는 인터페이스를 제공합니다.

### 관련 코드

- `CEffect_Group`
- `CEffect_Manager`
- `CObjPool_Manager`

---

## 7. 보스 파트 기여

보스 파트에서는 기존 State / Bullet 구조를 활용하여 일부 보스 패턴을 구성하고, 전투 상황에 맞는 이펙트를 연동했습니다.

보스 구현 자체의 프레임워크를 새로 설계한 것은 아니지만, 기존 구조를 활용해 보스 패턴과 이펙트가 적절한 타이밍에 재생되도록 연결하는 작업을 수행했습니다.

### 주요 기여

- 보스 State 구조 활용
- Bullet 구조 활용
- 보스 공격 패턴 일부 구성
- 보스 공격 타이밍에 맞는 이펙트 연동
- 전투 상황별 Particle / Mesh / Trail Effect 적용

---

## 8. 기술 스택

### Language

- C++
- HLSL

### Graphics

- DirectX11
- Render Target
- Instancing
- Dynamic Vertex Buffer
- Geometry Shader
- Pixel Shader
- Blend State
- Depth Stencil State

### Tool / Data

- ImGui
- ImGuizmo
- JSON
- ImGuiFileDialog

### Architecture / Pattern

- GameObject / Component
- Prototype
- Object Pool
- Manager
- Effect Group
- Data-driven Workflow

---

## 9. 주요 기술 포인트

- DirectX11 기반 실시간 이펙트 시스템 구현
- ImGui / ImGuizmo 기반 인게임 제작 툴 구현
- JSON 기반 이펙트 데이터 저장/로드 구조
- Instancing Buffer 기반 Particle Effect 구현
- Dynamic Vertex Buffer 기반 Trail Effect 구현
- Catmull-Rom 보간을 활용한 부드러운 트레일 생성
- Bone Socket 기반 이펙트 위치 계산
- 모델 기반 Mesh Effect 및 Dissolve / Distortion / RadialBlur 셰이더 패스 구현
- 여러 이펙트를 시작 시간 기준으로 조합하는 Group Effect 구현
- Effect Manager를 통한 데이터 로드, 프로토타입 등록, Object Pool 연동
- HLSL 기반 Billboard, Fire, Distortion, Dissolve, Trail Shader 구현
- 보스 패턴과 전투 이펙트 연동 경험

---

## 10. 시연 자료 구성

### 추천 영상 구성

| 구간 | 내용 |
| --- | --- |

### README에 넣을 자료

- 

---

## 11. 프로젝트를 통해 배운 점

이 프로젝트를 통해 단순히 화면에 보이는 이펙트를 만드는 것보다, 이펙트를 제작하고 재사용할 수 있는 구조를 만드는 것이 중요하다는 점을 배웠습니다.

초기에는 개별 이펙트 구현에 집중했지만, 프로젝트가 진행되면서 이펙트 값을 빠르게 조정하고 저장할 수 있는 툴, 여러 이펙트를 조합할 수 있는 그룹 구조, 런타임에서 데이터를 로드하고 재사용할 수 있는 매니저 구조가 필요하다는 것을 느꼈습니다.

그 결과 파티클, 트레일, 메쉬 이펙트를 각각 구현하는 데서 끝나지 않고, Effect Tool과 Effect Manager를 통해 데이터 기반 이펙트 제작 파이프라인을 구성했습니다.

이 경험을 통해 C++ 객체 관리, DirectX11 렌더링 흐름, HLSL 셰이더, 동적 버퍼 갱신, 인스턴싱, 툴 제작, JSON 데이터화, 오브젝트 풀, 게임 시스템 설계에 대한 이해를 쌓을 수 있었습니다.
