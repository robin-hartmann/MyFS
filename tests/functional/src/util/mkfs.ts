import { exec as cbBasedExec } from 'child_process';
import { promisify } from 'util';
import { tmpNameSync } from 'tmp';

import { ExecutionContext } from '../util/test';
import config from '../config';

const exec = promisify(cbBasedExec);

export const mkfs = async (t: ExecutionContext) => {
  const containerFile = tmpNameSync();

  try {
    await exec(`${config.BINARIES.MKFS} ${containerFile}`);
  } catch (e) {
    throw `Error while creating container\n${e}`;
  }

  t.context.containerFile = containerFile;
};
