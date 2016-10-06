# CraneFlak

---
###Description:
An interpreter for [Brain-Flak](https://github.com/DJMcMayhem/Brain-Flak) written in C.

Some differencs from the Ruby version:
* Programs can be read from stdin or provided to the interpreter via a command line flag.
	Programs can also be read from a file like in the Ruby Brain-Flak interpreter
* Requires code in skipped loops to be valid Brain-Flak
* Accurate character positions in error messages
* No debug flags (yet...)
* Less specific error messages (not for long...)
* No unicode support (yet...)
* No reverse flag (yet...)
* Values overflow above `LLONG_MAX` and underflow below `LLONG_MIN`
