export type IsMountedResult = boolean;

export interface IUmount {
  umount(device: string): Promise<void>;
  isMounted(device: string): Promise<IsMountedResult>;
}
