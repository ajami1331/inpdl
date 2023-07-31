# inpdl

## Description

A simple tool to download the test case using [competitive companion plugin](https://github.com/jmerle/competitive-companion).

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cd build
sudo make install
```

## Usage

```bash
inpd -w -d <directory>
```

### Flags
- `-w` - watch mode
- `-d` - directory to save the test case