#ifndef H_DELAY_H
#define H_DELAY_H
int return_delay(void);
void parse_delay(char* subscribeTopic, byte* payload, unsigned int length);
void init_delay(int time);
#endif