import { getNativeModule } from '../shared/native-module'
import { onNotificationEvent } from '../shared/notification-callback'
import { DesktopNotificationPermission } from '../shared/notification-permission'
import { INotificationOptions } from '../shared/notifications-options'

/**
 * Initializes the desktop-notifications system.
 *
 * @param toastActivatorClsid CLSID used by Windows to report notification events
 */
export const initializeNotifications: (
  opts: INotificationOptions
) => void = opts => {
  if (opts.toastActivatorClsid === undefined) {
    throw new Error('toastActivatorClsid is required on Windows')
  }

  getNativeModule()?.initializeNotifications(
    opts.toastActivatorClsid,
    onNotificationEvent
  )
}

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
