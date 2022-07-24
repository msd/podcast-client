# Requirements

* conan

* C/C++ compiler (configure your conan profile to find the compiler)

* cmake

# Compiling

## Create a conan profile
    
In case you have not created any conan profiles

```conan profile new --detect default```

##   Using the default conan profile

```bash
mkdir build && cd build

conan install ../src --build=missing -s build_type=Release

cmake ../src

cmake --build . --config Release
```

## Debug

For development purposes it might be useful to use

`build_type=Debug` instead of `build_type=Release`

`config=Debug` instead of `config=Release`

## Other profiles

If you are using a conan profile other than the default one you should specify it as such

```conan install ../src --build=missing -pr [profile_name] -s build_type=Release```


# Recommended Compiler

----------------------
| OS      | Compiler | 
|---------|----------|
| Windows | MSVC     |
| Linux   | GNU      |
----------------------
