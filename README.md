# 📁 MyFS

FUSE-based file system, which stores all data in a single file while emulating a block device (educational project)

## 🛠️ Development

These instructions will get you a copy of this project up and running on your local machine for development and testing purposes.

### 📋 Prerequisites

To be able to build and run this project, the following software has to be installed:

- fuse (only available for UNIX systems)
- pkg-config

For running the functional tests, the following needs to be installed additionally:

- [Node.js](https://nodejs.org) - JavaScript run-time environment

#### Setup on macOS

1. Download and install [osxfuse](https://osxfuse.github.io/) from [here](https://github.com/osxfuse/osxfuse/releases)
1. Install [Homebrew](https://brew.sh/) as described [here](https://docs.brew.sh/Installation)
1. Use Homebrew to install the package [`pkg-config`](https://brewformulas.org/Pkg-config):<br>
   `brew install pkg-config`
1. (Optional) Install [Node.js](https://nodejs.org)

## 📘 Documentation

⚠️ Written in German

### [Design](docs/design.md)

### [Specification](docs/spec.md)

### [Functions](docs/funktionen.md)

### [Tests](docs/tests.md)

### [Notes](docs/notes.md)

## 📋 Running the Tests

⚠️ The MyFS binaries are not built automatically before running the tests. So if the sources of MyFS have changed or haven't been built at all, make sure you run the build before running the tests. Otherwise the old binaries will be tested or the tests will fail altogether.

Run any of the following commands in the directory containing the `package.json` file:

```bash
# unit tests
npm run test-unit

# functional tests
npm install # only required for initial installation and updates
npm run test-functional

# manual functional tests
npm run test-functional-manual

# all tests except manual
npm test
```

## 🧰 Built With

- [libfuse](https://github.com/libfuse/libfuse)

## 👨‍💻 Authors

- **Robin Hartmann** - [robin-hartmann](https://github.com/robin-hartmann)
  - _Design_
  - _Build Process_
  - _Getting the Implementation to work_
  - _Functional Tests_
    - _Using a Bash Script_
    - _Using TypeScript and AVA_
      - _For more information see repository [myfs-test](https://github.com/robin-hartmann/myfs-test)_
  - _Most of the Documentation_
    <br>
    <br>
- **Lukas Hügle** - [lukashuegle](https://github.com/lukashuegle)
  - _Design_
  - _Initial Implementation_
  - _Getting the Implementation to work_
  - _Unit Tests_
  - _Documentation of Functions_
    <br>
    <br>
- **Robin Weber** - [robinweber97](https://github.com/robinweber97)
  - _Design_
  - _Initial Implementation_

## 📃 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👍 Acknowledgments

- Special thanks to [Lukas](https://github.com/lukashuegle) and [Robin](https://github.com/robinweber97) for letting me publish this project
