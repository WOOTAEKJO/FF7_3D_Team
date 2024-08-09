# 파이널판타지 7 리메이크 모작

## 1. 프로젝트 설명
+ DirectX11을 활용하여 파이널 판타지 7 리메이크를 모작했습니다.

## 2. 인원 및 파트
+ 인원 : 4명
+ 파트 : 이펙트, 보스

## 3. 개발 기간
+ 2024.02.13 ~ 2024.04.15

## 4. 기술 스택
+ C++
+ DirectX11
+ ImGui

## 5. 구현 설명
- ImGui를 이용해 다양한 이펙트 툴을 제작하고 게임 내 여러 이펙트를 구현
- 이펙트를 파티클, 이펙트 메쉬, 이펙트 트레일로 구분하여 구현
- 오브젝트 풀을 사용해 이펙트의 생성 및 삭제 처리
- 셰이더를 통해 이펙트에 다양한 효과 적용
- Behavior Tree와 상태머신을 이용하여 보스 AI를 구현
