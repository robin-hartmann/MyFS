import {
  readdirSync, constants as fsConstants, statSync,
} from 'fs';
import { userInfo as getUserInfo } from 'os';

import { mkfs } from '../util/mkfs';
import { mount, unmount, isMounted } from '../util/mount';
import { test, cleanup, init, FileInfo } from '../util/test';
import { getPath } from '../util/fs';

test.serial.before('init', init);

test.serial.before('creates container with 65 files', async (t) => {
  const initFiles: FileInfo[] = [];

  for (let i = 0; i < 65; i += 1) {
    initFiles[i] = {
      byteCount: 64 * i,
      path: `file-${i}`,
    };
  }

  await mkfs(t, initFiles);
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

test.serial.todo('allows to open a maximum of 64 files');

test('root directory has proper attributes', (t) => {
  const stats = statSync(getPath(t, '.'));
  const userInfo = getUserInfo();

  t.is(stats.mode & fsConstants.S_IFMT, fsConstants.S_IFDIR);
  t.is(stats.mode & ~fsConstants.S_IFMT, 0o555);
  t.is(stats.nlink, 2);

  // @todo is this needed?
  t.is.skip(stats.uid, userInfo.uid);
  t.is.skip(stats.gid, userInfo.gid);
});

test('contains 65 files and no directories', (t) => {
  const statsArray = readdirSync(t.context.mountDir)
    .map(entryName => statSync(getPath(t, entryName)));
  const dirCount = statsArray.filter(stats => stats.isDirectory()).length;
  const fileCount = statsArray.filter(stats => !stats.isDirectory()).length;

  t.is(dirCount, 0);
  t.is(fileCount, 65);
});

test('all files have proper attributes', (t) => {
  const userInfo = getUserInfo();
  const files = readdirSync(t.context.mountDir)
    .map(entryName => ({
      entryName,
      stats: statSync(getPath(t, entryName)),
    }))
    .filter(entryInfo => entryInfo.stats.isFile());

  files.forEach((fileInfo) => {
    const stats = fileInfo.stats;

    t.is(stats.uid, userInfo.uid);
    t.is(stats.gid, userInfo.gid);
    t.is(stats.mode & fsConstants.S_IFMT, fsConstants.S_IFREG);
    t.is(stats.mode & ~fsConstants.S_IFMT, 0o444);
    t.is(stats.nlink, 1);
    // @todo check other attributes and content
  });
});
