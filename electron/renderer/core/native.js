// Native bridge helpers shared across renderer code
// - IS_TEST_MODE: true when preload did not expose the IPC bridge
// - callNative: invoke a native method through window.bridge.invoke

export const IS_TEST_MODE = !(
  window.bridge && typeof window.bridge.invoke === 'function'
);

export async function callNative(name, ...args) {
  console.debug(`[NATIVE CALL] ${name}() with args:`, args);

  if (!window.bridge || typeof window.bridge.invoke !== 'function') {
    const errorMsg = `Native bridge is not available.`;
    console.error(`[NATIVE ERROR] ${errorMsg}`);
    return Promise.reject(new Error(errorMsg));
  }

  try {
    const startTime = performance.now();
    const raw = await window.bridge.invoke(name, ...args);
    const endTime = performance.now();
    const duration = (endTime - startTime).toFixed(2);

    console.debug(`[NATIVE SUCCESS] ${name}() completed in ${duration}ms`);
    console.trace(`[NATIVE RESPONSE RAW] ${name}():`, raw);

    // Unwrap single-element arrays produced by C++ tuple serialization
    const result = (Array.isArray(raw) && raw.length === 1) ? raw[0] : raw;
    console.trace(`[NATIVE RESPONSE] ${name}():`, result);

    return result;
  } catch (e) {
    console.error(`[NATIVE ERROR] ${name}() failed:`, e);
    console.error(`[NATIVE ERROR] Call args were:`, args);
    return Promise.reject(e);
  }
}

