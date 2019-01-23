import ava, { ExecutionContext as GenericExecutionContext, TestInterface } from 'ava';

export interface Context {
  containerFile: string;
  logFile: string;
  mountDir: string;
  cleanupCbs: (() => void)[];
}

export type ExecutionContext = GenericExecutionContext<Context>;

export const test = ava as TestInterface<Context>;

export const init = (t: ExecutionContext) => {
  t.context.cleanupCbs = [];
};

export const cleanup = (t: ExecutionContext) => {
  t.log(t.context);
  t.context.cleanupCbs.forEach(cleanupCb => cleanupCb());
};
