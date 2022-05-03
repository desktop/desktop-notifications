import { supportsNotifications } from './supports-notifications'
import { notificationCallback } from './notification-callback'
import { DesktopNotificationPermission } from './notification-permission'
import { INotificationOptions } from './notifications-options'
import { v4 as uuidv4 } from 'uuid'

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
) => void = opts =>
  getNativeModule()?.initializeNotifications(notificationCallback, opts)

/** Terminates the desktop-notifications system. */
export const terminateNotifications: () => void = () =>
  getNativeModule()?.terminateNotifications()

export const getNotificationsPermission: () => DesktopNotificationPermission = () =>
  getNativeModule()?.getNotificationsPermission()

export const showNotification: (
  title: string,
  body: string,
  userInfo?: Record<string, any>
) => string = (...args) => {
  const id = uuidv4()
  getNativeModule()?.showNotification(id, ...args) ?? null
  return id
}

export const closeNotification: (id: string) => void = (...args) =>
  getNativeModule()?.closeNotification(...args)
