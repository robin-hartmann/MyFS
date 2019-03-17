import ava, { ExecutionContext as GenericExecutionContext, TestInterface } from 'ava';

interface Context {
  cleanupCbs: (() => void)[];
  containerFile: string;
  initFiles: FileInfo[];
  initFilesDir: string;
  logFile: string;
  mountDir: string;
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
  t.log('The following context was used:', {
    cleanupCbsCount: t.context.cleanupCbs.length,
    containerFile: t.context.containerFile,
    initFilesCount: t.context.initFiles.length,
    initFilesDir: t.context.initFilesDir,
    logFile: t.context.logFile,
    mountDir: t.context.mountDir,
  });
  t.context.cleanupCbs.forEach(cleanupCb => cleanupCb());
};
