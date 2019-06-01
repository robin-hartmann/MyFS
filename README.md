# MyFS

## Prerequisites
To be able to build and run this project, the following software has to be installed:
* fuse
* pkg-config

For running the functional tests, the following needs to be installed additionally:
* [Node.js](https://nodejs.org) - JavaScript run-time environment

### Setup on macOS
1. Download and install [osxfuse](https://osxfuse.github.io/) from [here](https://github.com/osxfuse/osxfuse/releases)
1. Install [Homebrew](https://brew.sh/) as described [here](https://docs.brew.sh/Installation)
1. Use Homebrew to install the package [`pkg-config`](https://brewformulas.org/Pkg-config):<br>
  `brew install pkg-config`

## Documentation

### [Design](docs/design.md)

### [Specification](docs/spec.md)

### [Notes](docs/notes.md)

## Running the Tests

⚠️ The MyFS binaries are not built automatically before running the tests. So if the sources of MyFS have changed or haven't been built at all, make sure you run the build before running the tests. Otherwise the old binaries will be tested or the tests will fail altogether. ⚠️

### With the Terminal

Run the following command in the directory containing the `package.json` file:

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
