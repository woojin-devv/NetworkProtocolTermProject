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

##  빌드 및 실행

### 1. 빌드

```bash
make
```

### 2. 바이너리 복사 스크립트 (Windows Cygwin 환경 기준)

```bash
make clean
make
cp ./BUILD/myProtocol.bin /cygdrive/d/
```

### 3. 자동 복사 스크립트 실행
```
./cp2.sh
```