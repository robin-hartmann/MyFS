import { resolve } from 'path';

import { ExecutionContext } from 'util/test';

export const getPath = (t: ExecutionContext, entryName: string) =>
  resolve(t.context.mountDir, entryName);
