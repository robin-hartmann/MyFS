import { resolve } from 'path';
import { sync as getPkgRoot } from 'pkg-dir';

const PKG_ROOT = getPkgRoot(__dirname) as string;
const PKG_RES = resolve(PKG_ROOT, 'res');

const FS_ROOT = resolve(PKG_ROOT, '../..');
const FS_BIN = resolve(FS_ROOT, 'cmake-build-debug/bin');

export default Object.freeze({
  BINARIES: {
    MKFS: resolve(FS_BIN, 'mkfs.myfs'),
    MOUNT: resolve(FS_BIN, 'mount.myfs'),
  },
  RESOURCES: {
    LOREM_IPSUM: resolve(PKG_RES, 'lorem-ipsum-1024.txt'),
  },
});
