import ava, { ExecutionContext as GenericExecutionContext, TestInterface } from 'ava';

interface Context {
  containerFile: string;
  logFile: string;
  mountDir: string;
  initFilesDir: string;
  initFiles: FileInfo[];
  cleanupCbs: (() => void)[];
}

export interface FileInfo {
  path: string;
  byteCount: number;
}

export type ExecutionContext = GenericExecutionContext<Context>;

export const test = ava as TestInterface<Context>;

export const init = (t: ExecutionContext) => {
  t.context.initFiles = [];
  t.context.cleanupCbs = [];
};

export const cleanup = (t: ExecutionContext) => {
  t.log({
    containerFile: t.context.containerFile,
    initFilesDir: t.context.initFilesDir,
    logFile: t.context.logFile,
    mountDir: t.context.mountDir,
  });
  t.context.cleanupCbs.forEach(cleanupCb => cleanupCb());
};
