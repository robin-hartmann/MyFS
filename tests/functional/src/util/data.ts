import { readFileSync } from 'fs';
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
