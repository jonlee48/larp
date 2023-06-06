# larp

lame :speak_no_evil: rendering pipeline - a scratch built 3D rendering pipeline using only c++ and Simple DirectMedia Layer (SDL).

![Animation](docs/final_results.gif)

## Installation

First install the SDL library to provide low level access to the keyboard, mouse, and display.

```bash
sudo apt update
sudo apt install libsdl2-dev
```

Then compile and run the code.

```bash
git clone git@github.com:jonlee48/larp.git
cd larp
make
./larp
```

## TODO
-[ ] Makefile - o files and linker
-[ ] Makefile - does not detect changes to h files

## Valgrind

To check for memory leaks, run with Valgrind suppression file because SDL has leaks in the library.

```
valgrind --gen-suppressions=all --suppressions=./linux_sdl_gl.sup --leak-check=full --show-leak-kinds=all ./larp
```

## Resources

- [https://stackoverflow.com/questions/1997171/why-does-valgrind-say-basic-sdl-program-is-leaking-memory](https://stackoverflow.com/questions/1997171/why-does-valgrind-say-basic-sdl-program-is-leaking-memory)
- [https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks](https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks)
- [Simple DirectMedia Layer (SDL Wiki)](https://wiki.libsdl.org/SDL2/FrontPage)
- [SDL Dynamic API](https://github.com/libsdl-org/SDL/blob/main/docs/README-dynapi.md)
- [SDL Tutorials](http://lazyfoo.net/tutorials/SDL/index.php)
- [X11 forwarding on WSL2](https://stackoverflow.com/questions/61110603/how-to-set-up-working-x11-forwarding-on-wsl2)
- [Syncing HW Clock on WSL2](https://askubuntu.com/questions/1096930/sudo-apt-update-error-release-file-is-not-yet-valid)
- [Makefile Tutorial](https://makefiletutorial.com)
