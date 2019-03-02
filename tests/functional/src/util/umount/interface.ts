export type IsMountedResult = boolean;

export interface IUmount {
  umount(device: string): Promise<{}>;
  isMounted(device: string): Promise<IsMountedResult>;
}
