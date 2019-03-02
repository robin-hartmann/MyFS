import { platform } from 'os';

import { IUmount } from './interface';
import * as umountLinux from './umount-linux';
import * as umountMacOS from './umount-macos';

let umount: IUmount;

switch (platform()) {
  case 'darwin':
    umount = umountMacOS;
    break;

  case 'linux':
    umount = umountLinux;
    break;

  default:
    throw new Error('Unsupported platform');
}

export = umount;
