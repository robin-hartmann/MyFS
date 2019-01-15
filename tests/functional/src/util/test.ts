import ava, { ExecutionContext as GenericExecutionContext, TestInterface } from 'ava';
import { SynchrounousResult } from 'tmp';

export interface Context {
  containerFile: string;
  logFile: SynchrounousResult;
  mountDir: SynchrounousResult;
}

export type ExecutionContext = GenericExecutionContext<Context>;

export const test = ava as TestInterface<Context>;
