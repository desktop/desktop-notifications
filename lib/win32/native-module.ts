import { getNativeModule } from '../shared/native-module'
import { NotificationCallback } from '../shared/notification-callback'
import { DesktopNotificationPermission } from '../shared/notification-permission'

/**
 * Initializes the desktop-notifications system.
 *
 * @param toastActivatorClsid CLSID used by Windows to report notification events
 */
export const initializeNotifications: (
  toastActivatorClsid: string,
  callback: NotificationCallback
) => void = (...args) => getNativeModule()?.initializeNotifications(...args)

/** Terminates the desktop-notifications system. */
export const terminateNotifications: () => void = () =>
  getNativeModule()?.terminateNotifications()

export const getNotificationsPermission: () => DesktopNotificationPermission = () =>
  getNativeModule()?.getNotificationsPermission()

export const showNotification: (
  id: string,
  title: string,
  body: string
) => number | null = (...args) =>
  getNativeModule()?.showNotification(...args) ?? null

export const closeNotification: (id: string) => void = (...args) =>
  getNativeModule()?.closeNotification(...args)
