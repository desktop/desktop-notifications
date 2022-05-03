import { DesktopNotificationEvent } from './notification-event-type'
import { shownNotifications } from './notification'

export type NotificationCallback = (
  event: DesktopNotificationEvent,
  id: string,
  userInfo: Record<string, any>
) => void

let globalNotificationCallback: NotificationCallback | null = null

export const onNotificationEvent: NotificationCallback = (
  event: DesktopNotificationEvent,
  id: string,
  userInfo: Record<string, any>
) => {
  const notification = shownNotifications.get(id)
  if (notification === undefined) {
    // Events of notifications that are not in the cache will be handled via the
    // global callback. We can safely assume those notifications were posted
    // in a different app session.
    globalNotificationCallback?.(event, id, userInfo)
    return
  }

  if (event === 'click') {
    notification.onclick?.()
  }
  // TODO: handle other events?
}

export const setGlobalNotificationCallback = (
  callback: NotificationCallback | null
) => {
  globalNotificationCallback = callback
}
