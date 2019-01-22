import { resolve } from 'path';
import { sync as getPkgRoot } from 'pkg-dir';

const PKG_ROOT = getPkgRoot(__dirname) as string;

const FS_ROOT = resolve(PKG_ROOT, '../..');
const FS_BIN = resolve(FS_ROOT, 'cmake-build-debug/bin');

export default Object.freeze({
  BINARIES: {
    MKFS: resolve(FS_BIN, 'mkfs.myfs'),
    MOUNT: resolve(FS_BIN, 'mount.myfs'),
  },
});
