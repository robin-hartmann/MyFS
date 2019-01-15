import { exec as cbBasedExec } from 'child_process';
import { realpathSync } from 'fs';
import { dirSync, fileSync } from 'tmp';
import { umount as cbBasedUmount, isMounted as cbBasedIsMounted } from 'umount';
import { promisify } from 'util';

import { ExecutionContext } from '../util/test';
import config from '../config';

const exec = promisify(cbBasedExec);
const umount = promisify(cbBasedUmount);
const promiseBasedIsMounted = promisify(cbBasedIsMounted);

export const mount = async (t: ExecutionContext) => {
  t.context.logFile = fileSync({ prefix: 'myfs-log-', postfix: '.log' });
  t.context.mountDir = dirSync({ prefix: 'myfs-mount-' });

  // required, because /tmp is just a link to /private/tmp
  // and umount doesn't seem to work when used on links
  // @todo find a better solution
  t.context.mountDir.name = realpathSync(t.context.mountDir.name);

  if (!t.context.containerFile) {
    throw 'Context is missing required attribute "containerFile"';
  }

  try {
    // tslint:disable-next-line: max-line-length
    await exec(`"${config.BINARIES.MOUNT}" "${t.context.containerFile}" "${t.context.logFile.name}" "${t.context.mountDir.name}"`);
  } catch (e) {
    throw `Error while mounting device\n${e}`;
  }
};

export const unmount = async (t: ExecutionContext) => {
  if (!t.context.mountDir) {
    throw 'Context is missing required attribute "mountDir"';
  }

  try {
    await umount(t.context.mountDir.name);
  } catch (e) {
    throw `Error while unmounting device\n${e}`;
  }
};

export const isMounted = async (t: ExecutionContext) => {
  if (!t.context.mountDir) {
    throw 'Context is missing required attribute "mountDir"';
  }

  try {
    return await promiseBasedIsMounted(t.context.mountDir.name);
  } catch (e) {
    throw `Error while checking if device is mounted\n${e}`;
  }
};
