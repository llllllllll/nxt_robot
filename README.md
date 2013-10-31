nxt_robot
=========

Overview
--------

- `ncurses` UI.
- Bluetooth control.
- Console logging along with a mirrored logfile.
- Supports multiple sized terminals.
- Supports resizing.


Screen
------

Basically a wrapper for c in an object to play nice with lestat.
writeln, and writeln attr both lock the screen and wait for a lock to expire
before writing to the screen. This allows writeln* to be called from pthreads
without worrying about a data race on the logv, or logattr arrays. Also, it
prevents most issues caused in ncurses when 2 threads attempt to print to the
screen at the same time, and move the cursor to unexpected locations mid print.
draw_stats() populates the static UI with the updated values, but only updates
the battery level because of how slow it is to update them all. Instead, it is
assumed that the robot control, or r_* functions will be properly updating
these.

Members:
public:

- `Screen()`
- `~Screen()`
- `void writelnattr(char*,int)`
- `inline void writeln(char*)`
- `void draw_stats()`
- `void draw_menu()`
- `BlueComm nxt`
- `int m0,m1,m2`
- `SensorState s0,s1,s2,s2`
- `Opcodes *op`

private:

- `void print_ui_static()`
- `void handle_opts()`
- `int mr,mc,opt,logc,*logattr`
- `char **logv`
- `FILE *logf`
- `bool lock`
- `pthread_t stay_alive`

loging status:

- logs each status events in `DEFAULT_PAIR`.
- logs user input requests in `A_BOLD`.
- logs errors in `A_BOLD | RED_PAIR`.
- logs confirmations in `GREEN_PAIR`.
- logs session ready in `A_BOLD | GREEN_PAIR`.


Also, non meber function:

    void *stay_alive_sig(void*)

logging status: logs each signal sent in `YELLOW_PAIR`.


r_remote
--------

Remote control of the NXT using bluetooth. The controls are as follows:

- `W` - max speed foward (100,100).
- `w` - increment both left and right by 10 if they can be incremented
further (+10,+10).
- `S` - max speed backwards (-100,-100).
- `s` decrement both left and right by 10 if they can be decremented
further (-10,-10).
- `q` - increment left motor by 10 (+10,_).
- `e` - increment right motor by 10 (_,+10).
- `a` - decrement left motor by 10 (-10,_).
- `d` - decrement right motor by 10 (_,-10).
- `r` - request a new sensor reading.
- `space` - stop all motors (0,0).
- `p` - return to menu.

logs status updates (`DEFAULT_PAIR`).


r_right
-------

TODO: autonomous program for the right turn track.

logs status updates (`DEFAULT_PAIR`).


r_left
------

TODO: autonomous program for the left turn track.

logs status updates (`DEFAULT_PAIR`).
