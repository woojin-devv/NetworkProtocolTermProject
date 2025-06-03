#include "L3_role.h"
#include "mbed.h" 

uint16_t valid_node_ids[MAX_NODE_COUNT] = {0000}; //node 최소값 = 0, 최대값 = 65535
uint8_t valid_node_count = 0;

bool is_valid_node(uint16_t id) {
    for (int i = 0; i < valid_node_count; i++) {
        if (valid_node_ids[i] == id) return true;
    }
    return false;
}

void register_node(uint16_t id) {
    if (valid_node_count >= MAX_NODE_COUNT) return;

    // 중복 방지
    if (!is_valid_node(id)) {
        valid_node_ids[valid_node_count++] = id;
    }
}

void clear_node_table(void) {
    valid_node_count = 0;
}

bool is_host_node(uint16_t destId) {
    for (int i = 0; i < valid_node_count; i++) {
        if (valid_node_ids[i] == destId) {
            return false;
        }
    }
    return true;
}
