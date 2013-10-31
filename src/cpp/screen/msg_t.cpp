// Joe Jevnik
// 31.10.2013
// A type to be held by the message queue.

#include "msg_t.h"

msg_t::msg_t(char* txt,int attr){
    this->txt = txt;
    this->attr = attr;
}
