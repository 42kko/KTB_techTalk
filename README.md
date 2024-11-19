# KTB_techTalk

## 설명

C++로 작성된 간단한 디스코드 봇입니다.
`/ping`, `/명언` 두가지 명령어 기능과, 시간마다 `/명언`을 출력하는 봇입니다.

## 기능

- `/ping` : 봇이 살아있는지 확인합니다. ping을 보내면 pong으로 대답합니다.
- `/명언` : API호출을 통해 명언을 가져온뒤 명언을 출력합니다.
- Timer : 정해진 시간에 따라 명언을 출력합니다.


## 필수 요소

Dpp 라이브러리와 Dpp 라이브러리가 의존하는 요소들을 필요로합니다.

## 사용법

Bot token과 channel id를 필요로합니다.

channel id는 디스코드에서 개발자 모드를 켠뒤, 해당 채널을 우클릭하여 채널의 ID를 얻습니다.
