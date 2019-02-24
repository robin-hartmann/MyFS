import { statSync } from 'fs';
import { resolve } from 'path';

import { ExecutionContext } from './test';

export const getStats = (t: ExecutionContext, entryName: string) =>
  statSync(resolve(t.context.mountDir, entryName));
