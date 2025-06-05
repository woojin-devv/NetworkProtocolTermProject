#ifndef L3_ROLE_H
#define L3_ROLE_H

#include <stdint.h>

#define MAX_NODE_COUNT 10  // 등록 가능한 최대 노드 수

extern uint16_t valid_node_ids[MAX_NODE_COUNT];
extern uint8_t valid_node_count; 

bool is_valid_node(uint16_t id);
void register_node(uint16_t id);
bool is_host_node(uint16_t destId); 
void clear_node_table(void);

#endif // L3_ROLE_H
