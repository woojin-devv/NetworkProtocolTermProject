#ifndef L3_STATE_H
#define L3_STATE_H

// 상태 정의
typedef enum {
    IDLE, 
    WAIT_ANSWER,
    WAIT_QUIZ,
    SEND_ANSWER,
    CHAT_READY,
    TERMINATE

} L3_State;

// 상태 변수 선언 (외부 정의됨)
extern L3_State l3_state;

// 디버깅용 상태 문자열 변환 함수 
static inline const char* L3_stateToStr(L3_State state) {
    switch (state) {
        case IDLE: return "IDLE";
        case WAIT_QUIZ: return "WAIT_QUIZ";
        case WAIT_ANSWER: return "WAIT_ANSWER";
        case SEND_ANSWER: return "SEND_ANSWER";
        case CHAT_READY: return "CHAT_READY";
        case TERMINATE: return "TERMINATE";
        default: return "UNKNOWN_STATE";
    }
}

#endif // L3_STATE_H
