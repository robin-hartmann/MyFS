interface ExecException extends Error {
  cmd?: string;
  killed?: boolean;
  code?: number;
  signal?: string;
}

declare module 'umount' {
  export function umount(
    device: string,
    callback: (error: ExecException | null, stdout: string, stderr: string) => void,
  ): void;
  export function isMounted(
    device: string,
    callback: (error: ExecException | null, isMounted: boolean | null) => void,
  ): void;
}
