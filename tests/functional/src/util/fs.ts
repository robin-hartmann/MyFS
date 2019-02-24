import { resolve } from 'path';

import { ExecutionContext } from './test';

export const getPath = (t: ExecutionContext, entryName: string) =>
  resolve(t.context.mountDir, entryName);
