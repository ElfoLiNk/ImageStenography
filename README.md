ProgettoPiattaformeSW
=====================


# Dependencies
List of packages required for running and/or building

## Required
### Runtime
* Qt 5.1 or above 

### Build
* build-essential
* Qt5.1 devel
* git


## Installing prerequisites
Software dependences can be installed as root  in a opened console by running this command:
```
apt-get install build-essential qt5-default git
```

Note: At the moment there is not yet, in Debian Stable repository, Qt5 so you need to download it from official Qt-Project's site:
```
wget http://download.qt-project.org/official_releases/qt/5.2/5.2.1/qt-opensource-linux-x64-5.2.1.run
chmod +x qt-opensource-linux-x64-5.2.1.run
./qt-opensource-linux-x64-5.2.1.run
PATH=$HOME/Qt5.2.1/5.2.1/gcc_64/bin:$PATH
export PATH
```

# Build
## Build any distribution

Make sure that you have installed the needed dependencies for your distribution and in the same terminal follow these steps.


```
git clone https://github.com/ElfoLiNk/ProgettoPiattaformeSW.git
cd ProgettoPiattaformeSW
```

Now you may able to build the application:
```
qmake ProgettoPiattaformeSW.pro
make
```

# Install

## Any distribution install (after build any distribution)
Make sure that you have build. And run as root:
```
cd ProgettoPiattaformeSW
make install
```
