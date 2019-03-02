import { umount as cbBasedUmount, isMounted as cbBasedIsMounted } from 'umount';

import { IUmount, IsMountedResult } from './interface';

const umount = (device: string) =>
  new Promise((resolve, reject) =>
    cbBasedUmount(device, (error) => {
      if (error) {
        reject(error);
      } else {
        resolve();
      }
    }));

const isMounted = (device: string) =>
  new Promise<IsMountedResult>((resolve, reject) =>
    cbBasedIsMounted(device, (error, isMounted) => {
      if (error) {
        reject(error);
      } else {
        resolve(isMounted as boolean);
      }
    }));

const api: IUmount = {
  umount,
  isMounted,
};

export = api;
