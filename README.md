# NetworkProtocolTermProject

STM32 기반 네트워크 프로토콜 텀 프로젝트 저장소입니다.  
NUCLEO-F446RE 보드를 사용하여 Layer 2 및 Layer 3 프로토콜 스택을 구현합니다.

## 프로젝트 개요 
본 프로젝트는 임베디드 시스템 기반의 UART 직렬 통신을 통해, Host와 User간 1:1로 상호 작용이 가능한 L3 계층 프로토콜을 구현하는 것을 목표로 한다. 
L3 계층에서는 퀴즈 기반의 인증 과정을 통해 사용자의 유효성을 판단하고, 인증에 성공한 경우에만 채팅 기능을 제공하는 구조이다.
L3 계층은 State 기반 FSM으로 설계되었으며, 주요 State로는 IDLE, WAIT_QUIZ, WAIT_ANSWER, CHAT_READY, TERMINATE 등이 존재한다. 
또한 타이머 기능을 통해 사용자의 입력 시간이 제한되며, 상태 전이와 메시지 교환은 UART 통신을 통해 이루어진다. 본 프로젝트는 mbed 기반의 NUCLEO 보드에서 동작하며, C++로 구현되었다.

---

## 사용 보드: STM32 Nucleo-F446RE
![NUCLEO-F446RE 보드](https://www.bdtronics.com/pub/media/catalog/product/s/t/stm32-nucleo-f446re-stm32f446ret6-development-board.jpg)

| 항목            | 설명                                                                 |
|----------------|----------------------------------------------------------------------|
| **MCU**        | STM32F446RE (ARM Cortex-M4, 180MHz, 512KB Flash, 128KB RAM)          |
| **폼 팩터**     | Arduino Uno 호환 핀 배열 + ST Morpho 확장 핀                                   |
| **디버깅 지원**   | ST-LINK/V2-1 디버거/프로그래머 내장 (USB 하나로 전원 공급 + 디버깅 지원)              |
| **인터페이스**    | USB, GPIO, I2C, SPI, UART, ADC, PWM 등 지원                           |
| **전원 공급**    | USB 또는 외부 5V/3.3V                                                |
| **IDE 호환성**   | STM32CubeIDE, Mbed OS, Keil, IAR 등 다양한 개발 환경 지원                  |

---

# NetworkProtocolTermProject

STM32 기반 네트워크 프로토콜 텀 프로젝트 저장소입니다.  
NUCLEO-F446RE 보드를 사용하여 Layer 2 및 Layer 3 프로토콜 스택을 구현합니다.

---

## 사용 보드: STM32 Nucleo-F446RE
![NUCLEO-F446RE 보드](https://www.bdtronics.com/pub/media/catalog/product/s/t/stm32-nucleo-f446re-stm32f446ret6-development-board.jpg)

| 항목            | 설명                                                                 |
|----------------|----------------------------------------------------------------------|
| **MCU**        | STM32F446RE (ARM Cortex-M4, 180MHz, 512KB Flash, 128KB RAM)          |
| **폼 팩터**     | Arduino Uno 호환 핀 배열 + ST Morpho 확장 핀                                   |
| **디버깅 지원**   | ST-LINK/V2-1 디버거/프로그래머 내장 (USB 하나로 전원 공급 + 디버깅 지원)              |
| **인터페이스**    | USB, GPIO, I2C, SPI, UART, ADC, PWM 등 지원                           |
| **전원 공급**    | USB 또는 외부 5V/3.3V                                                |
| **IDE 호환성**   | STM32CubeIDE, Mbed OS, Keil, IAR 등 다양한 개발 환경 지원                  |

---


# 🛰 네트워크 프로토콜 설계

## 📌 프로젝트 개요
본 프로젝트는 사용자 인증 및 퀴즈 기반 상호작용을 중심으로 한 **네트워크 프로토콜 설계 채팅 시스템**입니다. `Host`와 `User` 간의 통신을 FSM(Finite State Machine)으로 구성하여, 상태 기반 동작 흐름을 구현하였습니다.



---



## 🧩 상태 전이 구조

<img src="https://github.com/user-attachments/assets/ceeed877-d6ea-458c-9b2b-1df8bd75a2d6" style="max-width: 100%; height: auto;" />


### 🔹 Host FSM

| 상태 | 입력 (이벤트) | 동작 | 다음 상태 |
|------|----------------|------|------------|
| `IDLE` | node 값 입력 | 노드 값 확인 | `WAIT_ANSWER` 또는 `WAIT_QUIZ` |
| `WAIT_ANSWER` | 사용자 정답 수신 | 정답 검증 | `CHAT_READY` 또는 `TERMINATE` |
| `CHAT_READY` | - | 채팅 가능 상태 | `END` |
| `TERMINATE` | - | 종료 처리 | `END` |


---


### 🔹 User FSM

| 상태 | 입력 (이벤트) | 동작 | 다음 상태 |
|------|----------------|------|------------|
| `SEND_NODE` | 노드 값 입력 | Host로 전송 | `WAIT_QUIZ` |
| `WAIT_QUIZ` | Host 퀴즈 수신 | 퀴즈 표시 | `SEND_ANSWER` |
| `SEND_ANSWER` | 사용자 입력 → 전송 | 정답 여부 판단 | `CHAT_READY` 또는 `TERMINATE` |
| `CHAT_READY` | - | 채팅 가능 상태 | `END` |



---



## 🛠 사용 기술
- **언어**: C++
- **보드**: STM32 Nucleo-F446RE
- **환경**: mbed OS (ARM Cortex-M 기반 개발보드)  
- **통신**: 시리얼 기반  
- **설계 방식**: FSM 기반 네트워크 프로토콜 흐름 구현



---



## 💡 주요 설계 포인트
- FSM을 기반으로 송수신 흐름 제어
- 노드 기반 인증 및 퀴즈 기반 상호작용 설계
- Host-User 간 통신 프로토콜 (PDU 구조 정의 포함)
- 정답 검증 및 오류 처리 (타임아웃, 횟수 제한 등)
- 성공 시 채팅 가능, 실패 시 종료 처리



---



## ▶ 시연 영상
> [📺 시뮬레이션 결과 영상 보기](https://youtu.be/9sQsU2FJ0Lw?si=EXIZ3rdM1I2ljWdQ))




---



## ✨ 얻은 점
- FSM 및 네트워크 흐름 설계 능력 향상
- 시리얼 통신 기반 상태 디버깅 경험
- HOST와 USER 간 상호작용(채팅) 흐름 실습

