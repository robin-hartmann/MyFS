import { readFileSync, existsSync, writeFileSync, mkdirSync } from 'fs';
import { dirname } from 'path';

import config from '../config';

const LOREM_IPSUM = readFileSync(config.RESOURCES.LOREM_IPSUM);

export const generateData = (byteCount: number) => {
  const data = Buffer.allocUnsafe(byteCount);
  let remainingBytes = byteCount;

  while (remainingBytes > 0) {
    remainingBytes -= LOREM_IPSUM.copy(
      data,
      byteCount - remainingBytes,
      0,
      Math.min(LOREM_IPSUM.byteLength, remainingBytes),
    );
  }

  if (remainingBytes) {
    throw new Error('remainingBytes must be 0');
  }

  return data;
};

export const generateFile = (path: string, byteCount: number) => {
  if (existsSync(path)) {
    throw new Error(`file "${path}" already exists`);
  }

  const dir = dirname(path);

  if (!existsSync(dir)) {
    mkdirSync(dir, { recursive: true });
  }

  writeFileSync(path, generateData(byteCount));
};
