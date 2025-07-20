# pstd
Pierre's STD containers

## Building step

C++ 23 is required to build this project!

Using a venv is recommended (I'm using [uv](https://github.com/astral-sh/uv])). You can run the following, or `just python-env` if you have just installed. Don't forget to activate your env.

```shell
uv venv
source .venv/[activation_script_loc]
uv pip install conan
```

This should install Conan's latest version, make sure its >= 2

Installing [just](https://github.com/casey/just) is also recommended as it will make all the next steps easier. If you don't have it just copy the commands into your terminal.

For each of the commands, you can specify if you want to build for release or debug. Not choosing (default option as shown below), will build both.

```shell
just conan-setup
just build
```

If you want to run the examples, additionally run

```shell
just install
```

which will install the library inside `install_dir`. Then `cd` into the example directory and run `just build`

