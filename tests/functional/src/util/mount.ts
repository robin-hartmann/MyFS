import { exec as cbBasedExec } from 'child_process';
import { realpathSync } from 'fs';
import { dirSync, fileSync, setGracefulCleanup } from 'tmp';
import { umount, isMounted as promiseBasedIsMounted } from './umount/umount';
import { promisify } from 'util';

import { ExecutionContext } from './test';
import config from '../config';

setGracefulCleanup();

const exec = promisify(cbBasedExec);

export const mount = async (t: ExecutionContext) => {
  const logFile = fileSync({ prefix: 'myfs-log-', postfix: '.log' });
  const mountDir = dirSync({ prefix: 'myfs-mount-', unsafeCleanup: true });

  t.context.logFile = logFile.name;
  // required, because /tmp is just a link to /private/tmp
  // and umount doesn't seem to work when used on links
  // @todo find a better solution
  t.context.mountDir = realpathSync(mountDir.name);

  if (!t.context.containerFile) {
    throw new Error('Context is missing required attribute "containerFile"');
  }

  try {
    // tslint:disable-next-line: max-line-length
    await exec(`"${config.BINARIES.MOUNT}" "${t.context.containerFile}" "${t.context.logFile}" "${t.context.mountDir}"`);
  } catch (e) {
    throw new Error(`Error while mounting device\n${e}`);
  }
};

export const unmount = async (t: ExecutionContext) => {
  if (!t.context.mountDir) {
    throw new Error('Context is missing required attribute "mountDir"');
  }

  try {
    await umount(t.context.mountDir);
  } catch (e) {
    throw new Error(`Error while unmounting device\n${e}`);
  }
};

export const isMounted = async (t: ExecutionContext) => {
  if (!t.context.mountDir) {
    throw new Error('Context is missing required attribute "mountDir"');
  }

  try {
    return await promiseBasedIsMounted(t.context.mountDir);
  } catch (e) {
    throw new Error(`Error while checking if device is mounted\n${e}`);
  }
};
