# larp
lame :speak_no_evil: rendering pipeline - a scratch built 3D rendering pipeline using only c++ and Simple DirectMedia Layer (SDL).

## Installation
First install the SDL library to provide low level access to the keyboard, mouse, and display.

```
sudo apt update
sudo apt install libsdl2-dev
```

Then compile and run the code.

```
git clone git@github.com:jonlee48/larp.git
cd larp
g++ hello_world.cpp -w -lSDL2 -o hello_world
./hello_world
```

## Resources
- [Simple DirectMedia Layer (SDL Wiki)](https://wiki.libsdl.org/SDL2/FrontPage)
- [SDL Dynamic API](https://github.com/libsdl-org/SDL/blob/main/docs/README-dynapi.md)
- [SDL Tutorials](http://lazyfoo.net/tutorials/SDL/index.php)
- [X11 forwarding on WSL2](https://stackoverflow.com/questions/61110603/how-to-set-up-working-x11-forwarding-on-wsl2)
