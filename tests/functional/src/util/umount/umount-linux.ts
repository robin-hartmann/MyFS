import { exec as cbBasedExec } from 'child_process';
import { promisify } from 'util';

import { IUmount, IsMountedResult } from './interface';

const exec = promisify(cbBasedExec);

const umount = (device: string) => exec(`fusermount -u "${device}"`);

const isMounted = (device: string) =>
  new Promise<IsMountedResult>((resolve, reject) => {
    cbBasedExec(`findmnt ${device}`)
      .on('error', reject)
      .on('exit', code => resolve(code === 0));
  });

const api: IUmount = {
  umount,
  isMounted,
};

export = api;
