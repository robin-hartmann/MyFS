import { ExecutionContext } from 'ava';
import { statSync } from 'fs';
import { resolve } from 'path';

import { Context } from './test';

export const getStats = (t: ExecutionContext<Context>, entryName: string) =>
  statSync(resolve(t.context.mountDir, entryName));
