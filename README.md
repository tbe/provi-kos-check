# Providence KOS Lookup
Helper for KOS Lookups

# Compiling
## Linux
### Requirements
* QT4
* CMake
* Phonon

### Build
```bash
mkdir build
cd build
cmake ..
make
```

## Windows
### Requirements
* QT4
* VS2010 Express

### Build
1. open the Visual Studio command shell
2. create directory for build target
3. enter build directory and execute cmake ..
4. open the resulting project files with VS and compile the code
5. copy the needed libraries and the resulting binary into the same directory
6. create the qt.conf file in the target directory
6. be happy

#### needed libraries
plugins\imageformats\qjpeg4.dll
plugins\phonon_backend\phonon_ds94.dll
QtXml4.dll
QtGui4.dll
QtCore4.dll
phonon4.dll
ssleay32.dll
libeay32.dll
QtScript4.dll
QtNetwork4.dll

#### qt.conf
```
[Paths]
Prefix = .
Binaries = .
Plugins = ./plugins
```
