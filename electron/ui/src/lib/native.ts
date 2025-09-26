export const IS_TEST = !(window.bridge && typeof window.bridge.invoke === 'function');

export async function callNative(name: string, ...args: any[]) {
  if (!window.bridge?.invoke) throw new Error('Native bridge is not available');
  const t0 = performance.now();
  const raw = await window.bridge.invoke(name, ...args);
  const dt = (performance.now() - t0).toFixed(1);
  console.debug(`[native] ${name}(${args.map(a=>JSON.stringify(a)).join(', ')}) -> in ${dt}ms`);
  return Array.isArray(raw) && raw.length === 1 ? raw[0] : raw;
}

export function subscribeNativeEvents(handler: (evt: any) => void) {
  if (typeof window.bridge?.onEvent === 'function') {
    return window.bridge.onEvent(handler);
  }
  return () => {};
}

