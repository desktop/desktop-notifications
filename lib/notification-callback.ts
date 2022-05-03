import { DesktopNotificationEvent } from './notification-event-type'

export type NotificationCallback = (
  event: DesktopNotificationEvent,
  id: string,
  userInfo: Record<string, any>
) => void

let globalNotificationCallback: NotificationCallback | null = null

export const notificationCallback: NotificationCallback = (...args) =>
  globalNotificationCallback?.(...args)

export const onNotificationEvent = (callback: NotificationCallback | null) => {
  globalNotificationCallback = callback
}
