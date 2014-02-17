ProgettoPiattaformeSW
=====================


# Dependencies
List of packages required for running and/or building

## Required
### Runtime
* Qt 5 or above 

### Build
* build-essential
* Qt5 devel
* git


## Installing prerequisites
Software dependences can be installed as root  in a opened console by running this command:
```
apt-get install build-essential libqt5 libqt5-dev git
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
