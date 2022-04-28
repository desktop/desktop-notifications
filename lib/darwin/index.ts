import { getNativeModule } from '../shared/native-module'
import { onNotificationEvent } from '../shared/notification-callback'
import { DesktopNotificationPermission } from '../shared/notification-permission'
import { INotificationOptions } from '../shared/notifications-options'

export const initializeNotifications: (
  opts: INotificationOptions
) => void = () =>
  getNativeModule()?.initializeNotifications(onNotificationEvent)

export const requestNotificationsPermission: () => Promise<boolean> = () =>
  getNativeModule()?.requestNotificationsPermission()

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
