import { DesktopNotificationEvent } from './notification-event-type'

export type NotificationCallback<
  T extends Record<string, any> = Record<string, any>
> = (event: DesktopNotificationEvent, id: string, userInfo: T) => void

let globalNotificationCallback: NotificationCallback | null = null

export const notificationCallback: NotificationCallback = (...args) =>
  globalNotificationCallback?.(...args)

export const onNotificationEvent = <
  T extends Record<string, any> = Record<string, any>
>(
  callback: NotificationCallback<T> | null
) => {
  globalNotificationCallback = callback as NotificationCallback
}
