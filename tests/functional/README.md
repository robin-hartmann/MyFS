# MyFS Functional Tests

The functional tests for MyFS are built using TypeScript and AVA.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites

#### Software

* [Node.js](https://nodejs.org) - JavaScript run-time environment

#### VS Code Extensions

This project is intended to be used with Visual Studio Code and the following extensions are recommended:

* [ESLint](https://marketplace.visualstudio.com/items?itemName=dbaeumer.vscode-eslint) - Integrates ESLint JavaScript into VS Code
* [TSLint](https://marketplace.visualstudio.com/items?itemName=eg2.tslint) - TSLint support for Visual Studio Code

### Preparing the Project

To download required packages, run the following command in the directory containing the `package.json` file:

```node
npm install
```

⚠️ This has to be done each time when the project's dependencies change. ⚠️

## Running the Tests

⚠️ The MyFS binaries are not built automatically before running the tests. So if the sources of MyFS have changed or haven't been built at all, make sure you run the build before running the tests. Otherwise the old binaries will be tested or the tests will fail altogether. ⚠️

### With the Terminal

Run the following command in the directory containing the `package.json` file:

```node
npm test
```

### With VS Code

This project includes task definitions for Visual Studio Code. Just open the parent folder of the `package.json` file in VS Code and then run the task `Run tests`.

## Debugging with VS Code

1. Set a breakpoint in a file inside the `src` folder
1. Go to `src/spec` and open the `.spec` file you want to run
1. Run the debugger (by default `F5`)

## Built With

* [AVA](https://github.com/avajs/ava) - Futuristic test runner for Node.js
* [TypeScript](https://www.typescriptlang.org/) - Typed superset of JavaScript that compiles to plain JavaScript

## Authors

* **Robin Hartmann** - *Initial work* - [robin-hartmann](https://github.com/robin-hartmann)
