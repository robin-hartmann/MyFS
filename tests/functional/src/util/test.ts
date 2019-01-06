// import ava, { Macro, TestInterface } from 'ava';
// import { readFileSync } from 'fs';
// import { resolve } from 'path';

import ava, { ExecutionContext as UntypedExecutionContext, TestInterface } from 'ava';
import { SynchrounousResult } from 'tmp';

export interface Context {
  containerFile: string;
  logFile: SynchrounousResult;
  mountDir: SynchrounousResult;
}

export type ExecutionContext = UntypedExecutionContext<Context>;

export const test = ava as TestInterface<Context>;

// const fileMacro: Macro<[string], Context> = (t, fileName) => {
//   const buffer = readFileSync(resolve(t.context.mountDir.name, fileName));
//   // in proper tests the buffer should not be cropped,
//   // but instead the entire buffer should be examined,
//   // e.g. when the contents of two files are compared for equality
//   // at most the buffer should be cropped to the files' sizes
//   const content = buffer.toString(undefined, 0, buffer.indexOf('\0'));
//   t.is(content, getExpectedFileContent(fileName));
// };

// fileMacro.title = (fileName = '') =>
//   `contains file '${fileName}' with content '${getExpectedFileContent(fileName)}'`;

// export { fileMacro };
