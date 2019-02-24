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

test('all entries are files', (t) => {
  const dirCount = readdirSync(t.context.mountDir)
    .filter(entryName => getStats(t, entryName).isDirectory())
    .length;
  t.is(dirCount, 0);
});

test('all files have proper attributes', (t) => {
  const userInfo = getUserInfo();
  const files = readdirSync(t.context.mountDir)
    .map(entryName => ({
      entryName,
      stats: getStats(t, entryName),
    }))
    .filter(entryInfo => entryInfo.stats.isFile());

  if (!files.length) {
    return t.pass();
  }

  files.forEach((fileInfo) => {
    const stats = fileInfo.stats;

    t.is(stats.mode & fsConstants.S_IFMT, fsConstants.S_IFREG);
    t.is(stats.mode & ~fsConstants.S_IFMT, 0o444);
    t.is(stats.nlink, 1);
    t.is(stats.uid, userInfo.uid);
    t.is(stats.gid, userInfo.gid);
    // @todo check other attributes and content
  });
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
