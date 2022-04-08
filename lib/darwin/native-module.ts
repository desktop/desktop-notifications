import { getNativeModule } from '../shared/native-module'
import { NotificationCallback } from '../shared/notification-callback'
import { DesktopNotificationPermission } from '../shared/notification-permission'

export const initializeNotifications: (
  callback: NotificationCallback
) => void = (...args) => getNativeModule()?.initializeNotifications(...args)

export const requestNotificationsPermission: () => Promise<void> = () =>
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
