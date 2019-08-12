# SPM (Simple Process Manager)
  SPM (Simple Process Manager) is a Simple Process Manager written in C.

### Features
1. Can manage and restart any processes managed by the manager.
2. Can stop/restart all processes.
3. Can restart a process upon it dying.

### Requirements
1. C99/C11 Compiler
2. Python 3+ (Only for testing)


## QnA  
### Why use this over something else?  
  Well, it doesn't really do much input/output stuff, aside from starting/stopping/restarting processes. Additionally everything is compiled in, so if you just need a handful of things you can do it.

### How do I use this?  
  Open up `user_procs.c` in `user_processes/` and put a `Create_Process()` call above the `user_setup` function (see the documentation provided there), and then put a corresponding `Process()` call in the `user_setup()` function with the right ID. Also the ID can be anything that is valid for a variable name in C (so it can be anything alphanumeric). Do not put quotes around the ID.

### How do I start/stop/restart a process?  
 Individual processes can not be restarted/stopped/restarted by the manager. you can stop all of them (run the compiled binary with an argument of `stop` or `restart`. Currently Restarting only works after running stop, and just runs the start command again. `stop` immediatley stops all processes and deletes them from memory until `restart` is ran.

### How do I stop the manager?  
 Run the compiled binary with `shutdown` as an argument and the manager will shutdown.

### What is the restart command for?  
 The restart command is currently only used when the process dies and needs to be restarted. If you exclude it, the process will only run once.
