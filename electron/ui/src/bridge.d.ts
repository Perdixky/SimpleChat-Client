export {};

declare global {
  interface Window {
    bridge?: {
      invoke: (name: string, ...args: any[]) => Promise<any>;
      log: (severity: string, message: string) => Promise<any>;
      onEvent?: (handler: (payload: any) => void) => () => void;
    };
  }
}

