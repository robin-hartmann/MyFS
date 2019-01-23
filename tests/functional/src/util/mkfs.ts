import { exec as cbBasedExec } from 'child_process';
import { promisify } from 'util';
import { tmpNameSync, setGracefulCleanup } from 'tmp';
import { unlinkSync } from 'fs';

import { ExecutionContext } from '../util/test';
import config from '../config';

setGracefulCleanup();

const exec = promisify(cbBasedExec);

export const mkfs = async (t: ExecutionContext) => {
  const containerFile = tmpNameSync({ prefix: 'myfs-container-', postfix: '.bin' });

  t.context.containerFile = containerFile;
  t.context.cleanupCbs.push(() => unlinkSync(t.context.containerFile));

  try {
    await exec(`"${config.BINARIES.MKFS}" "${t.context.containerFile}"`);
  } catch (e) {
    throw new Error(`Error while creating container\n${e}`);
  }
};
