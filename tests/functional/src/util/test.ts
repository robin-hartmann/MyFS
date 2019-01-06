import ava, { ExecutionContext as UntypedExecutionContext, TestInterface } from 'ava';
import { SynchrounousResult } from 'tmp';

export interface Context {
  containerFile: string;
  logFile: SynchrounousResult;
  mountDir: SynchrounousResult;
}

export type ExecutionContext = UntypedExecutionContext<Context>;

export const test = ava as TestInterface<Context>;
