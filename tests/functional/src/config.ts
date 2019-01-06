import { resolve } from 'path';

const FS_ROOT = resolve(__dirname, '../../..');
const FS_BUILD = resolve(FS_ROOT, 'cmake-build-debug/bin');

export default Object.freeze({
  BINARIES: {
    MKFS: resolve(FS_BUILD, 'mkfs.myfs'),
    MOUNT: resolve(FS_BUILD, 'mount.myfs'),
  },
});
