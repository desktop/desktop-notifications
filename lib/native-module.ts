import { supportsNotifications } from './supports-notifications'
import { onNotificationEvent } from './notification-callback'
import { DesktopNotificationPermission } from './notification-permission'
import { INotificationOptions } from './notifications-options'

// The native binary will be loaded lazily to avoid any possible crash at start
// time, which are harder to trace.
let _nativeModule: any | null | undefined = undefined

function getNativeModule(): any | null {
  if (_nativeModule !== undefined) {
    return _nativeModule
  }

  _nativeModule = supportsNotifications()
    ? require('../build/Release/desktop-notifications.node')
    : null

  return _nativeModule
}

/**
 * Initializes the desktop-notifications system.
 *
 * @param toastActivatorClsid CLSID used by Windows to report notification events
 */
export const initializeNotifications: (
  opts: INotificationOptions
) => void = opts => {
  getNativeModule()?.initializeNotifications(onNotificationEvent, opts)
}

/** Terminates the desktop-notifications system. */
export const terminateNotifications: () => void = () =>
  getNativeModule()?.terminateNotifications()

export const getNotificationsPermission: () => DesktopNotificationPermission = () =>
  getNativeModule()?.getNotificationsPermission()

export const showNotification: (
  id: string,
  title: string,
  body: string,
  userInfo?: Record<string, any>
) => number | null = (...args) =>
  getNativeModule()?.showNotification(...args) ?? null

export const closeNotification: (id: string) => void = (...args) =>
  getNativeModule()?.closeNotification(...args)
