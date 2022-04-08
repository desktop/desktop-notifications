import { supportsNotifications } from './supports-notifications'

// The native binary will be loaded lazily to avoid any possible crash at start
// time, which are harder to trace.
let _nativeModule: any | null | undefined = undefined

export function getNativeModule(): any | null {
  if (_nativeModule !== undefined) {
    return _nativeModule
  }

  _nativeModule = supportsNotifications()
    ? require('../../build/Release/desktop-notifications.node')
    : null

  return _nativeModule
}
