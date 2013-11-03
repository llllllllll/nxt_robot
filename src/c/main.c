// Joe Jevnik
// 1.11.2013
// Main function.

#include <ncurses.h>
#include <assert.h>

#include "C_NXT/nxt.h"

#include "screen/scr.h"


int main(){
    NXT *nxt = alloc_NXT();
    SCR *scr = alloc_SCR(nxt);
    assert(scr);
    SCR_draw_menu(scr);
    return 0;
}
