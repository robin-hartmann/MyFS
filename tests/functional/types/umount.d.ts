declare module 'umount' {
  export function umount(
    device: string,
    callback: (
      error: import('child_process').ExecException | null,
      stdout: string,
      stderr: string,
    ) => void,
  ): void;
  export function isMounted(
    device: string,
    callback: (
      error: import('child_process').ExecException | null,
      isMounted: boolean | null,
    ) => void,
  ): void;
}
