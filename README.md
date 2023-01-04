# teaclassic

game engine

### Dependencies

- [SDL2 release-2.26.1](https://github.com/libsdl-org/SDL.git)
- [openal-soft 1.22.2](https://github.com/kcat/openal-soft.git)
- [GLEW 2.2.0](https://github.com/nigels-com/glew.git)
- [Python 2.7.17]()

### Building

**`Linux`**

```sh
git clone --recursive https://github.com/realjf/teaclassic.git
cd teaclassic
make deps
make tc
```

**`Windows`**

```sh
git clone --recursive https://github.com/realjf/teaclassic.git
cd teaclassic
make deps PLAT=WINDOWS
make tc PLAT=WINDOWS
make launchers PLAT=WINDOWS
```
