import {
  readdirSync, constants as fsConstants, mkdirSync,
} from 'fs';
import { userInfo as getUserInfo, constants as OSConstants } from 'os';
import { resolve } from 'path';

import { mkfs } from '../util/mkfs';
import { mount, unmount, isMounted } from '../util/mount';
import { test, cleanup, init } from '../util/test';
import { getStats } from '../util/fs';

test.serial.before('init', init);

test.serial.before('creates container', async (t) => {
  await mkfs(t);
});

test.serial.before('mounts properly', async (t) => {
  await mount(t);
  t.true(await isMounted(t));
});

test.serial.after.always('unmounts properly', async (t) => {
  t.log(t.context);
  await unmount(t);
  t.false(await isMounted(t));
});

test.serial.after.always('cleanup', cleanup);

test('contains no entries', (t) => {
  const dirEntries = readdirSync(t.context.mountDir);
  t.is(dirEntries.length, 0);
});

test('root directory has proper attributes', (t) => {
  const stats = getStats(t, '.');
  const userInfo = getUserInfo();

  t.is(stats.mode & fsConstants.S_IFMT, fsConstants.S_IFDIR);
  t.is(stats.mode & ~fsConstants.S_IFMT, 0o555);
  t.is(stats.nlink, 2);
  t.is(stats.uid, userInfo.uid);
  t.is(stats.gid, userInfo.gid);
});

test('does not allow directories to be created', (t) => {
  const error = t.throws<NodeJS.ErrnoException>(
    () => mkdirSync(resolve(t.context.mountDir, 'some-new-directory')),
  );
  t.is(error.errno, -OSConstants.errno.EACCES);
});
